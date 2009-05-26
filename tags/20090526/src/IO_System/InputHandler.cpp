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

#ifndef NONS_INPUTHANDLER_CPP
#define NONS_INPUTHANDLER_CPP

#include "InputHandler.h"
#include "../Globals.h"

NONS_EventQueue::NONS_EventQueue(){
	this->mutex=SDL_CreateMutex();
}

NONS_EventQueue::~NONS_EventQueue(){
	SDL_DestroyMutex(this->mutex);
}

void NONS_EventQueue::push(SDL_Event a){
	SDL_LockMutex(this->mutex);
	this->data.push(a);
	SDL_UnlockMutex(this->mutex);
}

SDL_Event NONS_EventQueue::pop(){
	SDL_LockMutex(this->mutex);
	SDL_Event ret=this->data.front();
	this->data.pop();
	SDL_UnlockMutex(this->mutex);
	return ret;
}

void NONS_EventQueue::WaitForEvent(int delay){
	while (this->data.empty())
		SDL_Delay(delay);
}

NONS_InputObserver::NONS_InputObserver(){
	this->data.reserve(50);
	this->mutex=SDL_CreateMutex();
}

NONS_InputObserver::~NONS_InputObserver(){
	SDL_DestroyMutex(this->mutex);
	for (ulong a=0;a<this->data.size();a++)
		if (this->data[a])
			delete this->data[a];
}

NONS_EventQueue *NONS_InputObserver::attach(){
	SDL_LockMutex(this->mutex);
	NONS_EventQueue *res=new NONS_EventQueue();
	ulong pos=this->data.size();
	for (ulong a=0;a<pos;a++)
		if (!this->data[a])
			pos=a;
	if (pos==this->data.size())
		this->data.push_back(res);
	else
		this->data[pos]=res;
	SDL_UnlockMutex(this->mutex);
	return res;
}
void NONS_InputObserver::detach(NONS_EventQueue *what){
	SDL_LockMutex(this->mutex);
	for (ulong a=0;a<this->data.size();a++){
		if (this->data[a]==what){
			delete this->data[a];
			this->data[a]=0;
			break;
		}
	}
	SDL_UnlockMutex(this->mutex);
}
void NONS_InputObserver::notify(SDL_Event *event){
	SDL_LockMutex(this->mutex);
	for (ulong a=0;a<this->data.size();a++)
		if (!!this->data[a])
			this->data[a]->push(*event);
	SDL_UnlockMutex(this->mutex);
}
#endif
