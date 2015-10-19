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

#ifndef  __SENDRESULT_HPP
#define  __SENDRESULT_HPP

#include <string>

class SendResult {
public: 
	SendResult() {
    	success = true;
	}
    
    bool isSuccess() {
        return success;
    }
    
    void setSuccess(bool b) {
        success = b;
    }
    
    const string &getMessageId() {
        return messageId;
    }
    
    void setMessageId(const string &mId) {
        messageId = mId;
    }
    
    const string &getErrorMessage() {
        return errorMessage;
    }
    
    void setErrorMessage(const string &eMessage) {
        errorMessage = eMessage;
    }

private:
    bool success;
    string messageId;
    string errorMessage;
};


#endif 
