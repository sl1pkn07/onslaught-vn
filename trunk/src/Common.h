/*
* Copyright (c) 2008, 2009, Helios (helios.vmg@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, 
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the author may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*     * Products derived from this software may not be called "ONSlaught" nor
*       may "ONSlaught" appear in their names without specific prior written
*       permission from the author. 
*
* THIS SOFTWARE IS PROVIDED BY HELIOS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL HELIOS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NONS_COMMON_H
#define NONS_COMMON_H
#include <iostream>
//#include "Globals.h"
#include "CommonTypes.h"

extern bool ctrlIsPressed;
extern bool softwareCtrlIsPressed;

#define CURRENTLYSKIPPING (ctrlIsPressed || softwareCtrlIsPressed)

#define STRLITERAL(x) ((char*)(x))
#define WSTRLITERAL(x) ((wchar_t*)(x))

#if defined(_WIN32) || defined(_WIN64)
#define NONS_SYS_WINDOWS
#endif
#if defined(linux) || defined(__linux)
#define NONS_SYS_LINUX
#endif

#ifdef _MSC_VER
#pragma warning(disable:4018) //no comparison signed/unsigned mismatch
#pragma warning(disable:4244) //no possible loss of data
#pragma warning(disable:4996) //no "unsafe" functions
#pragma warning(disable:4309) //no truncation of constant value
#endif

#include <cwchar>
#if WCHAR_MAX<0xFFFF
#error "Wide characters on this platform are too narrow."
#endif
#endif
