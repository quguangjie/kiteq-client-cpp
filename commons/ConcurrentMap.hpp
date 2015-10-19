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

#ifndef   __CONCURRENTMAP_HPP
#define   __CONCURRENTMAP_HPP

#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

using namespace std;
using namespace boost;

template<class Key, class Mapped>
class ConcurrentMap
{
public:
	ConcurrentMap()
	{
	}
	~ConcurrentMap()
	{
		boost::mutex::scoped_lock lock(maplock); 
		values.clear();
	}
	const Mapped & putIfAbsent(const Key&k, const Mapped&v)
	{
		boost::mutex::scoped_lock lock(maplock); 
		typename boost::unordered_map<Key, Mapped>::iterator it = values.find(k);
		if(it != values.end())
		{
			return it->second;
		}
		values.insert(make_pair(k, v));
		return v;
	}
	bool remove(const Key&k, Mapped & v)
	{
		boost::mutex::scoped_lock lock(maplock); 
		typename unordered_map<Key, Mapped>::iterator it = values.find(k);
		if(it == values.end())
		{
			return false;
		}
		values.erase(it);
	 	v = it->second;
		return true;
	}
	bool remove(const Key&k)
	{
		boost::mutex::scoped_lock lock(maplock); 
		typename unordered_map<Key, Mapped>::iterator it = values.find(k);
		if(it == values.end())
		{
			return false;
		}
		values.erase(it);
		return true;
	}
	bool get(const Key&k, Mapped & v)
	{
		boost::mutex::scoped_lock lock(maplock); 
		typename unordered_map<Key, Mapped>::iterator it = values.find(k);
		if(it == values.end())
		{
			return false;
		}
	 	v = it->second;
		return true;
	}
	bool containsKey(const Key&k)
	{
		boost::mutex::scoped_lock lock(maplock); 
		typename unordered_map<Key, Mapped>::iterator it = values.find(k);
		if(it == values.end())
		{
			return false;
		}
		return true;
	}
private:
	mutex                      maplock;
	unordered_map<Key, Mapped> values;
};


#endif 

