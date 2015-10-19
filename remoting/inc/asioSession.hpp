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

#include <vector>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include "protocol.h"
#include "KitePacket.hpp"  

using namespace boost; 

template <typename SOCK_TYPE>
class asioSession 
{
public:
	asioSession (SOCK_TYPE & socket): socket_(socket), read_buffer_()
	{}
	virtual ~asioSession() 
	{}
	
	/*
	 *  异步写入,写入完成回调函数 handle
	 */
	template <typename Handler>
	void async_write(KitePacket &pkg,  Handler handle) 
	{
		/*
		 *   define asioSession<SOCK_TYPE>::async_write_complete <Handler> function var 'f1'  
		 */
		void (asioSession<SOCK_TYPE>::*f1) (const system::error_code&, 
				                                     std::size_t offset, 
													 tuple<Handler>)  
			                               = &asioSession<SOCK_TYPE>::async_write_complete <Handler> ;
		int total_output_size = 0; 
		char *ptr = pkg.toByteBuf(total_output_size);
		printf("write size %d\n", total_output_size);
		if(total_output_size > 0)
		{
			asio::async_write(socket_, asio::buffer(ptr, total_output_size), 
				asio::transfer_at_least(total_output_size), 
				bind(f1, this, asio::placeholders::error, asio::placeholders::bytes_transferred, make_tuple(handle)));
		}
	}

	/*
	 *  同步写入
	 */
	void write(KitePacket &pkg) 
	{
		int total_output_size = 0; 
		char *ptr = pkg.toByteBuf(total_output_size);
   		asio::write(socket_, asio::buffer(ptr,total_output_size)); 
	}

	/*
	 *  异步读取
	 */
	template <typename Handler>
	void async_read(KitePacket &pkg,  Handler handle) 
	{
		async_setup_read_varint(pkg, make_tuple(handle));
	}

	/*
	 *  同步读取
	 */
	void read(KitePacket &pkg) 
	{
	}

private:
	/*
	 * 异步写入完成,进行回调
	 */
	template <typename Handler>
	void async_write_complete(const system::error_code& e,std::size_t bytes_transferred, tuple<Handler> handle) 
	{
		boost::get<0>(handle)(e,bytes_transferred);
	}

	/*
	 * 读取消息, 直到遇到pkg 分割副号
	 */
	template <typename Handler>
	void async_setup_read_varint(KitePacket &pkg, ::tuple<Handler> handle) 
	{
		/*
		 *   define asioSession<SOCK_TYPE>::async_read_pkg <Handler> function var 'f1'  
		 */
		void (asioSession<SOCK_TYPE>::*f1)(const system::error_code&, std::size_t offset,
											KitePacket &pkg, tuple<Handler>) 
			                                = &asioSession<SOCK_TYPE>::async_read_pkg<Handler>;  

		asio::async_read_until(socket_, read_buffer_, KitePacket::getDelim(), 
				                bind(f1,this, asio::placeholders::error, asio::placeholders::bytes_transferred, ref(pkg),handle));
	}


	/*
	 * 异步读取消息头部完成,进行处理
	 */
	template <typename Handler>
	void async_read_pkg(const system::error_code& e,std::size_t bytes_read, 
			                     KitePacket &pkg, tuple<Handler> handle) 
	{
		/*  出错 */  
		if (e) {
			boost::get<0>(handle)(e, bytes_read);
			return ;
		}

		const char * data = asio::buffer_cast<const char *> (read_buffer_.data());
		int    pkglen = KitePacket::delimPacket(data, read_buffer_.size());
		if(pkglen <= KitePacket::getHeaderLen())
		{
			async_setup_read_varint(pkg, handle);
			return ;
		}
		read_buffer_.consume(pkglen);  
		if(KitePacket::parseFrom(pkg, data, pkglen))
		{
			boost::get<0>(handle)(e, pkglen);  
		}
		return; 
	}

private:
	SOCK_TYPE 				& 	socket_;
	asio::streambuf 			read_buffer_;
};


