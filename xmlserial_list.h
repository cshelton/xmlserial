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

#ifndef XMLSERIAL_LIST_H
#define XMLSERIAL_LIST_H

#include <list>
#include <sstream>
#include "xmlserial.h"

namespace XMLSERIALNAMESPACE {
	// if T is not "shiftable"
	template<typename T,typename A>
	struct TypeInfo<std::list<T,A>,
	typename Type_If<!IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() { return "list"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields,
				const std::list<T,A> &, S &) { }
		inline static bool isshort(const std::list<T,A> &) { return false; }
		inline static bool isinline(const std::list<T,A> &) { return false; }
		template<typename S>
		inline static void save(const std::list<T,A> &l,
				S &os,int indent) {
			os << std::endl;
			int c=0;
			for(typename std::list<T,A>::const_iterator i=l.begin();
					i!=l.end();++i,++c) {
				XMLTagInfo fields;
				SaveWrapper(*i,fields,os,indent+1);
			}
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(std::list<T,A> &l, const XMLTagInfo &info,
				S &is) {
			l.clear();
			XMLTagInfo eleminfo;
			while(1) {
				ReadTag(is,eleminfo);
				if (eleminfo.isend && !eleminfo.isstart) {
					if (eleminfo.name == namestr()) {
						return;
					}
					throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+namestr()+", received end tag for "+eleminfo.name);
				}
#if _cplusplus <= 199711L
				l.push_back(T());
#else
				l.emplace_back();
#endif
				LoadWrapper(l.back(),eleminfo,is);
			}
		}
	};

	// If T is "shiftable"
	template<typename T,typename A>
	struct TypeInfo<std::list<T,A>,
				typename Type_If<IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() {
			static char *ret = TName("list",1,TypeInfo<T>::namestr());
			return ret;
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const std::list<T,A> &l, S &os) {
			fields.attr["nelem"] = T2str(l.size());
		}
		inline static bool isshort(const std::list<T,A> &) { return false; }
		inline static bool isinline(const std::list<T,A> &) { return false; }
		template<typename S>
		inline static void save(const std::list<T,A> &l,
				S &os, int indent) {
			for(typename std::list<T,A>::const_iterator i=l.begin();i!=l.end();++i)
				os << *i << ' ';
		}
		template<typename S>
		inline static void load(std::list<T,A> &l, const XMLTagInfo &info,
				S &is) {
			std::map<std::string,std::string>::const_iterator ni
								= info.attr.find("nelem");
			if (ni == info.attr.end())
				throw streamexception("Stream Input Format Error: list (when elements are saved with <<) needs nelem attribute");
			int n = atoi(ni->second.c_str());

			for(int i=0;i<n;i++) {
#if _cplusplus <= 199711L
				l.push_back(T());
#else
                    l.emplace_back();
#endif
				is >> l.back();
			}
			ReadEndTag(is,namestr());
		}
	};

}
#endif
