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

#ifndef  __KITEIOCLIENT_H
#define  __KITEIOCLIENT_H

#include <set>
#include <boost/shared_ptr.hpp>  
#include <google/protobuf/message.h>
#include "remotingListener.hpp"
#include "ConcurrentSet.hpp"
#include "protocol.h"

using namespace std;
using namespace boost;
class KiteIOClient 
{
public:
	virtual ~KiteIOClient() {}
    virtual void send(int cmdType, shared_ptr<protobuf::MessageLite> message) = 0;
    virtual void sendAck(int o, int cmdType, shared_ptr<protobuf::MessageLite> message) = 0;
    virtual shared_ptr<HeartBeat> sendHeartBeatAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message) = 0;
    virtual shared_ptr<ConnAuthAck> sendConnAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message) = 0;
    virtual shared_ptr<MessageStoreAck> sendMessageAndGet(int cmdType, shared_ptr<protobuf::MessageLite> message) = 0;
    virtual void registerListener(shared_ptr<remotingListener> listener)  = 0;
    virtual bool start()  = 0;
    virtual bool reconnect()  = 0;
    virtual bool isDead()  =0;
    virtual void close()  = 0;
    virtual const string &getServerUrl()  = 0;
    virtual ConcurrentSet<string>& getAcceptedTopics() = 0;
    virtual bool handshake() = 0;
};

#endif 


