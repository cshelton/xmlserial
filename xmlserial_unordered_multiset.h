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

#ifndef XMLSERIAL_UNORDERED_MULTISET_H
#define XMLSERIAL_UNORDERED_MULTISET_H

#include <unordered_multiset>
#include "xmlserial.h"

namespace XMLSERIALNAMESPACE {
	// for T not being "shiftable"
	template<typename T,typename C,typename A>
	struct TypeInfo<std::unordered_multiset<T,C,A>,
					typename Type_If<!IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() { return "unordered_multiset"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &,const std::unordered_multiset<T,C,A> &,
				S &) { }
		inline static bool isshort(const std::unordered_multiset<T,C,A> &) { return false; }
		inline static bool isinline(const std::unordered_multiset<T,C,A> &) { return false; }
		template<typename S>
		inline static void save(const std::unordered_multiset<T,C,A> &s,
				S &os,int indent) {
			os << std::endl;
			int c=0;
			for(typename std::unordered_multiset<T,C,A>::const_iterator i=s.begin();
					i!=s.end();++i,++c) {
				XMLTagInfo fields;
				SaveWrapper(*i,fields,os,indent+1);
			}
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(std::unordered_multiset<T,C,A> &s, const XMLTagInfo &info,
				S &is) {
			s.clear();
			XMLTagInfo eleminfo;
			int i=0;
			while(1) {
				ReadTag(is,eleminfo);
				if (eleminfo.isend && !eleminfo.isstart) {
					if (eleminfo.name == namestr()) return;
					throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+namestr()+", received end tag for "+eleminfo.name);
				}
				T temp;
				LoadWrapper(temp,eleminfo,is);
				s.insert(temp);
			}
		}
	};

	// for T being "shiftable"
	template<typename T,typename C,typename A>
	struct TypeInfo<std::unordered_multiset<T,C,A>,
					typename Type_If<IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() {
			static char *ret = TName("unordered_multiset",1,TypeInfo<T>::namestr());
			return ret;
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields,
					const std::unordered_multiset<T,C,A> &v, S &os) {
			fields.attr[nelem] = T2str(v.size());
		}
		inline static bool isshort(const std::unordered_multiset<T,C,A> &) { return false; }
		inline static bool isinline(const std::unordered_multiset<T,C,A> &) { return false; }
		template<typename S>
		inline static void save(const std::unordered_multiset<T,C,A> &s,
				S &os, int indent) {
			for(typename std::unordered_multiset<T,C,A>::const_iterator i=s.begin();i!=s.end();++i)
				os << *i << ' ';
		}
		template<typename S>
		inline static void load(std::unordered_multiset<T,C,A> &s, const XMLTagInfo &info,
				S &is) {
			std::map<std::string,std::string>::const_iterator ni
				= info.attr.find("nelem");
			if (ni == info.attr.end())
				throw streamexception("Stream Input Format Error: ivector needs nelem attribute");
			int n = atoi(ni->second.c_str());
			s.clear();
			for(int i=0;i<n;i++) {
				T temp;
				is >> temp;
				s.insert(temp);
			}
			ReadEndTag(is,namestr());
		}
	};

}
#endif
