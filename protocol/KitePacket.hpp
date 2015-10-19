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

#ifndef __KITEPACKET_HPP
#define __KITEPACKET_HPP
#include <boost/shared_ptr.hpp>

#include "protocol.h"
#include "ByteBuf.hpp"
#include "AtomicInteger.hpp"

using namespace boost;

class KitePacket 
{
public:
	KitePacket() {
        this->opaque = getPacketId();
	}
    KitePacket(char t, const shared_ptr<protobuf::MessageLite> m):cmdType(t), message(m) {
        this->opaque = getPacketId();
    }
    void setOpaque(int o) {
        opaque = o;
    }
    void setCmdType(char cmd) {
        cmdType = cmd;
    }
    void setMessage(shared_ptr<protobuf::MessageLite> m) {
        message = m;
    }

    int getOpaque() {
        return opaque;
    }
    
    char getCmdType() {
        return cmdType;
    }

    shared_ptr<protobuf::MessageLite> getMessage() {
        return message;
    }

	static const string  getDelim() {
		string s("\r\n");
		return s;	
	}

	static int     getHeaderLen() {
		return Protocol::PACKET_HEAD_LEN;	
	}

    char * toByteBuf(int &num) {
		int serialized_size = message->ByteSize();
        int total_output_size = getHeaderLen() + serialized_size + 2;
		if (buff.size() < total_output_size) {
			buff.resize(total_output_size);
		}
        buff.writeInt(opaque);       // 4 byte
        buff.writeByte(cmdType);     // 1 byte
        buff.writeInt(serialized_size);  // 4 byte
		do {
			char *p = buff.getWritePtr(serialized_size);
			if(p == NULL)
			{
				buff.resize(total_output_size);
				continue;
			}
			ByteBuf  buf(serialized_size);
			message->SerializeToArray(buf.getWritePtr(serialized_size), serialized_size);
        	buff.writeBytes(buf.getBuffPtr(), serialized_size);
			break;
		} while(true); 
        buff.writeBytes(Protocol::CMD_STR_CRLF, sizeof(Protocol::CMD_STR_CRLF)); // \r\n
		num = total_output_size;
		return buff.getBuffPtr();
    }

	static int delimPacket(const char * buf, int bufsize) {
		int DataSize = 0;
		for (int i = 1; i < bufsize; ++i  )
		{
		//	unsigned char c = buf[i-1];
		//	printf("%02x ", c);
			  const char DChar1 = buf[i-1];
			  const char DChar2 = buf[i];
			  // 这里需要自己判断字符串内容, read_until的文档里这么说的
			  if ( DChar1 == '\r' && DChar2 == '\n')
			  {
			//unsigned char c = buf[i];
			//printf("%02x ", c);
				  DataSize = i + 1;
				  break;
			  }
		}
		//printf("\n");
		return DataSize;
	}
    
    static bool parseFrom(KitePacket &pkg, const char * buf, int pkglen) {
        int  opaque = ByteBuf::readInt(buf);
        char cmdType = ByteBuf::readByte(buf + sizeof(int));
        int  len = ByteBuf::readInt(buf + sizeof(int) + sizeof(char)); // read length

		shared_ptr<protobuf::MessageLite>  msg;

		switch (cmdType) {
			case Protocol::CMD_CONN_AUTH:
				msg =  shared_ptr<protobuf::MessageLite>(new ConnAuthAck());
				msg->ParseFromArray(buf + getHeaderLen(), len);
				break;
			case Protocol::CMD_MESSAGE_STORE_ACK:
				msg =  shared_ptr<protobuf::MessageLite>(new MessageStoreAck());
				msg->ParseFromArray(buf + getHeaderLen(), len);
				break;
			case Protocol::CMD_TX_ACK:
				msg =  shared_ptr<protobuf::MessageLite>(new TxACKPacket());
				msg->ParseFromArray(buf + getHeaderLen(), len);
				break;
			case Protocol::CMD_BYTES_MESSAGE:
				msg =  shared_ptr<protobuf::MessageLite>(new BytesMessage());
				msg->ParseFromArray(buf + getHeaderLen(), len);
				break;
			case Protocol::CMD_STRING_MESSAGE:
				msg =  shared_ptr<protobuf::MessageLite>(new StringMessage());
				msg->ParseFromArray(buf + getHeaderLen(), len);
				break;
			case Protocol::CMD_HEARTBEAT:
				msg =  shared_ptr<protobuf::MessageLite>(new HeartBeat());
				msg->ParseFromArray(buf + getHeaderLen(), len);
				break;
			default:
				return false;
		}
		pkg.setOpaque(opaque);
		pkg.setCmdType(cmdType);
		pkg.setMessage(msg);
		return true;
	}
private:
    KitePacket(int o, char t, const shared_ptr<protobuf::MessageLite> m) {
        new (this)KitePacket(t, m);
        this->opaque = o;
    }

    long getPacketId() {
        long id = UNIQUE_ID.getAndIncrement();
        if (id == AtomicInteger::MAX_VALUE) {
            UNIQUE_ID.compareAndSet(AtomicInteger::MAX_VALUE, 0);
            return UNIQUE_ID.getAndIncrement();
        }
        return id;
    }
    

private:	
    static AtomicInteger    UNIQUE_ID;
    long 				    opaque; 
	char 				    cmdType;
    shared_ptr<protobuf::MessageLite> message;
	ByteBuf                 buff;
};

#endif 


