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



#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

#include "DefaultKiteClient.h"
#include "ListenerAdapter.hpp"
#include "ThreadPoolManager.hpp"
#include "ClientConfigs.hpp"
#include "SendResult.hpp"
#include "KiteStats.hpp"
#include "TxResponse.hpp"
#include "Binding.hpp"
#include "TxCallback.h"
#include "MessageListener.h"
#include "BindingManager.h"
#include "ClientManager.h"
#include "KiteIOClient.h"
#include "protocol.h"

DefaultKiteClient::DefaultKiteClient(const string &zkAddr, shared_ptr<ClientConfigs> cConfigs) 
{
	shared_ptr<ListenerAdapter> l(new ListenerAdapter());
    new (this)DefaultKiteClient(zkAddr, cConfigs, l);
}

DefaultKiteClient::DefaultKiteClient(const string &zkAddr, shared_ptr<ClientConfigs> cConfigs, shared_ptr<MessageListener> l) :clientConfigs(cConfigs)
{
    bindingManager = BindingManager::getInstance(zkAddr);
    clientManager = shared_ptr<ClientManager>(new ClientManager(bindingManager, clientConfigs, l));
}
DefaultKiteClient::~DefaultKiteClient()
{
	printf("DefaultKiteClient::~DefaultKiteClient\n");
}

void DefaultKiteClient::setPublishTopics(const vector<string>& topics) 
{
    publishTopics = topics;
}

void DefaultKiteClient::setBindings(const vector<shared_ptr<Binding> >&b) 
{
    bindings = b;
}

void DefaultKiteClient::start() 
{
    if (publishTopics.size() > 0) {
		vector<string>::iterator it;
		for(it = publishTopics.begin(); it != publishTopics.end(); ++it)
		{
            clientManager->refreshServers((*it), bindingManager->getServerList(*it));
        }

		string producerName;
        getProducerName(producerName);
		for(it = publishTopics.begin(); it != publishTopics.end(); ++it)
		{
            bindingManager->registerProducer((*it), clientConfigs->groupId, producerName);
            bindingManager->registerClientManager((*it), clientManager);
        }
    }
    
    if (bindings.size() > 0) {
		vector<shared_ptr<Binding> >::iterator it;
		for(it = bindings.begin(); it != bindings.end(); ++it)
		{
            string topic = (*it)->getTopic();
            clientManager->refreshServers(topic, bindingManager->getServerList(topic));
        }

		for(it = bindings.begin(); it != bindings.end(); ++it)
		{
            bindingManager->registerClientManager((*it)->getTopic(), clientManager);
        }

        bindingManager->registerConsumer(bindings);
    }
}

void DefaultKiteClient::close() 
{
	ThreadPoolManager::shutdown();
	KiteStats::close();
    bindingManager->close();
    clientManager->close();
}

shared_ptr<SendResult> DefaultKiteClient::sendStringMessage(shared_ptr<StringMessage> message) throw (NoKiteqServerException)
{
    return innerSendMessage(Protocol::CMD_STRING_MESSAGE, shared_ptr<protobuf::MessageLite>(message), message->header());
}


shared_ptr<SendResult> DefaultKiteClient::sendBytesMessage(shared_ptr<BytesMessage> message) throw (NoKiteqServerException)
{
    return innerSendMessage(Protocol::CMD_BYTES_MESSAGE, shared_ptr<protobuf::MessageLite>(message), message->header());
}

shared_ptr<SendResult> DefaultKiteClient::sendTxMessage(shared_ptr<StringMessage> message, shared_ptr<TxCallback> txCallback) throw (NoKiteqServerException)
{
    shared_ptr<SendResult> result = sendStringMessage(message);
	printf("111111111result->isSuccess %d\n", result->isSuccess());
    if (result->isSuccess()) {
        handleTxCallback(txCallback,  message->header());
    }
    return result;
}

shared_ptr<SendResult> DefaultKiteClient::sendTxMessage(shared_ptr<BytesMessage> message, shared_ptr<TxCallback> txCallback) throw (NoKiteqServerException)
{
    shared_ptr<SendResult> result = sendBytesMessage(message);
    if (result->isSuccess()) {
        handleTxCallback(txCallback, message->header());
    }
    return result;
}

void DefaultKiteClient::handleTxCallback(shared_ptr<TxCallback> txCallback, const Header& header) throw (NoKiteqServerException)
{
    shared_ptr<TxResponse> txResponse (new TxResponse(header));
    try {
        txResponse->setMessageId(header.messageid());
		if(txCallback != NULL)
			txCallback->doTransaction(txResponse);
        
        if (txResponse->isRollback()) {
        } else {
            txResponse->commit();
        }
    } catch (std::runtime_error &e) {
        txResponse->rollback();
    }
    Header * _header = new Header();
	_header->CopyFrom(header);
    _header->set_commit(true);

    shared_ptr<TxACKPacket> txAck(new TxACKPacket());
    txAck->set_allocated_header(_header);
    txAck->set_status(txResponse->getStatus());
    txAck->set_feedback(txResponse->getFeedback());
    sendMessage(Protocol::CMD_TX_ACK, txAck, header.topic());  
}

void DefaultKiteClient::sendMessage(char cmdType, shared_ptr<protobuf::MessageLite> message, const string &topic) throw (NoKiteqServerException)
{
    shared_ptr<KiteIOClient> client = clientManager->get(topic);
    client->send(cmdType, message);
}

shared_ptr<SendResult> DefaultKiteClient::innerSendMessage(char cmdType, shared_ptr<protobuf::MessageLite> message, const Header & header) throw (NoKiteqServerException)
{
    shared_ptr<SendResult> result(new SendResult());
    try {
        shared_ptr<KiteIOClient> kiteIOClient = clientManager->get(header.topic());
        shared_ptr<MessageStoreAck> ack = kiteIOClient->sendMessageAndGet(cmdType, message);
        if (ack == NULL) {
            result->setSuccess(false);
            return result;
        }
        result->setMessageId(ack->messageid());
        result->setSuccess(ack->status());

    } catch (NoKiteqServerException &ex) {
        throw ex;
    } catch (std::exception &e) {
        result->setMessageId(header.messageid());
        result->setSuccess(false);
        result->setErrorMessage(e.what());
		printf("DefaultKiteClient::innerSendMessage %d\n", __LINE__);
    }
    return result;
}

bool DefaultKiteClient::getProducerName(string &name) 
{
	pid_t  pid = getpid();
	name.clear();
	name = boost::asio::ip::host_name();
	name.append("_");
	name.append(boost::lexical_cast<string>((long)pid));
	return true;
}
