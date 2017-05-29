#ifndef __TYPE_H_KB  
#define __TYPE_H_KB  

// 定义命名空间
#define NAMESPACE_BEGIN(a) namespace a {
#define NAMESPACE_END(a) }

// 定义KB命名空间
#define KB_NAMESPACE_BEGIN(a) namespace kb { namespace a {
#define KB_NAMESPACE_END(a) } }

// 定义KB_Linux命名空间
#define KB_LINUX_NAMESPACE_BEGIN(a) namespace kb { namespace lnx { namespace a {
#define KB_LINUX_NAMESPACE_END(a) } } }

// 定义KB_Windows命名空间
#define KB_WIN32_NAMESPACE_BEGIN(a) namespace kb { namespace win32 { namespace a {
#define KB_WIN32_NAMESPACE_END(a) } } }

#ifdef __LINUX__

#include <sys/types.h>

NAMESPACE_BEGIN(kb)

#define MAX_PATH_LEN 512

#define SINT8_MAX	127
#define SINT8_MIN	(-128)
#define UINT8_MAX	255

#define SINT16_MAX 32767
#define SINT16_MIN (-32768)
#define UINT16_MAX 65535

#define SINT32_MAX 2147483647
#define SINT32_MIN (-2147483648)
#define UINT32_MAX 4294967295U

#define SINT64_MAX 9223372036854775807LL
#define SINT64_MIN (-9223372036854775808LL)
#define UINT64_MAX 18446744073709551615ULL

typedef unsigned char   byte;
typedef unsigned char   uint8;
typedef signed char     sint8;
typedef u_int16_t       uint16;
typedef int16_t         sint16;
typedef u_int32_t       uint32;
typedef int32_t         sint32;
typedef u_int64_t       uint64;
typedef int64_t         sint64;
typedef float           float32;
typedef double          float64;

NAMESPACE_END(kb)

#elif WIN32

#include <limits.h>

NAMESPACE_BEGIN(kb)

typedef unsigned char       byte;
typedef unsigned __int8     uint8;
typedef signed __int8       sint8;
typedef unsigned short      uint16;
typedef signed short        sint16;
typedef unsigned long       uint32;
typedef signed long         sint32;
typedef unsigned __int64    uint64;
typedef signed __int64      sint64;
typedef float               float32;
typedef double              float64;

#define SINT8_MIN           _I8_MIN
#define SINT8_MAX           _I8_MAX
#define UINT8_MAX           _UI8_MAX

#define SINT16_MIN          _I16_MIN
#define SINT16_MAX          _I16_MAX
#define UINT16_MAX          _UI16_MAX

#define SINT32_MIN          _I32_MIN
#define SINT32_MAX          _I32_MAX
#define UINT32_MAX          _UI32_MAX

#define SINT64_MIN          _I64_MIN
#define SINT64_MAX          _I64_MAX
#define UINT64_MAX          _UI64_MAX

NAMESPACE_END(kb)

#endif // __LINUX__

#endif // end of __TYPE_H_KB  
