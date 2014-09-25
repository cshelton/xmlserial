xmlserial
=========

This code (set of marcos, and the templates and functions they invoke) will automatically set up serialization code for a class.  The serialization input/output is in an (almost) XML format (not necessarily a single container, and no "?xml..." first line).  In particular it will supply a Save and Load method for each class (which take a std::ostream and std::istream respectively and have return type void).  It will also declare a static member function LoadPtr that takes no arguments and returns a (newly allocated) pointer to the type.  If the type is polymorphic (see below), then that pointer could be to any derived type.  If the type is polymorphic it will also declare a virtual method SaveV like Save that will save the true type of the object (no slicing).

Additionally, for classes declared to this code as polymorphic, any pointer to the class will be loaded as its true class (determined at runtime), either the type of the pointer or a subclass thereof. 

If polymorphism is not used, then no virtual methods are created.  If it is used, they are created and you need to make sure your destructor is virtual.

There are two requirements on the class in order to use it with this code:

1.  There must be a constructor that takes no arguments (a default constructor)
2.  All member data or base classes that you wish to serialize must satistfy one of the following:
  1.  use this code for serialization
  2.  be a basic data type (int, double, char, etc.)
  3.  be an STL class of a type T that meets this requirement (currently pair<T1,T2>, set<T>, map<T1,T2>, or vector<T>)
  4.  be an STL string
  5.  be a pointer to a datatype that used this code for serialization

Note that this code, if used with archives, will serialize correctly structures in which two pointers point to the same object.  If used with a normal istream/ostream, it will not.  It also supports C++11 unique_ptr and shared_ptr.  It does not current support C++11 weak_ptr.

USAGE (C++11):
--------------
* Usage for C++03 is given below.  That method also works in C++11, but there
is usually a simpler syntax in C++11:

   ```c++
   class MyClass {
   public:
      MyClass() = default; // make sure to has a zero-arg constructor
      // rest of the interface
   private:
      int x;
      double y;
      std::string s;

      XMLSERIALCLASS(MyClass,,x,y,s)
   };
   ```

   *The macro XMLSERIALCLASS must be last*.  Its first argument is the name of the class.  The second argument (which may be blank, as above) is the name of the base class.  The remaining arguments (up to 30) are the members to be serialized.  If more than 30 are needed, use the C++03 syntax below.

* Base classes are placed in the second argument.  If there are more than one, they are in a parenthesized list.  A maximum of one may be a polymorphic class (see below).

   ```c++
   class ClassA {
   public:
      ClassA(); // be sure to have a default constructor
      // ... rest of declaration
   private:
      int a;
   XMLSERIALCLASS(ClassA,a)
   };
   class ClassB : public ClassA {
   public:
      ClassB(); // be sure to have a default constructor
      // ... rest of declaration
   private:
      float f;
   XMLSERIALCLASS(ClassB,ClassA,f)
   };

   class ClassA2 {
   public:
      ClassA2(); // be sure to have a default constructor
      // ... rest of declaration
   private:
      int a2;
   XMLSERIALCLASS(ClassA,,a2)
   };
   class ClassB2 : public ClassA, public ClassA2 {
   public:
      ClassB(); // be sure to have a default constructor
      // ... rest of declaration
   private:
      float f;
   XMLSERIALCLASS(ClassB,(ClassA,ClassA2),f)
   };
   ```

* If the class is polymorphic (that is, at load time a pointer to the class might refer to a subclass), use XMLSERIALCLASS_P instead (same syntax) on it and all subclasses.

* If the class is a template, the first argument should be a comma-separated list in parentheses.  The first element is the name of the class and the subsequent elements are the formal template parameter names.  If base classes have commas (because they are templates), they can be named by using a typedef (or using) declaration.

    ```c++
    template<typename I, typename T>
    class TemplateMap {
        typedef std::map<I,T> mymap;
    public:
        TemplateMap<I,T>(); // be sure to have a default constructor
        // ... rest of declaration
    private:
        mymap m;
        XMLSERIALCLASS2((TemplateMap,I,T),,m)
    };
    
    template<typename C>
    class CMap : public TemplateMap<C,std::string> {
        typedef TemplateMap<C,std::string> Base;
    public:
        CMap(); // be sure to have a default constructor
        // ... rest of declaration
        C x,y,z;
        XMLSERIALCLASS1((CMap,C),Base,x,y,z)
    };
    ```


* The other arguments (local variables to be saved) can be similarly augmented (by wrapping them in parentheses) to supply the field-name, a default value (for loading if none supplied in the stream/archive), and the type (should automatic type deduction not work?):

   ```c++
   class A {
   public:
      A(); // be sure to have a default constructor
   private:
      int a,b,c,d;
   XMLSERIALCLASS(A,,a,(b,"second int"),(c,"third int",-10),(d,"fourth int",90,int))
   };
   ```
   
   This creates save/load for member variables a (with name "a" and no default value), b (with name "second int" and no default value), c (with name "third int" and a default load value of -10), and d (with name "fourth int", a default of 90, and a confirmation that it should be an int).  Like default arguments in C++, they must be given in order and only trailing ones can be left unspecified.  If no default value is given, the stream/archive for loading must have a value specified.  The last version (with all four arguments) should not be necessary.


USAGE (C++03):
--------------

The usage is slightly more complex under C++03 because the types cannot be inferred (and so must be specified).  However, the use is slightly more flexible (for instance, allowing default values for base classes), and the macros may be placed anywhere in the class's definition.

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

*Note:* If you put a semicolon (;) after one of the macros, you will see strange compile error (usually something about how "xmlserial_xxxxlst" does not name a type).  Don't do this :)

The order of the member list in the "XMLSERIAL_" section will dictate the order in which they are written to the output.  However, input can be organized in any order!  The members themselves can be declared in any order.

More advanced usage cases are as follows.

* If you would like to specify a default value for a member variable or base class (see below), use "XMLSERIAL_VAR_DEFAULT" instead of XMLSERIAL_VAR (or XMLSERIAL_BASE_DEFAULT instead of XMLSERIAL_BASE).  It takes an extra argument: the value to assign to the member or base class if the value is not given on the input stream.

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

* If the class has a base class that needs to be serialized, all such base classes should be declared using XMLSERIAL_BASE which takes a single argument: the type of the base class
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
                XMLSERIAL_BASE(ClassA)
            XMLSERIAL_VAR(float,f)
        XMLSERIAL_END
    public:
        ClassB(); // be sure to have a default constructor
        // ... rest of declaration
    private:
        float f;
    };
    ```
    Multiple inheritance is okay (but see below): use XMLSERIAL_BASE multiple times. There are also "_N" versions of these macros (see point above).  Only one of the base classes may be polymorphic (see below).

* To declare a class to be polymorphic and able to load and save pointers to it or any derived type with the types being determined at *runtime*, all such classes should use XMLSERIAL_START_P instead of XMLSERIAL_START
    An example (modifying the example above only slightly):

    ```c++
    class ClassA {
        XMLSERIAL_START_P(ClassA)
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
        XMLSERIAL_START_P(ClassB)
            XMLSERIAL_BASE(ClassA)
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
        XMLSERIAL_START_P(ClassC)
            XMLSERIAL_BASE(ClassA)
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
    In this example, the member p in OtherClass might point to a ClassA, ClassB, or ClassC.  Because ClassA, ClassB, and ClassC used XMLSERIAL_START_P (instead of XMLSERIAL_START), this pointer will be correctly saved and loaded, no matter which class it actually points to.  If you have multiple inheritance, only one of the base classes may be declared as polymorphic in this fashion.  As an example, if ClassC also derived from ClassX, then only one of ClassA and ClassX could be declared with "XMLSERIAL_START_P" (the other would have to be just "XMLSERIAL_START"), but both could be included with "XMLSERIAL_BASE" in ClassC.
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
    If you wish to also make this class polymorphic, use XMLSERIAL_STARTn_P instead (for example, XMLSERIAL_START1_P above).

* To declare a member variable or a base class for a type that contains a comma (like a template with multiple arguments), use a typedef to give the type a new name.  Some examples:

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
    
    template<typename C>
    class CMap : public TemplateMap<char,std::string> {
        typedef TemplateMap<char,std::string> Base;
        XMLSERIAL_START(CMap)
            XMLSERIAL_BASE(Base)
            XMLSERIAL_VAR(C,x)
            XMLSERIAL_VAR(C,y)
            XMLSERIAL_VAR(C,z)
        XMLSERIAL_END
    public:
        C x,y,z;
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

    For classes like this, it is not possible to make them polymorphic.  There are template versions (named XMLSERIAL_USESHIFTn and XMLSERIAL_USESHIFTn_SHORT -- where n is the number of template args)


A few final points:
- The loading methods (Load) can throw a streamexception.
- All extra classes and functions are placed in their own namespace.  The name defaults to xmlserial.  However, if you define the compile symbol XMLSERIALNAMESPACE, you can change the namespace.  Inside of the class, extra types and methods are added.  These all begin with xmlserial_ or XMLSERIAL_.  This header defines compiler symbols (like macros) that all begin with XMLSERIAL_.
- If you do not directly use a class in your code (that is create an object of its type at some point), the class will not be available for polymorphic loading.

  To do this, include somewhere in your code (a .cpp file is better than a .h file, as this creates a single pointer, and you only need one of them) the macro ENSURECLASS(HiddenClass) which will cause the class HiddenClass to be registered.  There are templated version of this too (ENSURECLASSn), but the extra arguments are not template formal parameter, but template actual parameters.  As an example:

    ```c++
    template<typename T>
    class TClass : public ParentClass {
        XMLSERIAL_START1_P(TClass,T)
        XMLSERIAL_BASE(ParentClass)
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

Serialization:
--------------
Objects can then be saved and loaded with either a regular C++ stream or with an archive (xmlserial_archive.h).  The archive has a single-argument constructor of either an istream, an ostream, or an iostream after which it acts more-or-less like the stream upon which it was built.  Note that the stream's lifetime must be longer than that of the archive.  Archives have the advantage that they can remember pointers saved or loaded already so that the references objects are not duplicated, but rather the original pointer semantics are maintained (although the absolute value of the pointers, are, of course, not).

To save a non-pointer object o of a type modified as above, call o.Save(s) where s is either an ostream or an archive constrcuted from one.  To load the object back, call o.Load(s).

For objects not so modified (like pointers, smart pointers, C++ reference library objects), call xmlserial::Save(o,s) where o is the object and s is the ostream or archive.  To load, similiarly call xmlserial::Load(o,s).



Differences from Boost Serialization package:
---------------------------------------------
- the boost package works from an "archive" and asks that you write a single serialization function.  This gives it a few advanatages:
  1. You can write the same "loader" and "saver"
  2. It can do versioning
  3. The archive can dictate the format (ASCII, Binary, XML)
  4. It doesn't require (many) icky macros
- this package uses macros.  This gives it a few advantages:
  1. Can use standard istream and ostream
  2. You don't have to write any functions (just list the state to be serialized)
  3. The resulting XML can be read in even if the fields are rearranged!
  4. Classes can be more flexible in how they serialize (for instance, we can use << and >> for some "simple" classes)
  5. This package is header only (no linking!)

This package was originally written before the serialization library as offically part of boost.  Early versions of that did not have good support for determining the type of an object are load time (for polymorphic classes -- classes with subclasses).  The boost library has some support for pointers that point to the same object.  This package also has this support.

Differences from Cereal:
------------------------
- Cereal works much like boost serialization.  It has many of the same advantages (see above), plus some others:
  1. The syntax is a little simpler than boost serialization
  2. The linking is much easier than boost serialization (same as this package)
  3. The archive can dictate the format
  4. Can do versioning
- This package has the same advantages as over boost serialization, plus others:
  1. Can use standard istream and ostream (assuming you don't want pointer serialization).
  2. Can serialize raw pointers (assumes they are like unique_ptrs)
  3. Classes can be more flexible in how they serialize (use << and >>, for example, or place value in a value= attribute)
  4. Classes (except for some template classes) do not need to be registered for polymorphism!
  5. Does not require C++11

This package also supports polymorphism (like cereal) and smart pointers (like cereal).
