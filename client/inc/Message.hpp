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

#ifndef   __KMESSAGE_HPP  
#define   __KMESSAGE_HPP  
#include <string>
#include "protocol.h"

class Message 
{
public: 
    static const int  BODY_TYPE_STRING = 1;
    static const int  BODY_TYPE_BYTES= 2;
    
    const Header& getHeader() {
        return header;
    }
    
    void setHeader(const Header &h) {
        header = h;
    }
    
    bool isStringMessage() {
        return bodyType == BODY_TYPE_STRING;
    }
    
    bool isBytesMessage() {
        return bodyType == BODY_TYPE_BYTES;
    }
    
    void setBodyType(int b) {
        bodyType = b;
    }
    
    const string &getBodyString() {
        return bodyString;
    }
    
    void setBodyString(const string &b) {
        bodyString = b;
    }
    
    const string & getBodyBytes() {
        return bodyBytes;
    }
    
    void setBodyBytes(const string& b) {
        bodyBytes = b;
    }
    
private:
    Header header;
    int    bodyType;
    string bodyString;
    string bodyBytes;
};

#endif 

