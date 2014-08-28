xmlserial
=========

This code (set of marcos, and the templates and functions they invoke) will automatically set up serialization code for a class.  The serialization input/output is in an (almost) XML format (not necessarily a single container, and no "?xml..." first line).  In particular it will supply a Save and Load method for each class (which take a std::ostream and std::istream respectively and have return type void).  It will also declare a static member function LoadPtr that takes no arguments and returns a (newly allocated) pointer to the type.  If the type is "virtual" (see below), then that pointer could be to any derived type.  If the type is "virtual" it will also declare a virtual method SaveV like Save that will save the true type of the object (no slicing).

Additionally, for classes declared to this code as "virtual," any pointer to the class will be loaded as its true class (determined at runtime), either the type of the pointer or a subclass thereof. 

If "virtual" is not used, then no virtual methods are created.  If it is used, they are created and you need to make sure your destructor is virtual.

There are two requirements on the class in order to use it with this code:

1.  There must be a constructor that takes no arguments (a default constructor)
2.  All member data or super-classes that you wish to serialize must satistfy one of the following:
  1.  use this code for serialization
  2.  be a basic data type (int, double, char, etc.)
  3.  be an STL class of a type T that meets this requirement (currently pair<T1,T2>, set<T>, map<T1,T2>, or vector<T>)
  4.  be an STL string
  5.  be a pointer to a datatype that used this code for serialization

Note that this code will not correctly serialize data structures in which two pointers point to the same object.  It assumes that all pointers point to unique locations.  This is a planned improvement.

USAGE:
------

The basic usage for a simple class is illustrated as below.
```c++
class MyClass {
	XMLSERIAL_START(MyClass)
		XMLSERIAL_VAR(int,x)
		XMLSERIAL_VAR(double,y)
		XMLSERIAL_VAR(std::string,s)
	XMLSERIAL_END
public:
	MyClass(); // make sure to have a default constructor
	//... rest of interface ...
private:
	int x; // all members mentioned above for serialization
	double y; // must be declared
	std::string s; // (they are "private" here, but that is 
			// not required.  Order doesn't matter
};
```

Somewhere in the class declaration, include the macro "XMLSERIAL_START" with the argument of the class name.  Then, list the member variables to be serialized with "XMLSERIAL_VAR" which takes two arguments: the member type and the member name.  Finally, finish the serialization declarations with "XMLSERIAL_END" (which takes no arguments.  The XMLSERIAL_END macro leaves the code in the "public:" state.  However, to be safe, you should always place a "public:" "private:" or "protected:" right after it.

The order of the member list in the "XMLSERIAL_" section will dictate the order in which they are written to the output.  However, input can be organized in any order!  The members themselves can be declared in any order.

More advanced usage cases are as follows.

* If you would like to specify a default value for a member variable or super class (see below), use "XMLSERIAL_VAR_DEFAULT" instead of XMLSERIAL_VAR (or XMLSERIAL_SUPER_DEFAULT instead of XMLSERIAL_SUPER).  It takes an extra argument: the value to assign to the member or super class if the value is not given on the input stream.

    An example:
    ```c++
class ClassA {
	XMLSERIAL_START(ClassA)
		XMLSERIAL_VAR_DEFAULT(int,a,5) // if a is not mentioned on
						// an input stream, set it to 5
	XMLSERIAL_END
public:
	ClassA(); // be sure to have a default constructor
	// ... rest of declaration
private:
	int a;
};
    ```

* If you would like to specify a different name under which the member should be saved, you can use the XMLSERIAL_VAR_N and XMLSERIAL_VAR_DEFAULT_N macros which take one additional argument (at the end) that is a string with the name.

    An example:
    ```c++
class SaveRename {
	XMLSERIAL_START(SaveRename)
		XMLSERIAL_VAR_N(int,a,"important integer")
		XMLSERIAL_VAR_DEFAULT_N(double,d,3.14,"double member")
	XMLSERIAL_END
public:
	SaveRename(); // be sure to have a default constructor
	// ... rest of declaration
private:
	int a;
	double d;
    };
```
    Be careful!  You must make sure that all members of the same class have different names!

* If the class has a super-class that needs to be serialized, all such super classes should be declared using XMLSERIAL_SUPER which takes a single argument: the type of the superclass
    An example:

    ```c++
class ClassA {
	XMLSERIAL_START(ClassA)
		XMLSERIAL_VAR(int,a)
	XMLSERIAL_END
public:
	ClassA(); // be sure to have a default constructor
	// ... rest of declaration
private:
	int a;
};
class ClassB : public ClassA {
	XMLSERIAL_START(ClassB)
		XMLSERIAL_SUPER(ClassA)
		XMLSERIAL_VAR(float,f)
	XMLSERIAL_END
public:
	ClassB(); // be sure to have a default constructor
	// ... rest of declaration
private:
	float f;
};
    ```
    Multiple inheritance is okay (but see below): use XMLSERIAL_SUPER multiple times. There are also "_N" versions of these macros (see point above)

* To declare a class to be "virtual" and able to load and save pointers to it or any derived type with the types being determined at *runtime*, all such classes should use XMLSERIAL_START_V instead of XMLSERIAL_START
    An example (modifying the example above only slightly):

    ```c++
class ClassA {
	XMLSERIAL_START_V(ClassA)
		XMLSERIAL_VAR(int,a)
	XMLSERIAL_END
public:
	ClassA(); // be sure to have a default constructor
	virtual ~ClassA(); // be sure to have a virtual destructor
	// ... rest of declaration
private:
	int a;
};
	
class ClassB : public ClassA {
	XMLSERIAL_START_V(ClassB)
		XMLSERIAL_SUPER(ClassA)
		XMLSERIAL_VAR(float,f)
	XMLSERIAL_END
public:
	ClassB(); // be sure to have a default constructor
	virtual ~ClassB(); // be sure to have a virtual destructor
	// ... rest of declaration
private:
	float f;
};
class ClassC : public ClassA {
	XMLSERIAL_START_V(ClassC)
		XMLSERIAL_SUPER(ClassA)
		XMLSERIAL_VAR(double,d)
	XMLSERIAL_END
public:
	ClassC(); // be sure to have a default constructor
	virtual ~ClassC(); // be sure to have a virtual destructor
	// ... rest of declaration
private:
	double d;
};
class OtherClass {
	XMLSERIAL_START(OtherClass)
		XMLSERIAL_VAR(ClassA *,p)
	XMLSERIAL_END
public:
	OtherClass(); // be sure to have a default constructor
	// ... rest of declaration
private:
	ClassA *p;
};
    ```
    In this example, the member p in OtherClass might point to a ClassA, ClassB, or ClassC.  Because ClassA, ClassB, and ClassC used XMLSERIAL_START_V (instead of XMLSERIAL_START), this pointer will be correctly saved and loaded, no matter which class it actually points to.  If you have multiple inheritance, only one of the superclasses may be declared as "virtual" in this fashion.  As an example, if ClassC also derived from ClassX, then only one of ClassA and ClassX could be declared with "XMLSERIAL_START_V" (the other would have to be just "XMLSERIAL_START"), but both could be included with "XMLSERIAL_SUPER" in ClassC.
* To declare a template class, use the macro XMLSERIAL_STARTn instead of XMLSERIAL_START (where n is the number of arguments to the template). This macro takes extra arguments, one for each template parameter.
    An example:
  
    ```c++
template<typename T> class MyTempl {
	XMLSERIAL_START1(MyTempl,T)
		XMLSERIAL_VAR(int,x)
		XMLSERIAL_VAR(T,y)
	XMLSERIAL_END
public:
	MyTempl<T>(); // be sure to have a default constructor
	// ... rest of declaration
private:
	int x;
	T y;
};
    ```
    If you wish to also make this class "virtual," use XMLSERIAL_STARTn_V instead (for example, XMLSERIAL_START1_V above).

* To declare a member variable or a superclass for a type that contains a comma (like a template with multiple arguments), use a typedef to give the type a new name.  Some examples:

    ```c++
class YetAnotherClass {
	typedef std::map<int,double> mymap;
	XMLSERIAL_START(YetAnotherClass)
		XMLSERIAL_VAR(mymap,m)
	XMLSERIAL_END
public:
	YetAnotherClass(); // be sure to have a default constructor
	// ... rest of declaration
private:
	mymap m;
};
template<typename I, typename T>
class TemplateMap {
	typedef std::map<I,T> mymap;
	XMLSERIAL_START2(TemplateMap,I,T)
		XMLSERIAL_VAR(mymap,m)
	XMLSERIAL_END
public:
	TemplateMap<I,T>(); // be sure to have a default constructor
	// ... rest of declaration
private:
	mymap m;
};
class CMap : public TemplateMap<char,std::string> {
	typedef TemplateMap<char,std::string> Base;
	XMLSERIAL_START(CMap)
		XMLSERIAL_SUPER(Base)
	XMLSERIAL_END
public:
	CMap(); // be sure to have a default constructor
	// ... rest of declaration
};
    ```
* If there is some "set-up" or "clean-up" code that needs to be run before or after saving or loading (say to build caches or to compact the structure), the class can declare the following public methods, which will be run at the appropriate times:

    ```c++
    void xmlserial_preload();
    void xmlserial_postload();
    void xmlserial_presave() const;
    void xmlserial_postsave() const;
    ```
    
* To declare that your type will do its own serialization using	the shift operators (<< and >>), simply use "XMLSERIAL_USESHIFT" with the class name.  An example:

    ```c++
class Simple {
	XMLSERIAL_USESHIFT(Simple)
public:
	Simple(); // still need a default constructor
	// ... rest of declaration
	// need to make sure that somewhere the following
	// functions are declared:
	// std::istream &operator>>(std::istream &,Simple &);
	// std::ostream &operator>>(std::ostream &,const Simple &);
private:
	// whatever members are necessary -- make sure all
	// are saved/loaded by shift functions above
	// in ASCII format without using the 5 special characters
	// of XML {",',&,<,>}
};
    ```
Note that the operator<< and operator>> should *not* output characters quote("), single-quote('), ampersand(&), less-than(<), or greater-than(>).  If the output is very small, and you would like it be be placed in a "value" attribute (instead of between tags), then use the macro "XMLSERIAL_USESHIFT_SHORT" instead of "XMLSERIAL_USESHIFT" The two operators must read and write exactly the same characters (not leaving any extras on the stream or consuming extras from the stream)

For classes like this, it is not possible to make them "virtual."  There are template versions (named XMLSERIAL_USESHIFTn and XMLSERIAL_USESHIFTn_SHORT -- where n is the number of template args)


A few final points:
- The loading methods (Load) can throw a streamexception (see below).
- All extra classes and functions are placed in their own namespace.  The name defaults to xmlserial.  However, if you define the compile symbol XMLSERIALNAMESPACE, you can change the namespace.  Inside of the class, extra types and methods are added.  These all begin with xmlserial_ or XMLSERIAL_.  This header defines compiler symbols (like macros) that all begin with XMLSERIAL_.
- If you do not directly use a class in your code (that is create an object of its type at some point), the class will not be available for "virtual" loading.	To do this, include somewhere in your code (a .cpp file is better than a .h file, as this creates a single pointer, and you only need one of them) the macro ENSURECLASS(HiddenClass) which will cause the class HiddenClass to be registered.  There are templated version of this too (ENSURECLASSn), but the extra arguments are not template formal parameter, but template actual parameters.  As an example:

    ```c++
	template<typename T>
	class TClass : public ParentClass {
		XMLSERIAL_START1_V(TClass,T)
			XMLSERIAL_SUPER(ParentClass)
			//... normal macros for each member variable
		XMLSERIAL_END
	public:
		//... default constructor, etc
		//... and your class declarations/definitions
	};
	ENSURECLASS(TClass,int)
	ENSURECLASS(TClass,double)
    ```
Will make sure that TClass<int> and TClass<double> can be loaded even if they are never used in the code and you are only trying to load a pointer to ParentClass (which might point to a TClass<int> or TClass<double>).


Differences from Boost Serialization package:
---------------------------------------------
- the boost package works from an "archive" and asks that you write a single serialization function.  This gives it a few advanatages:
  1. You can write the same "loader" and "saver"
  2. It can do versioning
  3. The archive can dictate the format (ASCII, Binary, XML)
  4. It doesn't require icky macros
- this package uses macros.  This gives it a few advantages:
  1. Can use standard istream and ostream
  2. You don't have to write any functions (just list the state to be serialized)
  3. The resulting XML can be read in even if the fields are rearranged!
  4. Classes can be more flexible in how the serialize (for instance, we can use << and >> for some "simple" classes)

This package was originally written before the serialization library as offically part of boost.  Early versions of that did not have good support for determining the type of an object are load time (for polymorphic classes -- classes with subclasses).  The boost library has some support for pointers that point to the same object.  This package has no such support.  We hope to add that support by allowing archives (in addition to streams).
