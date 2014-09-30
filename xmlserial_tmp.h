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

#ifndef XMLSERIAL_TMP_H
#define XMLSERIAL_TMP_H

namespace XMLSERIALNAMESPACE {

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


	// properties of type T:
	// This one has appeared in one form or another all over the
	// internet, adapted only slightly here
	template<typename T>
	struct TypeProp {
		template<typename C, C> struct type_check;

		template<typename S> static char
			(& chksave(type_check<void (S::*)(std::ostream&,int),
					 &S::xmlserial_Save>*))[1];
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
						&S::xmlserial_shiftname>*))[1];
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

		//enum { BlankList = SameType<ListEnd,typename T::xmlserial_alllist>::value };
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


}

#endif
