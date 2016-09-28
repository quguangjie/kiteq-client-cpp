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

#include "ListenerManager.h"
#include "remotingListener.hpp"

void ListenerManager::registerListener(const string &channel, shared_ptr<remotingListener> listener) {
	mutex::scoped_lock lock(maplock);
	listeners.insert(make_pair(channel, listener));
}

void ListenerManager::unregisterListener(const string &channel) {
	mutex::scoped_lock lock(maplock);
	listeners.erase(channel);
}

shared_ptr<remotingListener> ListenerManager::getListener(const string &channel) {
	mutex::scoped_lock lock(maplock);
	map<string, shared_ptr<remotingListener> >::iterator  it = listeners.find(channel);
	if(it == listeners.end())
		return defaultListener;
	return it->second;
}
shared_ptr<remotingListener> ListenerManager::defaultListener = shared_ptr<remotingListener>(new remotingListener());
map<string, shared_ptr<remotingListener> > ListenerManager::listeners;
mutex ListenerManager::maplock;


