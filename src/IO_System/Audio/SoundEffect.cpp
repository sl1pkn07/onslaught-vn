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

#ifndef NONS_SOUNDEFFECT_CPP
#define NONS_SOUNDEFFECT_CPP

#include "SoundEffect.h"
#include "../../Functions.h"

NONS_SoundEffect::NONS_SoundEffect(int chan){
	this->channel=chan;
	this->sound=0;
	this->playingHasStarted=0;
	this->loops=0;
}

NONS_SoundEffect::~NONS_SoundEffect(){
	if (this->sound)
		this->sound->references--;
}

/*void NONS_SoundEffect::freeCacheElement(int chan){
	this->sound->references--;
	this->sound=0;
}*/

void NONS_SoundEffect::play(bool synchronous,long times){
	this->stop();
	Mix_PlayChannel(this->channel,this->sound->chunk,times);
	this->playingHasStarted=1;
	this->loops=times;
}

void NONS_SoundEffect::stop(){
	if (Mix_Playing(this->channel))
		Mix_HaltChannel(this->channel);
}

bool NONS_SoundEffect::loaded(){
	return !!this->sound;
}

void NONS_SoundEffect::load(NONS_CachedSound *sound){
	this->unload();
	this->sound=sound;
	this->playingHasStarted=0;
}

void NONS_SoundEffect::unload(){
	if (this->loaded()){
		this->stop();
		this->sound->references--;
		this->sound=0;
		this->playingHasStarted=0;
	}
}

int NONS_SoundEffect::volume(int vol){
	Mix_Volume(this->channel,vol);
	return Mix_Volume(this->channel,-1);
}
#endif
