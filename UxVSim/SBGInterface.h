// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#ifndef SIMSBGINTERFACE_H
#define SIMSBGINTERFACE_H

#ifndef DISABLE_SBGTHREAD
#define DISABLE_SBGTHREAD
#endif // !DISABLE_SBGTHREAD

#include "SBG.h"

THREAD_PROC_RETURN_VALUE SBGInterfaceThread(void* pParam);

#endif // SIMSBGINTERFACE_H
