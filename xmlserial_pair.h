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

#include "xmlserial.h"

namespace XMLSERIALNAMESPACE {
	// If T1 or T2 is not "quickly shiftable"
	template<typename T1,typename T2>
	struct TypeInfo<std::pair<T1,T2>,
				typename Type_If<!IsShiftable<T1>::quickly
						|| !IsShiftable<T2>::quickly,void>::type> {
		inline static const char *namestr() { return "pair"; }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &,
			const std::pair<T1,T2> &, S &,
			const char * fn = nullptr, const char * sn = nullptr) {
		}
		inline static bool isshort(const std::pair<T1,T2> &) { return false; }
		inline static bool isinline(const std::pair<T1,T2> &) { return false; }
		template<typename S>
		inline static void save(const std::pair<T1,T2> &p,
				S &os,int indent,
				const char *firstname= "first",
				const char *secondname="second") {
			os << std::endl;
			XMLTagInfo firstfields;
			firstfields.attr["name"] = firstname;
			SaveWrapper(p.first,firstfields,os,indent+1);
			XMLTagInfo secondfields;
			secondfields.attr["name"] = secondname;
			SaveWrapper(p.second,secondfields,os,indent+1);
			Indent(os,indent);
		}

		class PName {
			public:
				PName(std::pair<T1,T2> &pa,const char *n1, const char *n2)
					: p(pa), s1(n1), s2(n2) {}
				std::pair<T1,T2> &p;
				const char *s1,*s2;
		};

		struct Get1 {
			inline static T1 &getvalue(PName &o)
			{ return o.p.first; }
			inline static const char *getname(PName &o)
			{ return o.s1; }
		};
		struct Get2 {
			inline static T2 &getvalue(PName &o)
			{ return o.p.second; }
			inline static const char *getname(PName &o)
			{ return o.s2; }
		};

		typedef List<Get1,List<Get2,ListEnd> > PList;

		template<typename S>
		inline static void load(std::pair<T1,T2> &p,
				const XMLTagInfo &info, S &is,
				const char *firstname = "first",
				const char *secondname = "second") {
			PName pn(p,firstname,secondname);
			LoadList<PList>::exec(pn,is,"pair"); \
		}
	};

	// If T1 and T2 are "quickly shiftable"
	template<typename T1,typename T2>
	struct TypeInfo<std::pair<T1,T2>,
				typename Type_If<IsShiftable<T1>::quickly
						&& IsShiftable<T2>::quickly,void>::type> {
		inline static const char *namestr() {
			static std::string ret = std::string("pair.")
				+TypeInfo<T1>::namestr()
				+"."+TypeInfo<T2>::namestr();
			return ret.c_str();
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields,
						const std::pair<T1,T2> &p, S &os,
				const char *firstname="first",
				const char *secondname="second") {
			std::ostringstream ss1;
			dupfmt(ss1,os);
			ss1 << p.first;
			std::ostringstream ss2;
			dupfmt(ss2,os);
			ss2 << p.second;
			fields.attr[firstname] = ss1.str();
			fields.attr[secondname] = ss2.str();
		}
		inline static bool isshort(const std::pair<T1,T2> &) { return false; }
		inline static bool isinline(const std::pair<T1,T2> &) { return true; }
		template<typename S>
		inline static void save(const std::pair<T1,T2> &p,
				S &os,int indent,
				const char *firstname= "first",
				const char *secondname="second") {
		}

		template<typename S>
		inline static void load(std::pair<T1,T2> &p,
				const XMLTagInfo &info, S &is,
				const char *firstname = "first",
				const char *secondname = "second") {
			std::map<std::string,std::string>::const_iterator vi
				=info.attr.find(firstname);
			if (vi!=info.attr.end()) {
				std::istringstream ss(vi->second);
				ss.copyfmt(is);
				ss >> p.first;
			} else
				throw streamexception(std::string("Stream Input Format Error: missing attribute ")+firstname+" in tag "+info.name);
			vi = info.attr.find(secondname);
			if (vi!=info.attr.end()) {
				std::istringstream ss(vi->second);
				ss.copyfmt(is);
				ss >> p.second;
			} else
				throw streamexception(std::string("Stream Input Format Error: missing attribute ")+secondname+" in tag "+info.name);
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: tag ")+info.name+" has non-empty contents");
			}
		}
	};
}
#endif
