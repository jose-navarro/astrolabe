/*
 * C++ file streams for XTL
 *
 * Copyright (C) 2000 Lutz Latta, lutz@latta.de
 */
/* XTL - eXternalization Template Library - http://gsd.di.uminho.pt/~jop/xtl
 * Copyright (C) 1998-2000 Jose' Orlando Pereira, Universidade do Minho
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 *
 * $Id: iostrbuf.h,v 1.2 2005/03/01 16:41:23 philgrim Exp $
 */

#include <iostream>
#include <stdexcept>

// buffer classes to c++ istream, ostream and iostream
class istream_buffer {
 private:
	std::istream &stream;
	char *buffer;
	int buf_size;

	inline void buffer_require(int size) {
		if (buf_size < size) {
			if (buffer != 0)
				delete [] buffer;
			buffer = new char[size];
			buf_size = size;
		}
	}

	inline void read(char* ptr, int size) {
		stream.read(ptr, size);
		if (stream.fail())
			throw std::out_of_range("istream_buffer: read error");
	}

 public:
	istream_buffer(std::istream &str) : stream(str), buffer(0), buf_size(0) {}
	~istream_buffer() {
		delete [] buffer;
	}

	inline void* require(int size) {
		buffer_require(size);
		read(buffer, size);
		return buffer;
	}

	inline void unrequire(int n) {
		stream.seekg(-n);
	}
};

class ostream_buffer {
 private:
	std::ostream &stream;
	char *buffer;
	int buf_size;
	int write_bytes;

	inline void buffer_require(int size) {
		if (buf_size < size) {
			if (buffer != 0)
				delete [] buffer;
			buffer = new char[size];
			buf_size = size;
		}
	}

	inline void flush_buffer() {
		if (write_bytes > 0) {
			stream.write(buffer, write_bytes);
			if (stream.fail())
				throw std::out_of_range("iostream_buffer: write error");
			write_bytes = 0;
		}
	}

	inline void write(char const* ptr, int size) {
		flush_buffer();
		stream.write(ptr, size);
		if (stream.fail())
			throw std::out_of_range("ostream_buffer: write error");
	}

 public:
	ostream_buffer(std::ostream &str) : stream(str), buffer(0), buf_size(0),
			write_bytes(0) {}
	~ostream_buffer() {
		flush_buffer();
		delete [] buffer;
	}

	inline void* desire(int size) {
		flush_buffer();
		buffer_require(size);
		write_bytes = size;
		return buffer;
	}

	inline void flush() {
		flush_buffer();
		stream.flush();
	}

	inline void undesire(int n) {
		flush_buffer();
		stream.seekp(-n);
	}

	inline void rewind() {
		flush_buffer();
		stream.seekp(0, std::ostream::beg);
	}
};

