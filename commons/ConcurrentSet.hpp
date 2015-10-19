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

#ifndef   __CONCURRENTSET_HPP
#define   __CONCURRENTSET_HPP

#include <boost/thread/mutex.hpp>
#include <boost/unordered_set.hpp>

using namespace std;
using namespace boost;

template<class Key>
class ConcurrentSet
{
public:
	ConcurrentSet()
	{
	}
	~ConcurrentSet()
	{
		boost::mutex::scoped_lock l(lock); 
		values.clear();
	}
	void add(const Key&k)
	{
		boost::mutex::scoped_lock l(lock); 
		typename unordered_set<Key>::iterator it = values.find(k);
		if(it != values.end())
		{
			return;
		}
		values.insert(k);
		return ;
	}
	const Key & putIfAbsent(const Key&k)
	{
		boost::mutex::scoped_lock l(lock); 
		typename unordered_set<Key>::iterator it = values.find(k);
		if(it != values.end())
		{
			return it->second;
		}
		values.insert(k);
		return k;
	}
	bool remove(const Key&k)
	{
		boost::mutex::scoped_lock l(lock); 
		typename unordered_set<Key>::iterator it = values.find(k);
		if(it == values.end())
		{
			return false;
		}
		values.erase(it);
		return true;
	}
	bool containsKey(const Key&k)
	{
		boost::mutex::scoped_lock l(lock); 
		typename unordered_set<Key>::iterator it = values.find(k);
		if(it == values.end())
		{
			return false;
		}
		return true;
	}
	long size()
	{
		boost::mutex::scoped_lock l(lock); 
		return values.size();
	}
private:
	mutex                      lock;
	unordered_set<Key>         values;
};


#endif 

