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

#ifndef __BINDING_HPP
#define __BINDING_HPP
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include "cJSON.h"

using namespace std;
using namespace boost;
class Binding 
{
public: 
    static const int BIND_DIRECT = 0;
    static const int BIND_REGX = 0;
    static const int BIND_FANOUT = 0;
    
    
    const string &getGroupId() const {
        return groupId;
    }
    
    void setGroupId(const string &gId) {
        groupId = gId;
    }
    
    const string &getTopic() const {
        return topic;
    }
    
    void setTopic(const string &s) {
        topic = s;
    }
    
    const string &getMessageType() {
        return messageType;
    }
    
    void setMessageType(const string &mType) {
        messageType = mType;
    }
    
    int getBindType() {
        return bindType;
    }
    
    void setBindType(int bType) {
        bindType = bType;
    }
    
    const string &getVersion() {
        return version;
    }
    
    void setVersion(const string &v) {
        version = v;
    }
    
    int getWatermark() {
        return watermark;
    }
    void setWatermark(int mark) {
        watermark = mark;
    }
    
    bool isPersistent() {
        return persistent;
    }
    
    void setPersistent(bool p) {
        persistent = p;
    }
    
    static shared_ptr<Binding> bindDirect(const string &groupId, 
			                  const string &topic, 
							  const string &messageType, 
							  int watermark, bool persistent) {
        shared_ptr<Binding> b (new Binding());
        b->setGroupId(groupId);
        b->setTopic(topic);
        b->setMessageType(messageType);
        b->setWatermark(watermark);
        b->setPersistent(persistent);
        b->setBindType(BIND_DIRECT);
        return b;
    }
	static bool toJson(const list<shared_ptr<Binding> >&bindings, string &strout)
	{
        cJSON *root = NULL, *array = NULL, *item = NULL;
        char * str = NULL;
		do {
            root = cJSON_CreateObject();
            array = cJSON_CreateArray();
            cJSON_AddItemToObject (root, "bindings", array);

			std::list<shared_ptr<Binding> >::const_iterator it;

			for (it = bindings.begin(); it != bindings.end(); it++) 
			{
                item = cJSON_CreateObject();
                cJSON_AddItemToArray (array, item);

                cJSON_AddStringToObject (array, "groupId", (*it)->groupId.c_str());
			    cJSON_AddStringToObject (array, "topic", (*it)->topic.c_str());
			    cJSON_AddStringToObject (array, "messageType", (*it)->messageType.c_str());
			    cJSON_AddStringToObject (array, "version", (*it)->version.c_str());
                cJSON_AddNumberToObject (array, "bindType", (*it)->bindType);
			    cJSON_AddNumberToObject (array, "watermark", (*it)->watermark);

			    if ((*it)->persistent == cJSON_True) {
                    cJSON_AddTrueToObject (array, "persistent");
				} else {
					cJSON_AddFalseToObject (array, "persistent");
				} 
			}
		} while(0);
	    str = cJSON_Print(root);
	    strout.insert(0, str);	
        if (str) free(str);
        if (root) cJSON_Delete(root);
		return true;
	}
private:
    string groupId;
    string topic;
    string messageType;
    string version;
    int bindType;
    int watermark;
    bool persistent;
}; 
#endif 
