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

#ifndef XMLSERIAL_SET_H
#define XMLSERIAL_SET_H

#include <vector>
#include <sstream>
#include "xmlserial.h"

namespace XMLSERIALNAMESPACE {
	// if T is not "shiftable"
	template<typename T,typename A>
	struct TypeInfo<std::vector<T,A>,
	typename Type_If<!IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() { return "vector"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const std::vector<T,A> &v, S &os) {
			fields.attr["nelem"] = T2str(v.size());
		}
		inline static bool isshort(const std::vector<T,A> &) { return false; }
		inline static bool isinline(const std::vector<T,A> &) { return false; }
		template<typename S>
		inline static void save(const std::vector<T,A> &v,
				S &os,int indent) {
			os << std::endl;
			int c=0;
			for(typename std::vector<T,A>::const_iterator i=v.begin();
					i!=v.end();++i,++c) {
				XMLTagInfo fields;
				SaveWrapper(*i,fields,os,indent+1);
			}
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(std::vector<T,A> &v, const XMLTagInfo &info,
				S &is) {
			std::map<std::string,std::string>::const_iterator ni
				= info.attr.find("nelem");
			int n = 0;
			if (ni != info.attr.end())
				v.resize(n = atoi(ni->second.c_str()));
			else v.resize(0);
			XMLTagInfo eleminfo;
			int i=0;
			while(1) {
				ReadTag(is,eleminfo);
				if (eleminfo.isend && !eleminfo.isstart) {
					if (eleminfo.name == namestr()) {
						if (i!=n) v.resize(i);
						return;
					}
					throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+namestr()+", received end tag for "+eleminfo.name);
				}
				if (i>=n) v.resize(n=i+1);
				LoadWrapper(v[i++],eleminfo,is);
			}
		}
	};

	// If T is "shiftable"
	template<typename T,typename A>
	struct TypeInfo<std::vector<T,A>,
				typename Type_If<IsShiftable<T>::atall,void>::type> {
		inline static const char *namestr() {
			static char *ret = TName("vector",1,TypeInfo<T>::namestr());
			return ret;
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const std::vector<T,A> &v, S &os) {
			fields.attr["nelem"] = T2str(v.size());
		}
		inline static bool isshort(const std::vector<T,A> &) { return false; }
		inline static bool isinline(const std::vector<T,A> &) { return false; }
		template<typename S>
		inline static void save(const std::vector<T,A> &v,
				S &os, int indent) {
			for(typename std::vector<T,A>::const_iterator i=v.begin();i!=v.end();++i)
				os << *i << ' ';
		}
		template<typename S>
		inline static void load(std::vector<T,A> &v, const XMLTagInfo &info,
				S &is) {
			std::map<std::string,std::string>::const_iterator ni
				= info.attr.find("nelem");
			if (ni == info.attr.end())
				throw streamexception("Stream Input Format Error: ivector needs nelem attribute");
			int n = atoi(ni->second.c_str());
			v.resize(n);
			for(int i=0;i<n;i++)
				is >> v[i];
			ReadEndTag(is,namestr());
		}
	};

}
#endif
