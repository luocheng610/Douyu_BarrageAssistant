/*******************************************************************************
* 内容摘要  : 用于各种常见的字符串编码转换
*******************************************************************************/
#ifndef __TRANSCODE_H_KB__
#define __TRANSCODE_H_KB__

#include "kb_lib/tchar.h"

#ifdef WIN32
#else
#include <iconv.h>
#endif

KB_NAMESPACE_BEGIN(text)

class transcode
{
public:
    static void Unicode_to_UTF8(const wchar_t* in, uint32 len, std::string& out);
 
    static void UTF8_to_Unicode(const char* in, uint32 len, std::wstring& out);
  
    static void GBK_to_UTF8(const char* in, uint32 len, std::string& out);
  
    static void UTF8_to_GBK(const char* in, uint32 len, std::string& out);

    static void Unicode_to_GBK(const wchar_t* in, uint32 len, std::string& out);

    static void GBK_to_Unicode(const char* in, uint32 len, std::wstring& out);

    static void Unicode_to_ANSI(const wchar_t* in, uint32 len, std::string& out);

    static void ANSI_to_Unicode(const char* in, uint32 len, std::wstring& out);


    static void tstring_to_UTF8(const char* in, uint32 len, std::string& out);
    static void tstring_to_UTF8(const wchar_t* in, uint32 len, std::string& out);

    static void UTF8_to_tstring(const char* in, uint32 len, std::string& out);
    static void UTF8_to_tstring(const char* in, uint32 len, std::wstring& out);

    static void tstring_to_GBK(const char* in, uint32 len, std::string& out);
    static void tstring_to_GBK(const wchar_t* in, uint32 len, std::string& out);

    static void GBK_to_tstring(const char* in, uint32 len, std::string& out);
    static void GBK_to_tstring(const char* in, uint32 len, std::wstring& out);

    static void tstring_to_Unicode(const char* in, uint32 len, std::wstring& out);
    static void tstring_to_Unicode(const wchar_t* in, uint32 len, std::wstring& out);

    static void Unicode_to_tstring(const wchar_t* in, uint32 len, std::string& out);
    static void Unicode_to_tstring(const wchar_t* in, uint32 len, std::wstring& out);

    static void tstring_to_ANSI(const char* in, uint32 len, std::string& out);
    static void tstring_to_ANSI(const wchar_t* in, uint32 len, std::string& out);

    static void ANSI_to_tstring(const char* in, uint32 len, std::string& out);
    static void ANSI_to_tstring(const char* in, uint32 len, std::wstring& out);

};

KB_NAMESPACE_END(text)

#endif // end of __TRANSCODE_H_KB__

