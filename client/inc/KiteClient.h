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


#ifndef  __KITECLIENT_H
#define  __KITECLIENT_H
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "NoKiteqServerException.h"
#include "Binding.hpp"

using namespace std;
using namespace boost;

class SendResult;
class StringMessage;
class BytesMessage;
class TxCallback;

class KiteClient 
{
public:    
	virtual ~KiteClient() {}
	virtual void setPublishTopics(const vector<string>& topics) = 0;
	virtual void setBindings(const vector<shared_ptr<Binding> >& bindings) = 0;
	virtual void start() = 0;
	virtual void close() = 0;
	virtual shared_ptr<SendResult> sendStringMessage(shared_ptr<StringMessage> message) throw (NoKiteqServerException) = 0;
	virtual shared_ptr<SendResult> sendBytesMessage(shared_ptr<BytesMessage> message) throw (NoKiteqServerException) = 0;
	virtual shared_ptr<SendResult> sendTxMessage(shared_ptr<StringMessage> message, shared_ptr<TxCallback> txCallback) throw (NoKiteqServerException) = 0;
	virtual shared_ptr<SendResult> sendTxMessage(shared_ptr<BytesMessage> message, shared_ptr<TxCallback> txCallback) throw (NoKiteqServerException) = 0;
}; 

#endif  

