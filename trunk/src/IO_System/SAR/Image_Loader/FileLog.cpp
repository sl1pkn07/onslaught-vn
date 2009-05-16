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

#ifndef NONS_FILELOG_CPP
#define NONS_FILELOG_CPP

#include "FileLog.h"
#include "../../../Encryption.h"
#include "../../FileIO.h"
#include "../../SaveFile.h"
#include "../../../Functions.h"
#include "../../../Globals.h"
#include "../../../UTF.h"
#include <cstring>

NONS_LogStrings::NONS_LogStrings(const char *oldName,const char *newName){
	this->init(oldName,newName);
}

void NONS_LogStrings::init(const char *oldName,const char *newName){
	this->commit=0;
	long l;
	if (!config_directory)
		config_directory=getConfigLocation();
	this->saveAs=save_directory;
	this->saveAs.append(newName);
	char *buffer=(char *)readfile(this->saveAs.c_str(),&l);
	if (!buffer)
		buffer=(char *)readfile(oldName,&l);
	if (!buffer)
		return;
	if (!instr(buffer,"BZh")){
		char *temp=decompressBuffer_BZ2(buffer,l,(unsigned long *)&l);
		delete[] buffer;
		buffer=temp;
	}
	char *str=strpbrk(buffer,"\x0A\x0D");
	if (!str){
		delete[] buffer;
		return;
	}
	*str=0;
	char *copy=copyString(buffer);
	long entries=atol(copy);
	delete[] copy;
	long offset=str-buffer+1;
	for (;buffer[offset]==10 || buffer[offset]==13;offset++);
	bool newFormat=0;
	if (!instr(buffer+offset,"NONS")){
		newFormat=1;
		for (offset+=4;buffer[offset]==10 || buffer[offset]==13;offset++);
		inPlaceDecryption(buffer+offset,l-offset,XOR84_ENCRYPTION);
		for (long a=0;offset<l && a<entries;a++){
			wchar_t *newElement=UTF8_to_WChar(buffer+offset);
			offset+=strlen(buffer+offset)+1;
			if (!this->addString(newElement,1))
				delete[] newElement;
		}
	}else{
		for (long a=0;offset<l && a<entries;a++){
			offset++;
			long filel;
			for (filel=0;filel+offset<l && buffer[filel+offset]!='"';filel++);
			if (buffer[filel+offset]!='"')
				break;
			inPlaceDecryption(buffer+offset,filel,XOR84_ENCRYPTION);
			wchar_t *newElement;
			char *temp=copyString(buffer+offset,filel);
			newElement=SJIS_to_WChar(temp);
			delete[] temp;
			if (!this->addString(newElement,1))
				delete[] newElement;
			offset+=filel+1;
		}
	}
	delete[] buffer;
}

NONS_LogStrings::~NONS_LogStrings(){
	if (this->commit)
		this->writeOut();
	for (logSet_t::iterator i=this->log.begin();i!=this->log.end();i++)
		delete[] *i;
}

void NONS_LogStrings::writeOut(){
	std::stringstream stream;
	stream <<this->log.size();
	std::string buf(stream.str());
	buf.append("\x0ANONS\x0A");
	ulong startEncryption=buf.size();
	for (logSet_t::iterator i=this->log.begin();i!=this->log.end();i++){
		char *temp=WChar_to_UTF8(*i);
		buf.append(temp);
		buf.push_back(0);
		delete[] temp;
	}
	char *charBuf=new char[buf.size()];
	memcpy(charBuf,buf.c_str(),buf.size());
	inPlaceDecryption(charBuf+startEncryption,buf.size()-startEncryption,XOR84_ENCRYPTION);
	ulong l;
	char *writebuffer=compressBuffer_BZ2(charBuf,buf.size(),&l);
	delete[] charBuf;
	writefile(this->saveAs.c_str(),writebuffer,l);
	delete[] writebuffer;
}

bool NONS_LogStrings::addString(const wchar_t *string,bool takeOwnership){
	if (this->log.find((wchar_t *)string)!=this->log.end()){
		if (takeOwnership)
			delete[] string;
		return 0;
	}
	wchar_t *a;
	if (takeOwnership)
		a=(wchar_t *)string;
	else
		a=copyWString(string);
	SDL_LockMutex(exitMutex);
	this->log.insert(a);
	SDL_UnlockMutex(exitMutex);
	return 1;
}

bool NONS_LogStrings::addString(const char *string){
	wchar_t *copy=copyWString(string);
	if (!this->addString(copy,1)){
		delete[] copy;
		return 0;
	}
	return 1;
}

bool NONS_LogStrings::check(const wchar_t *string){
	return this->log.find((wchar_t *)string)!=this->log.end();
}

bool NONS_LogStrings::check(const char *string){
	wchar_t *copy=copyWString(string);
	bool ret=this->check(copy);
	delete[] copy;
	return ret;
}

//------------------------------------------------------------------------------

bool NONS_FileLog::addString(const wchar_t *string,bool takeOwnership){
	if (this->check(string)){
		if (takeOwnership)
			delete[] string;
		return 0;
	}
	wchar_t *a;
	if (takeOwnership)
		a=(wchar_t *)string;
	else
		a=copyWString(string);
	NONS_tolower(a);
	toforwardslash(a);
	SDL_LockMutex(exitMutex);
	this->log.insert(a);
	SDL_UnlockMutex(exitMutex);
	return 1;
}

bool NONS_FileLog::addString(const char *string){
	wchar_t *copy=copyWString(string);
	if (!this->addString(copy,1)){
		delete[] copy;
		return 0;
	}
	return 1;
}

bool NONS_FileLog::check(const wchar_t *string){
	wchar_t *copy=copyWString(string);
	NONS_tolower(copy);
	toforwardslash(copy);
	bool ret=(this->log.find(copy)!=this->log.end());
	delete[] copy;
	return ret;
}

bool NONS_FileLog::check(const char *string){
	wchar_t *copy=copyWString(string);
	bool ret=this->check(copy);
	delete[] copy;
	return ret;
}

//------------------------------------------------------------------------------

bool NONS_LabelLog::addString(const wchar_t *string,bool takeOwnership){
	if (this->check(string)){
		if (takeOwnership)
			delete[] string;
		return 0;
	}
	wchar_t *a;
	const wchar_t *b=string;
	for (;*b=='*';b++);
	if (!*b){
		if (takeOwnership)
			delete[] string;
		return 0;
	}
	if (takeOwnership){
		if (b==string)
			a=(wchar_t *)string;
		else{
			a=copyWString(b);
			delete[] string;
		}
	}else
		a=copyWString(b);
	NONS_tolower(a);
	SDL_LockMutex(exitMutex);
	this->log.insert(a);
	SDL_UnlockMutex(exitMutex);
	return 1;
}

bool NONS_LabelLog::addString(const char *string){
	for (;*string=='*';string++);
	wchar_t *copy=copyWString(string);
	if (!this->addString(copy,1)){
		delete[] copy;
		return 0;
	}
	return 1;
}

bool NONS_LabelLog::check(const wchar_t *string){
	for (;*string=='*';string++);
	bool ret=(this->log.find((wchar_t *)string)!=this->log.end());
	return ret;
}

bool NONS_LabelLog::check(const char *string){
	for (;*string=='*';string++);
	wchar_t *copy=copyWString(string);
	bool ret=this->check(copy);
	delete[] copy;
	return ret;
}
#endif
