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
#include "ScriptInterpreter.h"
#include "../Functions.h"
#include "../Globals.h"
#include <SDL/SDL.h>
#include <sstream>

NONS_VariableMember::NONS_VariableMember(yytokentype type){
	this->intValue=0;
	this->wcsValue=0;
	this->type=type;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	this->dimension=0;
	this->dimensionSize=0;
	this->negated=1;
	this->temporary=0;
}

NONS_VariableMember::NONS_VariableMember(long value){
	this->intValue=value;
	this->wcsValue=0;
	this->type=INTEGER;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	this->dimension=0;
	this->dimensionSize=0;
	this->negated=1;
	this->temporary=0;
}

NONS_VariableMember::NONS_VariableMember(const wchar_t *a,bool takeOwnership){
	this->intValue=0;
	this->wcsValue=0;
	this->type=STRING;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	this->dimension=0;
	this->dimensionSize=0;
	this->set(a,takeOwnership);
	this->negated=1;
	this->temporary=0;
}

NONS_VariableMember::NONS_VariableMember(std::vector<long> &sizes,size_t startAt){
	this->intValue=0;
	this->wcsValue=0;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	if (startAt<sizes.size()){
		this->type=INTEGER_ARRAY;
		this->dimensionSize=sizes[startAt];
		this->dimension=new NONS_VariableMember*[sizes[startAt]+1];
		for (ulong a=0;a<ulong(sizes[startAt]);a++)
			this->dimension[a]=new NONS_VariableMember(sizes,startAt+1);
	}else{
		this->type=INTEGER;
		this->dimension=0;
		this->dimensionSize=0;
	}
	this->negated=1;
	this->temporary=0;
}

NONS_VariableMember::NONS_VariableMember(const NONS_VariableMember &b){
	this->constant=b.constant;
	this->intValue=b.intValue;
	this->_long_upper_limit=b._long_upper_limit;
	this->_long_lower_limit=b._long_lower_limit;
	this->wcsValue=(b.type==STRING)?copyWString(b.wcsValue):0;
	this->type=b.type;
	this->dimensionSize=b.dimensionSize;
	if (this->type!=INTEGER_ARRAY)
		this->dimension=0;
	else{
		this->dimension=new NONS_VariableMember*[this->dimensionSize];
		for (ulong a=0;a<this->dimensionSize;a++)
			this->dimension[a]=new NONS_VariableMember(*b.dimension[a]);
	}
	this->negated=b.negated;
	this->temporary=0;
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

yytokentype NONS_VariableMember::getType(){
	return this->type;
}

void NONS_VariableMember::fixint(){
	if (this->intValue>this->_long_upper_limit)
		this->intValue=_long_upper_limit;
	else if (this->intValue<this->_long_lower_limit)
		this->intValue=_long_lower_limit;
}

long NONS_VariableMember::getInt(){
	if (this->type==INTEGER || this->type==INTEGER_ARRAY)
		return this->intValue;
	return 0;
}

const wchar_t *NONS_VariableMember::getWcs(){
	return (this->type==STRING && !!this->wcsValue)?this->wcsValue:L"";
}

wchar_t *NONS_VariableMember::getWcsCopy(){
	if (this->type==STRING)
		return copyWString(this->wcsValue);
	return copyWString(L"");
}

char *NONS_VariableMember::getStrCopy(){
	if (this->type==STRING)
		return copyString(this->wcsValue);
	return copyString(L"");
}

NONS_VariableMember *NONS_VariableMember::getIndex(ulong i){
	if (this->type==INTEGER_ARRAY && i<this->dimensionSize)
		return this->dimension[i];
	return 0;
}

void NONS_VariableMember::set(long a){
	if (this->constant)
		return;
	SDL_LockMutex(exitMutex);
	if (this->type==INTEGER){
		this->intValue=a;
		this->fixint();
	}
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::set(const wchar_t *a,bool takeOwnership){
	if (this->constant || this->type==INTEGER || this->type==INTEGER_ARRAY){
		if (takeOwnership)
			delete[] (wchar_t *)a;
		return;
	}
	SDL_LockMutex(exitMutex);
	if (this->type==STRING){
		if (!!this->wcsValue)
			delete[] this->wcsValue;
		this->wcsValue=takeOwnership?(wchar_t *)a:copyWString(a);
	}
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::inc(){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	this->intValue++;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::dec(){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	this->intValue--;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::add(long a){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	this->intValue+=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::sub(long a){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	this->intValue-=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::mul(long a){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	this->intValue*=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::div(long a){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	if (a)
		this->intValue/=a;
	else
		this->intValue=0;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::mod(long a){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	this->intValue%=a;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::setlimits(long lower,long upper){
	this->_long_lower_limit=lower;
	this->_long_upper_limit=upper;
	this->fixint();
}

void NONS_VariableMember::negate(bool a){
	if (this->type==INTEGER && !this->negated && a){
		this->negated=1;
		this->intValue=!this->intValue;
	}
}
#endif
