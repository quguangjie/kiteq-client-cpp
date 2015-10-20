/*
 *  GuangJie Qu <qgjie456@163.com>
 *  Copyright (C) 2015  BeiJing PZYJ Technology Co. Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, 
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asioKiteIOClient.h"
#include "HostPort.hpp"
#include "KiteStats.hpp"
#include "KiteResponse.hpp"
#include "ResponseFuture.hpp"
#include "protocol.h"
#include "ListenerManager.h"

#include <boost/date_time/posix_time/posix_time.hpp>  
#include <boost/thread.hpp>  
#include <stdexcept>  
#define BOOST_DATE_TIME_SOURCE  
  
class Heartbeat {
public:
	void setParent(shared_ptr<asioKiteIOClient> p) {
		parent = p;
	}
	void start() {
		gHbThread.create_thread(boost::bind(&Heartbeat::hbThread, this));
	}
	void setNextHeartbeatTime(long time) {
		long next = nextHeartbeatTime.getCount();
		if (time > next) {
			if (!nextHeartbeatTime.compareAndSet(next, time)) {
				setNextHeartbeatTime(time);
			}
		}
	}
	void reset() {
		stopCount.setCount(0);
	}
	void stop() {
		gHbThread.interrupt_all();
	}
	bool isDead()
	{
		return stopCount.getCount() >= 5;
	}
private:
	void     hbThread()
	try {
		while(true)
		{
			if (parent->getStatus().getCount() == asioKiteIOClient::RUNNING) {
				boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
				boost::posix_time::time_duration time_from_epoch =
					boost::posix_time::microsec_clock::universal_time() - epoch;
				long now = time_from_epoch.total_microseconds()/1000;
				long _nextHeartBeatTime = nextHeartbeatTime.getCount();
	printf("[%s.%d]\n", __FUNCTION__,__LINE__);
				if (now > _nextHeartBeatTime) {
	printf("[%s.%d]\n", __FUNCTION__,__LINE__);
					shared_ptr<HeartBeat> heartBeat(new HeartBeat());
					heartBeat->set_version(now);
					shared_ptr<HeartBeat> response = parent->sendHeartBeatAndGet(Protocol::CMD_HEARTBEAT, heartBeat);
					if (response != NULL && response->version() == now) {
						stopCount.setCount(0);
					} else {
						stopCount.inc();
					}
				}
				else {
					printf("%ld\n", (_nextHeartBeatTime - now)*1000);
					usleep((_nextHeartBeatTime - now)*1000);
	printf("[%s.%d]\n", __FUNCTION__,__LINE__);
				}
			}
			sleep(1);
		}
	} catch(thread_interrupted &e) {
			printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	}

private:
	shared_ptr<asioKiteIOClient>   parent;
	thread_group    	gHbThread;
	Counter   			stopCount;
	Counter   			nextHeartbeatTime;
};


asioKiteIOClient::asioKiteIOClient(const string &gid, const string &skey, const string &surl)
		:ioService(),ioWork(ioService),resolver_(ioService),socket_(ioService),
	        session_(socket_),status(NONE),
			groupId(gid),secretKey(skey),serverUrl(surl),readPkg()
{
	vector<string> parts;
	algorithm::split(parts, serverUrl, algorithm::is_any_of("\\?"));
	hostPort = HostPort::parse(parts[0]);
	heartbeat = shared_ptr<Heartbeat>(new Heartbeat());
	function0<void> f = bind(&asioKiteIOClient::workThread, this);
	thread thrd(f);
	printf("[%s.%d]\n", __FUNCTION__,__LINE__);
}

void     asioKiteIOClient::workThread()
{
	ioService.run();
}

bool asioKiteIOClient::start()
{
	if (!status.compareAndSet(NONE, PREPARE)) 
		return false;

	heartbeat->setParent(shared_from_this());
	int nretry = 0;					        
	while(true)
	{
		reconnect0(++nretry);
		printf("asioKiteIOClient::reconnect status %ld\n", status.getCount());
		if (status.getCount() == RECOVERING) {
			break;
		}
		else if(nretry >= 10 || status.getCount() == STOP)
			return false; 
	}
	printf("[%s.%d]\n", __FUNCTION__,__LINE__);
	KiteStats::start();
	heartbeat->start();
	return true;
}
bool asioKiteIOClient::reconnect()
{
	if (!status.compareAndSet(RUNNING, RECONNECTING)) 
		return false;
	int nretry = 0;					        
	while(true)
	{
		reconnect0(++nretry);
		printf("asioKiteIOClient::reconnect status %ld\n", status.getCount());
		if (status.getCount() == RECOVERING) {
			heartbeat->reset();
			if (handshake()) {
				printf("asioKiteIOClient::reconnect  set running");
				status.setCount(RUNNING);
				return true; 
			}
			break;
		}
		else if(nretry >= 10 || status.getCount() == STOP)
			return false; 
	}
	return false; 
}

bool asioKiteIOClient::reconnect0(int nretry)
{
	tcp::resolver::query  rq(hostPort->getHost(), hostPort->getPort());
	std::cout << "server: " << hostPort->getHost() << " port:" <<  hostPort->getPort() << std::endl;
	resolver_.async_resolve(rq, boost::bind(&asioKiteIOClient::handle_resolve, this, boost::asio::placeholders::error,  boost::asio::placeholders::iterator));
	sleep(nretry);
	return true;
}
bool asioKiteIOClient::isDead()
{
	return heartbeat->isDead();
}
void asioKiteIOClient::close()
{
	socket_.close();
	ioService.stop();
	heartbeat->stop();
}
void asioKiteIOClient::send(int cmdType,  shared_ptr<protobuf::MessageLite> message)
{
	KitePacket reqPacket(cmdType, message);
	session_.async_write(reqPacket, boost::bind(&asioKiteIOClient::handle_write_request, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));   
}
shared_ptr<HeartBeat>       asioKiteIOClient::sendHeartBeatAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message)
{
	KitePacket reqPacket(cmdType, message);
	shared_ptr<ResponseFuture> future(new ResponseFuture(reqPacket.getOpaque()));
	future->ResponseFutureUsed();

	session_.async_write(reqPacket, boost::bind(&asioKiteIOClient::handle_write_request, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));   
	try {
		shared_ptr<KiteResponse> resp = future->getResponse(1000);  //1000ms
		if (resp == NULL) {
			return shared_ptr<HeartBeat>();
		} else {
			boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
			boost::posix_time::time_duration time_from_epoch =
				boost::posix_time::microsec_clock::universal_time() - epoch;
			long time_now = time_from_epoch.total_microseconds()/1000;
			heartbeat->setNextHeartbeatTime(time_now + 3000);
		}
		shared_ptr<HeartBeat> msg = static_pointer_cast<HeartBeat>(resp->getMessage());
		return msg;
	} catch (std::exception &e) {
	}
	return shared_ptr<HeartBeat>();
}
shared_ptr<ConnAuthAck>     asioKiteIOClient::sendConnAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message)
{
	printf("[%s.%d]\n", __FUNCTION__,__LINE__);
	KitePacket reqPacket(cmdType, message);
	shared_ptr<ResponseFuture> future(new ResponseFuture(reqPacket.getOpaque()));
	future->ResponseFutureUsed();

	session_.async_write(reqPacket, boost::bind(&asioKiteIOClient::handle_write_request, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));   
	try {
		shared_ptr<KiteResponse> resp = future->getResponse(3000);  //1000ms
		if (resp == NULL) {
			return shared_ptr<ConnAuthAck>();
		} else {
			boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
			boost::posix_time::time_duration time_from_epoch =
				boost::posix_time::microsec_clock::universal_time() - epoch;
			long time_now = time_from_epoch.total_microseconds()/1000;
			heartbeat->setNextHeartbeatTime(time_now + 3000);
		}
		shared_ptr<ConnAuthAck> msg = static_pointer_cast<ConnAuthAck>(resp->getMessage());
		return msg;
	} catch (std::exception &e) {
	}
	return shared_ptr<ConnAuthAck>();
}
shared_ptr<MessageStoreAck> asioKiteIOClient::sendMessageAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message)
{
	KitePacket reqPacket(cmdType, message);
	shared_ptr<ResponseFuture> future(new ResponseFuture(reqPacket.getOpaque()));
	future->ResponseFutureUsed();
	printf("asioKiteIOClient::sendMessageAndGet %d\n");

	session_.async_write(reqPacket, boost::bind(&asioKiteIOClient::handle_write_request, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));   

	try {
		shared_ptr<KiteResponse> resp = future->getResponse(1000);  //1000ms
		if (resp == NULL) {
			return shared_ptr<MessageStoreAck>();
		} else {
			boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
			boost::posix_time::time_duration time_from_epoch =
				boost::posix_time::microsec_clock::universal_time() - epoch;
			long time_now = time_from_epoch.total_microseconds()/1000;
			heartbeat->setNextHeartbeatTime(time_now + 3000);
		}
		shared_ptr<MessageStoreAck> msg = static_pointer_cast<MessageStoreAck>(resp->getMessage());
		return msg;
	} catch (std::exception &e) {
	}
	return shared_ptr<MessageStoreAck>();
}


void asioKiteIOClient::registerListener(shared_ptr<remotingListener> listener)
{
	//string addr = socket_.remote_endpoint().address().to_string();
	string addr = hostPort->toString();
	ListenerManager::registerListener(addr, listener);
}
const string &   asioKiteIOClient::getServerUrl()
{
	return serverUrl;
}
ConcurrentSet<string>& asioKiteIOClient::getAcceptedTopics()
{
	return  acceptedTopics;
}

bool asioKiteIOClient::handshake()
{
	shared_ptr<ConnMeta> connMeta(new ConnMeta());
	connMeta->set_groupid(groupId);
	connMeta->set_secretkey(secretKey);

	printf("asioKiteIOClient::handshake [%s.%d]\n", __FUNCTION__,__LINE__);
	shared_ptr<ConnAuthAck> ack = sendConnAndGet(Protocol::CMD_CONN_META, connMeta);
	if(ack == NULL)
		return false;
	printf("asioKiteIOClient::handshake [%s.%d]\n", __FUNCTION__,__LINE__);
	bool success = ack->status();
	printf("asioKiteIOClient::handshake [%s.%d] success %d\n", __FUNCTION__,__LINE__, success);
	if (success) 
		status.setCount(RUNNING);
	return success;
}

void     asioKiteIOClient::handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
	printf("[%s.%d] %d\n", __FUNCTION__,__LINE__, (err));
	if(!err)
	{
#if BOOST_VERSION < 104801
		tcp::endpoint endpoint = *endpoint_iterator;
		socket_.async_connect(endpoint, boost::bind(&asioKiteIOClient::handle_connect, this, boost::asio::placeholders::error, ++endpoint_iterator));

#else
		boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&asioKiteIOClient::handle_connect, this, boost::asio::placeholders::error));
#endif

	}
}
#if BOOST_VERSION < 104801
void     asioKiteIOClient::handle_connect(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
#else
void     asioKiteIOClient::handle_connect(const boost::system::error_code& err)
#endif
{
	printf("[%s.%d] %d\n", __FUNCTION__,__LINE__, (err));
	if (!err) {
		status.setCount(RECOVERING);
		session_.async_read(readPkg, boost::bind(&asioKiteIOClient::handle_read_pkg, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
	}
}
void     asioKiteIOClient::handle_write_request(const boost::system::error_code& err, size_t written)
{
	printf("[%s.%d] %d\n", __FUNCTION__,__LINE__, (err));
	if (!err) {
		KiteStats::recordWrite();
	}
}
void     asioKiteIOClient::handle_read_pkg(const boost::system::error_code& err,std::size_t size)
{
	printf("[%s.%d] %d\n", __FUNCTION__,__LINE__, err);
	if (!err) {
		KiteStats::recordRead();

		char cmdType = readPkg.getCmdType();
	printf("[%s.%d] cmdType %d\n", __FUNCTION__,__LINE__, cmdType);
		if (cmdType == Protocol::CMD_CONN_AUTH ||
				cmdType == Protocol::CMD_MESSAGE_STORE_ACK ||
				cmdType == Protocol::CMD_HEARTBEAT) 
		{
			ResponseFuture::receiveResponse(shared_ptr<KiteResponse> (new KiteResponse(readPkg.getOpaque(), readPkg.getMessage())));
		} 
		else 
		{
		//	string addr = socket_.remote_endpoint().address().to_string();
			string addr = hostPort->toString();
			shared_ptr<remotingListener> listener = ListenerManager::getListener(addr);
			if (cmdType == Protocol::CMD_TX_ACK) {
				listener->txAckReceived(static_pointer_cast<TxACKPacket>(readPkg.getMessage()));
			} else if (cmdType == Protocol::CMD_BYTES_MESSAGE) {
				listener->bytesMessageReceived(static_pointer_cast<BytesMessage>(readPkg.getMessage()));
			} else if (cmdType == Protocol::CMD_STRING_MESSAGE) {
				listener->stringMessageReceived(static_pointer_cast<StringMessage>(readPkg.getMessage()));
			} else {
			}
		}
	//printf("[%s.%d] \n", __FUNCTION__,__LINE__);
		session_.async_read(readPkg, boost::bind(&asioKiteIOClient::handle_read_pkg, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
	printf("[%s.%d] \n", __FUNCTION__,__LINE__);

	}
}


