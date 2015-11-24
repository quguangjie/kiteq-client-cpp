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

#include "ClientManager.h"
#include "BindingManager.h"
#include "ZkClient.h"
#include "KiteIOClient.h"
#include "asioKiteIOClient.h"
#include "ClientConfigs.hpp"
#include "remotingListener.hpp"
#include "ThreadPoolManager.hpp"
#include "MessageUtils.hpp"
#include "MessageListener.h"


class watchTopicServerDel :public IZkDataListener
{
public:
	watchTopicServerDel(shared_ptr<ZkClient> cli, const string &path, shared_ptr<ClientManager> pmanager, shared_ptr<KiteIOClient> kitecli, const string &serUri, const string &topic): IZkDataListener(path),_serverUri(serUri),_topic(topic)
	{
		_zkcli = cli;
		_kiteclient = kitecli;
		_clientmanager = pmanager;
	}
	~watchTopicServerDel(){}
	void handleDataDeleted(string &filepath)
	{
		_kiteclient->getAcceptedTopics().remove(_topic);
		if (_kiteclient->getAcceptedTopics().size() == 0) 
		{
			_clientmanager->remove(_serverUri);
			_kiteclient->close();
		}
	}
private:
	shared_ptr<ZkClient>        _zkcli;        
	shared_ptr<KiteIOClient>    _kiteclient;    
	shared_ptr<ClientManager>   _clientmanager;  
	string                      _serverUri;
	string                      _topic;
};

class   kiteRemotingListener: public remotingListener
{
public:
	kiteRemotingListener(shared_ptr<ClientManager> pmanager, shared_ptr<KiteIOClient> kitecli,     shared_ptr<MessageListener> l):_clientmanager(pmanager), _kiteclient(kitecli),_listener(l)
	{
	}
	void txAckReceived(shared_ptr<TxACKPacket> txAck)
	{
		ThreadPoolManager::get_mutable_instance().getWorkerExecutor()->schedule(boost::bind(&kiteRemotingListener::txAckReceivedWorker, this, txAck));
	}
	void bytesMessageReceived(shared_ptr<BytesMessage> message)
	{
		ThreadPoolManager::get_mutable_instance().getWorkerExecutor()->schedule(boost::bind(&kiteRemotingListener::bytesMessageReceivedWorker, this, message));
	}
	void stringMessageReceived(shared_ptr<StringMessage> message)
	{
		ThreadPoolManager::get_mutable_instance().getWorkerExecutor()->schedule(boost::bind(&kiteRemotingListener::stringMessageReceivedWorker, this, message));
	}
private:
	void txAckReceivedWorker(shared_ptr<TxACKPacket> txAck)
	{
		shared_ptr<TxResponse> txResponse = TxResponse::parseFrom(txAck);

		if(_listener->onMessageCheck(txResponse))
		{
			shared_ptr<TxACKPacket> txAckSend(new TxACKPacket());
			txAckSend->set_status(txResponse->getStatus());
			_kiteclient->send(Protocol::CMD_TX_ACK, txAckSend);
		}
	}
	void bytesMessageReceivedWorker(shared_ptr<BytesMessage> message)
	{
		if (_listener->onMessage(MessageUtils::convertMessage(message))) {
			shared_ptr<DeliverAck> ack(new DeliverAck());
			ack->set_groupid(message->header().groupid());
			ack->set_messageid(message->header().messageid());
			ack->set_messagetype(message->header().messagetype());
			ack->set_topic(message->header().topic());
			ack->set_status(true);
			_kiteclient->send(Protocol::CMD_DELIVER_ACK, ack);
		}
	}
	void stringMessageReceivedWorker(shared_ptr<StringMessage> message)
	{
		if (_listener->onMessage(MessageUtils::convertMessage(message))) {
			shared_ptr<DeliverAck> ack(new DeliverAck());
			ack->set_groupid(message->header().groupid());
			ack->set_messageid(message->header().messageid());
			ack->set_messagetype(message->header().messagetype());
			ack->set_topic(message->header().topic());
			ack->set_status(true);
			_kiteclient->send(Protocol::CMD_DELIVER_ACK, ack);
		}
	}
private:
	shared_ptr<ClientManager>   _clientmanager;  
	shared_ptr<KiteIOClient>    _kiteclient;    
	shared_ptr<MessageListener> _listener;
};


ClientManager::ClientManager(shared_ptr<BindingManager> bManager, shared_ptr<ClientConfigs> cConfigs, shared_ptr<MessageListener> l):bindingManager(bManager),clientConfigs(cConfigs),listener(l)
{
	setlock = shared_ptr<mutex>(new mutex());
	gThread.create_thread(boost::bind(&ClientManager::kiteIOClientCheckThread, this));
}

void     ClientManager::kiteIOClientCheckThread()
{
	unordered_set<shared_ptr<KiteIOClient> >::iterator it;

	while(true)
	{
		sleep(3);
		setlock->lock();
		for(it = clients.begin(); it != clients.end(); )
		{
			if ((*it)->isDead())
			{
				string serverUri = (*it)->getServerUrl();
				shared_ptr<KiteIOClient> ioClient;
				if(connMap.remove(serverUri, ioClient) == true)
				{
					shared_ptr<KiteIOClient> _kiteIOClient;
					if ((waitingClients.putIfAbsent(serverUri, ioClient, _kiteIOClient)) == false) 
					{
						thread th(boost::bind(&ClientManager::kiteIOClientReconnectThread, this, ioClient, serverUri));
						th.detach();
					}
				}
				unordered_set<shared_ptr<KiteIOClient> >::iterator tmpit = it;
				++it;
				clients.erase(tmpit);
				continue;
			}
			++it;
		}
		setlock->unlock();
	}
}
void     ClientManager::kiteIOClientReconnectThread(shared_ptr<KiteIOClient> c, const string &serverUri)
{
	if (c->reconnect()) {
		waitingClients.remove(serverUri);
		putIfAbsent(serverUri, c);
	}
}

shared_ptr<KiteIOClient> ClientManager::get(const string& topic) throw (NoKiteqServerException)
{
        list<string> serverUris = bindingManager->getServerList(topic);
		list<string>::iterator it;
        if (serverUris.size() == 0) {
            throw new NoKiteqServerException(topic);
        }
		/*
		 * random index
		 */
		srand((unsigned int )time(NULL));
		int i = 0;
		int index = rand() % serverUris.size();
		for(i = 0,it = serverUris.begin(); it != serverUris.end(); ++it, ++i)
		{
			if(i >= index)
				break;
		}
		if(it == serverUris.end())
            throw new NoKiteqServerException(topic);
        string serverUri = (*it);
		shared_ptr<KiteIOClient> client;
		if(connMap.get(serverUri, client) == true)
			return client;
        throw new NoKiteqServerException(topic);
}

shared_ptr<KiteIOClient> ClientManager::putIfAbsent(const string& serverUrl, shared_ptr<KiteIOClient> client) 
{
	/*
	 * return true: already absent
	 * return false: no absent
	 */
	shared_ptr<KiteIOClient> _client;
	if ((connMap.putIfAbsent(serverUrl, client, _client)) == false) {
		setlock->lock();
		clients.insert(client);
		setlock->unlock();
	}
	return _client;
}

shared_ptr<KiteIOClient> ClientManager::remove(const string& serverUri) 
{
	shared_ptr<KiteIOClient> client;
	if(connMap.remove(serverUri, client) == true) {
		setlock->lock();
	//	printf("ClientManager::remove client size %ld\n", clients.size());
		clients.erase(client);
		setlock->unlock();
	}
	return client;
}

void ClientManager::close() 
{
	/*
	for (KiteIOClient client : connMap.values()) 
	{
		client.close();
	}
	*/
}

void ClientManager::refreshServers(const string& topic, list<string> newServerUris) 
{
	list<string>::iterator it;

	for (it = newServerUris.begin(); it != newServerUris.end(); ++it) {
		printf("ClientManager::refreshServers %s \n", it->c_str());
		if (!connMap.containsKey(*it)) {
			createKiteQClient(topic, (*it));
		}
	}
}

void ClientManager::createKiteQClient(const string& topic, const string& serverUri) 
{
	shared_ptr<KiteIOClient> kiteIOClient;
	if(connMap.get(serverUri, kiteIOClient) == false)
	{
		try {
			kiteIOClient = createKiteIOClient(serverUri);
		} catch (std::runtime_error &e) {
			throw e;
		}

		if (kiteIOClient->handshake()) {
			shared_ptr<KiteIOClient> _kiteIOClient;
			if ((_kiteIOClient = putIfAbsent(serverUri, kiteIOClient)) != NULL) {
				kiteIOClient = _kiteIOClient;
			}
		} else {
			throw  std::runtime_error("Unable to create kiteq IO client: handshake refused.");
		}
	}
	kiteIOClient->getAcceptedTopics().add(topic);

	string fullpath(BindingManager::SERVER_PATH);
	fullpath.append(topic);
	fullpath.append("/");
	fullpath.append(serverUri);
	shared_ptr<watchTopicServerDel> watchListner (new watchTopicServerDel(bindingManager->getCurator(), fullpath, shared_from_this(), kiteIOClient, serverUri, topic));
	try {
		bindingManager->getCurator()->subscribeDataChanges(fullpath, watchListner);
	}catch (ZkExceptionNoNode &e){
	}
}

shared_ptr<KiteIOClient> ClientManager::createKiteIOClient(const string& serverUri) throw (std::runtime_error)
{
	cout << serverUri << endl;
	cout << clientConfigs->groupId << endl;
	cout << clientConfigs->secretKey << endl;

	shared_ptr<KiteIOClient> kiteIOClient(new asioKiteIOClient(clientConfigs->groupId, clientConfigs->secretKey, serverUri));
	kiteIOClient->start();
	shared_ptr<remotingListener> l(new kiteRemotingListener(shared_from_this(), kiteIOClient, listener));
	kiteIOClient->registerListener(l);
	return kiteIOClient;
}
