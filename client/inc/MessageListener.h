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

#ifndef __MESSAGELISTENER_H
#define __MESSAGELISTENER_H

#include  <boost/shared_ptr.hpp>
using namespace boost;

#include  "Message.hpp"
#include  "TxResponse.hpp"

class MessageListener 
{
public:
	virtual ~MessageListener() {}
    virtual bool onMessage(shared_ptr<Message> message) = 0;
    virtual bool onMessageCheck(shared_ptr<TxResponse> tx) = 0;
}; 

#endif 
