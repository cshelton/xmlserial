/* By Christian R. Shelton
 * (christian.r.shelton@gmail.com)
 * August 2014
 *   [original release]
 *
 * Released under MIT software licence:
 * The MIT License (MIT)
 * Copyright (c) 2014 Christian R. Shelton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef XMLSERIAL_ARRAY_H
#define XMLSERIAL_ARRAY_H

#include <array>
#include <sstream>
#include "xmlserial.h"

namespace XMLSERIALNAMESPACE {
	// if T is not "shiftable"
	template<typename T,std::size_t N>
	struct TypeInfo<std::array<T,N>,
	typename Type_If<!IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() { return "array"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const std::array<T,N> &v, S &os) { }
		inline static bool isshort(const std::array<T,N> &) { return false; }
		inline static bool isinline(const std::array<T,N> &) { return false; }
		template<typename S>
		inline static void save(const std::array<T,N> &a,
				S &os,int indent) {
			os << std::endl;
			for(typename std::array<T,N>::const_iterator i=a.begin();
					i!=a.end();++i) {
				XMLTagInfo fields;
				SaveWrapper(*i,fields,os,indent+1);
			}
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(std::array<T,N> &a, const XMLTagInfo &info,
				S &is) {
			XMLTagInfo eleminfo;
			int i=0;
			while(1) {
				ReadTag(is,eleminfo);
				if (eleminfo.isend && !eleminfo.isstart) {
					if (eleminfo.name == namestr()) {
						if (i!=N) throw streamexception(std::string("Stream Input Format Error: expected ")+T2str(N)+" elements in array, received only "+T2str(i));
						return;
					}
					throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+namestr()+", received end tag for "+eleminfo.name);
				}
				LoadWrapper(a[i++],eleminfo,is);
			}
		}
	};

	// If T is "shiftable"
	template<typename T,std::size_t N>
	struct TypeInfo<std::array<T,N>,
				typename Type_If<IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() {
			static char *ret = TName("array",1,TypeInfo<T>::namestr());
			return ret;
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const std::array<T,N> &a, S &os) {
		}
		inline static bool isshort(const std::array<T,N> &) { return false; }
		inline static bool isinline(const std::array<T,N> &) { return false; }
		template<typename S>
		inline static void save(const std::array<T,N> &a,
				S &os, int indent) {
			for(typename std::array<T,N>::const_iterator i=a.begin();i!=a.end();++i)
				os << *i << ' ';
		}
		template<typename S>
		inline static void load(std::array<T,N> &a, const XMLTagInfo &info,
				S &is) {
			for(int i=0;i<N;i++)
				is >> a[i];
			ReadEndTag(is,namestr());
		}
	};

}
#endif
