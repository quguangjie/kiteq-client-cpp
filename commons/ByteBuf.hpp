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


#ifndef  __BYTEBUF_HPP
#define  __BYTEBUF_HPP

#include <vector> 

using namespace std;

class ByteBuf{
public:
	ByteBuf():readerIndex(0),writerIndex(0) {
	}
	ByteBuf(int size):readerIndex(0),writerIndex(0) {
		buff.reserve(size+1);	 
	}

	static char readByte(const char *b)
	{
		return *b;
	}
	static short readShort(const char *b)
	{
		unsigned int c1 = b[0];
		unsigned int c2 = b[1];
		return (short)(((c1&0xFF) << 8) |(c2&0xFF)); 
	}
	static int readInt(const char *b)
	{
		unsigned int c1 = b[0];
		unsigned int c2 = b[1];
		unsigned int c3 = b[2];
		unsigned int c4 = b[3];
		return (int)(((c1&0xFF)<<24) | ((c2&0xFF) << 16) | ((c3&0xFF) << 8) |(c4&0xFF)); 
	}
	static long long readLong(const char *b)
	{
		unsigned int c1 = b[0];
		unsigned int c2 = b[1];
		unsigned int c3 = b[2];
		unsigned int c4 = b[3];
		unsigned int c5 = b[4];
		unsigned int c6 = b[5];
		unsigned int c7 = b[6];
		unsigned int c8 = b[7];
		return (long long)(((c1&0xFF)<<56) | ((c2&0xFF) << 48) | ((c3&0xFF) << 40) |((c4&0xFF) << 32) |  ((c5&0xFF)<<24) | ((c6&0xFF) << 16) | ((c7&0xFF) << 8) |(c8&0xFF)); 
	}

	char readByte()
	{
		char c = buff[readerIndex++];
		return c; 
	}
	int readInt()
	{
		char c1 = buff[readerIndex++];
		char c2 = buff[readerIndex++];
		char c3 = buff[readerIndex++];
		char c4 = buff[readerIndex++];
		return (int)(((c1&0xFF)<<24) | ((c2&0xFF) << 16) | ((c3&0xFF) << 8) |(c4&0xFF)); 
	}
	void writeByte(char c)
	{
		buff.push_back(c);
		++writerIndex;
	}
	void writeShort(short i)
	{
		char c1 = (i >> 8) & 0xFF; 
		char c2 = (i) & 0xFF; 
		buff.push_back(c1);
		buff.push_back(c2);
		writerIndex += 2;
	}
	void writeInt(int i)
	{
		char c1 = (i >> 24) & 0xFF; 
		char c2 = (i >> 16) & 0xFF; 
		char c3 = (i >> 8) & 0xFF; 
		char c4 = (i) & 0xFF; 
		buff.push_back(c1);
		buff.push_back(c2);
		buff.push_back(c3);
		buff.push_back(c4);
		writerIndex += 4;
	}
	void writeLong(long i)
	{
		char c1 = (i >> 56) & 0xFF; 
		char c2 = (i >> 48) & 0xFF; 
		char c3 = (i >> 40) & 0xFF; 
		char c4 = (i >> 32) & 0xFF; 
		char c5 = (i >> 24) & 0xFF; 
		char c6 = (i >> 16) & 0xFF; 
		char c7 = (i >> 8) & 0xFF; 
		char c8 = (i) & 0xFF; 
		buff.push_back(c1);
		buff.push_back(c2);
		buff.push_back(c3);
		buff.push_back(c4);
		buff.push_back(c5);
		buff.push_back(c6);
		buff.push_back(c7);
		buff.push_back(c8);
		writerIndex += 8;
	}
	void writeBytes(const char *b, int num)  
	{
		if(num <= 0)
			return; 
		for(int i = 0; i < num; i++)
		{
			buff.push_back(b[i]);
		}
		writerIndex += num;
	}
	long readableBytes()
	{
		return buff.size() - readerIndex;
	}

	void resize(int size)
	{
		buff.reserve(size+1);	 
	}
	long  size()
	{
		return buff.size();	 
	}
	void  clear()
	{
		buff.clear();	 
	}
	char * getWritePtr(int len)
	{
		if(int(buff.capacity() - writerIndex) < len)	
		{
			return NULL;	
		}
		return &buff[writerIndex];
	}
	void  addWriterIndex(int s)
	{
		writerIndex += s; 
	}
	char * getBuffPtr()
	{
		return &buff[0];
	}

private:
	vector<char>    buff;
	int             readerIndex;
	int             writerIndex;
};

#endif 


