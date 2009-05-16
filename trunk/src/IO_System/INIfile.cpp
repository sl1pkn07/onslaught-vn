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

#ifndef NONS_INIsection_CPP
#define NONS_INIsection_CPP

#include "INIfile.h"
#include "../Functions.h"
#include "../Globals.h"
#include "FileIO.h"

long nextLine(const wchar_t *buffer,long offset,long l){
	for (;offset<l && buffer[offset]!=13 && buffer[offset]!=10;offset++);
	for (;offset<l && (buffer[offset]==13 || buffer[offset]==10);offset++);
	return offset;
}

template <typename T>
bool isanumber(const T *string){
	if (*string!='-' && (*string<'0' || *string>'9'))
		return 0;
	for (string++;*string;string++)
		if (*string<'0' || *string>'9')
			return 0;
	return 1;
}

INIvalue::INIvalue(long a){
	this->type='i';
	this->strValue=0;
	this->intValue=a;
}

INIvalue::INIvalue(wchar_t *a,bool takeOwnership){
	this->type='s';
	this->intValue=0;
	this->strValue=takeOwnership?a:copyWString(a);
}

INIvalue::INIvalue(const wchar_t *a){
	this->type='s';
	this->intValue=0;
	this->strValue=copyWString(a);
}

INIvalue::INIvalue(const char *a){
	this->type='s';
	this->intValue=0;
	this->strValue=copyWString(a);
}

INIvalue::~INIvalue(){
	if (this->strValue)
		delete[] this->strValue;
}

void INIvalue::setIntValue(long a){
	if (this->type!='i')
		return;
	this->intValue=a;
}

void INIvalue::setStrValue(const char *a){
	if (this->type!='s')
		return;
	if (this->strValue)
		delete[] this->strValue;
	this->strValue=copyWString(a);
}

void INIvalue::setStrValue(wchar_t *a,bool takeOwnership){
	if (this->type!='s'){
		if (takeOwnership)
			delete[] a;
		return;
	}
	if (this->strValue)
		delete[] this->strValue;
	this->strValue=takeOwnership?a:copyWString(a);
}

void INIvalue::setStrValue(const wchar_t *a){
	if (this->type!='s')
		return;
	if (this->strValue)
		delete[] this->strValue;
	this->strValue=copyWString(a);
}

char INIvalue::getType(){
	return this->type;
}

long INIvalue::getIntValue(){
	return this->intValue;
}

char *INIvalue::getStrValue(){
	if (this->type!='s')
		return 0;
	return copyString(this->strValue);
}

wchar_t *INIvalue::getWStrValue(bool getCopy){
	if (this->type!='s')
		return 0;
	return getCopy?copyWString(this->strValue):this->strValue;
}

INIsection::INIsection(){}

INIsection::INIsection(const wchar_t *buffer,long *offset,long l){
	this->readFile(buffer,offset,l);
}

INIsection::~INIsection(){
	for (std::map<wchar_t *,INIvalue *,wstrCmp>::iterator i=this->variables.begin();i!=this->variables.end();i++){
		if (i->first)
			delete[] i->first;
		if (i->second)
			delete i->second;
	}
}

void INIsection::readFile(const wchar_t *buffer,long *offset,long l){
	for (long offset2=*offset;offset2<l;){
		switch (buffer[offset2]){
			case 13:case 10:case ';':
				offset2=nextLine(buffer,offset2,l);
				break;
			case '[':
				*offset=offset2;
				return;
			default:
				{
					long equals=wcspbrk(buffer+offset2,L"=")-buffer,
						nextline=nextLine(buffer,offset2,l);
					if (equals>=nextline){
						offset2=nextline;
						continue;
					}
					for (;iswhitespace((char)buffer[offset2]);offset2++);
					long nameend=equals-1;
					for (;iswhitespace((char)buffer[nameend]);nameend--);
					nameend-=offset2-1;
					wchar_t *name=copyWString(buffer+offset2,nameend);
					if (wcspbrk(name,L";"))
						delete[] name;
						offset2=nextline;
						continue;
					long valstart=equals+1,valend=valstart;
					for (;iswhitespace((char)buffer[valstart]);valstart++);
					if (buffer[valstart]=='"'){
						valstart++;
						long quote=wcspbrk(buffer+valstart,L"\"")-buffer;
						if (quote>=nextline){
							delete[] name;
							offset2=nextline;
							continue;
						}
						valend=quote-valstart;
					}else{
						for (;buffer[valend]!=13 && buffer[valend]!=10 && buffer[valend]!=';';valend++);
						valend-=valstart;
					}
					if (INIvalue *val=this->getValue(name)){
						delete[] name;
						val->setStrValue(copyWString(buffer+valstart,valend),1);
					}else{
						wchar_t *strval=copyWString(buffer+valstart,valend);
						if (isanumber(strval)){
							char *strval2=copyString(strval);
							delete[] strval;
							long number=atol(strval2);
							delete[] strval2;
							INIvalue *p=new INIvalue(number);
							this->variables[name]=p;
						}else{
							INIvalue *p=new INIvalue(strval,1);
							this->variables[name]=p;
						}
					}
					offset2=nextline;
					continue;
				}
		}
	}
}

void INIsection::setIntValue(const char *index,long a){
	wchar_t *temp=copyWString(index);
	this->setIntValue(temp,a);
	delete[] temp;
}

void INIsection::setIntValue(const wchar_t *index,long a){
	INIvalue *v=this->getValue(index);
	if (!v){
		INIvalue *p=new INIvalue(a);
		this->variables[copyWString(index)]=p;
		return;
	}
	v->setIntValue(a);
}

void INIsection::setStrValue(const char *index,wchar_t * a,bool takeOwnership){
	wchar_t *temp=copyWString(index);
	this->setStrValue(temp,a,takeOwnership);
	delete[] temp;
}

void INIsection::setStrValue(const wchar_t *index,wchar_t * a,bool takeOwnership){
	INIvalue *v=this->getValue(index);
	if (!v){
		INIvalue *p=new INIvalue(a,takeOwnership);
		this->variables[copyWString(index)]=p;
		return;
	}
	v->setStrValue(a,takeOwnership);
}

//------------------------------------------------------------------------------

void INIsection::setStrValue(const char *index,const char * a){
	wchar_t *temp=copyWString(index);
	this->setStrValue(temp,a);
	delete[] temp;
}

void INIsection::setStrValue(const wchar_t *index,const char * a){
	INIvalue *v=this->getValue(index);
	if (!v){
		INIvalue *p=new INIvalue(a);
		this->variables[copyWString(index)]=p;
		return;
	}
	v->setStrValue(a);
}

void INIsection::setStrValue(const char *index,const wchar_t *a){
	wchar_t *temp=copyWString(index);
	this->setStrValue(temp,a);
	delete[] temp;
}

void INIsection::setStrValue(const wchar_t *index,const wchar_t *a){
	INIvalue *v=this->getValue(index);
	if (!v){
		INIvalue *p=new INIvalue(a);
		this->variables[copyWString(index)]=p;
		return;
	}
	v->setStrValue(a);
}

char INIsection::getType(const char *index){
	wchar_t *temp=copyWString(index);
	char ret=this->getType(temp);
	delete[] temp;
	return ret;
}

char INIsection::getType(const wchar_t *index){
	INIvalue *v=this->getValue(index);
	if (!v)
		return 0;
	return v->getType();
}

long INIsection::getIntValue(const char *index){
	wchar_t *temp=copyWString(index);
	long ret=this->getIntValue(temp);
	delete[] temp;
	return ret;
}

long INIsection::getIntValue(const wchar_t *index){
	INIvalue *v=this->getValue(index);
	if (!v)
		return 0;
	return v->getIntValue();
}

char * INIsection::getStrValue(const char *index){
	wchar_t *temp=copyWString(index);
	char * ret=this->getStrValue(temp);
	delete[] temp;
	return ret;
}
char * INIsection::getStrValue(const wchar_t *index){
	INIvalue *v=this->getValue(index);
	if (!v)
		return 0;
	return v->getStrValue();
}

wchar_t * INIsection::getWStrValue(const char *index,bool takeOwnership){
	wchar_t *temp=copyWString(index);
	wchar_t * ret=this->getWStrValue(temp,takeOwnership);
	delete[] temp;
	return ret;
}

wchar_t * INIsection::getWStrValue(const wchar_t *index,bool takeOwnership){
	INIvalue *v=this->getValue(index);
	if (!v)
		return 0;
	return v->getWStrValue(takeOwnership);
}

INIvalue *INIsection::getValue(const char *a){
	wchar_t *temp=copyWString(a);
	INIvalue *ret=this->getValue(temp);
	delete[] temp;
	return ret;
}

INIvalue *INIsection::getValue(const wchar_t *a){
	std::map<wchar_t *,INIvalue *,wstrCmp>::iterator i=this->variables.find((wchar_t *)a);
	if (i==this->variables.end())
		return 0;
	return i->second;
}

INIfile::INIfile(){}

INIfile::INIfile(const char *filename,ENCODINGS encoding){
	this->readFile(filename);
}

INIfile::INIfile(const wchar_t *filename,ENCODINGS encoding){
	this->readFile(filename);
}

INIfile::INIfile(const char *buffer,long l,ENCODINGS encoding){
	this->readFile(buffer,l,encoding);
}

INIfile::INIfile(const wchar_t *buffer,long l){
	this->readFile(buffer,l);
}

INIfile::~INIfile(){
	for (std::map<wchar_t *,INIsection *,wstrCmp>::iterator i=this->sections.begin();i!=this->sections.end();i++){
		if (i->first)
			delete[] i->first;
		if (i->second)
			delete i->second;
	}
}

ErrorCode INIfile::readFile(const char *filename,ENCODINGS encoding){
	long l;
	char *buffer=(char *)readfile(filename,&l);
	if (!buffer)
		return NONS_FILE_NOT_FOUND;
	this->readFile(buffer,l,encoding);
	return NONS_NO_ERROR;
}

ErrorCode INIfile::readFile(const wchar_t *filename,ENCODINGS encoding){
	char *temp=copyString(filename);
	ErrorCode ret=this->readFile(temp);
	delete[] temp;
	return ret;
}

void INIfile::readFile(const char *buffer,long l,ENCODINGS encoding){
	wchar_t *buffer2;
	switch (encoding){
		case ISO_8859_1_ENCODING:
			buffer2=ISO88591_to_WChar(buffer,l,&l);
			break;
		case SJIS_ENCODING:
			buffer2=SJIS_to_WChar(buffer,l,&l);
			break;
		case UCS2_ENCODING:
			buffer2=UCS2_to_WChar(buffer,l,&l);
			break;
		case UTF8_ENCODING:
			buffer2=UTF8_to_WChar(buffer,l,&l);
	}
	this->readFile(buffer2,l);
	delete[] buffer2;
}

void INIfile::readFile(const wchar_t *buffer,long l){
	long offset=0;
	INIsection *sec=this->getSection(L"");
	if (!sec){
		INIsection *p=new INIsection(buffer,&offset,l);
		this->sections[copyWString("")]=p;
	}else
		this->sections[(wchar_t *)L""]->readFile(buffer,&offset,l);
	for (;offset<l;){
		if (buffer[offset]=='['){
			long closing=wcspbrk(buffer+offset,L"]")-buffer,
				nextline=nextLine(buffer,offset,l);
			if (closing>=nextline){
				offset=nextline;
				continue;
			}
			for (offset++;iswhitespace((char)buffer[offset]);offset++);
			for (closing--;iswhitespace((char)buffer[closing]);closing--);
			closing-=offset-1;
			wchar_t *str=copyWString(buffer+offset,closing);
			if (sec=this->getSection(str)){
				delete[] str;
				offset=nextLine(buffer,offset,l);
				sec->readFile(buffer,&offset,l);
				continue;
			}
			INIsection *p=new INIsection(buffer,&offset,l);
			this->sections[str]=p;
		}else
			offset=nextLine(buffer,offset,l);
	}
}

INIsection *INIfile::getSection(const char *index){
	wchar_t *temp=copyWString(index);
	INIsection *ret=this->getSection(temp);
	delete[] temp;
	return ret;
}

INIsection *INIfile::getSection(const wchar_t *index){
	std::map<wchar_t *,INIsection *,wstrCmp>::iterator i=this->sections.find((wchar_t *)index);
	if (i==this->sections.end())
		return 0;
	return i->second;
}
#endif
