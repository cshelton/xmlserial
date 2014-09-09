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

#ifndef XMLSERIAL_PTRS_H
#define XMLSERIAL_PTRS_H

// Define here all of the types of pointers that should be saved as such
//        using macros like those below
// Then, create a PtrInfo<...> structure (as those farther below) for each

// for C++03, just standard pointer
#define XMLSERIAL_PTR_TYPE1 BT*
#define XMLSERIAL_PTR_NUM 1

#if __cplusplus > 199711L
// for C++11, add shared_ptr and unique_ptr (with default destructors only)
#include <memory>
#define XMLSERIAL_PTR_TYPE2 std::shared_ptr<BT>
#define XMLSERIAL_PTR_TYPE3 std::unique_ptr<BT>

// redefine the number...
#undef XMLSERIAL_PTR_NUM
#define XMLSERIAL_PTR_NUM 3
#endif

namespace XMLSERIALNAMESPACE {

template<typename T>
struct PtrInfo {
	XMLSERIAL_DECVAL(isptr,false);
};

template<typename T>
struct PtrInfo<T*> {
	XMLSERIAL_DECVAL(isptr,true);
	typedef T BaseType;
	typedef T* P;
	
	static const T &deref_const(T * const &v) { return *v; }
	static T &deref(T *&v) { return *v; }

	static void setnull(T *&v) { v = nullptr; }
	static void setraw(T *&v, T *newv) { v = newv; }
	static void allocnew(P &v) { v = new T(); }
	template<typename AT>
	struct valloc {
		static void exec(P &v) { v = new AT(); }
		static P ret() { return new AT(); }
	};

	static bool isnull(T * const &v) { return v == nullptr; }
	static P getnull() { return nullptr; }

	template<typename S>
	static P cast(S *p) { return dynamic_cast<P>(p); }
};

#if __cplusplus > 199711L
template<typename T>
struct PtrInfo<std::shared_ptr<T>> {
	XMLSERIAL_DECVAL(isptr,true);
	typedef T BaseType;
	typedef std::shared_ptr<T> P;

	static const T &deref_const(const P &v) { return *v; }
	static T &deref(P &v) { return *v; }

	static void setnull(P &v) { v.reset(); }
	static void setraw(P &v, T *newv) { v.reset(newv); }
	static void allocnew(P &v) { v = std::make_shared<T>(); }
	template<typename AT>
	struct valloc {
		static void exec(P &v) { v = std::make_shared<AT>(); }
		static P ret() { return std::make_shared<AT>(); }
	};

	static bool isnull(const P &v) { return !v; }
	static P getnull() { return P(); }

	template<typename S>
	static P cast(std::shared_ptr<S> &p)
		{ return std::dynamic_pointer_cast<T>(p); }
};

template<typename T>
struct PtrInfo<std::unique_ptr<T>> {
	XMLSERIAL_DECVAL(isptr,true);
	typedef T BaseType;
	typedef std::unique_ptr<T> P;

	static const T &deref_const(const P &v) { return *v; }
	static T &deref(P &v) { return *v; }

	static void setnull(P &v) { v.reset(); }
	static void setraw(P &v, T *newv) { v.reset(newv); }
	static void allocnew(P &v) { v.reset(new T()); }
	template<typename AT>
	struct valloc {
		static void exec(P &v) { v = P(new AT()); }
		static P ret() { return P(new AT()); }
	};

	static bool isnull(const P &v) { return !v; }
	static P getnull() { return P(); }
	template<typename S>
	static P cast(std::shared_ptr<S> &p) {
		if (!p) return P();
		T *retptr = dynamic_cast<T *>(p.get());
		if (!retptr) return P();
		p.release();
		return P(retptr);
	}
};
#endif // of c++11 code block





#define XMLSERIAL_PTR_TYPE(I) XMLSERIAL_PTR_TYPE ## I

#define XMLSERIAL_REPREAL_0(MAC,BASE) 
#define XMLSERIAL_REPREAL_1(MAC,BASE) MAC(BASE,1)
#define XMLSERIAL_REPREAL_2(MAC,BASE) MAC(BASE,2) XMLSERIAL_REPREAL_1(MAC,BASE)
#define XMLSERIAL_REPREAL_3(MAC,BASE) MAC(BASE,3) XMLSERIAL_REPREAL_2(MAC,BASE)
#define XMLSERIAL_REPREAL_4(MAC,BASE) MAC(BASE,4) XMLSERIAL_REPREAL_3(MAC,BASE)
#define XMLSERIAL_REPREAL_5(MAC,BASE) MAC(BASE,5) XMLSERIAL_REPREAL_4(MAC,BASE)
#define XMLSERIAL_REPREAL_6(MAC,BASE) MAC(BASE,6) XMLSERIAL_REPREAL_5(MAC,BASE)
#define XMLSERIAL_REPREAL_7(MAC,BASE) MAC(BASE,7) XMLSERIAL_REPREAL_6(MAC,BASE)
#define XMLSERIAL_REPREAL_8(MAC,BASE) MAC(BASE,8) XMLSERIAL_REPREAL_7(MAC,BASE)
#define XMLSERIAL_REPREAL_9(MAC,BASE) MAC(BASE,9) XMLSERIAL_REPREAL_8(MAC,BASE)
#define XMLSERIAL_REPREAL_10(MAC,BASE) MAC(BASE,10) XMLSERIAL_REPREAL_9(MAC,BASE)

#define XMLSERIAL_CAT_REAL(A,B) A ## B
#define XMLSERIAL_CAT_2(A,B) XMLSERIAL_CAT_REAL(A,B)
#define XMLSERIAL_CAT(A,B) XMLSERIAL_CAT_2(A,B)

#define XMLSERIAL_INVOKE( A, B ) A B


#define XMLSERIAL_REP_(MAC,BASE,N) \
	XMLSERIAL_CAT(XMLSERIAL_REPREAL_,N)(MAC,BASE)
#define XMLSERIAL_REP(MAC,BASE,N) XMLSERIAL_REP_(MAC,BASE,N)


#define XMLSERIAL_PTR_NULL_DEF(BASE,I) \
	virtual BASE(I) valloc(BASE(I) p) { \
		return PtrInfo<BASE(I) >::getnull(); \
	}
template<typename BT>
class basefactory {
public:
	virtual ~basefactory() {};

	XMLSERIAL_REP(XMLSERIAL_PTR_NULL_DEF,XMLSERIAL_PTR_TYPE,XMLSERIAL_PTR_NUM)
/*
	virtual BT *valloc(BT *p) {
		return PtrInfo<BT*>::getnull();
	}

#if __cplusplus > 199711L
	virtual std::shared_ptr<BT> valloc(std::shared_ptr<BT> p) {
		return PtrInfo<std::shared_ptr<BT>>::getnull();
	}
	virtual std::unique_ptr<BT> valloc(std::unique_ptr<BT> p) {
		return PtrInfo<std::unique_ptr<BT>>::getnull();
	}
#endif
*/
};

#define XMLSERIAL_PTR_DEF(BASE,I) \
	virtual BASE(I) valloc(BASE(I) p) { \
		typedef PtrInfo<BASE(I)> pT; \
		typedef typename pT::template valloc<AT> allocT; \
		return allocT::ret(); \
	}

template<typename AT,typename BT>
class factory : public basefactory<BT> {
public:
	virtual ~factory() {};

	XMLSERIAL_REP(XMLSERIAL_PTR_DEF,XMLSERIAL_PTR_TYPE,XMLSERIAL_PTR_NUM)
};

#undef XMLSERIAL_PTR_DEF
#undef XMLSERIAL_PTR_NULL_DEF
#undef XMLSERIAL_PTR_TYPE
#undef XMLSERIAL_PTR_TYPE1
#undef XMLSERIAL_PTR_TYPE2
#undef XMLSERIAL_PTR_TYPE3
#undef XMLSERIAL_PTR_TYPE4
#undef XMLSERIAL_PTR_TYPE5
#undef XMLSERIAL_PTR_TYPE6
#undef XMLSERIAL_PTR_TYPE7
#undef XMLSERIAL_PTR_TYPE8
#undef XMLSERIAL_PTR_TYPE9
#undef XMLSERIAL_PTR_TYPE10
#undef XMLSERIAL_PTR_NUM
#undef XMLSERIAL_REPREAL_0
#undef XMLSERIAL_REPREAL_1
#undef XMLSERIAL_REPREAL_2
#undef XMLSERIAL_REPREAL_3
#undef XMLSERIAL_REPREAL_4
#undef XMLSERIAL_REPREAL_5
#undef XMLSERIAL_REPREAL_6
#undef XMLSERIAL_REPREAL_7
#undef XMLSERIAL_REPREAL_8
#undef XMLSERIAL_REPREAL_9
#undef XMLSERIAL_REPREAL_10

#undef XMLSERIAL_CAT_REAL
#undef XMLSERIAL_CAT_2
#undef XMLSERIAL_CAT

#undef XMLSERIAL_INVOKE
#undef XMLSERIAL_REP_
#undef XMLSERIAL_REP

}

#endif // of file guard
