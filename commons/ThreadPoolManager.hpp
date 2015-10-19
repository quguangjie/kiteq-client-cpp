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

#ifndef __THREADPOOLMANAGER_HPP
#define __THREADPOOLMANAGER_HPP
#include <boost/shared_ptr.hpp>  
#include <boost/threadpool.hpp>  
#include <boost/serialization/singleton.hpp>
#include "Configs.h"

using namespace boost;
using boost::serialization::singleton;

class ThreadPoolManager : public singleton<ThreadPoolManager> {
public:
	typedef  shared_ptr<boost::threadpool::pool> ThreadPoolExecutor; 

	ThreadPoolManager() {
	}
   static ThreadPoolExecutor getDispatcherExecutor() {
        return dispatcherExecutor;
    }
    
   static ThreadPoolExecutor getWorkerExecutor() {
        return workerExecutor;
    }
    
   static void shutdown() {
	   workerExecutor->clear();
	   dispatcherExecutor->clear();
    }
private: 
    static void shutdownExecutor(ThreadPoolExecutor executor) {
	   executor->clear();
    }
private:
    static const int  corePoolSize = Configs::THREAD_CORE_POOL_SIZE;
    static const int  maximumPoolSize = Configs::THREAD_MAX_POOL_SIZE;
    static const long keepAliveTime = Configs::THREAD_KEEPALIVE_TIME;

    static ThreadPoolExecutor 	dispatcherExecutor;
    static ThreadPoolExecutor 	workerExecutor;
};

#endif 


