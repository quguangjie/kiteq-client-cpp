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

#ifndef  __TXRESPONSE_HPP
#define  __TXRESPONSE_HPP

#include <map>
#include <list>
#include <map>
#include <string>
#include "protocol.h"


class TxResponse 
{
public:

    TxResponse() {  
		properties.clear();
	}

    TxResponse(const Header &header) {
		::google::protobuf::RepeatedPtrField< ::Entry >::iterator it;
        ::google::protobuf::RepeatedPtrField< ::Entry > propertiesList= header.properties();
        if (propertiesList.size() > 0) {
            for (it = propertiesList.begin(); it != propertiesList.end(); ++it) {
				properties.insert(make_pair(it->key(), it->value()));
            }
        }
    }
    
    const string &getMessageId() {
        return messageId;
    }
    
    void setMessageId(const string &id) {
        messageId = id;
    }
    
    int getStatus() {
        return status;
    }
    
    void setStatus(int s) {
        status = s;
    }
    
    const string &getFeedback() {
        return feedback;
    }
    
    void setFeedback(const string &fb) {
        feedback = fb;
    }
    
    void commit() {
        status = Protocol::TX_COMMIT;
    }
    
    void rollback() {
        status = Protocol::TX_ROLLBACK;
    }
    
    bool isRollback() {
        return status == Protocol::TX_ROLLBACK;
    }
    
    static shared_ptr<TxResponse> parseFrom(shared_ptr<TxACKPacket> txAck) {
        shared_ptr<TxResponse> txResponse(new TxResponse());

		::google::protobuf::RepeatedPtrField< ::Entry >::iterator it;
        ::google::protobuf::RepeatedPtrField< ::Entry > propertiesList= txAck->header().properties();
        if (propertiesList.size() > 0) {
            for (it = propertiesList.begin(); it != propertiesList.end(); ++it) {
				txResponse->properties.insert(make_pair(it->key(), it->value()));
            }
        }
        txResponse->setStatus(txAck->status());
        txResponse->setFeedback(txAck->feedback());
        return txResponse;
    }

    map<string, string> getProperties() {
        return properties;
    }
private:
    string messageId;
    int status;
    string feedback;

    map<string, string> properties;
}; 


#endif 

