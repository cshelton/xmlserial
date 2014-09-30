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

#ifndef XMLSERIAL_MULTIMAP_H
#define XMLSERIAL_MULTIMAP_H

#include <multimap>
#include "xmlserial.h"
#include "xmlserial_pair.h"

namespace XMLSERIALNAMESPACE {
	template<typename K,typename T,typename C, typename A>
	struct TypeInfo<std::multimap<K,T,C,A>, void> {
		inline static const char *namestr() { return "multimap"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &,const std::multimap<K,T,C,A> &,S &) { }
		inline static bool isshort(const std::multimap<K,T,C,A> &) { return false; }
		inline static bool isinline(const std::multimap<K,T,C,A> &) { return false; }
		template<typename S>
		inline static void save(const std::multimap<K,T,C,A> m,
				S &os,int indent) {
			os << std::endl;
			int c=0;
			for(typename std::multimap<K,T,C,A>::const_iterator i=m.begin();
					i!=m.end();++i,++c) {
				XMLTagInfo fields;
				SaveWrapper(*i,fields,os,indent+1,"key","value");
			}
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(std::multimap<K,T,C,A> &m, const XMLTagInfo &info,
				S &is) {
			m.clear();
			XMLTagInfo eleminfo;
			int i=0;
			while(1) {
				ReadTag(is,eleminfo);
				if (eleminfo.isend && !eleminfo.isstart) {
					if (eleminfo.name == namestr()) return;
					throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+namestr()+", received end tag for "+eleminfo.name);
				}
				std::pair<K,T> elem;
				TypeInfo<std::pair<K,T> >::load(elem,eleminfo,is,"key","value");
#if __cplusplus > 199711L
					// not as efficient as I would like
				m.insert(elem); // copying elements here
#else
				// might need to be fixed if moving is not possible!
				m.emplace_back(std::move(elem)); // moving here
#endif
			}
		}
	};
}
#endif
