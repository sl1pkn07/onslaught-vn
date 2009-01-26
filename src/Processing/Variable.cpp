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
*     * Helios' name may not be used to endorse or promote products derived from
*       this software without specific prior written permission.
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

#ifndef NONS_VARIABLE_CPP
#define NONS_VARIABLE_CPP

#include "Variable.h"
#include "../Functions.h"
#include "../Globals.h"

NONS_Variable::NONS_Variable(ulong *dimensions,ulong size){
	this->intValue=0;
	this->wcsValue=0;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	if (size){
		this->dimensionSize=*dimensions;
		this->type='?';
		this->dimension=new NONS_Variable*[*dimensions];
		for (ulong a=0;a<*dimensions;a++)
			this->dimension[a]=new NONS_Variable(dimensions+1,size-1);
	}else{
		this->type='%';
		this->dimension=0;
		this->dimensionSize=0;
	}
}

NONS_Variable::NONS_Variable(const NONS_Variable &b){
	this->intValue=b.intValue;
	this->wcsValue=copyWString(b.wcsValue);
	this->_long_upper_limit=b._long_upper_limit;
	this->_long_lower_limit=b._long_lower_limit;
	this->type=b.type;
	this->dimensionSize=b.dimensionSize;
	if (this->type!='?')
		this->dimension=0;
	else{
		this->dimension=new NONS_Variable*[this->dimensionSize];
		for (ulong a=0;a<this->dimensionSize;a++)
			this->dimension[a]=new NONS_Variable(*b.dimension[a]);
	}
}

NONS_Variable::~NONS_Variable(){
	if (this->wcsValue)
		delete[] this->wcsValue;
	if (this->dimension){
		for (ulong a=0;a<this->dimensionSize;a++)
			delete[] this->dimension[a];
		delete[] this->dimension;
	}
}

void NONS_Variable::fixint(){
	if (this->intValue>this->_long_upper_limit)
		this->intValue=_long_upper_limit;
	else if (this->intValue<this->_long_lower_limit)
		this->intValue=_long_lower_limit;
	/*while (this->intValue>this->_long_upper_limit)
		this->intValue-=this->_long_upper_limit-this->_long_lower_limit+1;
	while (this->intValue<this->_long_lower_limit)
		this->intValue+=this->_long_upper_limit-this->_long_lower_limit+1;*/
}

void NONS_Variable::set(long a){
	SDL_LockMutex(exitMutex);
	this->intValue=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::inc(){
	SDL_LockMutex(exitMutex);
	this->intValue++;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::dec(){
	SDL_LockMutex(exitMutex);
	this->intValue--;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::add(long a){
	SDL_LockMutex(exitMutex);
	this->intValue+=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::sub(long a){
	SDL_LockMutex(exitMutex);
	this->intValue-=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::mul(long a){
	SDL_LockMutex(exitMutex);
	this->intValue*=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::div(long a){
	SDL_LockMutex(exitMutex);
	this->intValue/=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_Variable::mod(long a){
	SDL_LockMutex(exitMutex);
	this->intValue%=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}
#endif
