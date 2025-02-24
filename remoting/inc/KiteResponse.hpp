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

#ifndef __KITERESPONSE_HPP
#define __KITERESPONSE_HPP

#include <boost/shared_ptr.hpp>
#include "protocol.h"

using namespace boost;

class KiteResponse {
public:
    KiteResponse(int id, shared_ptr<protobuf::MessageLite>m ): requestId(id), message(m) {
    }

    int getRequestId() {
        return requestId;
    }
    shared_ptr<protobuf::MessageLite> getMessage() {
        return message;
    }
private:
    int requestId;
	shared_ptr<protobuf::MessageLite> message;
}; 

#endif 
