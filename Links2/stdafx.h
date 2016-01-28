// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// CRT
#include <time.h>
#include <stdlib.h>

// Windows
#include <tchar.h>
#include <Windows.h>
#include <Shobjidl.h>

#include <Shlobj.h>
#pragma comment (lib, "Shell32.lib")

#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

// ATL
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlstr.h>
#include <atlbase.h>
#include <atlpath.h>

// STD
#include <set>
#include <string>
#include <vector>

// Macros
#ifdef _DEBUG
#define LOG_DEBUG(fmt, ...)				\
	do {								\
		CString __s;					\
		__s.Format(fmt, __VA_ARGS__);	\
		::OutputDebugString(__s);		\
	} while(0)
#define LOG_WARNING(fmt, ...) LOG_DEBUG(fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_DEBUG(fmt, __VA_ARGS__)
#else
#define LOG_DEBUG
#define LOG_WARNING
#define LOG_ERROR
#endif