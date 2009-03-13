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

#ifndef NONS_COMMANDLINEOPTIONS_CPP
#define NONS_COMMANDLINEOPTIONS_CPP

#include "CommandLineOptions.h"
#include "Globals.h"

#define DEFAULT_INPUT_WIDTH 640
#define DEFAULT_INPUT_HEIGHT 480
//#define PSP_RESOLUTION
//#define BIG_RESOLUTION
#ifdef PSP_RESOLUTION
#define DEFAULT_OUTPUT_WIDTH 480
#define DEFAULT_OUTPUT_HEIGHT 272
#elif defined(BIG_RESOLUTION)
#define DEFAULT_OUTPUT_WIDTH 1024
#define DEFAULT_OUTPUT_HEIGHT 768
#else
#define DEFAULT_OUTPUT_WIDTH DEFAULT_INPUT_WIDTH
#define DEFAULT_OUTPUT_HEIGHT DEFAULT_INPUT_HEIGHT
#endif

NONS_CommandLineOptions::NONS_CommandLineOptions(){
	this->scriptencoding=SJIS_ENCODING;
	this->musicFormat=0;
	this->musicDirectory=0;
	this->layerMethod=CLASSIC_METHOD;
	this->animMethod=CLASSIC_METHOD;
	this->cacheSize=0;
	this->scriptPath=0;
	this->scriptEncryption=NO_ENCRYPTION;
	this->override_stdout=1;
	this->reset_redirection_files=1;
	this->debugMode=0;
	this->noconsole=0;
	this->virtualWidth=DEFAULT_INPUT_WIDTH;
	this->virtualHeight=DEFAULT_INPUT_HEIGHT;
	this->realWidth=DEFAULT_OUTPUT_WIDTH;
	this->realHeight=DEFAULT_OUTPUT_HEIGHT;
	this->startFullscreen=0;
	this->verbosity=0;
	this->no_sound=0;
	this->savedir=0;
	this->stopOnFirstError=0;
}

NONS_CommandLineOptions::~NONS_CommandLineOptions(){
	if (this->musicFormat)
		delete[] this->musicFormat;
	if (this->musicDirectory)
		delete[] this->musicDirectory;
	if (this->scriptPath)
		delete[] this->scriptPath;
	if (this->savedir)
		delete[] this->savedir;
}
#endif
