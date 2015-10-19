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
	static int readInt(const char *b)
	{
		char c1 = b[0];
		char c2 = b[1];
		char c3 = b[2];
		char c4 = b[3];
		return (c1<<24) | (c2 << 16) | (c3 << 8) |(c4); 
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
		return (c1<<24) | (c2 << 16) | (c3 << 8) |(c4); 
	}
	void writeByte(char c)
	{
		buff.push_back(c);
		++writerIndex;
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
	void writeBytes(char b[], int num)  
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


