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

#ifndef  __MESSAGEUTILS_HPP
#define  __MESSAGEUTILS_HPP
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/algorithm/string.hpp>


#include "Message.hpp" 
#include "protocol.h" 

using namespace boost;

class MessageUtils 
{
public:

	static string messageId()
	{
		uuids::random_generator  rgen;
	    uuids::uuid u = rgen();
	    return replace_all_copy(to_string(u), "-", "");
	}

    static boost::shared_ptr<Message> convertMessage(shared_ptr<StringMessage> sMessage) {
		boost::shared_ptr<Message> message (new Message());
        message->setHeader(sMessage->header());
        message->setBodyType(Message::BODY_TYPE_STRING);
        message->setBodyString(sMessage->body());
        return message;
    }
    
    static boost::shared_ptr<Message> convertMessage(shared_ptr<BytesMessage>bMessage) {
		boost::shared_ptr<Message> message (new Message());
        message->setHeader(bMessage->header());
        message->setBodyType(Message::BODY_TYPE_BYTES); 
		message->setBodyBytes(bMessage->body());
        return message;
    }
};


#endif 
