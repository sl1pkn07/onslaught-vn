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
	this->intValue=a;
}

INIvalue::INIvalue(const std::wstring &a){
	this->type='s';
	this->intValue=0;
	this->strValue=a;
}

void INIvalue::setIntValue(long a){
	if (this->type!='i')
		return;
	this->intValue=a;
}

void INIvalue::setStrValue(const std::wstring &a){
	if (this->type!='s')
		return;
	this->strValue=a;
}

char INIvalue::getType(){
	return this->type;
}

long INIvalue::getIntValue(){
	return this->intValue;
}

const std::wstring &INIvalue::getStrValue(){
	return this->strValue;
}

INIsection::~INIsection(){
	for (std::map<std::wstring,INIvalue *>::iterator i=this->variables.begin();i!=this->variables.end();i++)
		delete i->second;
}

INIsection::INIsection(const wchar_t *buffer,long *offset,long l){
	this->readFile(buffer,offset,l);
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
						val->setStrValue(std::wstring(buffer+valstart,valend));
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
							INIvalue *p=new INIvalue(std::wstring(strval));
							this->variables[name]=p;
						}
					}
					offset2=nextline;
					continue;
				}
		}
	}
}

void INIsection::setIntValue(const std::wstring &index,long a){
	INIvalue *v=this->getValue(index);
	if (!v)
		this->variables[index]=new INIvalue(a);
	else
		v->setIntValue(a);
}

void INIsection::setStrValue(const std::wstring &index,const std::wstring &a){
	INIvalue *v=this->getValue(index);
	if (!v)
		this->variables[index]=new INIvalue(a);
	else
		v->setStrValue(a);
}

char INIsection::getType(const std::wstring &index){
	INIvalue *v=this->getValue(index);
	if (!v)
		return 0;
	return v->getType();
}

long INIsection::getIntValue(const std::wstring &index){
	INIvalue *v=this->getValue(index);
	if (!v)
		return 0;
	return v->getIntValue();
}

const std::wstring &INIsection::getStrValue(const std::wstring &index){
	INIvalue *v=this->getValue(index);
	if (!v){
		this->setStrValue(index,L"");
		return this->getValue(index)->getStrValue();
	}
	return v->getStrValue();
}

INIvalue *INIsection::getValue(const std::wstring &a){
	std::map<std::wstring,INIvalue *>::iterator i=this->variables.find(a);
	if (i==this->variables.end())
		return 0;
	return i->second;
}

INIfile::INIfile(){}

INIfile::INIfile(const std::string &filename,ENCODINGS encoding){
	this->readFile(filename);
}

INIfile::INIfile(const char *buffer,long l,ENCODINGS encoding){
	this->readFile(buffer,l,encoding);
}

INIfile::INIfile(const wchar_t *buffer,long l){
	this->readFile(buffer,l);
}

INIfile::~INIfile(){
	for (std::map<std::wstring,INIsection *>::iterator i=this->sections.begin();i!=this->sections.end();i++)
		if (i->second)
			delete i->second;
}

ErrorCode INIfile::readFile(const std::string &filename,ENCODINGS encoding){
	ulong l;
	char *buffer=(char *)readfile(filename.c_str(),l);
	if (!buffer)
		return NONS_FILE_NOT_FOUND;
	this->readFile(buffer,l,encoding);
	return NONS_NO_ERROR;
}

void INIfile::readFile(const char *buffer,long l,ENCODINGS encoding){
	std::wstring buffer2;
	switch (encoding){
		case ISO_8859_1_ENCODING:
			buffer2=UniFromISO88591(std::string(buffer,l));
			break;
		case SJIS_ENCODING:
			buffer2=UniFromSJIS(std::string(buffer,l));
			break;
		case UCS2_ENCODING:
			buffer2=UniFromUCS2(std::string(buffer,l),UNDEFINED_ENDIANNESS);
			break;
		case UTF8_ENCODING:
			buffer2=UniFromUTF8(std::string(buffer,l));
	}
	this->readFile(&buffer2[0],buffer2.size());
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

INIsection *INIfile::getSection(const std::wstring &index){
	std::map<std::wstring,INIsection *>::iterator i=this->sections.find(index);
	if (i==this->sections.end())
		return 0;
	return i->second;
}
#endif
