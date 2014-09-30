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

#ifndef XMLSERIAL_STACK_H
#define XMLSERIAL_STACK_H

#include <stack>
#include "xmlserial.h"
#include "xmlserial_getcontainer.h"

namespace XMLSERIALNAMESPACE {

	template<typename T, typename C>
	struct TypeInfo<std::stack<T,C>, void> {
		inline static const char *namestr() { return "stack"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &,const std::stack<T,C> &,S &) { }
		inline static bool isshort(const std::stack<T,C> &) { return false; }
		inline static bool isinline(const std::stack<T,C> &) { return false; }
		template<typename S>
		inline static void save(const std::stack<T,C> s, S &os,int indent) {
			os << std::endl;
			XMLTagInfo fields;
			SaveWrapper(getcontainer<std::stack<T,C> >::get(s),fields,os,indent+1);
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(std::stack<T,C> &s, const XMLTagInfo &info,
				S &is) {
			XMLTagInfo eleminfo;
			ReadTag(is,eleminfo);
			LoadWrapper(getcontainer<std::stack<T,C> >::get(s),eleminfo,is);
			ReadEndTag(is,namestr());
		}
	};
}
#endif
