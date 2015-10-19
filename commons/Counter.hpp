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

#ifndef  __COUNTER_HPP
#define  __COUNTER_HPP

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

class Counter {
public:
	Counter():_counter(0) {
	}
	Counter(long c):_counter(c) {
	}
    long inc() {
		boost::unique_lock<boost::shared_mutex> l(_mutex);
		return ++_counter;
    }
	long getAndIncrement() {
		boost::unique_lock<boost::shared_mutex> l(_mutex);
		return ++_counter;
	}

    long getCountChange() {
		long c;
		bool ret;
		do {
			c = getCount();
			ret = compareAndSet(c, 0);
		} while(!ret);
		return c; 
    }
	long getCount() {
		boost::shared_lock<boost::shared_mutex> l(_mutex);
		return _counter;
	}
	void setCount(long n) {
		boost::shared_lock<boost::shared_mutex> l(_mutex);
		_counter = n;
	}
	bool compareAndSet(long expval, long newval) {
		boost::unique_lock<boost::shared_mutex> l(_mutex);
		if(_counter == expval)
		{
			_counter = newval;	
			return true;
		}
		return false;
	}
private:
	boost::shared_mutex    _mutex;
	long                  _counter;
};

#endif 
