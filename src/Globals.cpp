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

#ifndef NONS_GLOBALS_CPP
#define NONS_GLOBALS_CPP
#include "Globals.h"
#include <SDL/SDL.h>

NONS_CommandLineOptions CLOptions;
NONS_InputObserver InputObserver;
bool exitLocked=0;
const char *errorMessages[]={
	"",
	"Invalid parameter.",
	"Invalid archive.",
	"Archive uninitialized.",
	"Command not implemented.",
	"Attempted variable declaration while not in Define Mode.",
	"Variable redefinition.",
	"The NSA archive had already been initialized.",
	"No such block.",
	"",
	"A return was found while the call stack was empty.",
	"Syntax error in expression.",
	"Division by zero.",
	"Non integral variable in expression.",
	"Undefined variable.",
	"Unrecognized command.",
	"Operands do not match.",
	"Invalid variable name.",
	"Not enough parameters passed to the instruction.",
	"File not found.",
	"No music loaded.",
	"No sound effect loaded.",
	"Invalid parameter.",
	"Effect redefinition.",
	"Invalid run time parameter value.",
	"Unmatched [].",
	"Unrecognized operator.",
	"Array index out of bounds.",
	"Missing ? in array declaration.",
	"Missing [] in array declaration.",
	"Invalid run time parameter type.",
	"Too many parameters passed. Some will be ignored.",
	"No jumps matching the condition found.",
	"Unmatched quotes.",
	"Zero value in skip.",
	"Effect code is out of range.",
	"Empty string.",
	"Invalid channel index.",
	"Screen is uninitialized.",
	"Could not open script file.",
	"INI section not found.",
	"INI key not found.",
	"Invalid hex integer.",
	"This command left unimplemented.",
	"Effect not implemented.",
	"Undefined effect.",
	"Next or break found without a for inside the current block.",
	"No next for open for.",
	"Transparency method not implemented.",
	"No trap was set.",
	"Menu not ininitalized.",
	"There is no source image for the button.",
	"There are not defined buttons at the moment.",
	"The button layer is too big to fit for the screen.",
	"There is no \"*start\" label.",
	"",
	"No sprite is loaded at that index.",
	"Invalid transparency method.",
	"Unsupported savegame version.",
};
NONS_RedirectedOutput v_stdout(std::cout);
NONS_RedirectedOutput v_stderr(std::cerr);
NONS_RedirectedOutput v_stdlog(std::clog);
NONS_ImageLoader *ImageLoader=0;
bool ctrlIsPressed;
bool softwareCtrlIsPressed;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const int rmask=0xFF000000;
const int gmask=0x00FF0000;
const int bmask=0x0000FF00;
const int amask=0x000000FF;
const int rshift=24;
const int gshift=16;
const int bshift=8;
const int ashift=0;
#else
const int rmask=0x000000FF;
const int gmask=0x0000FF00;
const int bmask=0x00FF0000;
const int amask=0xFF000000;
const int rshift=0;
const int gshift=8;
const int bshift=16;
const int ashift=24;
#endif
uchar trapFlag=0;
int lastClickX=0;
int lastClickY=0;
void *gScriptInterpreter=0;
SDL_mutex *exitMutex=0;
SDL_mutex *screenMutex=0;

#ifdef NONS_PARALLELIZE
ulong cpu_count=1;
#endif
#endif
