#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
#ifndef UNICODE
#define UNICODE
#endif
#endif

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef ulong ErrorCode;

#define NONS_FATAL_ERROR							0x40000000
#define NONS_BREAK_WORTHY_ERROR						0x20000000
#define NONS_NO_ERROR_FLAG							0x10000000
#define NONS_INTERNAL_ERROR							(0x08000000|NONS_FATAL_ERROR)
#define NONS_SYNTAX_ERROR							0x04000000
#define NONS_WARNING								0x02000000
#define NONS_END									0x01000000
#define NONS_UNDEFINED_ERROR						0x00010000

#define NONS_NO_ERROR								(0|NONS_NO_ERROR_FLAG)
#define NONS_NOT_IMPLEMENTED						4
#define NONS_INTERNAL_INVALID_PARAMETER				(22|NONS_INTERNAL_ERROR)

typedef uchar integer8;
typedef char Sinteger8;
#ifdef _MSC_VER
typedef unsigned __int16 integer16;
typedef signed __int16 Sinteger16;
typedef unsigned __int32 integer32;
typedef signed __int32 Sinteger32;
typedef unsigned __int64 integer64;
typedef signed __int64 Sinteger64;
#else
#include <climits>
#if USHRT_MAX>=0xFFFF
typedef ushort integer16;
typedef short Sinteger16;
#endif
#if ULONG_MAX>=0xFFFFFFFF
typedef ulong integer32;
typedef long Sinteger32;
#endif
#endif
#pragma warning(disable:4018) //no comparison signed/unsigned mismatch
#pragma warning(disable:4244) //no possible loss of data
#pragma warning(disable:4996) //no "unsafe" functions
#pragma warning(disable:4309) //no truncation of constant value
#ifdef _MSC_VER
#endif
