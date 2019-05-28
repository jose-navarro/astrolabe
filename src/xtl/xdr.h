/*
 * XDR format driver for XTL
 *
 * Copyright (C) 1998-2000 Jose' Orlando Pereira, jop@di.uminho.pt
 * Copyright (C) 2000 Angus Leeming, a.leeming@ic.ac.uk
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
 * $Id: xdr.h,v 1.3 2005/04/14 15:28:37 keithsnively Exp $
 */

#ifndef __XTL_XDR
#define __XTL_XDR

#include "config.h"

// data is stored with big endian ordering (XDR standard)
// this must be global due to a joint g++/glibc/i386 "feature"
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
inline void _xtl_big_end(char const in[], char out[]) {
	*reinterpret_cast<unsigned int*>(out) =
	  bswap_32(*reinterpret_cast<const unsigned int*>(in));
}
#define LOW 1
#define HIGH 0
#elif (__BYTE_ORDER == __BIG_ENDIAN)
inline void _xtl_big_end(char const in[], char out[]) {
 	*reinterpret_cast<unsigned int*>(out) =
	  *reinterpret_cast<const unsigned int*>(in);
}
#define LOW 0
#define HIGH 1
#endif
  
// Macros to keep things neat and tidy in class XDR_format.
// All data is stored in 32 bit chunks (XDR standard), those
// types longer than 32 bits being accessed through a union to avoid
// "unaligned access errors" on 64 bit machines.
#define def_input_simple_i(type1, type2) \
	void input_simple(type1& data) { \
		type2 store; \
		_xtl_big_end( reinterpret_cast<char*>( this->xtl_require(4) ), \
			 reinterpret_cast<char*>( &store ) ); \
		data = static_cast<type1>( store ); \
	} 

#define def_input_simple_ll(type1, type2) \
	void input_simple(type1& data) { \
		union { type2 ll; int i[2]; } store; \
		_xtl_big_end( reinterpret_cast<char*>( this->xtl_require(4) ), \
			 reinterpret_cast<char*>( &store.i[LOW] ) ); \
		_xtl_big_end( reinterpret_cast<char*>( this->xtl_require(4) ), \
		         reinterpret_cast<char*>( &store.i[HIGH] ) ); \
		data = static_cast<type1>( store.ll ); \
	}

#define def_output_simple_i(type1, type2) \
 	void output_simple(type1 const& data) { \
		type2 store = static_cast<type2>( data ); \
		_xtl_big_end( reinterpret_cast<char*>( &store ), \
			 reinterpret_cast<char*>( this->xtl_desire(4) ) ); \
	}

#define def_output_simple_ll(type1, type2) \
 	void output_simple(type1 const& data) { \
		union { type2 ll; int i[2]; } store; \
		store.ll = static_cast<type2>( data ); \
		_xtl_big_end( reinterpret_cast<char*>( &store.i[LOW] ), \
			 reinterpret_cast<char*>( this->xtl_desire(4) ) ); \
		_xtl_big_end( reinterpret_cast<char*>( &store.i[HIGH] ), \
			 reinterpret_cast<char*>( this->xtl_desire(4) ) ); \
	}

template <class Buffer>
class XDR_format: public generic_format<Buffer> {
 private:
 public:
	typedef Buffer buffer;

	XDR_format(Buffer& buf):generic_format<Buffer>(buf) {}
        virtual ~XDR_format(){}  // Added by Pep Navarro.
	
	template <class Idx>
	void input_start_array(Idx& n) {input_simple(n);}
	template <class Idx>
	bool input_end_array(Idx& n) {return n--<=0;}

	def_input_simple_i(bool, int)
	def_input_simple_i(char, int)
	def_input_simple_i(unsigned char, int)
	def_input_simple_i(short, int)
	def_input_simple_i(unsigned short, int)
	def_input_simple_i(int, int)
	def_input_simple_i(unsigned int, int)
	def_input_simple_i(long, int)
	def_input_simple_i(unsigned long, int)
	def_input_simple_ll(longlong, longlong)
	def_input_simple_ll(unsignedlonglong, longlong)
	def_input_simple_i(float, float)
	def_input_simple_ll(double, double)

	void input_chars(char* data, int size) {
		input_raw(data, size);
	}

	// This routine is identical to that in GIOP_format
	void input_raw(char* data, int size) {
		int i;
		for(i=0;i<(size>>8)-1;i++,data+=256)
			std::memcpy(data, this->xtl_require(256), 256);
		int res=size-(i<<8);
		std::memcpy(data, this->xtl_require(res), res);
		if (res%4!=0)
			this->xtl_require(4-res%4);
	}

	template <class Idx>
	void output_start_array(Idx n) {output_simple(n);}
	void output_end_array() {}

	def_output_simple_i(bool, int)
	def_output_simple_i(char, int)
	def_output_simple_i(unsigned char, int)
	def_output_simple_i(short, int)
	def_output_simple_i(unsigned short, int)
	def_output_simple_i(int, int)
	def_output_simple_i(unsigned int, int)
	def_output_simple_i(long, int)
	def_output_simple_i(unsigned long, int)
	def_output_simple_ll(longlong, longlong)
	def_output_simple_ll(unsignedlonglong, longlong)
	def_output_simple_i(float, float)
	def_output_simple_ll(double, double)

	void output_chars(char const* data, int size) {
		output_raw(data, size);
	}

	// This routine is identical to that in GIOP_format
	void output_raw(char const* data, int size) {
		int i;
		for(i=0;i<(size>>8)-1;i++,data+=256)
			std::memcpy(this->xtl_desire(256), data, 256);
		int res=size-(i<<8);
		std::memcpy(this->xtl_desire(res), data, res);
		if (res%4!=0)
			std::memset(this->xtl_desire(4-res%4), 0, 4-res%4);
	}
};

#undef def_input_simple_i
#undef def_input_simple_ll
#undef def_output_simple_i
#undef def_output_simple_ll
#undef LOW
#undef HIGH

#endif
