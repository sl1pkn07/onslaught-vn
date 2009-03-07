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

#ifndef NONS_VARIABLEMEMBER_CPP
#define NONS_VARIABLEMEMBER_CPP

#include "VariableMember.h"
#include "../Functions.h"
#include "../Globals.h"
#include <SDL/SDL.h>
#include <sstream>

NONS_VariableMember::NONS_VariableMember(char type){
	this->intValue=0;
	this->wcsValue=0;
	this->type=(type=='$')?'$':'%';
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	this->dimension=0;
	this->dimensionSize=0;
}

NONS_VariableMember::NONS_VariableMember(ulong *dimensions,ulong size){
	this->intValue=0;
	this->wcsValue=0;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	if (!size && !dimensions){
		this->type='?';
		this->dimensionSize=0;
		this->dimension=0;
	}else if (size){
		this->type='?';
		this->dimensionSize=*dimensions;
		this->dimension=new NONS_VariableMember*[*dimensions];
		for (ulong a=0;a<*dimensions;a++)
			this->dimension[a]=new NONS_VariableMember(dimensions+1,size-1);
	}else{
		this->type='%';
		this->dimension=0;
		this->dimensionSize=0;
	}
}

NONS_VariableMember::NONS_VariableMember(const NONS_VariableMember &b){
	this->constant=b.constant;
	this->intValue=b.intValue;
	this->_long_upper_limit=b._long_upper_limit;
	this->_long_lower_limit=b._long_lower_limit;
	this->wcsValue=(b.type=='$')?copyWString(b.wcsValue):0;
	this->type=b.type;
	this->dimensionSize=b.dimensionSize;
	if (this->type!='?')
		this->dimension=0;
	else{
		this->dimension=new NONS_VariableMember*[this->dimensionSize];
		for (ulong a=0;a<this->dimensionSize;a++)
			this->dimension[a]=new NONS_VariableMember(*b.dimension[a]);
	}
}

NONS_VariableMember::~NONS_VariableMember(){
	if (!!this->wcsValue)
		delete[] this->wcsValue;
	if (!!this->dimension){
		for (ulong a=0;a<this->dimensionSize;a++)
			delete this->dimension[a];
		delete[] this->dimension;
	}
}

void NONS_VariableMember::makeConstant(){
	this->constant=1;
}

bool NONS_VariableMember::isConstant(){
	return this->constant;
}

char NONS_VariableMember::getType(){
	return this->type;
}

void NONS_VariableMember::fixint(){
	if (this->intValue>this->_long_upper_limit)
		this->intValue=_long_upper_limit;
	else if (this->intValue<this->_long_lower_limit)
		this->intValue=_long_lower_limit;
}

template <typename T>
int atoi2(T *str){
	char *temp=copyString(str);
	int res=atoi(temp);
	delete[] temp;
	return res;
}

long NONS_VariableMember::getInt(){
	return (this->type=='%')?this->intValue:atoi2(this->wcsValue);
}

const wchar_t *NONS_VariableMember::getWcs(){
	return (this->type=='$')?this->wcsValue:0;
}

wchar_t *NONS_VariableMember::getWcsCopy(){
	if (this->type=='$')
		return copyWString(this->wcsValue);
	else{
		std::stringstream stream;
		stream <<this->intValue;
		std::string string;
		std::getline(stream,string);
		return copyWString(string.c_str());
	}
}

char *NONS_VariableMember::getStrCopy(){
	if (this->type=='$')
		return copyString(this->wcsValue);
	else{
		std::stringstream stream;
		stream <<this->intValue;
		std::string string;
		std::getline(stream,string);
		return copyString(string.c_str());
	}
}

void NONS_VariableMember::set(long a){
	if (this->constant)
		return;
	//SDL_LockMutex(exitMutex);
	if (this->type=='%'){
		this->intValue=a;
		this->fixint();
	}else{
		if (!!this->wcsValue)
			delete[] this->wcsValue;
		std::stringstream stream;
		stream <<a;
		std::string string;
		std::getline(stream,string);
		this->wcsValue=copyWString(string.c_str());
	}
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::set(const wchar_t *a,bool takeOwnership){
	if (this->constant)
		return;
	if (this->type=='%'){
		this->intValue=atoi2(a);
		this->fixint();
	}else{
		if (!!this->wcsValue)
			delete[] this->wcsValue;
		this->wcsValue=takeOwnership?(wchar_t *)a:copyWString(a);
	}
}

void NONS_VariableMember::inc(){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue++;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::dec(){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue--;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::add(long a){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue+=a;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::sub(long a){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue-=a;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::mul(long a){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue*=a;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::div(long a){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue/=a;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::mod(long a){
	if (this->constant || this->type!='%')
		return;
	//SDL_LockMutex(exitMutex);
	this->intValue%=a;
	this->fixint();
	//SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::setlimits(long lower,long upper){
	this->_long_lower_limit=lower;
	this->_long_upper_limit=upper;
	this->fixint();
}
#endif
