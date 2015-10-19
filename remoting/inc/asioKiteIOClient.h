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

#ifndef  __ASIOKITEIOCLIENT_H
#define  __ASIOKITEIOCLIENT_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include  <boost/enable_shared_from_this.hpp>
#include "protocol.h"
#include "asioSession.hpp"
#include "Counter.hpp"
#include "KiteIOClient.h"

using namespace boost;
using boost::asio::ip::tcp;

class  HostPort;  
class  Heartbeat;
class  asioKiteIOClient : public KiteIOClient, public enable_shared_from_this<asioKiteIOClient>
{
public:
	enum STATE 
	{
		NONE = 0,
		PREPARE, // handshake
		RUNNING, 
		RECONNECTING, 
		RECOVERING, 
		STOP
	};

	asioKiteIOClient(const string &groupId, const string &secretKey, const string &serverUrl);

    void send(int cmdType,  shared_ptr<protobuf::MessageLite> message);
	shared_ptr<HeartBeat> sendHeartBeatAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message);
	shared_ptr<ConnAuthAck> sendConnAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message);
	shared_ptr<MessageStoreAck> sendMessageAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message);
    void registerListener(shared_ptr<remotingListener> listener);
    bool start();
    bool reconnect();
    bool isDead();
    void close();
    const string &getServerUrl();
    ConcurrentSet<string>& getAcceptedTopics();
    bool handshake();
	Counter & getStatus()  { return status;  }
private:
	void     workThread();
	bool     reconnect0(int nretry);
	void     handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
#if BOOST_VERSION < 104801
	void     handle_connect(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
#else
	void     handle_connect(const boost::system::error_code& err); 
#endif
	void     handle_write_request(const boost::system::error_code& err, size_t written); 
	void     handle_read_pkg(const boost::system::error_code& err, std::size_t size);

private:
	boost::asio::io_service         ioService;
	boost::asio::io_service::work   ioWork;
	tcp::resolver 			resolver_;
	tcp::socket 			socket_;
	boost::asio::streambuf  request_;
	boost::asio::streambuf  response_;
	asioSession<boost::asio::ip::tcp::socket> session_;
	Counter                 status;

	string groupId;
	string secretKey;
	string serverUrl;
	shared_ptr<HostPort> hostPort;
	shared_ptr<Heartbeat> heartbeat;
	KitePacket              readPkg;
    ConcurrentSet<string> acceptedTopics;
};

#endif 


