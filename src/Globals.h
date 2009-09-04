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

#ifndef NONS_GLOBALS_H
#define NONS_GLOBALS_H
#include "Common.h"
#include "CommandLineOptions.h"
#include "IO_System/InputHandler.h"
#include "IO_System/StdOut.h"
#include "IO_System/SAR/ImageLoader.h"
#include <map>
#include <vector>
#include <cassert>

extern NONS_CommandLineOptions CLOptions;
extern NONS_InputObserver InputObserver;
extern bool exitLocked;
extern const char *errorMessages[];
extern NONS_RedirectedOutput o_stdout;
extern NONS_RedirectedOutput o_stderr;
//extern NONS_RedirectedOutput o_stdlog;
extern NONS_ImageLoader *ImageLoader;
extern const int rmask;
extern const int gmask;
extern const int bmask;
extern const int amask;
extern const int rshift;
extern const int gshift;
extern const int bshift;
extern const int ashift;
extern uchar trapFlag;
extern int lastClickX;
extern int lastClickY;
class NONS_ScriptInterpreter;
extern NONS_ScriptInterpreter *gScriptInterpreter;
extern SDL_mutex *exitMutex;
extern SDL_mutex *screenMutex;

extern ulong cpu_count;

extern std::wstring save_directory;
extern std::wstring config_directory;

extern std::ofstream textDumpFile;

//#define LOOKUP_BLEND_CONSTANT
#ifdef LOOKUP_BLEND_CONSTANT
extern uchar blendData[65536];
#endif

class NONS_ThreadManager;
extern NONS_ThreadManager threadManager;
#endif
