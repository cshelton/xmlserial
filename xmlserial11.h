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

//--------------
// C++11 Macros
//--------------
#if __cplusplus > 199711L

#include "xmlserial_foreach.h"

#define XMLSERIAL_START0_P(cname) XMLSERIAL_START_P(cname)
#define XMLSERIAL_START0(cname) XMLSERIAL_START(cname)

// base might be empty! 
#define XMLSERIAL11_BASE_I1(base) /**/
#define XMLSERIAL11_BASE_I0(base) XMLSERIAL_BASE(base)

#define XMLSERIAL11_BASE(base) \
          CPPX_INVOKE_N(CAT(XMLSERIAL11_BASE_I,HASNOARG(base)),(base))

#define XMLSERIAL11_START(cname,...) \
          CALLVAR_SUB1(XMLSERIAL_START,STRIPPAREN(cname)) \
          FOREACH(XMLSERIAL11_BASE,__VA_ARGS__)

#define XMLSERIAL11_START_P(cname,...) \
          CPPX_INVOKE(CAT(PP_NARG_SUB1(XMLSERIAL_START,STRIPPAREN(cname)),_P),(STRIPPAREN(cname))) \
          FOREACH(XMLSERIAL11_BASE,__VA_ARGS__)

#define XMLSERIAL11_VAR0() /**/
#define XMLSERIAL11_VAR1(vname) \
          XMLSERIAL_VAR(decltype(XMLSERIAL_BASETYPE::vname),vname)
#define XMLSERIAL11_VAR2(vname,savename) \
          XMLSERIAL_VAR_N(decltype(XMLSERIAL_BASETYPE::vname),vname,savename)
#define XMLSERIAL11_VAR3(vname,savename,defaultval) \
          XMLSERIAL_VAR_DEFAULT_N(decltype(XMLSERIAL_BASETYPE::vname),vname,defaultval,savename)
#define XMLSERIAL11_VAR4(vname,savename,defaultval,vtype) \
          XMLSERIAL_VAR_DEFAULT_N(vtype,vname,defaultval,savename)

#define XMLSERIAL11_CNAME(classes) \
          CPPX_INVOKE_N2(XMLSERIAL11_CNAME_I,(STRIPPAREN(classes)))
#define XMLSERIAL11_CNAME_I(classname,...) classname

#define XMLSERIAL11_VAR(vardecl) \
     CALLVAR_N2(XMLSERIAL11_VAR,STRIPPAREN(vardecl))

#define XMLSERIALCLASS(classname,bases,...) \
     XMLSERIAL11_START(classname,STRIPPAREN(bases)) \
     FOREACH(XMLSERIAL11_VAR,__VA_ARGS__) \
     XMLSERIAL_END

#define XMLSERIALCLASS_P(classname,bases,...) \
     XMLSERIAL11_START_P(classname,STRIPPAREN(bases)) \
     FOREACH(XMLSERIAL11_VAR,__VA_ARGS__) \
     XMLSERIAL_END


#endif
