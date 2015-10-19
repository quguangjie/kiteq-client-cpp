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

#ifndef  __KITESTATS_HPP
#define  __KITESTATS_HPP
#include <boost/serialization/singleton.hpp>
#include "Counter.hpp"

using namespace boost;
using boost::serialization::singleton;

class KiteStats 
{
public:    
    static void recordRead() {
        readCounter.inc();
    }
    
    static void recordWrite() {
        writeCounter.inc();
    }
    
    static void start() 
	{
    }
    
    static void close() {
    }
private:
    static Counter readCounter;
    static Counter writeCounter;
}; 

#endif 


