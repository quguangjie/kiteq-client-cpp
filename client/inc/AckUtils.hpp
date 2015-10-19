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

#ifndef  __ACKUTILS_HPP
#define  __ACKUTILS_HPP
#include <boost/shared_ptr.hpp>

#include "protocol.h"

class AckUtils {
public: 
    static boost::shared_ptr<DeliverAck> buildDeliverAck(const Header &header) {
		boost::shared_ptr<DeliverAck> ack(new DeliverAck());
		if(ack == NULL)
			return ack;
		ack->set_groupid(header.get_groupid());
		ack->set_messageid(header.get_messageid());
		ack->set_messagetype(header.get_messagetype());
		ack->set_topic(header.get_topic());
		ack->set_status(true);
        return ack;
    }
    
}

#endif 

