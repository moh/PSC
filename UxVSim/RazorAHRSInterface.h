// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef SIMRAZORAHRSINTERFACE_H
#define SIMRAZORAHRSINTERFACE_H

#include "OSNet.h"
#include "OSMisc.h"

THREAD_PROC_RETURN_VALUE RazorAHRSInterfaceThread(void* pParam);

#endif // SIMRAZORAHRSINTERFACE_H
