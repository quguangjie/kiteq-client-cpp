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

#ifndef  __ATOMICINTEGER_HPP
#define  __ATOMICINTEGER_HPP

#pragma once
#include <boost/thread.hpp>

template <class T>
class Atomic
{
public:
    Atomic()
        : val()
    {
    }

    Atomic(T val_)
        : val(val_)
    {
    }

    virtual ~Atomic(){}

    void set(T val_)
    {
        boost::mutex::scoped_lock lock(mtx);
        val = val_;
    }

    T get()
    {
        boost::mutex::scoped_lock lock(mtx);
        return val;
    }

    bool updateTo(T update)
    {
        boost::mutex::scoped_lock lock(mtx);
        if (val != update)
        {
            val = update;
            return true;
        }
        return false;           
    }

    bool compareAndSet(T expect, T update)
    {
        boost::mutex::scoped_lock lock(mtx);
        if (val == expect)
        {
            val = update;
            return true;
        }
        return false;
    }

    T getAndSet(T update)
    {
        boost::mutex::scoped_lock lock(mtx);
        T tmp = val;
        val = update;
        return tmp;
    }

protected:

    T val;
    boost::mutex mtx;
};

class AtomicInteger
    : public Atomic<int>
{
public:
	const static int MAX_VALUE  = 0x7FFFFFFF;

    AtomicInteger()
        : Atomic<int>(0)
    {
    }

    AtomicInteger(int val_)
        : Atomic<int>(val_)
    {
    }

    bool operator==(int comp)
    {
        boost::mutex::scoped_lock lock(mtx);
        return (val == comp);
    }

    int operator++(int)
    {
        boost::mutex::scoped_lock lock(mtx);
        return (val++);
    }

    int operator++()
    {
        boost::mutex::scoped_lock lock(mtx);
        return (++val);
    }

    int operator--(int)
    {
        boost::mutex::scoped_lock lock(mtx);
        return (val--);
    }

    int operator--()
    {
        boost::mutex::scoped_lock lock(mtx);
        return (--val);
    }

    void operator+=(int delta)
    {
        boost::mutex::scoped_lock lock(mtx);
        val += delta;           
    }

    void operator-=(int delta)
    {
        boost::mutex::scoped_lock lock(mtx);
        val -= delta;           
    }
	int getAndIncrement()
	{
        boost::mutex::scoped_lock lock(mtx);
        return (++val);
	}
	int compareAndSet(int oldval, int newval)
	{
        boost::mutex::scoped_lock lock(mtx);
		if(oldval == val)
		{
			val = newval;
			return newval;
		}
		return oldval;	
	}
};

#endif 



