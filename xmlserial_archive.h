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

#ifndef XMLSERIAL_ARCHIVE_H
#define XMLSERIAL_ARCHIVE_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

namespace XMLSERIALNAMESPACE {

class archive {
public:
	archive(std::istream &input) : is(&input), os(0) {}
	archive(std::ostream &output) : os(&output), is(0) {}
	archive(std::iostream &stream) : is(&stream), os(&stream) {}

	//operator std::istream &() { return *is; }
	//operator std::ostream &() { return *os; }
	//operator const std::istream &() const { return *is; }
	//operator const std::ostream &() const { return *os; }

	template<typename T>
	archive &operator<<(const T &t) {
		if (os) (*os) << t;
		return *this;
	}
	template<typename T>
	archive &operator>>(T &t) {
		if (is) (*is) >> t;
		return *this;
	}

	typedef std::ostream& (*osmanip)(std::ostream&);
	archive &operator<<(osmanip m) {
		if (os) m(*os);
		return *this;
	}

	int get() { return is ? is->get() : -1; }
	int peek() { return is ? is->peek() : -1; }
	bool fail() const { return is ? is->fail() : os->fail(); }
	archive &unget() { if (is) is->unget(); return *this; }

	std::ios_base::fmtflags flags() const {
		return is ? is->flags() : os->flags();
	}
	std::ios_base::fmtflags flags(std::ios_base::fmtflags f) {
		return is ? is->flags(f) : os->flags(f);
	}

	std::streamsize precision() const {
		return is ? is->precision() : os->precision();
	}
	std::streamsize precision(std::streamsize p) const {
		return is ? is->precision(p) : os->precision(p);
	}

	std::streamsize width() const {
		return is ? is->width() : os->width();
	}
	std::streamsize width(std::streamsize w) const {
		return is ? is->width(w) : os->width(w);
	}

	std::locale getloc() const {
		return is ? is->getloc() : os->getloc();
	}
	std::locale imbue(const std::locale &loc) {
		return is ? is->imbue(loc) : os->imbue(loc);
	}

	bool validid(int id) const {
		return id>=0 && id < id2ptr.size();
	}

	template<typename T>
	bool lookupptr(int id, T &ptr) const {
		if (id2ptr.size()<=id) return false;
		return PtrInfo<T>::setfrom(ptr,id2ptr[id].first,
						id2ptr[id].second);
	}

	template<typename T>
	bool addptr(int id, const T &ptr) {
		if (id != id2ptr.size()) return false;
		const char *name = PtrInfo<T>::name();
		id2ptr.push_back(std::make_pair((void*)(&ptr),name));
		void *p = PtrInfo<T>::getptr(ptr);
		ptr2id.insert(std::make_pair(p,std::make_pair(id,name)));
		return true;
	}

	// returns true if already saved
	template<typename T>
	bool findoradd(const T &ptr, int &id) {
		void *p = PtrInfo<T>::getptr(ptr);
		ptr2idT::iterator loc = ptr2id.lower_bound(p);
		if (loc==ptr2id.end() || loc->first!=p) {
			id = id2ptr.size();
			const char *name = PtrInfo<T>::name();
			id2ptr.push_back(std::make_pair((void*)(&ptr),name));
			ptr2id.insert(loc,std::make_pair(p,std::make_pair(id,name)));
			return false;
		} else {
			id = loc->second.first;
			return true;
		}
	}

private:
	std::istream *is;
	std::ostream *os;

	// const char * are the names of the pointers from PtrInfo<T>::name()
	// void* below is the address of the object pointed to
	typedef std::map<void*,std::pair<int,const char *> > ptr2idT;
	// void* below is a pointer to the pointer type
	typedef std::vector<std::pair<void*,const char *> > id2ptrT;
		
	ptr2idT ptr2id;
	id2ptrT id2ptr;
};

template<typename T>
struct ArchiveInfo {
	XMLSERIAL_DECVAL(isarchive,false);
	XMLSERIAL_DECVAL(hasptrcache,false);
};

template<>
struct ArchiveInfo<XMLSERIALNAMESPACE::archive> {
	XMLSERIAL_DECVAL(isarchive,true);
	XMLSERIAL_DECVAL(hasptrcache,true);
};

}

#endif // of file guard
