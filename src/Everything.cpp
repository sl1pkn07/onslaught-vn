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

#ifndef NONS_EVERYTHING_CPP
#define NONS_EVERYTHING_CPP

#include "Everything.h"
#include "Globals.h"

NONS_Everything::NONS_Everything(){
	this->screen=0;
	this->audio=0;
	this->archive=new NONS_GeneralArchive();
	this->script=0;
}

NONS_Everything::~NONS_Everything(){
	if (this->screen)
		delete this->screen;
	if (this->audio)
		delete this->audio;
	if (this->archive)
		delete this->archive;
	if (this->script)
		delete this->script;
}

ErrorCode NONS_Everything::init_screen(){
	TTF_Init();
	NONS_Font *font;/*=new NONS_Font("default.ttf",18,TTF_STYLE_NORMAL);
	if (!font->font){
		delete font;
		ulong l;
		this->archive->getFileBuffer("default.ttf",&l)
		uchar *buffer=readfile;
		if (!buffer){
			o_stderr <<"FATAL ERROR: Could not find \"default.ttf\" font file. If your system is\n"
				"case-sensitive, make sure the file name is capitalized correctly."<<std::endl;
			exit(0);
		}
		SDL_RWops *rw=SDL_RWFromMem(buffer,l);
		font=new NONS_Font(rw,buffer,18,TTF_STYLE_NORMAL);
		delete[] buffer;
	}*/
	INIT_NONS_FONT(font,18,this->archive)
	NONS_GFXstore *gfx=new NONS_GFXstore();
	this->screen=new NONS_ScreenSpace(20,font,gfx);
	this->screen->output->shadeLayer->Clear();
	this->screen->Background->Clear();
	this->screen->BlendNoCursor(1);
	std::cout <<"Screen initialized."<<std::endl;
	return NONS_NO_ERROR;
}

ErrorCode NONS_Everything::init_audio(const std::string &musicDir){
	this->audio=new NONS_Audio(musicDir);
	return NONS_NO_ERROR;
}

ErrorCode NONS_Everything::init_script(const std::string &filename,ulong encoding,ulong encryption){
	this->script=new NONS_Script();
	{
		ErrorCode error_code=this->script->init(filename.c_str(),this->archive,encoding,encryption);
		if (error_code!=NONS_NO_ERROR){
			delete this->script;
			this->script=0;
			return error_code;
		}
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_Everything::init_script(ulong encoding){
	if (this->init_script("0.txt",encoding,NO_ENCRYPTION)==NONS_NO_ERROR)
		return NONS_NO_ERROR;
	if (this->init_script("00.txt",encoding,NO_ENCRYPTION)==NONS_NO_ERROR)
		return NONS_NO_ERROR;
	if (this->init_script("nscr_sec.dat",encoding,VARIABLE_XOR_ENCRYPTION)==NONS_NO_ERROR)
		return NONS_NO_ERROR;
	ErrorCode error_code=this->init_script("nscript.___",encoding,TRANSFORM_THEN_XOR84_ENCRYPTION);
	if (error_code==NONS_NO_ERROR)
		return NONS_NO_ERROR;
	if (this->init_script("nscript.dat",encoding,XOR84_ENCRYPTION)==NONS_NO_ERROR)
		return NONS_NO_ERROR;
	if (error_code==NONS_NOT_IMPLEMENTED)
		return NONS_NOT_IMPLEMENTED;
	return NONS_SCRIPT_NOT_FOUND;
}
#endif
