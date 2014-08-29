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

#ifndef XMLSERIALNAMESPACE
#define XMLSERIALNAMESPACE xmlserial
#endif

namespace XMLSERIALNAMESPACE {

#ifndef nullptr
#define nullptr (0)
#endif

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

}

/* Here starts "helper" macros: */

#define XMLSERIAL_GETTERCOMMON(expr,cexpr,vtype,vname) \
			typedef vtype valtype; \
			static inline vtype &getvalue(XMLSERIAL_BASETYPE *o) \
				{ return expr; } \
			static inline const char *getname(XMLSERIAL_BASETYPE *o) \
				{ return vname; } \
			\
			static inline vtype const &getvalue(const XMLSERIAL_BASETYPE *o) \
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
		template<typename XMLSERIAL__T,bool XMLSERIAL__B> \
		struct xmlserial__vallocT { \
			const static bool valid = !XMLSERIAL__B; \
			typedef XMLSERIAL_BASETYPE rettype; \
			typedef XMLSERIAL_BASETYPE *(*createfntype)(void); \
			typedef std::map<std::string,createfntype> allocmaptype; \
			inline static XMLSERIAL_BASETYPE *\
					allocbyname(const std::string &name) { \
				return nullptr; } \
			inline static allocmaptype &alloctable() { \
				static allocmaptype table; \
				return table; \
			} \
			inline static const char * \
					addalloc(const char *name, createfntype fn) \
				{ return name; } \
		}; \

#define XMLSERIAL_V_COMMON(myname) \
	friend struct XMLSERIALNAMESPACE::IsEmpty<XMLSERIAL_BASETYPE,void>; \
	private: \
		template<typename XMLSERIAL__T, bool XMLSERIAL__B> \
		struct xmlserial__vallocT { \
			const static bool valid = XMLSERIAL__B; \
			typedef XMLSERIAL_BASETYPE rettype; \
			typedef XMLSERIAL_BASETYPE *(*createfntype)(void); \
			typedef std::map<std::string,createfntype> allocmaptype; \
			inline static XMLSERIAL_BASETYPE * \
					allocbyname(const std::string &name) { \
				typename allocmaptype::iterator i = alloctable().find(name); \
				if (i==alloctable().end()) return nullptr; \
				else return i->second(); \
			} \
			inline static allocmaptype &alloctable() { \
				static allocmaptype table; \
				return table; \
			} \
			inline static const char * \
					addalloc(const char *name, createfntype fn) { \
				alloctable()[std::string(name)] = fn; \
				return name; \
			} \
		}; \
	public: \
		virtual void SaveV(std::ostream &os, const char *name="", int indent=0) const { \
			Save(os,name,indent); \
		} \
		inline static XMLSERIAL_BASETYPE *LoadV(std::istream &is) { \
			XMLSERIAL_BASETYPE *ret; XMLSERIALNAMESPACE::LoadWrapper(ret,is); \
			return ret; \
		} \
		virtual void xmlserial__loadwrapv(std::istream &is, \
					const XMLSERIALNAMESPACE::XMLTagInfo &info) { \
			XMLSERIALNAMESPACE::LoadWrapper(*this,info,is); \
		}

#define XMLSERIAL_SUPER_COMMON(cname) \
		xmlserial__class##cname##lst; \
		template<bool XMLSERIAL__B> \
		struct xmlserial__vallocT<typename XMLSERIALNAMESPACE::Type_If<XMLSERIAL__B && cname::xmlserial__valloc::valid,XMLSERIAL_BASETYPE>::type,XMLSERIAL__B> { \
			const static bool valid = XMLSERIAL__B; \
			typedef cname::xmlserial__valloc::rettype rettype; \
			typedef cname::xmlserial__valloc::createfntype createfntype; \
			inline static XMLSERIAL_BASETYPE *allocbyname(const std::string &name) { \
				cname::xmlserial__valloc::rettype *cret = cname::xmlserial__valloc::allocbyname(name); \
				if (cret == nullptr) return nullptr; \
				XMLSERIAL_BASETYPE *ret = dynamic_cast<XMLSERIAL_BASETYPE *>(cret); \
				if (ret == nullptr) return nullptr; \
				return ret; \
			} \
			inline static const char *addalloc(const char *name, createfntype fn) \
				{ return cname::xmlserial__valloc::addalloc(name,fn); } \
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
	static const char *xmlserial__shiftname() { \
		static const char *ret = XMLSERIAL_NAME0(cname); \
		return ret; \
	} \

#define XMLSERIAL_SNAME1(cname,tname1) \
	static const char *xmlserial__shiftname() { \
		static const char *ret = XMLSERIAL_NAME1(cname,tname1); \
		return ret; \
	}

#define XMLSERIAL_SNAME2(cname,tname1,tname2) \
	static const char *xmlserial__shiftname() { \
		static const char *ret = XMLSERIAL_NAME2(cname,tname1,tname2); \
		return ret; \
	}

#define XMLSERIAL_SNAME3(cname,tname1,tname2,tname3) \
	static const char *xmlserial__shiftname() { \
		static const char *ret = XMLSERIAL_NAME3(cname,tname1,tname2,tname3); \
		return ret; \
	}

#define XMLSERIAL_SNAME4(cname,tname1,tname2,tname3,tname4) \
	static const char *xmlserial__shiftname() { \
		static const char *ret = \
			XMLSERIAL_NAME4(cname,tname1,tname2,tname3,tname4); \
		return ret; \
	}


#define XMLSERIAL_ID0_V(cname) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial__valloc::addalloc( \
			XMLSERIAL_NAME0(cname), \
			XMLSERIALNAMESPACE::creator<XMLSERIAL_BASETYPE>::create<xmlserial__valloc::rettype>); \
		return ret; \
	} \

#define XMLSERIAL_ID1_V(cname,tname1) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial__valloc::addalloc( \
			XMLSERIAL_NAME1(cname,tname1), \
			XMLSERIALNAMESPACE::template creator<XMLSERIAL_BASETYPE>::template create<typename xmlserial__valloc::rettype>); \
		return ret; \
	}

#define XMLSERIAL_ID2_V(cname,tname1,tname2) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial__valloc::addalloc( \
			XMLSERIAL_NAME2(cname,tname1,tname2), \
			XMLSERIALNAMESPACE::template creator<XMLSERIAL_BASETYPE>::template create<typename xmlserial__valloc::rettype>); \
		return ret; \
	}

#define XMLSERIAL_ID3_V(cname,tname1,tname2,tname3) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial__valloc::addalloc( \
			XMLSERIAL_NAME3(cname,tname1,tname2,tname3), \
			XMLSERIALNAMESPACE::template creator<XMLSERIAL_BASETYPE>::template create<typename xmlserial__valloc::rettype>); \
		return ret; \
	}

#define XMLSERIAL_ID4_V(cname,tname1,tname2,tname3,tname4) \
	static const char *xmlserial_IDname() { \
		static const char *ret = xmlserial__valloc::addalloc( \
			XMLSERIAL_NAME4(cname,tname1,tname2,tname3,tname4), \
			XMLSERIALNAMESPACE::creator<XMLSERIAL_BASETYPE>::create<typename xmlserial__valloc::rettype>); \
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
		//inline static bool xmlserial__isshort() { return false; }

#define XMLSERIAL_USESHIFT_SHORT(cname) \
	private: \
		typedef cname XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME0(cname) \
		enum { XMLSERIAL_ISSHORT = true };
		//inline static bool xmlserial__isshort() { return true; }

#define XMLSERIAL_USESHIFT1(cname,tname1) \
	private: \
		typedef cname<tname1> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME1(cname,tname1) \
		inline static bool xmlserial__isshort() { return false; }

#define XMLSERIAL_USESHIFT1_SHORT(cname) \
	private: \
		typedef cname<tname1> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME1(cname,tname1) \
		inline static bool xmlserial__isshort() { return true; }

#define XMLSERIAL_USESHIFT2(cname,tname1,tname2) \
	private: \
		typedef cname<tname1,tname2> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME2(cname,tname1,tname2) \
		inline static bool xmlserial__isshort() { return false; }

#define XMLSERIAL_USESHIFT2_SHORT(cname,tname1,tname2) \
	private: \
		typedef cname<tname1,tname2> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME2(cname,tname1,tname2) \
		inline static bool xmlserial__isshort() { return true; }

#define XMLSERIAL_USESHIFT3(cname) \
	private: \
		typedef cname<tname1,tname2,tname3> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME3(cname,tname1,tname2,tname3) \
		inline static bool xmlserial__isshort() { return false; }

#define XMLSERIAL_USESHIFT3_SHORT(cname,tname1,tname2,tname3) \
	private: \
		typedef cname<tname1,tname2,tname3> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME3(cname,tname1,tname2,tname3) \
		inline static bool xmlserial__isshort() { return true; }

#define XMLSERIAL_USESHIFT4(cname,tname1,tname2,tname3,tname4) \
	private: \
		typedef cname<tname1,tname2,tname3,tname4> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME4(cname,tname1,tname2,tname3,tname4) \
		inline static bool xmlserial__isshort() { return false; }

#define XMLSERIAL_USESHIFT4_SHORT(cname) \
	private: \
		typedef cname<tname1,tname2,tname3,tname4> XMLSERIAL_BASETYPE; \
	XMLSERIAL_NOTV_COMMON \
	public: \
		XMLSERIAL_SNAME4(cname,tname1,tname2,tname3,tname4) \
		inline static bool xmlserial__isshort() { return true; }

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

/* Here starts "START_V" macros: */

#define XMLSERIAL_START_V(cname) \
	private: \
		typedef cname XMLSERIAL_BASETYPE; \
		XMLSERIAL_V_COMMON(cname) \
	public: \
		XMLSERIAL_ID0_V(cname); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START1_V(cname,tname) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_V_COMMON(cname) \
	public: \
		XMLSERIAL_ID1_V(cname,tname); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START2_V(cname,tname1,tname2) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_V_COMMON(cname) \
	public: \
		XMLSERIAL_ID2_V(cname,tname1,tname2); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START3_V(cname,tname1,tname2,tname3) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_V_COMMON(cname) \
	public: \
		XMLSERIAL_ID3_V(cname,tname1,tname2,tname3); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

#define XMLSERIAL_START4_V(cname,tname1,tname2,tname3,tname4) \
	private: \
		typedef cname<tname> XMLSERIAL_BASETYPE; \
		XMLSERIAL_V_COMMON(cname) \
	public: \
		XMLSERIAL_ID4_V(cname,tname1,tname2,tname3,tname4); \
	private: \
		typedef XMLSERIALNAMESPACE::ListEnd

/* Here starts "VAR" macros */

#define XMLSERIAL_VAR(vtype,vname) \
		xmlserial__##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT((o->vname),(o->vname),vtype,#vname) \
			xmlserial__get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial__get##vname,xmlserial__##vname##lst>

#define XMLSERIAL_VAR_DEFAULT(vtype,vname,val) \
		xmlserial__##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((o->vname),(o->vname),vtype,#vname,val) \
			xmlserial__get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial__get##vname,xmlserial__##vname##lst>

#define XMLSERIAL_VAR_N(vtype,vname,sname) \
		xmlserial__##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT((o->vname),(o->vname),vtype,sname) \
			xmlserial__get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial__get##vname,xmlserial__##vname##lst>

#define XMLSERIAL_VAR_DEFAULT_N(vtype,vname,val,sname) \
		xmlserial__##vname##lst; \
		typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((o->vname),(o->vname),vtype,sname,val) \
			xmlserial__get##vname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial__get##vname,xmlserial__##vname##lst>

/* Here starts "SUPER" macros */

#define XMLSERIAL_SUPER(cname) \
		XMLSERIAL_SUPER_COMMON(cname) \
		typedef XMLSERIAL_GETTERSTRUCT((*(static_cast<cname *>(o))),*(static_cast<const cname *>(o)),cname,#cname) \
			xmlserial__getclass##cname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial__getclass##cname,xmlserial__class##cname##lst>
			
			
#define XMLSERIAL_SUPER_DEFAULT(cname,val) \
	XMLSERIAL_SUPER_COMMON(cname) \
	typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((*(static_cast<cname *>(o))), \
		*(static_cast<const cname *>(o)),cname,#cname,val) \
		xmlserial__getclass##cname; \
	typedef XMLSERIALNAMESPACE::List<xmlserial__getclass##cname, \
		xmlserial__class##cname##lst>
			
#define XMLSERIAL_SUPER_N(cname,sname) \
		XMLSERIAL_SUPER_COMMON(cname) \
		typedef XMLSERIAL_GETTERSTRUCT((*(static_cast<cname *>(o))),*(static_cast<const cname *>(o)),cname,sname) \
			xmlserial__getclass##cname; \
		typedef XMLSERIALNAMESPACE::List<xmlserial__getclass##cname,xmlserial__class##cname##lst>
			
			
#define XMLSERIAL_SUPER_DEFAULT_N(cname,val,sname) \
	XMLSERIAL_SUPER_COMMON(cname) \
	typedef XMLSERIAL_GETTERSTRUCT_DEFAULT((*(static_cast<cname *>(o))), \
		*(static_cast<const cname *>(o)),cname,sname,val) \
		xmlserial__getclass##cname; \
	typedef XMLSERIALNAMESPACE::List<xmlserial__getclass##cname, \
		xmlserial__class##cname##lst>
			

/* Here starts "ENSURECLASS" macros */

#define ENSURECLASS(cname) \
	namespace { static const char *namefor__##cname=cname::xmlserial_IDname(); }

#define ENSURECLASS1(cname,tname1) \
	namespace { static const char *namefor__##cname##tname1 = \
		cname<tname1>::xmlserial_IDname(); }

#define ENSURECLASS2(cname,tname1,tname2) \
	namespace { static const char *namefor__##cname##tname1##tname2 = \
		cname<tname1,tname2>::xmlserial_IDname(); }

#define ENSURECLASS3(cname,tname1,tname2,tname3) \
	namespace { \
		static const char *namefor__##cname##tname1##tname2##tname3 = \
			cname<tname1,tname2,tname3>::xmlserial_IDname(); \
	}

#define ENSURECLASS4(cname,tname1,tname2,tname3,tname4) \
	namespace { \
		static const char *namefor__##cname##tname1##tname2##tname3##tname4 \
			= cname<tname1,tname2,tname3,tname4>::xmlserial_IDname(); \
	}

/* Here starts "END" macros (only 1, actually) */

#define XMLSERIAL_END \
        xmlserial__tmp__alllist; \
    public: \
        typedef xmlserial__tmp__alllist xmlserial__alllist; \
        typedef xmlserial__vallocT<XMLSERIAL_BASETYPE,true> xmlserial__valloc; \
        inline void xmlserial__Save(std::ostream &os, int indent=0) const { \
            XMLSERIALNAMESPACE::SaveItt<xmlserial__alllist>::exec(this,os,indent); } \
        inline void Save(std::ostream &os, const char *name="", \
                        int indent=0) const { \
            XMLSERIALNAMESPACE::SaveWrapper(*this,name,os,indent); \
        } \
        inline void xmlserial__Load(std::istream &is) { \
            XMLSERIALNAMESPACE::LoadList<xmlserial__alllist>::exec \
                (this,is,xmlserial_IDname()); \
        } \
        inline void Load(std::istream &is) { \
            XMLSERIALNAMESPACE::LoadWrapper(*this,is); \
        } \
        inline static XMLSERIAL_BASETYPE *LoadPtr(std::istream &is) { \
            XMLSERIAL_BASETYPE *ret; \
            XMLSERIALNAMESPACE::LoadWrapper(ret,is); \
            return ret; \
        }

//------------------------------------
// implementation (templated part)
//------------------------------------

#include <map>
#include <set>
#include <set>

namespace XMLSERIALNAMESPACE {

	char *TName(const char *cname, int n, ...);
	void Indent(std::ostream &os, int indent);
	void *AllocByName(const std::string &name);
	typedef void *(*createfntype)(void);
	const char *AddAlloc(const char *name, createfntype fn);

	// a simple list:
	struct ListEnd {};

	template<typename H, typename T>
		struct List {};

	// pretty standard enable_if template (see Boost, for example)
	// (included here so as not to reply on boost or c++11)
	// (called Type_If to avoid confusion, and really this is a more
	//  descriptive name: it gives a type if the condition holds)
	template<bool B, typename T>
	struct Type_If {
		typedef T type;
	};
	template<typename T>
	struct Type_If<false,T> {
	};

	// pick how to add a value to a struct (enum or static int const)
#define XMLSERIAL_DECVAL(vname,expr) \
	enum { vname = (expr) };
	//static int const vname = (expr);

	// properties of type T:
	// This one has appeared in one form or another all over the
	// internet, adapted only slightly here
	template<typename T>
	struct TypeProp {
		template<typename C, C> struct type_check;

		template<typename S> static char
			(& chksave(type_check<void (S::*)(std::ostream&,int),
					 &S::xmlserial__Save>*))[1];
		template<typename> static char (& chksave(...))[2];
		XMLSERIAL_DECVAL(HasSave,sizeof(chksave<T>(0)) == 1)

		template<typename S> static char
			(& chkid(type_check<const char *(*)(),
						&S::xmlserial_IDname>*))[1];
		template<typename> static char
			(& chkid(...))[2];
		XMLSERIAL_DECVAL(HasIDname,sizeof(chkid<T>(0)) == 1)

		template<typename S> static char
			(& chkshift(type_check<const char *(*)(),
						&S::xmlserial__shiftname>*))[1];
		template<typename> static char
			(& chkshift(...))[2];
		XMLSERIAL_DECVAL(HasShift,sizeof(chkshift<T>(0)) == 1)

		template<typename S> static char
			(& chkdef(type_check<void (*)(typename S::valtype &),
					&S::setdefault>*))[1];
		template<typename> static char
			(& chkdef(...))[2];
		XMLSERIAL_DECVAL(HasDefault,sizeof(chkdef<T>(0)) == 1)

		template<typename S> static char
			(& chkv(type_check<T *(*)(std::istream &),
				   &S::LoadV>*))[1];
		template<typename> static char
			(& chkv(...))[2];
		XMLSERIAL_DECVAL(HasV,sizeof(chkv<T>(0)) == 1)

		template<typename S> static char
			(& chkpreload(type_check<void (S::*)(void),
					    &S::xmlserial_preload>*))[1];
		template<typename> static char
			(& chkpreload(...))[2];
		XMLSERIAL_DECVAL(HasPreLoad,sizeof(chkpreload<T>(0)) == 1)

		template<typename S> static char
			(& chkpostload(type_check<void (S::*)(void),
						&S::xmlserial_postload>*))[1];
		template<typename> static char
			(& chkpostload(...))[2];
		XMLSERIAL_DECVAL(HasPostLoad,sizeof(chkpostload<T>(0)) == 1)

			template<typename S> static char
			(& chkpresave(type_check<void (S::*)(void) const,
					    &S::xmlserial_presave>*))[1];
		template<typename> static char
			(& chkpresave(...))[2];
		XMLSERIAL_DECVAL(HasPreSave,sizeof(chkpresave<T>(0)) == 1)

		template<typename S> static char
			(& chkpostsave(type_check<void (S::*)(void) const,
						&S::xmlserial_postsave>*))[1];
		template<typename> static char
			(& chkpostsave(...))[2];
		XMLSERIAL_DECVAL(HasPostSave,sizeof(chkpostsave<T>(0)) == 1)

		//enum { BlankList = SameType<ListEnd,typename T::xmlserial__alllist>::value };
	};

	// Whether it has a default constructor (why not above?  I don't know)
	template<typename T>
	struct HasDefCon {
		struct small { char x[1]; };
		struct large { char x[2]; };
		template<typename U>
			static small chk(U (*)[1]);
		template<typename U>
			static large chk(...);
		//XMLSERIAL_DECVAL(value,sizeof(HasDefCon<T>::template chk<T>(0))==sizeof(small))
		XMLSERIAL_DECVAL(value,sizeof(chk<T>(0))==sizeof(small))
	};


	// whether a list (as defined above) is empty
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
	struct IsEmpty<T,typename Type_If<ListEmpty<typename T::xmlserial__alllist>::value,void>::type> {
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



	// construct the object, if it has a default constructor
	template<typename AT>
	struct creator {
		template<typename RT>
		inline static typename Type_If<HasDefCon<AT>::value,RT *>::type
			create() { return new AT(); }

		template<typename RT>
		inline static typename Type_If<!HasDefCon<AT>::value,RT *>::type
			create() { return nullptr; }
	};

	// information about XML tags
	struct XMLTagInfo {
		std::string name;
		std::map<std::string,std::string> attr;
		bool isstart,isend;
	};

	// forward decls
	template<typename G> struct SaveItem;
	template<typename L> struct LoadList;
	void ReadTag(std::istream &is,XMLTagInfo &info);
	void ReadEndTag(std::istream &is,const char *ename);
	void ReadStr(std::istream &is, std::string &ret,const char *endchar);
	void WriteStr(std::ostream &os, const std::string &s,bool escape=true);

	// a reverse iterator on a list for saving
	// runs SaveItem on each element in list
	template<typename L,typename Condition=void>
	struct SaveItt {
		template<typename O>
		inline static void exec(O o,std::ostream &os,int indent) { }
	};

	template<typename H, typename T>
	struct SaveItt<List<H,T>,
	typename Type_If<!IsEmpty<typename H::valtype>::value,void>::type> {
		template<typename O>
		inline static void exec(O o,std::ostream &os,int indent) {
				SaveItt<T>::exec(o,os,indent);
				SaveItem<H>::exec(o,os,indent);
		}
	};

	template<typename H, typename T>
	struct SaveItt<List<H,T>,
			typename Type_If<IsEmpty<typename H::valtype>::value,
											void>::type> {
		template<typename O>
		inline static void exec(O o,std::ostream &os,int indent) {
			SaveItt<T>::exec(o,os,indent);
		}
	};

	template<>
	struct SaveItt<ListEnd,void> {
		template<typename O>
		inline static void exec(O o,std::ostream &os, int indent) { }
	};


	// Given an XMLTagInfo, find relevant membe and call LoadWrapper
	template<typename L>
	struct LoadOne {
		template<typename O>
		inline static bool exec(O o,std::istream &, const XMLTagInfo &) {
			return false;
		}
	};

	template<>
	struct LoadOne<ListEnd> {
		template<typename O>
		inline static bool exec(O o,std::istream &, const XMLTagInfo &) {
			return false;
		}
	};

	template<typename H, typename T>
	struct LoadOne<List<H,T> > {
		template<typename O>
		inline static bool exec(O o,std::istream &is,
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
#define XMLSERIAL__BASETYPE(tname,strname) \
	template<> \
	struct IsShiftable<tname,void> { \
		XMLSERIAL_DECVAL(atall,true) \
		XMLSERIAL_DECVAL(quickly,true) \
	}; \
	template<> \
	struct TypeInfo<tname,void> { \
		inline static const char *namestr() { return #strname; } \
		inline static void writeotherattr(std::ostream &, const tname &) { } \
		inline static bool isshort(const tname &) { return true; } \
		inline static bool isinline(const tname &) { return false; } \
		inline static void save(const tname &t,std::ostream &os, int indent) { \
			os << t; \
		} \
		inline static void load(tname &t, const XMLTagInfo &info, std::istream &is) {\
			std::map<std::string,std::string>::const_iterator vi  \
			=info.attr.find("value"); \
			if (vi!=info.attr.end()) { \
				std::istringstream ss(vi->second); \
				ss.copyfmt(is); \
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
		enum { x = sizeof(T::__Attempting_to_serialize_type_without_serialization_information) };

		inline static const char *namestr() {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		inline static void writeotherattr(std::ostream &, const T &) {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		inline static bool isshort(const T &)  {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		inline static bool isinline(const T &)  {
			throw streamexception("Streaming Error: type info asked about unknown class");
		}
		inline static void save(const T &t,std::ostream &os, int indent) {
			throw streamexception("Streaming Error: save called for unknown class");
		}
		inline static void load(T &t, const XMLTagInfo &info,
				std::istream &is) {
			throw streamexception("Streaming Error: load called for unknown class");
		}
	};

	// These were moved upward to avoid ambiguity.
	XMLSERIAL__BASETYPE(bool,bool)

	XMLSERIAL__BASETYPE(char,char)
	XMLSERIAL__BASETYPE(signed char,s_char)
	XMLSERIAL__BASETYPE(unsigned char,u_char)
	//XMLSERIAL__BASETYPE(wchar_t,w_char)

	XMLSERIAL__BASETYPE(short,short)
	XMLSERIAL__BASETYPE(unsigned short,u_short)
	XMLSERIAL__BASETYPE(int,int)
	XMLSERIAL__BASETYPE(unsigned int,u_int)
	XMLSERIAL__BASETYPE(long,long)
	XMLSERIAL__BASETYPE(unsigned long,u_long)
	XMLSERIAL__BASETYPE(float,float)
	XMLSERIAL__BASETYPE(double,double)
	XMLSERIAL__BASETYPE(long double,l_double)

#if __cplusplus > 199711L
	//XMLSERIAL__BASETYPE(char16_t,char16)
	//XMLSERIAL__BASETYPE(char32_t,char32)
	XMLSERIAL__BASETYPE(long long,l_long)
	XMLSERIAL__BASETYPE(unsigned long long,u_l_long)
#endif

	template<typename T> // take care of const case...
	struct TypeInfo<const T,void> {
		inline static const char *namestr() {
			return TypeInfo<T>::namestr();
		}
		inline static void writeotherattr(std::ostream &os, const T &t) {
			TypeInfo<T>::writeotherattr(os,t);
		}
		inline static void writeotherattr(std::ostream &os, const T &t,
				const char *s1, const char *s2) {
			TypeInfo<T>::otherattr(os,t,s1,s2);
		}
		inline static bool isshort(const T &t)  {
			return TypeInfo<T>::isshort(t);
		}
		inline static bool isinline(const T &t)  {
			return TypeInfo<T>::isinline(t);
		}
		inline static void save(const T &t,std::ostream &os, int indent) {
			TypeInfo<T>::save(t,os,indent);
		}
		inline static void load(const T &t, const XMLTagInfo &info,
				std::istream &is) {
			throw streamexception("Streaming Error: load called for constant type");
		}
	};


	// Saving method, general
	template<typename T>
	inline void SaveWrapper(const T &v,const char *vname,
			std::ostream &os,int indent) {
		Indent(os,indent);
		os << "<" << TypeInfo<T>::namestr();
		TypeInfo<T>::writeotherattr(os,v);
		if (vname && vname[0]!=0) os << " name=\"" << vname << "\"";
		if (TypeInfo<T>::isshort(v)) {
			os << " value=\"";
			TypeInfo<T>::save(v,os,indent);
			os << "\" \\>" << std::endl;
		} else if (TypeInfo<T>::isinline(v)) {
			os << " \\>" << std::endl;
		} else {
			os << ">";
			TypeInfo<T>::save(v,os,indent);
			os << "<\\" << TypeInfo<T>::namestr() << ">" << std::endl;
		}
	}


	// Saving method, raw pointer, virtual
	template<typename T>
	inline typename Type_If<TypeProp<T>::HasV,void>::type
	SaveWrapper(T * const &v, const char *vname,
			std::ostream &os, int indent) {
		if (v == nullptr) {
			Indent(os,indent);
			os << "<" << TypeInfo<T>::namestr();
			if (vname && vname[0]!=0) os << " name=\"" << vname << "\"";
			os << " isnull=\"1\" \\>" << std::endl;
		} else v->SaveV(os,vname,indent);
	}

	// Saving method, raw pointer, non-virtual
	template<typename T>
	inline typename Type_If<!TypeProp<T>::HasV,void>::type
	SaveWrapper(T * const &v, const char *vname,
			std::ostream &os, int indent) {
		if (v == nullptr) {
			Indent(os,indent);
			os << "<" << TypeInfo<T>::namestr();
			if (vname && vname[0]!=0) os << " name=\"" << vname << "\"";
			os << " isnull=\"1\" \\>" << std::endl;
		} else SaveWrapper(*v,vname,os,indent);
	}

	// Saving, with extra arguments ("key" and "value" in map, e.g.)
	template<typename T>
	inline void SaveWrapper(const T &v,const char *vname,
			std::ostream &os,int indent, const char *e1, const char *e2) {
		Indent(os,indent);
		os << "<" << TypeInfo<T>::namestr();
		TypeInfo<T>::writeotherattr(os,v,e1,e2);
		if (vname && vname[0]!=0) os << " name=\"" << vname << "\"";
		if (TypeInfo<T>::isinline(v)) {
			os << " \\>" << std::endl;
		} else {
			os << ">";
			TypeInfo<T>::save(v,os,indent,e1,e2);
			os << "<\\" << TypeInfo<T>::namestr() << ">" << std::endl;
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
	template<typename T>
	inline void LoadWrapper(T &v, const XMLTagInfo &info, std::istream &is) {
		CheckTag<T>::check(info);
		TypeInfo<T>::load(v,info,is);
	}

	// Load, pointer, virtual
	template<typename T>
	inline typename Type_If<TypeProp<T>::HasV,void>::type
	LoadWrapper(T *&v, const XMLTagInfo &info, std::istream &is) {
		if (!info.isstart)
			throw streamexception(std::string("Stream Input Format Error: expected start tag, received end tag for ")+info.name);
		std::map<std::string,std::string>::const_iterator vi
			=info.attr.find("isnull");
		if (vi!=info.attr.end() && vi->second=="1") {
			v = nullptr;
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: null pointer object for type ")+TypeInfo<T>::namestr()+" and name "+info.name+" has non-empty contents");
			}
			return;
		}
		T *vv = T::xmlserial__valloc::allocbyname(info.name);
		if (vv == nullptr)
			throw streamexception(std::string("Stream Input Format Error: expected start tag for subtype of ")+TypeInfo<T>::namestr()+", received start tag for type "+info.name+" which is either unknown or not a subtype");
		v = vv;
		v->xmlserial__loadwrapv(is,info);
	}

	// Load, pointer, non-virtual
	template<typename T>
	inline typename Type_If<!TypeProp<T>::HasV,void>::type
	LoadWrapper(T *&v, const XMLTagInfo &info, std::istream &is) {
		std::map<std::string,std::string>::const_iterator vi
			=info.attr.find("isnull");
		if (vi!=info.attr.end() && vi->second=="1") {
			v = nullptr;
			if (!info.isend) {
				XMLTagInfo einfo;
				ReadTag(is,einfo);
				if (einfo.name!=info.name || !info.isend || info.isstart)
					throw streamexception(std::string("Stream Input Format Error: null pointer object for type ")+TypeInfo<T>::namestr()+" and name "+info.name+" has non-empty contents");
			}
			return;
		}
		v = new T();
		LoadWrapper(*v,info,is);
	}

	// Load, first read tag
	template<typename T>
	inline void LoadWrapper(T &v, std::istream &is) {
		XMLTagInfo info;
		ReadTag(is,info);
		LoadWrapper(v,info,is);
	}

	// next: SerialLoadWrap and SerialSaveWrap
	// that call preload/postload or presave/postsave if they
	// exist before/after loading/saving
	template<typename T>
	inline typename Type_If<!TypeProp<T>::HasPreLoad
						&& !TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, std::istream &is) {
		t.xmlserial__Load(is);
	}

	template<typename T>
	inline typename Type_If<TypeProp<T>::HasPreLoad
						&& !TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, std::istream &is) {
		t.xmlserial_preload();
		t.xmlserial__Load(is);
	}

	template<typename T>
	inline typename Type_If<!TypeProp<T>::HasPreLoad
						&& TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, std::istream &is) {
		t.xmlserial__Load(is);
		t.xmlserial_postload();
	}

	template<typename T>
	inline typename Type_If<TypeProp<T>::HasPreLoad
						&& TypeProp<T>::HasPostLoad,void>::type
	SerialLoadWrap(T &t, std::istream &is) {
		t.xmlserial_preload();
		t.xmlserial__Load(is);
		t.xmlserial_postload();
	}

	template<typename T>
	inline typename Type_If<!TypeProp<T>::HasPreSave
						&& !TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, std::ostream &os, int indent) {
		t.xmlserial__Save(os,indent);
	}

	template<typename T>
	inline typename Type_If<TypeProp<T>::HasPreSave
						&& !TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, std::ostream &os, int indent) {
		t.xmlserial_presave();
		t.xmlserial__Save(os,indent);
	}

	template<typename T>
	inline typename Type_If<!TypeProp<T>::HasPreSave
						&& TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, std::ostream &os, int indent) {
		t.xmlserial__Save(os,indent);
		t.xmlserial_postsave();
	}

	template<typename T>
	inline typename Type_If<TypeProp<T>::HasPreSave
						&& TypeProp<T>::HasPostSave,void>::type
	SerialSaveWrap(const T &t, std::ostream &os, int indent) {
		t.xmlserial_presave();
		t.xmlserial__Save(os,indent);
		t.xmlserial_postsave();
	}

// how to deal with ones declared by these macros
	template<typename T>
	struct TypeInfo<T, typename Type_If<TypeProp<T>::HasIDname,void>::type> {
		inline static const char *namestr() { return T::xmlserial_IDname(); }
		inline static void writeotherattr(std::ostream &,const T &) { }
		inline static bool isshort(const T &) { return false; }
		inline static bool isinline(const T &) { return false; }
		inline static void save(const T &t,std::ostream &os,int indent) {
			os << std::endl;
			SerialSaveWrap(t,os,indent+1);
			Indent(os,indent);
		}
		inline static void load(T &t,const XMLTagInfo &info,
				std::istream &is) {
			SerialLoadWrap(t,is);
		}
	};

// ... or ones that are "shiftable"
	template<typename T>
	struct TypeInfo<T, typename Type_If<TypeProp<T>::HasShift,void>::type> {
		inline static const char *namestr() { return T::xmlserial__shiftname(); }
		inline static void writeotherattr(std::ostream &, const T &) {}
		inline static bool isshort(const T &) { return T::XMLSERIAL_ISSHORT; }
		inline static bool isinline(const T &) { return false; }
		inline static void save(const T &t,std::ostream &os,int indent) {
			os << t;
		}
		inline static void load(T &t, const XMLTagInfo &info,
				std::istream &is) {
			std::map<std::string,std::string>::const_iterator vi
				=info.attr.find("value");
			if (vi!=info.attr.end()) {
				std::istringstream ss(vi->second);
				ss.copyfmt(is);
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
		template<typename T>
			inline static void exec(T o,std::ostream &os,int indent) {
				SaveWrapper(G::getvalue(o),G::getname(o),os,indent);
			}
	};

	// same for Loading
	template<typename G>
	struct LoadItem {
		template<typename T>
		inline static bool exec(T o,std::istream &is,
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
		template<typename O>
		inline static void exec(O o, std::istream &is, const char *cname) {
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
#endif
