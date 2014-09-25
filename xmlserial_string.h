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

#ifndef XMLSERIAL_STRING_H
#define XMLSERIAL_STRING_H

#include "xmlserial.h"

namespace XMLSERIALNAMESPACE {
	template<>
	struct TypeInfo<std::string,void> {
		inline static const char *namestr() { return "string"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &,const std::string &,
				S &) { }
		inline static bool isshort(const std::string &s) { return s.length()<20; }
		inline static bool isinline(const std::string &s) { return false; }
		template<typename S>
		inline static void save(const std::string &s, S &os,
				int indent) {
			WriteStr(os,s,isshort(s));
		}
		template<typename S>
		inline static void load(std::string &s, const XMLTagInfo &info,
				S &is) {
			std::map<std::string,std::string>::const_iterator vi
				=info.attr.find("value");
			if (vi!=info.attr.end()) {
				std::istringstream ss(vi->second);
				dupfmt(ss,is);
				ReadStr(ss,s,"");
				if (info.isend) return;
			} else ReadStr(is,s,"<");
			ReadEndTag(is,namestr());
		}
	};

}
#endif
