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

#ifndef NONS_VARIABLESTORE_CPP
#define NONS_VARIABLESTORE_CPP

#include "VariableStore.h"
#include "../Functions.h"
#include "../Globals.h"
#include "../IO_System/FileIO.h"
#include "../IO_System/IOFunctions.h"
#include "../UTF.h"
#include <cctype>

std::set<wchar_t *,wstrCmp> labelsUsed;

NONS_VariableStore::NONS_VariableStore(){
	long l;
	this->commitGlobals=0;
	char *dir=addStrings(save_directory,"global.sav");
	uchar *buffer=readfile(dir,&l);
	if (!buffer){
		delete[] dir;
		buffer=readfile("gloval.sav",&l);
		if (!buffer)
			return;
		for (long a=0,stackpos=200;a<l;stackpos++){
			NONS_Variable *var=new NONS_Variable();
			var->intValue->set(readSignedDWord((char *)buffer,&a));
			wchar_t *temp;
			_READ_BINARY_SJIS_STRING(temp,buffer,a)
			var->wcsValue->set(temp,1);
			this->variables[stackpos]=var;
		}
	}else{
		if (!instr((char *)buffer,"BZh")){
			char *temp=decompressBuffer_BZ2((char *)buffer,l,(unsigned long *)&l);
			delete[] buffer;
			buffer=(uchar *)temp;
		}
		long offset=0;
		Uint32 intervalsN=readDWord((char *)buffer,&offset);
		std::vector<Sint32> intervals;
		for (Uint32 a=0;a<intervalsN;a++){
			Uint32 b=readDWord((char *)buffer,&offset);
			if (b&0x80000000){
				b&=0x7FFFFFFF;
				/*if (b&0x40000000)
					b&=0x80000000;*/
				intervals.push_back((Sint32)b);
				intervals.push_back(1);
			}else{
				/*b&=0x7FFFFFFF;
				if (b&0x40000000)
					b&=0x80000000;*/
				intervals.push_back((Sint32)b);
				intervals.push_back(readSignedDWord((char *)buffer,&offset));
			}
		}
		ulong currentInterval=0;
		while (offset<l){
			ulong a=intervals[currentInterval],
				b=intervals[currentInterval+1];
			currentInterval+=2;
			for (ulong c=0;c<b;c++){
				NONS_Variable *var=new NONS_Variable();
				var->intValue->set(readSignedDWord((char *)buffer,&offset));
				wchar_t *temp;
				_READ_BINARY_UTF8_STRING(temp,buffer,offset)
				var->wcsValue->set(temp,1);
				this->variables[a++]=var;
			}
		}
	}
	delete[] buffer;
}

NONS_VariableStore::~NONS_VariableStore(){
	this->saveData();
	for (std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI>::iterator i=this->constants.begin();i!=this->constants.end();i++){
		delete[] i->first;
		delete i->second;
	}
	for (std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI>::iterator i=this->arrays.begin();i!=this->arrays.end();i++){
		delete[] i->first;
		delete i->second;
	}
	for (variables_map_T::iterator i=this->variables.begin();i!=this->variables.end();i++)
		delete i->second;
}

void NONS_VariableStore::saveData(){
	if (!this->commitGlobals)
		return;
	std::string buffer;
	variables_map_T::iterator i=this->variables.find(200);
	if (i==this->variables.end())
		i--;
	if (i->first<200)
		writeDWord(0,&buffer);
	else{
		for (;i!=this->variables.end() && VARIABLE_HAS_NO_DATA(i->second);i++);
		if (i==this->variables.end())
			writeDWord(0,&buffer);
		else{
			std::vector<ulong> intervals;
			ulong last=i->first;
			intervals.push_back(last++);
			for (i++;i!=this->variables.end();i++){
				if (VARIABLE_HAS_NO_DATA(i->second))
					continue;
				if (i->first!=last){
					intervals.push_back(last-intervals[intervals.size()-1]);
					last=i->first;
					intervals.push_back(last++);
				}else
					last++;
			}
			intervals.push_back(last-intervals[intervals.size()-1]);
			writeDWord(intervals.size()/2,&buffer);
			for (ulong a=0;a<intervals.size();){
				if (intervals[a+1]>1){
					writeDWord(intervals[a++],&buffer);
					writeDWord(intervals[a++],&buffer);
				}else{
					writeDWord(intervals[a]|0x80000000,&buffer);
					a+=2;
				}
			}
			for (i=this->variables.find(intervals[0]);i!=this->variables.end();i++){
				if (VARIABLE_HAS_NO_DATA(i->second))
					continue;
				writeDWord(i->second->intValue->getInt(),&buffer);
				writeString(i->second->wcsValue->getWcs(),&buffer);
			}
		}
	}
	ulong l;
	char *writebuffer=compressBuffer_BZ2((char *)buffer.c_str(),buffer.size(),&l);
	char *dir=addStrings(save_directory,"global.sav");
	writefile(dir,writebuffer,l);
	delete[] dir;
	delete[] writebuffer;
}

ErrorCode NONS_VariableStore::evaluate(const wchar_t *exp,long *result,bool invert_terms){
	std::vector<simpleoperation<char> *> operations;
	char *exp2=copyString(exp);
	_HANDLE_POSSIBLE_ERRORS(parse_expression(exp2,0,&operations),
		for (ulong a=0;a<operations.size();a++)
			delete operations[a];
		)
	if (CLOptions.verbosity>=2)
		v_stdlog <<"Expression: \""<<exp2<<"\""<<std::endl;
	delete[] exp2;
	long *results=new long[operations.size()];
	static const char *operators[]={"||","&&","==","!=","<>",">=","<=","=",">","<","+","-","*","/","|","&",0};
	for (ulong a=0;a<operations.size();a++){
		long opA,opB,res;
		switch (operations[a]->operandA->type){
			case 0:
				opA=operations[a]->operandA->constant;
				break;
			case 1:
				{
					NONS_VariableMember *var;
					if (*(operations[a]->operandA->symbol)!='?'){
						wchar_t *temp=copyWString(operations[a]->operandA->symbol);
						ErrorCode error;
						var=this->retrieve(temp,&error);
						delete[] temp;
						if (!var)
							return NONS_UNDEFINED_CONSTANT;
					}else{
						wchar_t *temp=copyWString(operations[a]->operandA->symbol);
						_HANDLE_POSSIBLE_ERRORS(this->resolveIndexing(temp,&var),
							delete[] temp;
							delete[] results;)
						delete[] temp;
					}
					/*if (var->getType()!='%')
						return NONS_NON_INTEGRAL_VARIABLE_IN_EXPRESSION;*/
					opA=var->getInt();
				}
				break;
			case 2:
				opA=results[operations[a]->operandA->position];
		}
		if (!operations[a]->function){
			results[a]=opA;
			if (invert_terms)
				results[a]=!results[a];
			continue;
		}
		if (!strcmp(operations[a]->function,"fchk")){
			char *name=0;
			_HANDLE_POSSIBLE_ERRORS(this->getStrValue(operations[a]->operandA->symbol,&name),
				delete[] results;
				for (ulong a=0;a<operations.size();a++)
					delete operations[a];
				)
			results[a]=ImageLoader->filelog.check(name);
			if (invert_terms)
				results[a]=!results[a];
			continue;
		}
		if (!strcmp(operations[a]->function,"lchk")){
			wchar_t *name=0;
			char *temp=0;
			_HANDLE_POSSIBLE_ERRORS(this->getStrValue(operations[a]->operandA->symbol,&temp),
				delete[] results;
				for (ulong a=0;a<operations.size();a++)
					delete operations[a];
				)
			name=copyWString(temp);
			delete[] temp;
			results[a]=(labelsUsed.find(name)!=labelsUsed.end());
			delete[] name;
			if (invert_terms)
				results[a]=!results[a];
			continue;
		}
		switch (operations[a]->operandB->type){
			case 0:
				opB=operations[a]->operandB->constant;
				break;
			case 1:
				{
					NONS_VariableMember *var;
					if (*(operations[a]->operandB->symbol)!='?'){
						wchar_t *temp=copyWString(operations[a]->operandB->symbol);
						ErrorCode error;
						var=this->retrieve(temp,&error);
						delete[] temp;
						if (!var)
							return error;
					}else{
						wchar_t *temp=copyWString(operations[a]->operandB->symbol);
						_HANDLE_POSSIBLE_ERRORS(this->resolveIndexing(temp,&var),
							delete[] temp;
							delete[] results;
							for (ulong a=0;a<operations.size();a++)
								delete operations[a];
							)
						delete[] temp;
					}
					if (var->getType()!='%')
						return NONS_NON_INTEGRAL_VARIABLE_IN_EXPRESSION;
					opB=var->getInt();
				}
				break;
			case 2:
				opB=results[operations[a]->operandB->position];
		}
		short function=-1;
		for (short b=0;operators[b] && function==-1;b++)
			if (!strcmp(operations[a]->function,operators[b]))
				function=b;
		switch (function){
			case 0:
				res=opA||opB;
				break;
			case 1:
				res=opA&&opB;
				break;
			case 2:case 7:
				res=opA==opB;
				if (invert_terms)
					res=!res;
				break;
			case 3:case 4:
				res=opA!=opB;
				if (invert_terms)
					res=!res;
				break;
			case 5:
				res=opA>=opB;
				if (invert_terms)
					res=!res;
				break;
			case 6:
				res=opA<=opB;
				if (invert_terms)
					res=!res;
				break;
			case 8:
				res=opA>opB;
				if (invert_terms)
					res=!res;
				break;
			case 9:
				res=opA<opB;
				if (invert_terms)
					res=!res;
				break;
			case 10:
				res=opA+opB;
				break;
			case 11:
				res=opA-opB;
				break;
			case 12:
				res=opA*opB;
				break;
			case 13:
				if (!opB)
					return NONS_DIVISION_BY_ZERO;
				res=opA/opB;
				break;
			case 14:
				res=opA||opB;
				break;
			case 15:
				res=opA&&opB;
				break;
			default:
				res=0;
		}
		results[a]=res;
	}
	if (result){
		*result=results[operations.size()-1];
		if (CLOptions.verbosity>=2){
			for (ulong a=0;a<operations.size();a++){
				v_stdlog <<a<<": "<<(operations[a]->function?operations[a]->function:"NOP")<<"(";
				switch (operations[a]->operandA->type){
				case 0:
					v_stdlog <<operations[a]->operandA->constant;
					break;
				case 1:
					v_stdlog <<operations[a]->operandA->symbol;
					break;
				case 2:
					v_stdlog <<"&"<<operations[a]->operandA->position;
				}
				if (operations[a]->operandB){
					v_stdlog <<",";
					switch (operations[a]->operandB->type){
					case 0:
						v_stdlog <<operations[a]->operandB->constant;
						break;
					case 1:
						v_stdlog <<operations[a]->operandB->symbol;
						break;
					case 2:
						v_stdlog <<"&"<<operations[a]->operandB->position;
					}
				}
				v_stdlog <<")"<<std::endl;
			}
			v_stdlog <<"Result: "<<results[operations.size()-1]<<std::endl;
		}
	}
	delete[] results;
	for (ulong a=0;a<operations.size();a++)
		delete operations[a];
	return NONS_NO_ERROR;
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

template <typename T>
int atoi2(T *str){
	char *temp=copyString(str);
	int res=atoi(temp);
	delete[] temp;
	return res;
}

NONS_VariableMember *NONS_VariableStore::retrieve(const wchar_t *name,ErrorCode *error){
	if (!name || !*name)
		return 0;
	if (*name=='%' || *name=='$'){
		NONS_VariableMember *var=this->retrieve(name+1,0);
		if (!var){
			long res=0;
			ErrorCode localError=this->evaluate(name+1,&res);
			if (localError!=NONS_NO_ERROR){
				if (!!error)
					*error=localError;
				return 0;
			}
			NONS_Variable *tempVar=this->retrieve(res,&localError);
			if (!tempVar){
				if (!!error)
					*error=localError;
				return 0;
			}
			if (!!error)
				*error=NONS_NO_ERROR;
			if (*name=='%')
				return tempVar->intValue;
			else
				return tempVar->wcsValue;
		}
		if (isanumber(name+1)){
			if (!!error)
				*error=NONS_NO_ERROR;
			return var;
		}
		if (var->getType()=='%'){
			ErrorCode localError;
			NONS_Variable *tempVar=this->retrieve(var->getInt(),&localError);
			if (!tempVar){
				if (!!error)
					*error=localError;
				return 0;
			}
			if (!!error)
				*error=NONS_NO_ERROR;
			if (*name=='%')
				return tempVar->intValue;
			else
				return tempVar->wcsValue;
		}else{
			int temp=atoi2(var->getWcs());
			ErrorCode localError;
			NONS_Variable *tempVar=this->retrieve(temp,&localError);
			if (!tempVar){
				if (!!error)
					*error=localError;
				return 0;
			}
			if (*name=='%')
				return tempVar->intValue;
			else
				return tempVar->wcsValue;
		}
	}
	if (*name=='?'){
		NONS_VariableMember *res=0;
		ErrorCode error=this->resolveIndexing(name,&res);
		if (error!=NONS_NO_ERROR){
			handleErrors(error,-1,"NONS_VariableStore::retrieve",1);
			return 0;
		}
		return res;
	}
	std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI>::iterator i=this->constants.find((wchar_t *)name);
	if (i!=this->constants.end())
		return i->second;
	return 0;
	/*if (!isanumber(name))
		return 0;
	char *copy=copyString(name);
	ulong n=atol(copy);
	delete[] copy;
	return this->retrieve(n);*/
}

NONS_Variable *NONS_VariableStore::retrieve(Sint32 position,ErrorCode *error){
	if (position<-1073741824 || position>1073741823){
		if (!!error)
			*error=NONS_VARIABLE_OUT_OF_RANGE;
		return 0;
	}
	if (!!error)
		*error=NONS_NO_ERROR;
	variables_map_T::iterator i=this->variables.find(position);
	if (i==this->variables.end()){
		//SDL_LockMutex(exitMutex);
		NONS_Variable *var=new NONS_Variable();
		this->variables[position]=var;
		//SDL_UnlockMutex(exitMutex);
		return var;
	}
	if (!i->second)
		i->second=new NONS_Variable();
	return i->second;
}

NONS_VariableMember *NONS_VariableStore::retrieve(const wchar_t *name,const std::vector<ulong> *index,ErrorCode *error){
	if (!name || !*name || !index){
		if (!!error)
			*error=NONS_INTERNAL_INVALID_PARAMETER;
		return 0;
	}
	if (*name=='?')
		return this->retrieve(name+1,index,error);
	std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI>::iterator i=this->arrays.find((wchar_t *)name);
	if (i==this->arrays.end()){
		if (!!error)
			*error=NONS_UNDEFINED_ARRAY;
		return 0;
	}
	NONS_VariableMember *var=i->second;
	for (ulong a=0;a<index->size();a++){
		if ((*index)[a]>=var->dimensionSize){
			if (!!error)
				*error=NONS_OUT_OF_BOUNDS;
			return 0;
		}
		var=var->dimension[(*index)[a]];
	}
	if (!!error)
		*error=NONS_NO_ERROR;
	return var;
}

NONS_VariableMember *NONS_VariableStore::retrieve(const wchar_t *name,ulong *index,ulong size,ErrorCode *error){
	if (!name || !*name || !index && size){
		if (!!error)
			*error=NONS_INTERNAL_INVALID_PARAMETER;
		return 0;
	}
	if (*name=='?')
		return this->retrieve(name+1,index,size,error);
	std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI>::iterator i=this->arrays.find((wchar_t *)name);
	if (i==this->arrays.end()){
		if (!!error)
			*error=NONS_UNDEFINED_ARRAY;
		return 0;
	}
	NONS_VariableMember *var=i->second;
	for (ulong a=0;a<size;a++){
		if (index[a]>=var->dimensionSize){
			if (!!error)
				*error=NONS_OUT_OF_BOUNDS;
			return 0;
		}
		var=var->dimension[index[a]];
	}
	if (!!error)
		*error=NONS_NO_ERROR;
	return var;
}

ErrorCode NONS_VariableStore::resolveIndexing(const wchar_t *expression,NONS_VariableMember **res){
	if (!expression || !res)
		return NONS_INTERNAL_INVALID_PARAMETER;
	if (*expression=='?')
		return this->resolveIndexing(expression+1,res);
	long first=instr(expression,L"[");
	if (first<0)
		return NONS_INTERNAL_INVALID_PARAMETER;
	wchar_t *copy=copyWString(expression,first);
	NONS_VariableMember *var=this->retrieve(copy,0,0);
	delete[] copy;
	if (!var)
		return NONS_UNDEFINED_CONSTANT;
	if (var->getType()=='$'){
		long temp;
		ErrorCode error=this->evaluate(var->getWcs(),&temp);
		if (CHECK_FLAG(error,NONS_NO_ERROR))
			return error;
		if (!this->retrieve(temp,&error))
			return error;
		var=this->retrieve(temp,0)->intValue;
	}
	std::vector<ulong> indices;
	while (first>=0){
		expression+=first+1;
		ulong nesting=1;
		for (first=0;expression[first];first++){
			switch (expression[first]){
				case '[':
					nesting++;
					break;
				case ']':
					nesting--;
			}
			if (!nesting)
				break;
		}
		copy=copyWString(expression,first);
		ulong pusher;
		_HANDLE_POSSIBLE_ERRORS(this->evaluate(copy,(long *)&pusher),delete[] copy;)
		delete[] copy;
		indices.push_back(pusher);
		expression+=first+1;
		first=instr(expression,L"[");
	}
	for (ulong a=0;a<indices.size();a++){
		if (indices[a]>=var->dimensionSize)
			return NONS_ARRAY_INDEX_OUT_OF_BOUNDS;
		var=var->dimension[indices[a]];
	}
	*res=var;
	return NONS_NO_ERROR;
}

ErrorCode NONS_VariableStore::getStrValue(wchar_t *str,char **value){
	if (*str=='%')
		return NONS_UNMATCHING_OPERANDS;
	if (*str=='\"' || *str=='`'){
		wchar_t find[]={0,0};
		find[0]=*str;
		long endstr=instr(str+1,find);
		if (endstr<0)
			return NONS_UNMATCHED_QUOTES;
		if (*value)
			delete[] *value;
		*value=copyString(str+1,endstr);
	}else{
		NONS_VariableMember *var=0;
		if (*str=='?'){
			_HANDLE_POSSIBLE_ERRORS(this->resolveIndexing(str,&var),)
		}else{
			ErrorCode error;
			var=this->retrieve(str,&error);
			if (!var)
				return error;
		}
		if (*value)
			delete[] *value;
		if (!var->getWcs())
			*value=copyString("");
		*value=copyString(var->getWcs());
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_VariableStore::getStrValue(char *str,char **value){
	wchar_t *copy=copyWString(str);
	ErrorCode ret=this->getStrValue(copy,value);
	delete[] copy;
	return ret;
}
#endif
