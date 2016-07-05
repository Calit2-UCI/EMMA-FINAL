/*
 * Array.h
 *
 * Copyright (C) 1999 Stephen F. White
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file "COPYING" for details); if 
 * not, write to the Free Software Foundation, Inc., 675 Mass Ave, 
 * Cambridge, MA 02139, USA.
 */

#ifndef _ARRAY_H
#define _ARRAY_H

#define DEFAULT_CAPACITY 8

#ifdef IRIX
#pragma set woff 3303
#endif

#include <assert.h>

template<class T> 
class Array {
public:
		Array(int capacity = DEFAULT_CAPACITY )
		{ _capacity = capacity; _data = new T[capacity]; _size = 0; }
		Array(const Array<T> &a)
		{ _capacity = _size = a._size; _data = new T[_capacity];
		  for (int i = 0; i < _size; i++) _data[i] = a._data[i];
		}
		Array(const T *a, int len)
		{ _capacity = _size = len; _data = new T[len];
		  for (int i = 0; i < len; i++) _data[i] = *(a + i);
		}
    	       ~Array()
	       	{ delete[] _data; }

    const T    &get(int index) const
		{ return _data[index]; }
    void	set(int index, T t)
		{ if (index >= _size) resize(index+1); _data[index] = t; }
    T	       &operator[](int index)
    		{ if (index >= _size) resize(index+1); return _data[index]; }
    const T    &operator[](int index) const
    		{ return _data[index]; }
    const T    *getData() const { return _data; }
    int		size() const
    		{ return _size; }
    void	append(T t)
    		{ (*this)[_size] = t; }
    void	remove(int pos) {
		    for (int i = pos; i < _size - 1; i++)
			_data[i] = _data[i + 1];
		    _size--;
		}
    void	remove(int start, int end) {
		    assert(start >= 0 && start < _size);
		    assert(end >= 0 && end < _size);
		    int len = end - start + 1;
	       	for (int i = start; i < (_size-len); i++) _data[i] = _data[i + len];
		    _size -= len;
		}
    void	resize(int size) {
		    if (_capacity < size) {
			while (_capacity < size)
			    _capacity <<= 1;
			T *newData = new T[_capacity];
			for (int i = 0; i < _size; i++)
			    newData[i] = _data[i];
			delete[] _data;
			_data = newData;
		    }
		    _size = size;
		}
    int		find(const T t) const {
		    for (int i = 0; i < _size; i++)
			if (_data[i] == t) return i;
		    return -1;
		}
	
protected:
    int		_size;
    int		_capacity;
    T	       *_data;

};

#endif // ARRAY_H
