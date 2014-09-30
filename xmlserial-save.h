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

#ifndef XMLSERIAL_H
#define XMLSERIAL_H

#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <typeinfo>
#include <cstdarg>
#include <utility>
#include <map>
#include <set>
#include <string.h>


#ifndef XMLSERIALNAMESPACE
#define XMLSERIALNAMESPACE xmlserial
#endif

// pick how to add a value to a struct (enum or static int const)
#define XMLSERIAL_DECVAL(vname,expr) \
	enum { vname = (expr) };
	//static int const vname = (expr);
	
#if _cplusplus <= 199711L
#define nullptr (0)
#endif

#include "xmlserial_tmp.h"
#include "xmlserial_ptrs.h"
#include "xmlserial_archive.h"

namespace XMLSERIALNAMESPACE {


	// can be thrown by the "Load" method
	class streamexception : public std::exception {
	public:
		streamexception(std::string txt) throw() : expl(txt) { }
		streamexception(const streamexception &e) throw()
		{ expl = e.expl; }
		streamexception &operator=(const streamexception &e) throw() 
		{ if (this != &e) expl = e.expl; return *this; }
		virtual ~streamexception() throw() { }
		virtual const char *what() const throw() { return expl.c_str(); }
	private:
		std::string expl;
	};

	// from s2 to s1!
	template<typename S1, typename S2>
	void dupfmt(S1 &s1, S2 &s2) {
		s1.flags(s2.flags());
		s1.precision(s2.precision());
		s1.width(s2.width());
		s1.imbue(s2.getloc());
	}

}

/* Here starts "helper" macros: */

#define XMLSERIAL_GETTERCOMMON(expr,cexpr,vtype,vname) \
			typedef vtype valtype; \
			static inline valtype &getvalue(XMLSERIAL_BASETYPE *o) \
				{ return expr; } \
			static inline const char *getname(XMLSERIAL_BASETYPE *o) \
				{ return vname; } \
			\
			static inline valtype const &getvalue(const XMLSERIAL_BASETYPE *o) \
				{ return cexpr; } \
			static inline const char *getname(const XMLSERIAL_BASETYPE *o) \
				{ return vname; } \

#define XMLSERIAL_GETTERSTRUCT(expr,cexpr,vtype,vname) \
		struct { XMLSERIAL_GETTERCOMMON(expr,cexpr,vtype,vname) }

#define XMLSERIAL_GETTERSTRUCT_DEFAULT(expr,cexpr,vtype,vname,dval) \
		struct { \
			XMLSERIAL_GETTERCOMMON(expr,cexpr,vtype,vname) \
			static inline void setdefault(vtype &v) { v = dval; } \
		} 

#define XMLSERIAL_NOTV_COMMON \
	friend struct XMLSERIALNAMESPACE::IsEmpty<XMLSERIAL_BASETYPE,void>; \
	private: \
		template<typename XMLSERIAL_T,bool XMLSERIAL_B> \
		struct xmlserial_vallocT { \
			const static bool valid = !XMLSERIAL_B; \
			typedef XMLSERIAL_BASETYPE rettype; \
			typedef XMLSERIALNAMESPACE::basefactory<rettype> createtype; \
			typedef std::map<std::string,createtype*> allocmaptype; \
			template<typename PTRT> \
			inline static void allocbyname(const std::string &name, \
					PTRT &ret) { \
				XMLSERIALNAMESPACE::PtrInfo<PTRT>::setnull(ret); \
			} \
			inline static allocmaptype &alloctable() { \
				static allocmaptype table; \
				return table; \
			} \
			inline static const char * \
					addalloc(const char *name, createtype *fn) \
				{ return name; } \
		}; \

#define XMLSERIAL_P_COMMON(myname) \
	friend struct XMLSERIALNAMESPACE::IsEmpty<XMLSERIAL_BASETYPE,void>; \
	private: \
		template<typename XMLSERIAL_T, bool XMLSERIAL_B> \
		struct xmlserial_vallocT { \
			const static bool valid = XMLSERIAL_B; \
			typedef XMLSERIAL_BASETYPE rettype; \
			typedef XMLSERIALNAMESPACE::basefactory<rettype> createtype; \
			typedef std::map<std::string,createtype*> allocmaptype; \
			template<typename PTRT> \
			inline static void allocbyname(const std::string &name, \
					PTRT &ret) { \
				typename allocmaptype::iterator i = alloctable().find(name); \
				if (i==alloctable().end()) \
					XMLSERIALNAMESPACE::PtrInfo<PTRT>::setnull(ret); \
				else ret = XMLSERIALNAMESPACE::PtrInfo<PTRT>::cast(i->second->valloc(PTRT())); \
			} \
			inline static allocmaptype &alloctable() { \
				static allocmaptype table; \
				return table; \
			} \
			inline static const char * \
					addalloc(const char *name, createtype *fn) { \
				alloctable()[std::string(name)] = fn; \
				return name; \
			} \
		}; \
	public: \
		virtual void SaveV(std::ostream &os, \
				XMLSERIALNAMESPACE::XMLTagInfo &fields, \
				int indent=0) const { \
			Save(os,fields,indent); \
		} \
		virtual void SaveV(XMLSERIALNAMESPACE::archive &oa, \
				XMLSERIALNAMESPACE::XMLTagInfo &fields, \
				int indent=0) const { \
			Save(oa,fields,indent); \
		} \
		template<typename S> \
		inline static XMLSERIAL_BASETYPE *LoadV(S &is) { \
			XMLSERIAL_BASETYPE *ret; XMLSERIALNAMESPACE::LoadWrapper(ret,is); \
			return ret; \
		} \
		virtual void xmlserial_loadwrapv(std::istream &is, \
					const XMLSERIALNAMESPACE::XMLTagInfo &info) { \
			XMLSERIALNAMESPACE::LoadWrapper(*this,info,is); \
		} \
		virtual void xmlserial_loadwrapv(XMLSERIALNAMESPACE::archive &ia, \
					const XMLSERIALNAMESPACE::XMLTagInfo &info) { \
			XMLSERIALNAMESPACE::LoadWrapper(*this,info,ia); \
		}

#define XMLSERIAL_BASE_COMMON(cname) \
		xmlserial_class##cname##lst; \
		template<bool XMLSERIAL_B> \
		struct xmlserial_vallocT<typename XMLSERIALNAMESPACE::Type_If<XMLSERIAL_B && cname::xmlserial_valloc::valid,XMLSERIAL_BASETYPE>::type,XMLSERIAL_B> { \
			const static bool valid = XMLSERIAL_B; \
			typedef cname::xmlserial_valloc::rettype rettype; \
			typedef XMLSERIALNAMESPACE::basefactory<rettype> createtype; \
			template<typename PTRT> \
			inline static void allocbyname(const std::string &name, \
						PTRT &ret) { \
				cname::xmlserial_valloc::allocbyname<PTRT>(name,ret); \
			} \
			inline static const char *addalloc(const char *name, createtype *fn) \
				{ return cname::xmlserial_valloc::addalloc(name,fn); } \
		}; \

#define XMLSERIAL_NAME0(cname) \
		#cname

#define XMLSERIAL_NAME1(cname,tname1) \
		XMLSERIALNAMESPACE::TName(#cname,1, \
					XMLSERIALNAMESPACE::TypeInfo<tname1>::namestr())

#define XMLSERIAL_NAME2(cname,tname1,tname2) \
		XMLSERIALNAMESPACE::TName(#cname,2, \
					XMLSERIALNAMESPACE::TypeInfo<tname1>::namestr(), \
					XMLSERIALNAMESPACE::TypeInfo<tname2>::namestr())

#define XMLSERIAL_NAME3(cname,tname1,tname2,tname3) \
		XMLSERIALNAMESPACE::TName(#cname,3, \
					XMLSERIALNAMESPACE::TypeInfo<tname1>::namestr(), \
					XMLSERIALNAMESPACE::TypeInfo<tname2>::namestr(), \
					XMLSERIALNAMESPACE::TypeInfo<tname3>::namestr())

#define XMLSERIAL_NAME4(cname,tname1,tname2,tname3,tname4) \
		XMLSERIALNAMESPACE::TName(#cname,4, \
					XMLSERIALNAMESPACE::TypeInfo<tname1>::namestr(), \
					XMLSERIALNAMESPACE::TypeInfo<tname2>::namestr(), \
					XMLSERIALNAMESPACE::TypeInfo<tname3>::namestr(), \
					XMLSERIALNAMESPACE::TypeInfo<tname4>::namestr())

#define XMLSERIAL_ID0(cname) \
	static const char *xmlserial_IDname() { \
		static const char *ret = XMLSERIAL_NAME0(cname); \
		return ret; \
	} \

#define XMLSERIAL_ID1(cname,tname1) \
	static const char *xmlserial_IDname() { \
		static const char *ret = XMLSERIAL_NAME1(cname,tname1); \
		return ret; \
	}

#define XMLSERIAL_ID2(cname,tname1,tname2) \
	static const char *xmlserial_IDname() { \
		static const char *ret = XMLSERIAL_NAME2(cname,tname1,tname2); \
		return ret; \
	}

#define XMLSERIAL_ID3(cname,tname1,tname2,tname3) \
	static const char *xmlserial_IDname() { \
		static const char *ret = XMLSERIAL_NAME3(cname,tname1,tname2,tname3); \
		return ret; \
	}

#define XMLSERIAL_ID4(cname,tname1,tname2,tname3,tname4) \
	static const char *xmlserial_IDname() { \
		static const char *ret = \
			XMLSERIAL_NAME4(cname,tname1,tname2,tname3,tname4); \
		return ret; \
	}

#define XMLSERIAL_SNAME0(cname) \
	static const char *xmlserial_shiftname() { \
		static const char *ret = XMLSERIAL_NAME0(cname); \
		return ret; \
	} \

#define XMLSERIAL_SNAME1(cname,tname1) \
	static const char *xmlserial_shiftname() { \
		static const char *ret = XMLSERIAL_NAME1(cname,tname1); \
		return ret; \
	}

#define XMLSERIAL_SNAME2(cname,tname1,tname2) \
	static const char *xmlserial_shiftname() { \
		static const char *ret = XMLSERIAL_NAME2(cname,tname1,tname2); \
		return ret; \
	}

#define XMLSERIAL_SNAME3(cname,tname1,tname2,tname3) \
	static const char *xmlserial_shiftname() { \
		static const char *ret = XMLSERIAL_NAME3(cname,tname1,tname2,tname3); \
		return ret; \
	}

#define XMLSERIAL_SNAME4(cname,tname1,tname2,tname3,tname4) \
	static const char *xmlserial_shiftname() { \
		static const char *ret = \
			XMLSERIAL_NAME4(cname,tname1,tname2,tname3,tname4); \
		return ret; \
	}


#define XMLSERIAL_ID0_P(cname) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial_valloc::addalloc( \
			XMLSERIAL_NAME0(cname), \
			new XMLSERIALNAMESPACE::factory<XMLSERIAL_BASETYPE,xmlserial_valloc::rettype>()); \
		return ret; \
	} \

#define XMLSERIAL_ID1_P(cname,tname1) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial_valloc::addalloc( \
			XMLSERIAL_NAME1(cname,tname1), \
			new XMLSERIALNAMESPACE::factory<XMLSERIAL_BASETYPE,xmlserial_valloc::rettype>()); \
		return ret; \
	}

#define XMLSERIAL_ID2_P(cname,tname1,tname2) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial_valloc::addalloc( \
			XMLSERIAL_NAME2(cname,tname1,tname2), \
			new XMLSERIALNAMESPACE::factory<XMLSERIAL_BASETYPE,xmlserial_valloc::rettype>()); \
		return ret; \
	}

#define XMLSERIAL_ID3_P(cname,tname1,tname2,tname3) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial_valloc::addalloc( \
			XMLSERIAL_NAME3(cname,tname1,tname2,tname3), \
			new XMLSERIALNAMESPACE::factory<XMLSERIAL_BASETYPE,xmlserial_valloc::rettype>()); \
		return ret; \
	}

#define XMLSERIAL_ID4_P(cname,tname1,tname2,tname3,tname4) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial_valloc::addalloc( \
			XMLSERIAL_NAME4(cname,tname1,tname2,tname3,tname4), \
			new XMLSERIALNAMESPACE::factory<XMLSERIAL_BASETYPE,xmlserial_valloc::rettype>()); \
		return ret; \
	}

/* Here starts "USESHIFT" macros: */

#define XMLSERIAL_USESHIFT(cname) \
	private: \
		typedef cname XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME0(cname) \
		enum { XMLSERIAL_ISSHORT = false };
		//inline static bool xmlserial_isshort() { return false; }

#define XMLSERIAL_USESHIFT_SHORT(cname) \
	private: \
		typedef cname XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME0(cname) \
		enum { XMLSERIAL_ISSHORT = true };
		//inline static bool xmlserial_isshort() { return true; }

#define XMLSERIAL_USESHIFT1(cname,tname1) \
	private: \
		typedef cname<tname1> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME1(cname,tname1) \
		inline static bool xmlserial_isshort() { return false; }

#define XMLSERIAL_USESHIFT1_SHORT(cname) \
	private: \
		typedef cname<tname1> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME1(cname,tname1) \
		inline static bool xmlserial_isshort() { return true; }

#define XMLSERIAL_USESHIFT2(cname,tname1,tname2) \
	private: \
		typedef cname<tname1,tname2> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME2(cname,tname1,tname2) \
		inline static bool xmlserial_isshort() { return false; }

#define XMLSERIAL_USESHIFT2_SHORT(cname,tname1,tname2) \
	private: \
		typedef cname<tname1,tname2> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME2(cname,tname1,tname2) \
		inline static bool xmlserial_isshort() { return true; }

#define XMLSERIAL_USESHIFT3(cname) \
	private: \
		typedef cname<tname1,tname2,tname3> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME3(cname,tname1,tname2,tname3) \
		inline static bool xmlserial_isshort() { return false; }

#define XMLSERIAL_USESHIFT3_SHORT(cname,tname1,tname2,tname3) \
	private: \
		typedef cname<tname1,tname2,tname3> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME3(cname,tname1,tname2,tname3) \
		inline static bool xmlserial_isshort() { return true; }

#define XMLSERIAL_USESHIFT4(cname,tname1,tname2,tname3,tname4) \
	private: \
		typedef cname<tname1,tname2,tname3,tname4> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME4(cname,tname1,tname2,tname3,tname4) \
		inline static bool xmlserial_isshort() { return false; }

#define XMLSERIAL_USESHIFT4_SHORT(cname) \
	private: \
		typedef cname<tname1,tname2,tname3,tname4> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME4(cname,tname1,tname2,tname3,tname4) \
		inline static bool xmlserial_isshort() { return true; }

/* Here starts "START" macros: */

#define XMLSERIAL_START(cname) \
	private: \
		typedef cname XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_ID0(cname) \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd 

#define XMLSERIAL_START1(cname,tname) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_ID1(cname,tname) \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START2(cname,tname1,tname2) \
	private: \
		typedef cname<tname1,tname2> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_ID2(cname,tname1,tname2) \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START3(cname,tname1,tname2,tname3) \
	private: \
		typedef cname<tname1,tname2,tname3> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_ID3(cname,tname1,tname2,tname3) \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START4(cname,tname1,tname2,tname3,tname4) \
	private: \
		typedef cname<tname1,tname2,tname3,tname4> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_ID4(cname,tname1,tname2,tname3,tname4) \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

/* Here starts "START_P" macros: */

#define XMLSERIAL_START_P(cname) \
	private: \
		typedef cname XMLSERIAL_BASETYPE; \
		XMLSERIAL_P_COMMON(cname) \
	public: \
		XMLSERIAL_ID0_P(cname); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START1_P(cname,tname) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_P_COMMON(cname) \
	public: \
		XMLSERIAL_ID1_P(cname,tname); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START2_P(cname,tname1,tname2) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_P_COMMON(cname) \
	public: \
		XMLSERIAL_ID2_P(cname,tname1,tname2); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START3_P(cname,tname1,tname2,tname3) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_P_COMMON(cname) \
	public: \
		XMLSERIAL_ID3_P(cname,tname1,tname2,tname3); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START4_P(cname,tname1,tname2,tname3,tname4) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_P_COMMON(cname) \
	public: \
		XMLSERIAL_ID4_P(cname,tname1,tname2,tname3,tname4); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

/* Here starts "VAR" macros */

#define XMLSERIAL_VAR(vtype,vname) \
		xmlserial_##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT((o->vname),(o->vname),vtype,#vname) \
			xmlserial_get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial_get##vname,xmlserial_##vname##lst>

#define XMLSERIAL_VAR_DEFAULT(vtype,vname,val) \
		xmlserial_##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((o->vname),(o->vname),vtype,#vname,val) \
			xmlserial_get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial_get##vname,xmlserial_##vname##lst>

#define XMLSERIAL_VAR_N(vtype,vname,sname) \
		xmlserial_##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT((o->vname),(o->vname),vtype,sname) \
			xmlserial_get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial_get##vname,xmlserial_##vname##lst>

#define XMLSERIAL_VAR_DEFAULT_N(vtype,vname,val,sname) \
		xmlserial_##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((o->vname),(o->vname),vtype,sname,val) \
			xmlserial_get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial_get##vname,xmlserial_##vname##lst>

/* Here starts "BASE" macros */

#define XMLSERIAL_BASE(cname) \
		XMLSERIAL_BASE_COMMON(cname) \
		typedef XMLSERIAL_GETTERSTRUCT((*(static_cast<cname *>(o))),*(static_cast<const cname *>(o)),cname,#cname) \
			xmlserial_getclass##cname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial_getclass##cname,xmlserial_class##cname##lst>
			
			
#define XMLSERIAL_BASE_DEFAULT(cname,val) \
	XMLSERIAL_BASE_COMMON(cname) \
	typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((*(static_cast<cname *>(o))), \
		*(static_cast<const cname *>(o)),cname,#cname,val) \
		xmlserial_getclass##cname; \
	typedef XMLSERIALNAMESPACE::List<xmlserial_getclass##cname, \
		xmlserial_class##cname##lst>
			
#define XMLSERIAL_BASE_N(cname,sname) \
		XMLSERIAL_BASE_COMMON(cname) \
		typedef XMLSERIAL_GETTERSTRUCT((*(static_cast<cname *>(o))),*(static_cast<const cname *>(o)),cname,sname) \
			xmlserial_getclass##cname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial_getclass##cname,xmlserial_class##cname##lst>
			
			
#define XMLSERIAL_BASE_DEFAULT_N(cname,val,sname) \
	XMLSERIAL_BASE_COMMON(cname) \
	typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((*(static_cast<cname *>(o))), \
		*(static_cast<const cname *>(o)),cname,sname,val) \
		xmlserial_getclass##cname; \
	typedef XMLSERIALNAMESPACE::List<xmlserial_getclass##cname, \
		xmlserial_class##cname##lst>
			

/* Here starts "ENSURECLASS" macros */

#define ENSURECLASS(cname) \
	namespace { static const char *namefor_##cname=cname::xmlserial_IDname(); }

#define ENSURECLASS1(cname,tname1) \
	namespace { static const char *namefor_##cname##tname1 = \
		cname<tname1>::xmlserial_IDname(); }

#define ENSURECLASS2(cname,tname1,tname2) \
	namespace { static const char *namefor_##cname##tname1##tname2 = \
		cname<tname1,tname2>::xmlserial_IDname(); }

#define ENSURECLASS3(cname,tname1,tname2,tname3) \
	namespace { \
		static const char *namefor_##cname##tname1##tname2##tname3 = \
			cname<tname1,tname2,tname3>::xmlserial_IDname(); \
	}

#define ENSURECLASS4(cname,tname1,tname2,tname3,tname4) \
	namespace { \
		static const char *namefor_##cname##tname1##tname2##tname3##tname4 \
			= cname<tname1,tname2,tname3,tname4>::xmlserial_IDname(); \
	}

/* Here starts "END" macros (only 1, actually) */

#define XMLSERIAL_END \
        xmlserial_tmp_alllist; \
    public: \
        typedef xmlserial_tmp_alllist xmlserial_alllist; \
        typedef xmlserial_vallocT<XMLSERIAL_BASETYPE,true> xmlserial_valloc; \
        template<typename S> \
        inline void xmlserial_Save(S &os, int indent=0) const { \
            XMLSERIALNAMESPACE::SaveItt<xmlserial_alllist>::exec(this,os,indent); } \
        template<typename S> \
        inline void Save(S &os, XMLSERIALNAMESPACE::XMLTagInfo &fields, \
				int indent=0) const { \
            XMLSERIALNAMESPACE::SaveWrapper(*this,fields,os,indent); \
        } \
        template<typename S> \
        inline void Save(S &os, int indent=0) const { \
		XMLSERIALNAMESPACE::XMLTagInfo fields; \
            XMLSERIALNAMESPACE::SaveWrapper(*this,fields,os,indent); \
        } \
        template<typename S> \
        inline void xmlserial_Load(S &is) { \
            XMLSERIALNAMESPACE::LoadList<xmlserial_alllist>::exec \
                (this,is,xmlserial_IDname()); \
        } \
        template<typename S> \
        inline void Load(S &is) { \
            XMLSERIALNAMESPACE::LoadWrapper(*this,is); \
        } \
        template<typename S> \
        inline static XMLSERIAL_BASETYPE *LoadPtr(S &is) { \
            XMLSERIAL_BASETYPE *ret; \
            XMLSERIALNAMESPACE::LoadWrapper(ret,is); \
            return ret; \
        }

//--------------
// C++11 Macros
//--------------
#if __cplusplus > 199711L

#include "xmlserial_foreach.h"

#define XMLSERIAL11_START1(cname) \
		XMLSERIAL_START(cname)
#define XMLSERIAL11_START2(cname,base1) \
		XMLSERIAL_START(cname) \
		XMLSERIAL_BASE(base1)
#define XMLSERIAL11_START3(cname,base1,base2) \
		XMLSERIAL_START(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2)
#define XMLSERIAL11_START4(cname,base1,base2,base3) \
		XMLSERIAL_START(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2) \
		XMLSERIAL_BASE(base3)
#define XMLSERIAL11_START5(cname,base1,base2,base3,base4) \
		XMLSERIAL_START(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2) \
		XMLSERIAL_BASE(base3) \
		XMLSERIAL_BASE(base4)
#define XMLSERIAL11_START6(cname,base1,base2,base3,base4,base5) \
		XMLSERIAL_START(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2) \
		XMLSERIAL_BASE(base3) \
		XMLSERIAL_BASE(base4) \
		XMLSERIAL_BASE(base5)

#define XMLSERIAL11_START_P1(cname) \
		XMLSERIAL_START_P(cname)
#define XMLSERIAL11_START_P2(cname,base1) \
		XMLSERIAL_START_P(cname) \
		XMLSERIAL_BASE(base1)
#define XMLSERIAL11_START_P3(cname,base1,base2) \
		XMLSERIAL_START_P(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2)
#define XMLSERIAL11_START_P4(cname,base1,base2,base3) \
		XMLSERIAL_START_P(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2) \
		XMLSERIAL_BASE(base3)
#define XMLSERIAL11_START_P5(cname,base1,base2,base3,base4) \
		XMLSERIAL_START_P(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2) \
		XMLSERIAL_BASE(base3) \
		XMLSERIAL_BASE(base4)
#define XMLSERIAL11_START_P6(cname,base1,base2,base3,base4,base5) \
		XMLSERIAL_START_P(cname) \
		XMLSERIAL_BASE(base1) \
		XMLSERIAL_BASE(base2) \
		XMLSERIAL_BASE(base3) \
		XMLSERIAL_BASE(base4) \
		XMLSERIAL_BASE(base5)
// any more than 5 base classes and you'll have to do it yourself,
// or I'll write more later

#define XMLSERIAL11_VAR2(cname,vname) \
		XMLSERIAL_VAR(decltype(cname::vname),vname)
#define XMLSERIAL11_VAR3(cname,vname,savename) \
		XMLSERIAL_VAR_N(decltype(cname::vname),vname,savename)
#define XMLSERIAL11_VAR4(cname,vname,savename,defaultval) \
		XMLSERIAL_VAR_DEFAULT_N(decltype(cname::vname),vname,defaultval,savename)
#define XMLSERIAL11_VAR5(cname,vname,savename,defaultval,vtype) \
		XMLSERIAL_VAR_DEFAULT_N(vtype,vname,defaultval,savename)

#define XMLSERIAL11_CNAME(classes) \
		CPPX_INVOKE_N2(XMLSERIAL11_CNAME_I,(STRIPPAREN(classes)))
#define XMLSERIAL11_CNAME_I(classname,...) classname

#define XMLSERIAL11_VAR(cname,vardecl) \
	CALLVAR_N2(XMLSERIAL11_VAR,cname,STRIPPAREN(vardecl))

#define XMLSERIALCLASS(classes,...) \
	CALLVAR_N2(XMLSERIAL11_START,STRIPPAREN(classes)) \
	FOREACHARG(XMLSERIAL11_VAR,XMLSERIAL11_CNAME(classes),__VA_ARGS__) \
	XMLSERIAL_END

#define XMLSERIALCLASS_P(classes,...) \
	CALLVAR_N2(XMLSERIAL11_START_P,STRIPPAREN(classes)) \
	FOREACHARG(XMLSERIAL11_VAR,XMLSERIAL11_CNAME(classes),__VA_ARGS__) \
	XMLSERIAL_END

#endif

//----------------
// implementation 
//----------------


namespace XMLSERIALNAMESPACE {
	template<typename S>
	void WriteStr(S &os, const std::string &s,bool escape=true);

	// helpful formatting fn
	template<typename S>
	void Indent(S &os, int indent) {
		for(int i=0;i<indent;i++) os << "\t";
	}

	// information about XML tags
	struct XMLTagInfo {
		std::string name;
		std::map<std::string,std::string> attr;
		bool isstart,isend;

		template<typename S>
		void write(S &os, int indent) const {
			if (isstart) {
				Indent(os,indent);
				os << "<" << name;
				for(std::map<std::string,std::string>::const_iterator i
							= attr.begin(); i!=attr.end();++i) {
					os << ' ';
					WriteStr(os,i->first);
					os << "=\"";
					WriteStr(os,i->second);
					os << "\"";
				}
				if (isend) os << " \\>" << std::endl;
				else os << ">";
			} else {
				os << "<\\" << name << ">" << std::endl;
			}
		}
	};

	// forward decls
	template<typename G> struct SaveItem;
	template<typename L> struct LoadList;
	template<typename S>
	void ReadTag(S &is,XMLTagInfo &info);
	template<typename S>
	void ReadEndTag(S &is,const char *ename);
	template<typename S>
	void ReadStr(S &is, std::string &ret,const char *endchar);


	template<typename S>
	void IgnoreWS(S &is) {
		while(1) {
			char c = is.peek();
			if (is.fail() || !isspace(c)) return;
			is.get();
		}
	}


	template<typename S>
	char ReadEscChar(S &is) {
		char c=is.get();
		switch(c) {
			case '\\': return '\\';
			case 'a': return '\a';
			case 'b': return '\b';
			case 'f': return '\f';
			case 'n': return '\n';
			case 'r': return '\r';
			case 't': return '\t';
			case 'v': return '\v';
			default: return c;
		}
	}

	template<typename S>
	bool ConsumeToken(S &is, const char *tok) {
		int i;
		for(i=0;tok[i]!=0 && tok[i]==is.get();i++)
			;
		if (tok[i]==0) return true;
		for(;i>=0;i--)
			is.unget();
		return false;
	}

	template<typename S>
	char ReadAmpChar(S &is) {
		if (ConsumeToken(is,"quot;")) return '"';
		if (ConsumeToken(is,"amp;")) return '&';
		if (ConsumeToken(is,"apos;")) return '\'';
		if (ConsumeToken(is,"lt;")) return '<';
		if (ConsumeToken(is,"gt;")) return '>';
		return '&';
	}

	template<typename S>
	void ReadToken(S &is, std::string &ret) {
		ret.clear();
		IgnoreWS(is);
		bool isquoted = is.peek()=='"';
		if (isquoted) is.get();
		ReadStr(is,ret,isquoted ? "\"" : " \t\n\r\v>\\=");
		if (isquoted) is.get();
	}

	inline bool charin(const char *endchar,char c) {
		return strchr(endchar,c) != nullptr;
	}

	template<typename S>
	void ReadStr(S &is, std::string &ret,const char *endchar) {
		ret.clear();
		while(char c=is.peek()) {
			if (charin(endchar,c)) return;
			is.get();
			if (is.fail()) return;
			if (c=='\\') c = ReadEscChar(is);
			else if (c=='&') c = ReadAmpChar(is);
			ret.push_back(c);
		}
	}

	template<typename S>
	void WriteStr(S &os, const std::string &s,bool escape) {
		for(std::string::const_iterator i=s.begin();i!=s.end();++i) {
			switch(*i) {
				case '"':
				    os << "&quot;";
				    break;
				case '&':
				    os << "&amp;";
				    break;
				case '\'':
				    os << "&apos;";
				    break;
				case '<':
				    os << "&lt;";
				    break;
				case '>':
				    os << "&gt;";
				    break;
				case '\\':
				    os << "\\\\";
				    break;
				case '\a':
				    (escape ? (os << '\\' << 'a') : (os << '\a'));
					break;
				case '\b':
				    (escape ? (os << '\\' << 'b') : (os << '\b'));
					break;
				case '\f':
				    (escape ? (os << '\\' << 'f') : (os << '\f'));
					break;
				case '\n':
				    (escape ? (os << '\\' << 'n') : (os << '\n'));
					break;
				case '\r':
				    (escape ? (os << '\\' << 'r') : (os << '\r'));
					break;
				case '\t':
				    (escape ? (os << '\\' << 't') : (os << '\t'));
					break;
				case '\v':
				    (escape ? (os << '\\' << 'v') : (os << '\v'));
					break;
				default:
				    os << *i;
				    break;
			}
		}
	}


	template<typename S>
	void ReadTag(S &is,XMLTagInfo &info) {
		info.attr.clear();
		IgnoreWS(is);
		char c = is.get();
		if (is.fail() || c!='<') throw streamexception("Stream Input Format Error:  expected <");
		if (is.peek()=='\\') {
			info.isstart=false;
			info.isend=true;
			is.get();
		} else {
			info.isstart=true;
			info.isend=false;
		}
		ReadToken(is,info.name);
		IgnoreWS(is);
		if (info.isend) {
			if (is.get()=='>') return;
			throw streamexception("Stream Input Format Error:  expected >");
		}
		while(!is.fail()) {
			char c = is.peek();
			if (c=='\\') {
				is.get();
				info.isend=true;
				if (is.get()=='>') return;
				throw streamexception("Stream Input Format Error:  expected >");
			}
			if (c=='>') {
				is.get();
				return;
			}
			std::string aname;
			ReadToken(is,aname);
			if (aname.empty()) 
				throw streamexception("Stream Input Format Error: tag missing name");
			IgnoreWS(is);
			if (is.peek()=='=') {
				is.get();
				std::string aval;
				ReadToken(is,aval);
				info.attr[aname] = aval;
			} else {
				info.attr[aname] = "1";
			}
			IgnoreWS(is);
		}
		throw streamexception("Stream Input Format Error: unexpected stream end");
	}

	template<typename S>
	void ReadEndTag(S &is,const char *ename) {
	    XMLTagInfo einfo;
		ReadTag(is,einfo);
		if (!einfo.isend || einfo.name!=ename)
			throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+ename+", received "+(einfo.isend ? "end" : "start")+" tag for "+einfo.name);
	}

	// How to construct a name for a template class
	inline char *TName(const char *cname, int n, ...) {
		int s = strlen(cname)+1;
		va_list ap;
		va_start(ap,n);
		for(int i=0;i<n;i++)
			s += strlen(va_arg(ap,const char *))+1;
		va_end(ap);
		char *ret = new char[s];
		strcpy(ret,cname);
		va_start(ap,n);
		for(int i=0;i<n;i++) {
			strcat(ret, "."); // slightly inefficient to keep scanning
			strcat(ret, va_arg(ap,const char *)); // for the end, but much
				// more readable
		}
		return ret;
	}

	// whether a list (as defined in xmlserial_tmp.h) is empty
	template<typename L>
	struct ListEmpty {
		XMLSERIAL_DECVAL(value,false)
	};

	// whether T does not have anything to be saved
	template<typename T,typename Condition=void>
	struct IsEmpty {
		XMLSERIAL_DECVAL(value,false)
	};

	template<typename T>
	struct IsEmpty<T,typename Type_If<ListEmpty<typename T::xmlserial_alllist>::value,void>::type> {
		XMLSERIAL_DECVAL(value,true)
	};

	template<>
	struct ListEmpty<ListEnd> {
		XMLSERIAL_DECVAL(value,true)
	};

	template<typename H, typename T>
	struct ListEmpty<List<H,T> > {
		XMLSERIAL_DECVAL(value,
			IsEmpty<typename H::valtype>::value && ListEmpty<T>::value)
	};






	// a reverse iterator on a list for saving
	// runs SaveItem on each element in list
	template<typename L,typename Condition=void>
	struct SaveItt {
		template<typename O, typename S>
		inline static void exec(O o,S &os,int indent) { }
	};

	template<typename H, typename T>
	struct SaveItt<List<H,T>,
	typename Type_If<!IsEmpty<typename H::valtype>::value,void>::type> {
		template<typename O, typename S>
		inline static void exec(O o,S &os,int indent) {
				SaveItt<T>::exec(o,os,indent);
				SaveItem<H>::exec(o,os,indent);
		}
	};

	template<typename H, typename T>
	struct SaveItt<List<H,T>,
			typename Type_If<IsEmpty<typename H::valtype>::value,
											void>::type> {
		template<typename O, typename S>
		inline static void exec(O o,S &os,int indent) {
			SaveItt<T>::exec(o,os,indent);
		}
	};

	template<>
	struct SaveItt<ListEnd,void> {
		template<typename O, typename S>
		inline static void exec(O o,S &os, int indent) { }
	};


	// Given an XMLTagInfo, find relevant member and call LoadWrapper
	template<typename L>
	struct LoadOne {
		template<typename O, typename S>
		inline static bool exec(O o,S &, const XMLTagInfo &) {
			return false;
		}
	};

	template<>
	struct LoadOne<ListEnd> {
		template<typename O,typename S>
		inline static bool exec(O o,S &, const XMLTagInfo &) {
			return false;
		}
	};

	template<typename H, typename T>
	struct LoadOne<List<H,T> > {
		template<typename O,typename S>
		inline static bool exec(O o,S &is,
				const XMLTagInfo &info) {
			std::map<std::string,std::string>::const_iterator ni
				=info.attr.find("name");
			if (ni!=info.attr.end() && ni->second == H::getname(o)) {
				LoadWrapper(H::getvalue(o),info,is);
				return true;
			} else
				return LoadOne<T>::exec(o,is,info);
		}
	};

	// have all of the objects in L been loaded (nset gives names that have
	//   been loaded)
	template<typename L,typename Condition=void>
	struct AllLoaded {
	template<typename O>
		inline static bool exec(O o,const std::set<std::string> &nset) {
			return true;
		}
	};

	template<>
	struct AllLoaded<ListEnd,void> {
		template<typename O>
		inline static bool exec(O o,const std::set<std::string> &nset) {
			return true;
		}
	};

	template<typename H, typename T>
	struct AllLoaded<List<H,T>,
				typename Type_If<TypeProp<H>::HasDefault,void>::type > {
		template<typename O>
		inline static bool exec(O o,const std::set<std::string> &nset) {
			if (nset.find(H::getname(o)) == nset.end())
				H::setdefault(H::getvalue(o));
			return AllLoaded<T>::exec(o,nset);
		}
	};

	template<typename H, typename T>
	struct AllLoaded<List<H,T>,
	typename Type_If<!TypeProp<H>::HasDefault
			&& IsEmpty<typename H::valtype>::value,void>::type > {
		template<typename O>
		inline static bool exec(O o,const std::set<std::string> &nset) {
			return AllLoaded<T>::exec(o,nset);
		}
	};

	template<typename H, typename T>
	struct AllLoaded<List<H,T>,
				typename Type_If<!TypeProp<H>::HasDefault
					&& !IsEmpty<typename H::valtype>::value,void>::type > {
		template<typename O>
		inline static bool exec(O o,const std::set<std::string> &nset) {
			if (nset.find(H::getname(o)) == nset.end()) return false;
			else return AllLoaded<T>::exec(o,nset);
		}
	};


	// can T be saved by << and loaded with >> ?
	template<typename T,typename Condition=void>
	struct IsShiftable {
		XMLSERIAL_DECVAL(atall,false)
		XMLSERIAL_DECVAL(quickly,false)
	};

	template<typename T> // take care of const case
	struct IsShiftable<const T,void> {
		XMLSERIAL_DECVAL(atall,IsShiftable<T>::atall)
		XMLSERIAL_DECVAL(quickly,IsShiftable<T>::quickly)
	};

	template<typename T>
	struct IsShiftable<T,typename Type_If<TypeProp<T>::execHasShift,void>::type> {
		XMLSERIAL_DECVAL(atall,true)
		XMLSERIAL_DECVAL(quickly,T::XMLSERIAL_ISSHORT)
	};

// macro to automatically define "IsShiftable" and basic type info
// for "base types" (like int, double, char)
#define XMLSERIAL_BASICTYPE(tname,strname) \
	template<> \
	struct IsShiftable<tname,void> { \
		XMLSERIAL_DECVAL(atall,true) \
		XMLSERIAL_DECVAL(quickly,true) \
	}; \
	template<> \
	struct TypeInfo<tname,void> { \
		inline static const char *namestr() { return #strname; } \
		template<typename S> \
		inline static void addotherattr(XMLTagInfo &fields, const tname &, \
				S &) {}  \
		inline static bool isshort(const tname &) { return true; } \
		inline static bool isinline(const tname &) { return false; } \
		template<typename S> \
		inline static void save(const tname &t,S &os, int indent) { \
			os << t; \
		} \
		template<typename S> \
		inline static void load(tname &t, const XMLTagInfo &info, S &is) {\
			std::map<std::string,std::string>::const_iterator vi  \
			=info.attr.find("value"); \
			if (vi!=info.attr.end()) { \
				std::istringstream ss(vi->second); \
				dupfmt(ss,is); \
				ss >> t; \
				if (info.isend) return; \
			} else { \
				is >> t; \
			} \
			ReadEndTag(is,namestr()); \
		} \
	};

	template<typename T,typename Condition=void>
	struct TypeInfo {
		enum { x = sizeof(T::_Attempting_to_serialize_type_without_serialization_information) };

		inline static const char *namestr() {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const T &,S &) { 
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		inline static bool isshort(const T &)  {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		inline static bool isinline(const T &)  {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		template<typename S>
		inline static void save(const T &t,S &os, int indent) {
			throw streamexception("Streaming Error: save called for unknown class");
		}
		template<typename S>
		inline static void load(T &t, const XMLTagInfo &info, S &is) {
			throw streamexception("Streaming Error: load called for unknown class");
		}
	};

	// These were moved upward to avoid ambiguity.
	XMLSERIAL_BASICTYPE(bool,bool)

	XMLSERIAL_BASICTYPE(char,char)
	XMLSERIAL_BASICTYPE(signed char,s_char)
	XMLSERIAL_BASICTYPE(unsigned char,u_char)
	//XMLSERIAL_BASICTYPE(wchar_t,w_char)

	XMLSERIAL_BASICTYPE(short,short)
	XMLSERIAL_BASICTYPE(unsigned short,u_short)
	XMLSERIAL_BASICTYPE(int,int)
	XMLSERIAL_BASICTYPE(unsigned int,u_int)
	XMLSERIAL_BASICTYPE(long,long)
	XMLSERIAL_BASICTYPE(unsigned long,u_long)
	XMLSERIAL_BASICTYPE(float,float)
	XMLSERIAL_BASICTYPE(double,double)
	XMLSERIAL_BASICTYPE(long double,l_double)

#if _cplusplus > 199711L
	//XMLSERIAL_BASICTYPE(char16_t,char16)
	//XMLSERIAL_BASICTYPE(char32_t,char32)
	XMLSERIAL_BASICTYPE(long long,l_long)
	XMLSERIAL_BASICTYPE(unsigned long long,u_l_long)
#endif

	template<typename T> // take care of const case...
	struct TypeInfo<const T,void> {
		inline static const char *namestr() {
			return TypeInfo<T>::namestr();
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const T &t,
						S &os) { 
			TypeInfo<T>::addotherattr(fields,t,os);
		}
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const T &t,
				S &os,
				const char *s1, const char *s2) {
			TypeInfo<T>::addotherattr(fields,t,os,s1,s2);
		}
		inline static bool isshort(const T &t)  {
			return TypeInfo<T>::isshort(t);
		}
		inline static bool isinline(const T &t)  {
			return TypeInfo<T>::isinline(t);
		}
		template<typename S>
		inline static void save(const T &t,S &os, int indent) {
			TypeInfo<T>::save(t,os,indent);
		}
		template<typename S>
		inline static void load(const T &t, const XMLTagInfo &info, S &is) {
			throw streamexception("Streaming Error: load called for constant type");
		}
	};


}

namespace XMLSERIALNAMESPACE {

	template<typename T>
	std::string T2str(const T &i) {
#if _cplusplus >199711L
	// C++11 version
		return std::to_string(i);
#else
		std::ostringstream ss;
		ss << i;
		return ss.str();
#endif
	}


	// Saving method, general
	template<typename T, typename S>
	inline typename Type_If<!PtrInfo<T>::isptr,void>::type
	SaveWrapper(const T &v,XMLTagInfo &fields, S &os,int indent) {
		fields.isstart = true;
		fields.name = TypeInfo<T>::namestr();
		fields.isend = TypeInfo<T>::isinline(v) || TypeInfo<T>::isshort(v);
		TypeInfo<T>::addotherattr(fields,v,os);
		if (TypeInfo<T>::isshort(v)) {
			std::ostringstream ss;
			dupfmt(ss,os);
			TypeInfo<T>::save(v,ss,indent);
			fields.attr["value"] = ss.str();
		}
		fields.write(os,indent);
		if (!fields.isend) {
			TypeInfo<T>::save(v,os,indent);
			fields.isstart = false;
			fields.isend = true;
			fields.write(os,indent);
		}
	}

	// Saving method, pointer, virtual, no ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& !ArchiveInfo<S>::hasptrcache
			&& TypeProp<typename PtrInfo<T>::BaseType>::HasV, void>::type
	SaveWrapper(const T &v, XMLTagInfo &fields, S &os, int indent) {
		if (PtrInfo<T>::isnull(v)) {
			fields.isstart = true;
			fields.isend = true;
			fields.name = TypeInfo<typename PtrInfo<T>::BaseType>::namestr();
			fields.attr["isnull"] = "1";
			fields.write(os,indent);
		} else PtrInfo<T>::deref_const(v).SaveV(os,fields,indent);
	}

	// Saving method, pointer, non-virtual, no ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& !ArchiveInfo<S>::hasptrcache
			&& !TypeProp<typename PtrInfo<T>::BaseType>::HasV,void>::type
	SaveWrapper(const T &v, XMLTagInfo &fields, S &os, int indent) {
		fields.attr["ptrcache"] = T2str(ArchiveInfo<S>::hasptrcache==true);
		if (PtrInfo<T>::isnull(v)) {
			fields.isstart = true;
			fields.isend = true;
			fields.name = TypeInfo<typename PtrInfo<T>::BaseType>::namestr();
			fields.attr["isnull"] = "1";
			fields.write(os,indent);
		} else SaveWrapper(PtrInfo<T>::deref_const(v),fields,os,indent);
	}

	// Saving method, pointer, virtual, ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& ArchiveInfo<S>::hasptrcache
			&& TypeProp<typename PtrInfo<T>::BaseType>::HasV, void>::type
	SaveWrapper(const T &v, XMLTagInfo &fields, S &os, int indent) {
		if (PtrInfo<T>::isnull(v)) {
			fields.isstart = true;
			fields.isend = true;
			fields.attr["isnull"] = "1";
			fields.name = TypeInfo<typename PtrInfo<T>::BaseType>::namestr();
			fields.write(os,indent);
		} else {
			int id;
			if (os.findoradd(v,id)) {
				fields.attr["ptrid"] = T2str(id);
				fields.isstart = true;
				fields.isend = true;
				fields.name = TypeInfo<typename PtrInfo<T>::BaseType>::namestr();
				fields.write(os,indent);
			} else {
				fields.attr["ptrid"] = T2str(id);
				PtrInfo<T>::deref_const(v).SaveV(os,fields,indent);
			}
		}
	}

	// Saving method, pointer, non-virtual, ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& ArchiveInfo<S>::hasptrcache
			&& !TypeProp<typename PtrInfo<T>::BaseType>::HasV,void>::type
	SaveWrapper(const T &v, XMLTagInfo &fields, S &os, int indent) {
		if (PtrInfo<T>::isnull(v)) {
			fields.isstart = true;
			fields.isend = true;
			fields.name = TypeInfo<typename PtrInfo<T>::BaseType>::namestr();
			fields.attr["isnull"] = "1";
			fields.write(os,indent);
		} else {
			int id;
			if (os.findoradd(v,id)) {
				fields.attr["ptrid"] = T2str(id);
				fields.isstart = true;
				fields.isend = true;
				fields.name = TypeInfo<typename PtrInfo<T>::BaseType>::namestr();
				fields.write(os,indent);
			} else {
				fields.attr["ptrid"] = T2str(id);
				SaveWrapper(PtrInfo<T>::deref_const(v),fields,os,indent);
			}
		}
	}

	// Saving, with extra arguments ("key" and "value" in map, e.g.)
	template<typename T, typename S>
	inline void SaveWrapper(const T &v,XMLTagInfo &fields, S &os,
				int indent, const char *e1, const char *e2) {
		TypeInfo<T>::addotherattr(fields,v,os,e1,e2);
		fields.isstart = true;
		fields.name = TypeInfo<T>::namestr();
		fields.isend = TypeInfo<T>::isinline(v);	
		fields.write(os,indent);
		if (!fields.isend) {
			TypeInfo<T>::save(v,os,indent,e1,e2);
			fields.isstart = false;
			fields.isend = true;
			fields.write(os,indent);
		}
	}


	// does info match tag expected by T?
	template<typename T>
	struct CheckTag {
		inline static void check(const XMLTagInfo &info) {
			if (!info.isstart)
				throw streamexception(std::string("Stream Input Format Error: expected start tag for ")+TypeInfo<T>::namestr()+", received end tag for "+info.name);
			if (info.name != TypeInfo<T>::namestr())
				throw streamexception(std::string("Stream Input Format Error: expected start tag for ")+TypeInfo<T>::namestr()+", received start tag for "+info.name);
		}
	};

	// general Load
	template<typename T, typename S>
	inline typename Type_If<!PtrInfo<T>::isptr,void>::type
	LoadWrapper(T &v, const XMLTagInfo &info, S &is) {
		CheckTag<T>::check(info);
		TypeInfo<T>::load(v,info,is);
	}

	// Load, pointer, virtual, no ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& !ArchiveInfo<S>::hasptrcache
			&& TypeProp<typename PtrInfo<T>::BaseType>::HasV,void>::type
	LoadWrapper(T &v, const XMLTagInfo &info, S &is) {
		if (!info.isstart)
			throw streamexception(std::string("Stream Input Format Error: expected start tag, received end tag for ")+info.name);
		std::map<std::string,std::string>::const_iterator vi
			=info.attr.find("isnull");
		if (vi!=info.attr.end() && vi->second=="1") {
			PtrInfo<T>::setnull(v);
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: null pointer object for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name+" has non-empty contents");
			}
			return;
		}
		PtrInfo<T>::BaseType::xmlserial_valloc::allocbyname(info.name,v);
		if (PtrInfo<T>::isnull(v))
			throw streamexception(std::string("Stream Input Format Error: expected start tag for subtype of ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+", received start tag for type "+info.name+" which is either unknown or not a subtype");
		PtrInfo<T>::deref(v).xmlserial_loadwrapv(is,info);
	}

	// Load, pointer, non-virtual, no ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& !ArchiveInfo<S>::hasptrcache
			&& !TypeProp<typename PtrInfo<T>::BaseType>::HasV,void>::type
	LoadWrapper(T &v, const XMLTagInfo &info, S &is) {
		std::map<std::string,std::string>::const_iterator vi
			=info.attr.find("isnull");
		if (vi!=info.attr.end() && vi->second=="1") {
			PtrInfo<T>::setnull(v);
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: null pointer object for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name+" has non-empty contents");
			}
			return;
		}
		PtrInfo<T>::allocnew(v);
		LoadWrapper(PtrInfo<T>::deref(v),info,is);
	}

	// Load, pointer, virtual, ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& ArchiveInfo<S>::hasptrcache
			&& TypeProp<typename PtrInfo<T>::BaseType>::HasV,void>::type
	LoadWrapper(T &v, const XMLTagInfo &info, S &is) {
		if (!info.isstart)
			throw streamexception(std::string("Stream Input Format Error: expected start tag, received end tag for ")+info.name);
		std::map<std::string,std::string>::const_iterator vi
			=info.attr.find("isnull");
		if (vi!=info.attr.end() && vi->second=="1") {
			PtrInfo<T>::setnull(v);
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: null pointer object for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name+" has non-empty contents");
			}
			return;
		}
		vi = info.attr.find("ptrid");
		if (vi!=info.attr.end()) {
			int id = atoi(vi->second.c_str());
			if (is.validid(id)) {
				if (!is.lookupptr(id,v))
					throw streamexception(std::string("Stream Input Format Error: pointer cannot be converted for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name);
			} else { // new pointer...
				PtrInfo<T>::BaseType::xmlserial_valloc::allocbyname(info.name,v);
				if (PtrInfo<T>::isnull(v))
					throw streamexception(std::string("Stream Input Format Error: expected start tag for subtype of ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+", received start tag for type "+info.name+" which is either unknown or not a subtype");
				if (!is.addptr(id,v))
					throw streamexception(std::string("Stream Input Format Error: pointer cannot be cached (pointer number sequence out of order?) for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name);
				PtrInfo<T>::deref(v).xmlserial_loadwrapv(is,info);
			}
		}
	}

	// Load, pointer, non-virtual, ptr cache
	template<typename T, typename S>
	inline typename Type_If<PtrInfo<T>::isptr
			&& ArchiveInfo<S>::hasptrcache
			&& !TypeProp<typename PtrInfo<T>::BaseType>::HasV,void>::type
	LoadWrapper(T &v, const XMLTagInfo &info, S &is) {
		std::map<std::string,std::string>::const_iterator vi
			=info.attr.find("isnull");
		if (vi!=info.attr.end() && vi->second=="1") {
			PtrInfo<T>::setnull(v);
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: null pointer object for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name+" has non-empty contents");
			}
			return;
		}
		vi = info.attr.find("ptrid");
		if (vi!=info.attr.end()) {
			int id = atoi(vi->second.c_str());
			if (is.validid(id)) {
				if (!is.lookupptr(id,v))
					throw streamexception(std::string("Stream Input Format Error: pointer cannot be converted for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name);
			} else { // new pointer...
				PtrInfo<T>::allocnew(v);
				if (!is.addptr(id,v))
					throw streamexception(std::string("Stream Input Format Error: pointer cannot be cached (pointer number sequence out of order?) for type ")+TypeInfo<typename PtrInfo<T>::BaseType>::namestr()+" and name "+info.name);
				LoadWrapper(PtrInfo<T>::deref(v),info,is);
			}
		}
	}

	// Load, first read tag
	template<typename T, typename S>
	inline void LoadWrapper(T &v, S &is) {
		XMLTagInfo info;
		ReadTag(is,info);
		LoadWrapper(v,info,is);
	}

	// next: SerialLoadWrap and SerialSaveWrap
	// that call preload/postload or presave/postsave if they
	// exist before/after loading/saving
	template<typename T, typename S>
	inline typename Type_If<!TypeProp<T>::HasPreLoad
						&& !TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, S &is) {
		t.xmlserial_Load(is);
	}

	template<typename T, typename S>
	inline typename Type_If<TypeProp<T>::HasPreLoad
						&& !TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, S &is) {
		t.xmlserial_preload();
		t.xmlserial_Load(is);
	}

	template<typename T, typename S>
	inline typename Type_If<!TypeProp<T>::HasPreLoad
						&& TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, S &is) {
		t.xmlserial_Load(is);
		t.xmlserial_postload();
	}

	template<typename T, typename S>
	inline typename Type_If<TypeProp<T>::HasPreLoad
						&& TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, S &is) {
		t.xmlserial_preload();
		t.xmlserial_Load(is);
		t.xmlserial_postload();
	}

	template<typename T, typename S>
	inline typename Type_If<!TypeProp<T>::HasPreSave
						&& !TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, S &os, int indent) {
		t.xmlserial_Save(os,indent);
	}

	template<typename T, typename S>
	inline typename Type_If<TypeProp<T>::HasPreSave
						&& !TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, S &os, int indent) {
		t.xmlserial_presave();
		t.xmlserial_Save(os,indent);
	}

	template<typename T, typename S>
	inline typename Type_If<!TypeProp<T>::HasPreSave
						&& TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, S &os, int indent) {
		t.xmlserial_Save(os,indent);
		t.xmlserial_postsave();
	}

	template<typename T, typename S>
	inline typename Type_If<TypeProp<T>::HasPreSave
						&& TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, S &os, int indent) {
		t.xmlserial_presave();
		t.xmlserial_Save(os,indent);
		t.xmlserial_postsave();
	}

// how to deal with ones declared by these macros
	template<typename T>
	struct TypeInfo<T, typename Type_If<TypeProp<T>::HasIDname,void>::type> {
		inline static const char *namestr() { return T::xmlserial_IDname(); }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const T &,S &) {} 
		inline static bool isshort(const T &) { return false; }
		inline static bool isinline(const T &) { return false; }
		template<typename S>
		inline static void save(const T &t,S &os,int indent) {
			os << std::endl;
			SerialSaveWrap(t,os,indent+1);
			Indent(os,indent);
		}
		template<typename S>
		inline static void load(T &t,const XMLTagInfo &info, S &is) {
			SerialLoadWrap(t,is);
		}
	};

// ... or ones that are "shiftable"
	template<typename T>
	struct TypeInfo<T, typename Type_If<TypeProp<T>::HasShift,void>::type> {
		inline static const char *namestr() { return T::xmlserial_shiftname(); }
		template<typename S>
		inline static void addotherattr(XMLTagInfo &fields, const T &,S &) {} 
		inline static bool isshort(const T &) { return T::XMLSERIAL_ISSHORT; }
		inline static bool isinline(const T &) { return false; }
		template<typename S>
		inline static void save(const T &t,S &os,int indent) {
			os << t;
		}
		template<typename S>
		inline static void load(T &t, const XMLTagInfo &info,
				S &is) {
			std::map<std::string,std::string>::const_iterator vi
				=info.attr.find("value");
			if (vi!=info.attr.end()) {
				std::istringstream ss(vi->second);
				dupfmt(ss,is);
				ss >> t;
				if (info.isend) return;
			} else {
				is >> t;
			}
			ReadEndTag(is,namestr());
		}
	};


	// how to save an item where G is a "getter" for value,type,name
	template<typename G>
	struct SaveItem {
		template<typename T, typename S>
			inline static void exec(T o,S &os,int indent) {
				XMLTagInfo fields;
				fields.attr["name"] = G::getname(o);
				SaveWrapper(G::getvalue(o),fields,os,indent);
			}
	};

	// same for Loading
	template<typename G>
	struct LoadItem {
		template<typename T, typename S>
		inline static bool exec(T o,S &is,
								const XMLTagInfo &info) {
			std::map<std::string,std::string>::const_iterator ni
					=info.attr.find("name");
			if (ni!=info.attr.end() || ni->second != G::getname(o))
				return false;
			LoadWrapper(G::getvalue(o),info,is);
				return true;
		}
	};

	// Load all of list L on object O
	template<typename L>
	struct LoadList {
		template<typename O, typename S>
		inline static void exec(O o, S &is, const char *cname) {
			XMLTagInfo info;
			std::set<std::string> loadedmem;
			while(1) {
				ReadTag(is,info);
				if (info.isend && !info.isstart) {
					if (info.name != cname)
						throw streamexception(std::string("Stream Input Format Error: expected end tag for ")+cname+", received end tag for "+info.name);
					if (!AllLoaded<L>::exec(o,loadedmem))
						throw streamexception(std::string("Stream Input Format Error: not all fields present for ")+cname);
					return;
				}
				if (LoadOne<L>::exec(o,is,info))
					loadedmem.insert(info.attr["name"]);
				else
					throw streamexception(std::string("Extra field ")+info.attr["name"]+" of type "+info.name+" in object "+cname);
			}
		}
	};


}
#undef XMLSERIAL_DECVAL
/*
#undef XMLSERIAL_GETTERCOMMON
#undef XMLSERIAL_GETTERSTRUCT
#undef XMLSERIAL_GETTERSTRUCT_DEFAULT
#undef XMLSERIAL_NOTV_COMMON
#undef XMLSERIAL_P_COMMON
#undef XMLSERIAL_NAME0
#undef XMLSERIAL_NAME1
#undef XMLSERIAL_NAME2
#undef XMLSERIAL_NAME3
#undef XMLSERIAL_NAME4
#undef XMLSERIAL_SNAME0
#undef XMLSERIAL_SNAME1
#undef XMLSERIAL_SNAME2
#undef XMLSERIAL_SNAME3
#undef XMLSERIAL_SNAME4
#undef XMLSERIAL_ID0
#undef XMLSERIAL_ID1
#undef XMLSERIAL_ID2
#undef XMLSERIAL_ID3
#undef XMLSERIAL_ID4
#undef XMLSERIAL_ID0_V
#undef XMLSERIAL_ID1_V
#undef XMLSERIAL_ID2_V
#undef XMLSERIAL_ID3_V
#undef XMLSERIAL_ID4_V
*/

#endif
