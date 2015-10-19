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

#ifndef  __DEFAULTKITECLIENT_H
#define  __DEFAULTKITECLIENT_H

#include <boost/shared_ptr.hpp>
#include "KiteClient.h"
#include "protocol.h"
#include "NoKiteqServerException.h"

using namespace std; 
using namespace boost; 


class ClientConfigs;
class Header;
class MessageListener;
class BindingManager;
class ClientManager;
class ClientConfigs;

class DefaultKiteClient :  public KiteClient 
{
public:
    DefaultKiteClient(const string &zkAddr, shared_ptr<ClientConfigs> clientConfigs);
    DefaultKiteClient(const string &zkAddr, shared_ptr<ClientConfigs> cConfigs, shared_ptr<MessageListener> l);
	~DefaultKiteClient();
    void setPublishTopics(const vector<string>& topics);
    void setBindings(const vector<shared_ptr<Binding> >& bindings);
    void start();
    void close();
    shared_ptr<SendResult> sendStringMessage(shared_ptr<StringMessage> message) throw (NoKiteqServerException);
    shared_ptr<SendResult> sendBytesMessage(shared_ptr<BytesMessage> message) throw (NoKiteqServerException);
    shared_ptr<SendResult> sendTxMessage(shared_ptr<StringMessage> message, shared_ptr<TxCallback> txCallback) throw (NoKiteqServerException);
    shared_ptr<SendResult> sendTxMessage(shared_ptr<BytesMessage> message, shared_ptr<TxCallback> txCallback) throw (NoKiteqServerException);

private:
    bool getProducerName(string &name);
    void handleTxCallback(shared_ptr<TxCallback> txCallback, const Header& header) throw (NoKiteqServerException);
    void sendMessage(char cmdType, shared_ptr<google::protobuf::MessageLite> message, const string &topic) throw (NoKiteqServerException);
    shared_ptr<SendResult> innerSendMessage(char cmdType, shared_ptr<google::protobuf::MessageLite> message, const Header& header) throw (NoKiteqServerException);

private:
    vector<string>  			publishTopics;
    vector<shared_ptr<Binding> > 	bindings;
    shared_ptr<BindingManager>  bindingManager;
    shared_ptr<ClientManager>   clientManager;
    shared_ptr<ClientConfigs>   clientConfigs;
}; 
#endif 



