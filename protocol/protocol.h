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

#ifndef  __KITEQ_PROTOCOL_H
#define  __KITEQ_PROTOCOL_H

#include <protobuf/generated/kite_remoting.pb.h>
using namespace google;

class  Protocol
{
	public:
		static const char CMD_HEARTBEAT = 0x01;
		static const char CMD_CONN_META = 0x02;
		static const char CMD_CONN_AUTH = 0x03;

		static const char CMD_MESSAGE_STORE_ACK = 0x04;
		static const char CMD_DELIVER_ACK = 0x05;
		static const char CMD_TX_ACK = 0x06;

		static const char CMD_BYTES_MESSAGE = 0x11;
		static const char CMD_STRING_MESSAGE = 0x12;


		static const int PACKET_HEAD_LEN = 4 + 1 + 4;

		static const int TX_UNKNOWN = 0;
		static const int TX_COMMIT = 1;
		static const int TX_ROLLBACK = 2;
		static char CMD_STR_CRLF[2];
};

#endif 


