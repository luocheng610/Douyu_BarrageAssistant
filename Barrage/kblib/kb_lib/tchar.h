/*******************************************************************************
* 内容摘要	: UNICODE工程和ASCII工程定义统一的字符类型以及stl字符串类型
*******************************************************************************/
#ifndef __TCHAR_H_KB__  
#define __TCHAR_H_KB__  

#include "./type.h"

#ifdef __LINUX__

#include <string>

NAMESPACE_BEGIN(kb)

typedef char            tchar;
typedef std::string     tstring;

#define TSTRING(text)   text

NAMESPACE_END(kb)

#elif WIN32

#include <string> // stl string
#include <fstream>
#include <sstream>
#include <strstream>

NAMESPACE_BEGIN(kb)

#if defined(UNICODE) || defined(_UNICODE)

typedef wchar_t         tchar;
typedef std::wstring    tstring;

typedef std::wistream   tistream;
typedef std::wostream   tostream;

typedef std::wfstream   tfstream;
typedef std::wifstream  tifstream;
typedef std::wofstream  tofstream;

typedef std::wstringstream  tstringstream;
typedef std::wistringstream tistringstream;
typedef std::wostringstream tostringstream;

#define TSTRING(text)   L##text

#else // #if defined(UNICODE) || defined(_UNICODE)

typedef char            tchar;
typedef std::string     tstring;

typedef std::istream   tistream;
typedef std::ostream   tostream;

typedef std::fstream   tfstream;
typedef std::ifstream  tifstream;
typedef std::ofstream  tofstream;

typedef std::stringstream  tstringstream;
typedef std::istringstream tistringstream;
typedef std::ostringstream tostringstream;

#define TSTRING(text)   text

#endif // #if defined(UNICODE) || defined(_UNICODE)

NAMESPACE_END(kb)

#endif // #ifdef __LINUX__ #elif WIN32

#endif // end of __TYPE_H_KB__  
