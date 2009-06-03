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

#ifndef NONS_MUSIC_CPP
#define NONS_MUSIC_CPP

#include "Music.h"
#include "../../Functions.h"
#include "../../Globals.h"

NONS_Music::NONS_Music(const std::string &filename){
	this->data=Mix_LoadMUS(filename.c_str());
	this->buffer=0;
	this->buffersize=0;
	this->filename=filename;
	this->RWop=0;
}

NONS_Music::NONS_Music(const std::string &filename,char *databuffer,long size){
	this->buffer=databuffer;
	this->buffersize=size;
	this->RWop=SDL_RWFromMem((void *)databuffer,size);
	this->data=Mix_LoadMUS_RW(this->RWop);
	this->filename=filename;
}

NONS_Music::~NONS_Music(){
	this->stop();
	if (this->data){
		Mix_FreeMusic(this->data);
	}
}

void NONS_Music::play(long times){
	if (Mix_PlayingMusic())
		return;
	Mix_PlayMusic(this->data,times);
	int x=Mix_VolumeMusic(-1);
	Mix_VolumeMusic(0);
	SDL_Delay(50);
	Mix_VolumeMusic(x);
}

void NONS_Music::stop(){
	if (Mix_PlayingMusic() || Mix_PausedMusic())
		Mix_HaltMusic();
}

void NONS_Music::pause(){
	if (Mix_PlayingMusic() && !Mix_PausedMusic())
		Mix_PauseMusic();
	else
		Mix_ResumeMusic();
}

bool NONS_Music::loaded(){
	return this->data!=0;
}

int NONS_Music::volume(int vol){
	Mix_VolumeMusic(vol);
	return Mix_VolumeMusic(-1);
}
#endif
