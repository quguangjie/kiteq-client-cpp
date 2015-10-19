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

#ifndef  __HOSTPORT_HPP
#define  __HOSTPORT_HPP

#include <string>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

class HostPort  {
public:
    HostPort(const string &h, const string &p) {
        host = h;
        port = p;
    }

    const string &getHost() {
        return host;
    }

    void setHost(const string &h) {
        host = h;
    }

    const string& getPort() {
        return port;
    }

    void setPort(const string& p) {
        port = p;
    }

    /**
     * Ping host:port to check its alive.
     */
    void ping() throw (std::exception) {
    }

    bool isAvailable() {
        try {
            ping();
            return true;
        } catch (std::exception &e) {
            return false;
        }
    }

    /**
     * @param hostport
     *            192.168.1.1:6359
     */
   static shared_ptr<HostPort> parse(const string &hostport) {
		vector<string> parts;
		algorithm::split(parts, hostport, algorithm::is_any_of(":")); 
		if(parts.size() != 2)
			return shared_ptr<HostPort>(); 
		string h = parts.front().size() ? parts.front() : "127.0.0.1";
		string p = parts.back().size() ? parts.back() : "13800";
        return shared_ptr<HostPort>(new HostPort(h, p));
    }
    
    string toString() {
		string s(host);
		s.append(":");
		s.append(lexical_cast<string>(port));
        return s; 
    }
private:
    string host;
    string port;
}; 


#endif 
