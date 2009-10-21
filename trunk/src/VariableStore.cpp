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

#include "VariableStore.h"
#include "IOFunctions.h"
#include "CommandLineOptions.h"
#include "ImageLoader.h"

const std::wstring NONS_VariableMember::null;

NONS_VariableMember::NONS_VariableMember(yytokentype type){
	this->intValue=0;
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
	this->type=INTEGER;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	this->dimension=0;
	this->dimensionSize=0;
	this->negated=1;
	this->temporary=0;
}

NONS_VariableMember::NONS_VariableMember(const std::wstring &a){
	this->intValue=0;
	this->type=STRING;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	this->dimension=0;
	this->dimensionSize=0;
	this->set(a);
	this->negated=1;
	this->temporary=0;
}

NONS_VariableMember::NONS_VariableMember(std::vector<long> &sizes,size_t startAt){
	this->intValue=0;
	this->_long_upper_limit=LONG_MAX;
	this->_long_lower_limit=LONG_MIN;
	this->constant=0;
	if (startAt<sizes.size()){
		this->type=INTEGER_ARRAY;
		this->dimensionSize=sizes[startAt]+1;
		this->dimension=new NONS_VariableMember*[this->dimensionSize];
		for (ulong a=0;a<ulong(this->dimensionSize);a++)
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
	this->wcsValue=b.wcsValue;
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

const std::wstring &NONS_VariableMember::getWcs(){
	return this->type==STRING?this->wcsValue:this->null;
}

NONS_VariableMember *NONS_VariableMember::getIndex(ulong i){
	if (this->type==INTEGER_ARRAY && i<this->dimensionSize)
		return this->dimension[i];
	return 0;
}

extern SDL_mutex *exitMutex;

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

void NONS_VariableMember::atoi(const std::wstring &a){
	if (this->constant || this->type!=INTEGER)
		return;
	SDL_LockMutex(exitMutex);
	std::wstringstream stream;
	stream <<a;
	stream >>this->intValue;
	this->fixint();
	SDL_UnlockMutex(exitMutex);
}

void NONS_VariableMember::set(const std::wstring &a){
	if (this->constant || this->type==INTEGER || this->type==INTEGER_ARRAY)
		return;
	SDL_LockMutex(exitMutex);
	if (this->type==STRING)
		this->wcsValue=a;
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

NONS_Variable::NONS_Variable(){
	this->intValue=new NONS_VariableMember(INTEGER);
	this->wcsValue=new NONS_VariableMember(STRING);
}

NONS_Variable::NONS_Variable(const NONS_Variable &b){
	this->intValue=new NONS_VariableMember(*b.intValue);
	this->wcsValue=new NONS_VariableMember(*b.wcsValue);
}

NONS_Variable &NONS_Variable::operator=(const NONS_Variable &b){
	delete this->intValue;
	delete this->wcsValue;
	this->intValue=new NONS_VariableMember(*b.intValue);
	this->wcsValue=new NONS_VariableMember(*b.wcsValue);
	return *this;
}

NONS_Variable::~NONS_Variable(){
	delete this->intValue;
	delete this->wcsValue;
}

NONS_LabelLog labellog;

const Sint32 NONS_VariableStore::indexLowerLimit=-1073741824;
const Sint32 NONS_VariableStore::indexUpperLimit=1073741823;

extern std::wstring save_directory;

NONS_VariableStore::NONS_VariableStore(){
	ulong l;
	this->commitGlobals=0;
	std::wstring dir=save_directory+L"global.sav";
	uchar *buffer=readfile(dir.c_str(),l);
	if (!buffer){
		buffer=readfile(L"gloval.sav",l);
		if (!buffer)
			return;
		for (ulong a=0,stackpos=200;a<l;stackpos++){
			NONS_Variable *var=new NONS_Variable();
			var->intValue->set(readSignedDWord((char *)buffer,a));
			var->wcsValue->set(UniFromSJIS(readString((char *)buffer,a)));
			this->variables[stackpos]=var;
		}
	}else{
		if (firstchars(std::string((char *)buffer),0,"BZh")){
			char *temp=decompressBuffer_BZ2((char *)buffer,l,(unsigned long *)&l);
			delete[] buffer;
			buffer=(uchar *)temp;
		}
		ulong offset=0;
		Uint32 intervalsN=readDWord((char *)buffer,offset);
		std::vector<Sint32> intervals;
		for (Uint32 a=0;a<intervalsN;a++){
			Uint32 b=readDWord((char *)buffer,offset);
			if (b&0x80000000){
				b&=0x7FFFFFFF;
				intervals.push_back((Sint32)b);
				intervals.push_back(1);
			}else{
				intervals.push_back((Sint32)b);
				intervals.push_back(readSignedDWord((char *)buffer,offset));
			}
		}
		ulong currentInterval=0;
		while (offset<l){
			ulong a=intervals[currentInterval],
				b=intervals[currentInterval+1];
			currentInterval+=2;
			for (ulong c=0;c<b;c++){
				NONS_Variable *var=new NONS_Variable();
				var->intValue->set(readSignedDWord((char *)buffer,offset));
				var->wcsValue->set(UniFromUTF8(readString((char *)buffer,offset)));
				this->variables[a++]=var;
			}
		}
	}
	delete[] buffer;
}

NONS_VariableStore::~NONS_VariableStore(){
	this->saveData();
	for (constants_map_T::iterator i=this->constants.begin();i!=this->constants.end();i++)
		delete i->second;
	this->reset();
}

void NONS_VariableStore::reset(){
	for (variables_map_T::iterator i=this->variables.begin();i!=this->variables.end();i++)
		delete i->second;
	this->variables.clear();
	for (arrays_map_T::iterator i=this->arrays.begin();i!=this->arrays.end();i++)
		delete i->second;
	this->arrays.clear();
}

void NONS_VariableStore::saveData(){
	if (!this->commitGlobals)
		return;
	std::string buffer;
	variables_map_T::iterator i=this->variables.find(200);
	if (i==this->variables.end())
		i--;
	if (!this->variables.size() || i->first<200)
		writeDWord(0,buffer);
	else{
		for (;i!=this->variables.end() && VARIABLE_HAS_NO_DATA(i->second);i++);
		if (i==this->variables.end())
			writeDWord(0,buffer);
		else{
			variables_map_T::iterator i2=i;
			std::vector<Sint32> intervals;
			Sint32 last=i->first;
			intervals.push_back(last++);
			for (i++;i!=this->variables.end();i++){
				if (VARIABLE_HAS_NO_DATA(i->second))
					continue;
				if (i->first!=last){
					intervals.push_back(last-intervals.back());
					last=i->first;
					intervals.push_back(last++);
				}else
					last++;
			}
			intervals.push_back(last-intervals.back());
			writeDWord(intervals.size()/2,buffer);
			for (ulong a=0;a<intervals.size();){
				if (intervals[a+1]>1){
					writeDWord(intervals[a++],buffer);
					writeDWord(intervals[a++],buffer);
				}else{
					writeDWord(intervals[a]|0x80000000,buffer);
					a+=2;
				}
			}
			for (i=i2;i!=this->variables.end();i++){
				if (VARIABLE_HAS_NO_DATA(i->second))
					continue;
				writeDWord(i->second->intValue->getInt(),buffer);
				writeString(i->second->wcsValue->getWcs(),buffer);
			}
		}
	}
	ulong l;
	char *writebuffer=compressBuffer_BZ2((char *)buffer.c_str(),buffer.size(),&l);
	std::wstring dir=save_directory+L"global.sav";
	writefile(dir.c_str(),writebuffer,l);
	delete[] writebuffer;
}

ErrorCode NONS_VariableStore::evaluate(
		const std::wstring &exp,
		long *result,
		bool invert_terms,
		std::vector<long> *array_decl,
		NONS_VariableMember **retrievedVar,
		std::wstring *string){
	if (!!result)
		*result=0;
	std::wstringstream param;
	param <<exp;
	switch (expressionParser_yyparse(&param,this,&ImageLoader->filelog,result,invert_terms,array_decl,retrievedVar,string)){
		case 0:
			if (!!result && CLOptions.verbosity>=2){
				if (invert_terms)
					o_stderr <<"notif";
				o_stderr <<"("<<exp<<")=="<<*result<<"\n";
			}
			return NONS_NO_ERROR;
		case 1:
			if (!!result)
				*result=0;
			return NONS_UNDEFINED_ERROR;
		case 2:
			//return NONS_OUT_OF_MEMORY;
			return NONS_UNDEFINED_ERROR;
	}
	return NONS_UNDEFINED_ERROR;
}

void NONS_VariableStore::push(Sint32 pos,NONS_Variable *var){
	variables_map_T::iterator i=this->variables.find(pos);
	if (i==this->variables.end())
		this->variables[pos]=var;
	else{
		delete i->second;
		i->second=var;
	}
}

NONS_VariableMember *NONS_VariableStore::retrieve(const std::wstring &name,ErrorCode *error){
	NONS_VariableMember *ret=0;
	this->evaluate(name,0,0,0,&ret,0);
	if (!ret && !!error)
		*error=NONS_UNDEFINED_ERROR;
	return ret;
}

NONS_Variable *NONS_VariableStore::retrieve(Sint32 position,ErrorCode *error){
	if (position<indexLowerLimit || position>indexUpperLimit){
		if (!!error)
			*error=NONS_VARIABLE_OUT_OF_RANGE;
		return 0;
	}
	if (!!error)
		*error=NONS_NO_ERROR;
	variables_map_T::iterator i=this->variables.find(position);
	if (i==this->variables.end()){
		SDL_LockMutex(exitMutex);
		NONS_Variable *var=new NONS_Variable();
		this->variables[position]=var;
		SDL_UnlockMutex(exitMutex);
		return var;
	}
	if (!i->second)
		i->second=new NONS_Variable();
	return i->second;
}

ErrorCode NONS_VariableStore::getWcsValue(const std::wstring &str,std::wstring &value){
	return this->evaluate(str,0,0,0,0,&value);
}

ErrorCode NONS_VariableStore::getIntValue(const std::wstring &str,long &value){
	return this->evaluate(str,&value,0,0,0,0);
}

NONS_VariableMember *NONS_VariableStore::getArray(Sint32 arrayNo){
	arrays_map_T::iterator i=this->arrays.find(arrayNo);
	if (i==this->arrays.end())
		return 0;
	return i->second;
}

NONS_VariableMember *NONS_VariableStore::getConstant(const std::wstring &name){
	constants_map_T::iterator i=this->constants.find(name);
	if (i==this->constants.end())
		return 0;
	return i->second;
}

Sint32 NONS_VariableStore::getVariableIndex(NONS_VariableMember *var){
	if (var->getType()==INTEGER){
		for (variables_map_T::iterator i=this->variables.begin(),end=this->variables.end();i!=end;i++)
			if (i->second->intValue==var)
				return i->first;
	}else{
		for (variables_map_T::iterator i=this->variables.begin(),end=this->variables.end();i!=end;i++)
			if (i->second->wcsValue==var)
				return i->first;
	}
	return 0;
}
