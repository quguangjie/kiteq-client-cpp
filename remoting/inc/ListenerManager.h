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

#ifndef __LISTENERMANAGER_HPP
#define __LISTENERMANAGER_HPP

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

using namespace std; 
using namespace boost; 

class remotingListener;
class ListenerManager 
{
public:    
    static void registerListener(const string &channel, shared_ptr<remotingListener> listener);
    static void unregisterListener(const string &channel);
    static shared_ptr<remotingListener> getListener(const string &channel);
public:
    static map<string, shared_ptr<remotingListener> > listeners;
private:
    static shared_ptr<remotingListener> defaultListener;
	static mutex    maplock;
}; 

#endif 


