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

#ifndef XMLSERIAL_FOREACH_H
#define XMLSERIAL_FOREACH_H

#define ARG_N( \
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N

#define ARG_N_SUB1( \
     _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N

#define PP_OUT(macro) \
	macro ## 63,macro ## 62,macro ## 61,macro ## 60, \
	macro ## 59,macro ## 58,macro ## 57,macro ## 56,macro ## 55,macro ## 54,macro ## 53,macro ## 52,macro ## 51,macro ## 50,\
	macro ## 49,macro ## 48,macro ## 47,macro ## 46,macro ## 45,macro ## 44,macro ## 43,macro ## 42,macro ## 41,macro ## 40,\
	macro ## 39,macro ## 38,macro ## 37,macro ## 36,macro ## 35,macro ## 34,macro ## 33,macro ## 32,macro ## 31,macro ## 30,\
	macro ## 29,macro ## 28,macro ## 27,macro ## 26,macro ## 25,macro ## 24,macro ## 23,macro ## 22,macro ## 21,macro ## 20,\
	macro ## 19,macro ## 18,macro ## 17,macro ## 16,macro ## 15,macro ## 14,macro ## 13,macro ## 12,macro ## 11,macro ## 10,\
	macro ## 9,macro ## 8,macro ## 7,macro ## 6,macro ## 5,macro ## 4,macro ## 3,macro ## 2,macro ## 1,macro ## 0

#define CPPX_INVOKE(macro,args) macro args
#define CPPX_INVOKE_N(macro,args) macro args
#define CPPX_INVOKE_N2(macro,args) macro args
#define CPPX_INVOKE_N3(macro,args) macro args
#define CPPX_INVOKE_N4(macro,args) macro args
#define CPPX_INVOKE_N10(macro,args) macro args
#define CPPX_INVOKE_N11(macro,args) macro args
#define CPPX_INVOKE_N12(macro,args) macro args
#define CPPX_INVOKE_N20(macro,args) macro args
#define CPPX_INVOKE_N21(macro,args) macro args
#define CPPX_INVOKE_N22(macro,args) macro args
#define CPPX_INVOKE_N30(macro,args) macro args
#define CPPX_INVOKE_N31(macro,args) macro args
#define CPPX_INVOKE_N32(macro,args) macro args

#define PP_NARG_(...) CPPX_INVOKE(ARG_N,(__VA_ARGS__))
#define PP_NARG(macro,...) PP_NARG_(__VA_ARGS__,PP_OUT(macro))
#define PP_NARG__N(...) CPPX_INVOKE_N(ARG_N,(__VA_ARGS__))
#define PP_NARG_N(macro,...) PP_NARG__N(__VA_ARGS__,PP_OUT(macro))
#define PP_NARG__N2(...) CPPX_INVOKE_N2(ARG_N,(__VA_ARGS__))
#define PP_NARG_N2(macro,...) PP_NARG__N2(__VA_ARGS__,PP_OUT(macro))

// see http://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
#define HASCOMMA(...) \
		ARG_N(__VA_ARGS__,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0)

#define TRIGPAREN(...) ,

#define HASNOARG(...) HASCOMMA(TRIGPAREN __VA_ARGS__ (/**/))
#define HASNOARG2(...) HASCOMMA(TRIGPAREN __VA_ARGS__ )
#define HASNOARG3(...) HASCOMMA(__VA_ARGS__ )
#define HASNOARG4(...) HASCOMMA(__VA_ARGS__ (/**/))

#define IFEMPTY(macronoargs,macroargs,test,...) \
	CPPX_INVOKE_N30(CAT(IFEMPTY_I,CAT_N(HASNOARG(STRIPPAREN(test)),HASNOARG2(STRIPPAREN(test)))),(macronoargs,macroargs,__VA_ARGS__))

#define IFEMPTY_N(macronoargs,macroargs,test,...) \
	CPPX_INVOKE_N31(CAT(IFEMPTY_I_N,CAT_N(HASNOARG(STRIPPAREN(test)),HASNOARG2(STRIPPAREN(test)))),(macronoargs,macroargs,__VA_ARGS__))

#define IFEMPTY_N2(macronoargs,macroargs,test,...) \
	CPPX_INVOKE_N32(CAT(IFEMPTY_I_N2,CAT_N(HASNOARG(STRIPPAREN(test)),HASNOARG2(STRIPPAREN(test)))),(macronoargs,macroargs,__VA_ARGS__))

#define IFEMPTY_I00(macronoargs,macroargs,...)\
		CPPX_INVOKE_N20(macroargs,(__VA_ARGS__))
#define IFEMPTY_I01(macronoargs,macroargs,...)\
		CPPX_INVOKE_N20(macroargs,(__VA_ARGS__))
#define IFEMPTY_I10(macronoargs,macroargs,...) \
		CPPX_INVOKE_N20(macronoargs,(__VA_ARGS__))
#define IFEMPTY_I11(macronoargs,macroargs,...) \
		CPPX_INVOKE_N20(macroargs,(__VA_ARGS__))

#define IFEMPTY_I_N00(macronoargs,macroargs,...)\
		CPPX_INVOKE_N21(macroargs,(__VA_ARGS__))
#define IFEMPTY_I_N01(macronoargs,macroargs,...)\
		CPPX_INVOKE_N21(macroargs,(__VA_ARGS__))
#define IFEMPTY_I_N10(macronoargs,macroargs,...) \
		CPPX_INVOKE_N21(macronoargs,(__VA_ARGS__))
#define IFEMPTY_I_N11(macronoargs,macroargs,...) \
		CPPX_INVOKE_N21(macroargs,(__VA_ARGS__))

#define IFEMPTY_I_N200(macronoargs,macroargs,...)\
		CPPX_INVOKE_N22(macroargs,(__VA_ARGS__))
#define IFEMPTY_I_N201(macronoargs,macroargs,...)\
		CPPX_INVOKE_N22(macroargs,(__VA_ARGS__))
#define IFEMPTY_I_N210(macronoargs,macroargs,...) \
		CPPX_INVOKE_N22(macronoargs,(__VA_ARGS__))
#define IFEMPTY_I_N211(macronoargs,macroargs,...) \
		CPPX_INVOKE_N22(macroargs,(__VA_ARGS__))

/*
#define CALLVAR(macro,...) \
	CPPX_INVOKE(PP_NARG(macro,__VA_ARGS__),(__VA_ARGS__))
#define CALLVAR_N(macro,...) \
	CPPX_INVOKE_N(PP_NARG_N(macro,__VA_ARGS__),(__VA_ARGS__))
#define CALLVAR_N2(macro,...) \
	CPPX_INVOKE_N2(PP_NARG_N2(macro,__VA_ARGS__),(__VA_ARGS__))
*/

#define CALLVAR(macro,...) \
	CPPX_INVOKE_N10(IFEMPTY_N,(CALLVAR_I_1,CALLVAR_I_0,(__VA_ARGS__),macro,__VA_ARGS__))
#define CALLVAR_N(macro,...) \
	CPPX_INVOKE_N11(IFEMPTY_N2,(CALLVAR_I_N_1,CALLVAR_I_N_0,(__VA_ARGS__),macro,__VA_ARGS__))
#define CALLVAR_N2(macro,...) \
	CPPX_INVOKE_N12(IFEMPTY,(CALLVAR_I_N2_1,CALLVAR_I_N2_0,(__VA_ARGS__),macro,__VA_ARGS__))

#define CALLVAR_I_0(macro,...) \
	CPPX_INVOKE(PP_NARG(macro,__VA_ARGS__),(__VA_ARGS__))
#define CALLVAR_I_N_0(macro,...) \
	CPPX_INVOKE_N(PP_NARG_N(macro,__VA_ARGS__),(__VA_ARGS__))
#define CALLVAR_I_N2_0(macro,...) \
	CPPX_INVOKE_N2(PP_NARG_N2(macro,__VA_ARGS__),(__VA_ARGS__))
#define CALLVAR_I_1(macro,...) CPPX_INVOKE(CAT(macro,0),())
#define CALLVAR_I_N_1(macro,...) CPPX_INVOKE_N(CAT(macro,0),())
#define CALLVAR_I_N2_1(macro,...) CPPX_INVOKE_N2(CAT(macro,0),())

#define PP_NARG_SUB1_(...) CPPX_INVOKE(ARG_N_SUB1,(__VA_ARGS__))
#define PP_NARG_SUB1(macro,...) PP_NARG_SUB1_(__VA_ARGS__,PP_OUT(macro))
#define CALLVAR_SUB1(macro,...) CPPX_INVOKE(PP_NARG_SUB1(macro,__VA_ARGS__),(__VA_ARGS__))

#define FOREACH0()
#define FOREACH1(macro)
#define FOREACH2(macro,arg) macro(arg)
#define FOREACH3(macro,arg,...) \
	macro(arg) FOREACH2(macro,__VA_ARGS__)
#define FOREACH4(macro,arg,...) \
	macro(arg) FOREACH3(macro,__VA_ARGS__)
#define FOREACH5(macro,arg,...) \
	macro(arg) FOREACH4(macro,__VA_ARGS__)
#define FOREACH6(macro,arg,...) \
	macro(arg) FOREACH5(macro,__VA_ARGS__)
#define FOREACH7(macro,arg,...) \
	macro(arg) FOREACH6(macro,__VA_ARGS__)
#define FOREACH8(macro,arg,...) \
	macro(arg) FOREACH7(macro,__VA_ARGS__)
#define FOREACH9(macro,arg,...) \
	macro(arg) FOREACH8(macro,__VA_ARGS__)
#define FOREACH10(macro,arg,...) \
	macro(arg) FOREACH9(macro,__VA_ARGS__)
#define FOREACH11(macro,arg,...) \
	macro(arg) FOREACH10(macro,__VA_ARGS__)
#define FOREACH12(macro,arg,...) \
	macro(arg) FOREACH11(macro,__VA_ARGS__)
#define FOREACH13(macro,arg,...) \
	macro(arg) FOREACH12(macro,__VA_ARGS__)
#define FOREACH14(macro,arg,...) \
	macro(arg) FOREACH13(macro,__VA_ARGS__)
#define FOREACH15(macro,arg,...) \
	macro(arg) FOREACH14(macro,__VA_ARGS__)
#define FOREACH16(macro,arg,...) \
	macro(arg) FOREACH15(macro,__VA_ARGS__)
#define FOREACH17(macro,arg,...) \
	macro(arg) FOREACH16(macro,__VA_ARGS__)
#define FOREACH18(macro,arg,...) \
	macro(arg) FOREACH17(macro,__VA_ARGS__)
#define FOREACH19(macro,arg,...) \
	macro(arg) FOREACH18(macro,__VA_ARGS__)
#define FOREACH20(macro,arg,...) \
	macro(arg) FOREACH19(macro,__VA_ARGS__)
#define FOREACH21(macro,arg,...) \
	macro(arg) FOREACH20(macro,__VA_ARGS__)
#define FOREACH22(macro,arg,...) \
	macro(arg) FOREACH21(macro,__VA_ARGS__)
#define FOREACH23(macro,arg,...) \
	macro(arg) FOREACH22(macro,__VA_ARGS__)
#define FOREACH24(macro,arg,...) \
	macro(arg) FOREACH23(macro,__VA_ARGS__)
#define FOREACH25(macro,arg,...) \
	macro(arg) FOREACH24(macro,__VA_ARGS__)
#define FOREACH26(macro,arg,...) \
	macro(arg) FOREACH25(macro,__VA_ARGS__)
#define FOREACH27(macro,arg,...) \
	macro(arg) FOREACH26(macro,__VA_ARGS__)
#define FOREACH28(macro,arg,...) \
	macro(arg) FOREACH27(macro,__VA_ARGS__)
#define FOREACH29(macro,arg,...) \
	macro(arg) FOREACH28(macro,__VA_ARGS__)
#define FOREACH30(macro,arg,...) \
	macro(arg) FOREACH29(macro,__VA_ARGS__)
#define FOREACH31(macro,arg,...) \
	macro(arg) FOREACH30(macro,__VA_ARGS__)

#define FOREACH(macro,...) \
	CALLVAR(FOREACH,macro,__VA_ARGS__)

#define FOREACHARG0()
#define FOREACHARG1(macro)
#define FOREACHARG2(macro,arg1)
#define FOREACHARG3(macro,arg1,arg) CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg))
#define FOREACHARG4(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG3(macro,arg1,__VA_ARGS__)
#define FOREACHARG5(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG4(macro,arg1,__VA_ARGS__)
#define FOREACHARG6(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG5(macro,arg1,__VA_ARGS__)
#define FOREACHARG7(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG6(macro,arg1,__VA_ARGS__)
#define FOREACHARG8(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG7(macro,arg1,__VA_ARGS__)
#define FOREACHARG9(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG8(macro,arg1,__VA_ARGS__)
#define FOREACHARG10(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG9(macro,arg1,__VA_ARGS__)
#define FOREACHARG11(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG10(macro,arg1,__VA_ARGS__)
#define FOREACHARG12(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG11(macro,arg1,__VA_ARGS__)
#define FOREACHARG13(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG12(macro,arg1,__VA_ARGS__)
#define FOREACHARG14(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG13(macro,arg1,__VA_ARGS__)
#define FOREACHARG15(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG14(macro,arg1,__VA_ARGS__)
#define FOREACHARG16(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG15(macro,arg1,__VA_ARGS__)
#define FOREACHARG17(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG16(macro,arg1,__VA_ARGS__)
#define FOREACHARG18(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG17(macro,arg1,__VA_ARGS__)
#define FOREACHARG19(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG18(macro,arg1,__VA_ARGS__)
#define FOREACHARG20(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG19(macro,arg1,__VA_ARGS__)
#define FOREACHARG21(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG20(macro,arg1,__VA_ARGS__)
#define FOREACHARG22(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG21(macro,arg1,__VA_ARGS__)
#define FOREACHARG23(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG22(macro,arg1,__VA_ARGS__)
#define FOREACHARG24(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG23(macro,arg1,__VA_ARGS__)
#define FOREACHARG25(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG24(macro,arg1,__VA_ARGS__)
#define FOREACHARG26(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG25(macro,arg1,__VA_ARGS__)
#define FOREACHARG27(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG26(macro,arg1,__VA_ARGS__)
#define FOREACHARG28(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG27(macro,arg1,__VA_ARGS__)
#define FOREACHARG29(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG28(macro,arg1,__VA_ARGS__)
#define FOREACHARG30(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG29(macro,arg1,__VA_ARGS__)
#define FOREACHARG31(macro,arg1,arg,...) \
	CPPX_INVOKE_N(macro,(STRIPPAREN(arg1),arg)) FOREACHARG30(macro,arg1,__VA_ARGS__)
#define FOREACHARG32(macro,arg1,arg,...) \
	macro(STRIPPAREN(arg1),arg) FOREACHARG31(macro,arg1,__VA_ARGS__)

#define FOREACHARG(macro,args,...) CALLVAR(FOREACHARG,macro,args,__VA_ARGS__)

// see http://boost.2283326.n4.nabble.com/preprocessor-removing-parentheses-td2591973.html
#define STRIPPAREN(x) EVAL((STRIPPAREN_I x),x)
#define STRIPPAREN_I(...) 1,1

#define EVAL(test,x) EVAL_I(test,x)
#define EVAL_I(test,x) MAYBESTRIP(TESTARITY test,x)

#define TESTARITY(...) CPPX_INVOKE_N2(TESTARITY_I,(__VA_ARGS__,2,1))
#define TESTARITY_I(a,b,c,...) c

#define CAT(x,y) CAT_I(x,y)
#define CAT_I(x,y) x ## y
#define CAT_N(x,y) CAT_N_I(x,y)
#define CAT_N_I(x,y) x ## y

#define MAYBESTRIP(cond,x) MAYBESTRIP_I(cond,x)
#define MAYBESTRIP_I(cond,x) CAT(MAYBESTRIP_,cond)(x)

#define MAYBESTRIP_1(x) x
#define MAYBESTRIP_2(x) CPPX_INVOKE_N2(MAYBESTRIP_2_I,x)
#define MAYBESTRIP_2_I(...) __VA_ARGS__


#endif

