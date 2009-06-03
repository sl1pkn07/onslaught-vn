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
	ulong l;
	if (!config_directory.size())
		config_directory=getConfigLocation();
	this->saveAs=save_directory;
	this->saveAs.append(newName);
	char *buffer=(char *)readfile(this->saveAs.c_str(),l);
	if (!buffer)
		buffer=(char *)readfile(oldName,l);
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
	ulong entries=atol(copy);
	delete[] copy;
	ulong offset=str-buffer+1;
	for (;buffer[offset]==10 || buffer[offset]==13;offset++);
	bool newFormat=0;
	if (!instr(buffer+offset,"NONS")){
		newFormat=1;
		for (offset+=4;buffer[offset]==10 || buffer[offset]==13;offset++);
		inPlaceDecryption(buffer+offset,l-offset,XOR84_ENCRYPTION);
		for (ulong a=0;offset<l && a<entries;a++){
			std::wstring newElement=UniFromUTF8(std::string(buffer+offset));
			offset+=strlen(buffer+offset)+1;
			this->addString(newElement);
		}
	}else{
		for (ulong a=0;offset<l && a<entries;a++){
			offset++;
			ulong filel;
			for (filel=0;filel+offset<l && buffer[filel+offset]!='"';filel++);
			if (buffer[filel+offset]!='"')
				break;
			inPlaceDecryption(buffer+offset,filel,XOR84_ENCRYPTION);
			std::wstring newElement=UniFromSJIS(std::string(buffer+offset,filel));
			this->addString(newElement);
			offset+=filel+1;
		}
	}
	delete[] buffer;
}

NONS_LogStrings::~NONS_LogStrings(){
	if (this->commit)
		this->writeOut();
}

void NONS_LogStrings::writeOut(){
	std::stringstream stream;
	stream <<this->log.size();
	std::string buf(stream.str());
	buf.append("\x0ANONS\x0A");
	ulong startEncryption=buf.size();
	for (logSet_t::iterator i=this->log.begin();i!=this->log.end();i++){
		buf.append(UniToUTF8(*i));
		buf.push_back(0);
	}
	inPlaceDecryption(&buf[startEncryption],buf.size()-startEncryption,XOR84_ENCRYPTION);
	ulong l;
	char *writebuffer=compressBuffer_BZ2(&buf[0],buf.size(),&l);
	writefile(this->saveAs.c_str(),writebuffer,l);
	delete[] writebuffer;
}

bool NONS_LogStrings::addString(const std::wstring &string){
	if (this->log.find(string)!=this->log.end())
		return 0;
	SDL_LockMutex(exitMutex);
	this->log.insert(string);
	SDL_UnlockMutex(exitMutex);
	return 1;
}

bool NONS_LogStrings::check(const std::wstring &string){
	return this->log.find(string)!=this->log.end();
}

//------------------------------------------------------------------------------

bool NONS_FileLog::addString(const std::wstring &string){
	if (this->check(string))
		return 0;
	std::wstring a=string;
	tolower(a);
	toforwardslash(a);
	SDL_LockMutex(exitMutex);
	this->log.insert(a);
	SDL_UnlockMutex(exitMutex);
	return 1;
}

bool NONS_FileLog::check(std::wstring string){
	tolower(string);
	toforwardslash(string);
	return this->log.find(string)!=this->log.end();
}

//------------------------------------------------------------------------------

bool NONS_LabelLog::addString(const std::wstring &string){
	if (this->check(string))
		return 0;
	size_t a=0;
	for (;string[a]=='*';a++);
	std::wstring copy(string,a);
	tolower(copy);
	SDL_LockMutex(exitMutex);
	this->log.insert(copy);
	SDL_UnlockMutex(exitMutex);
	return 1;
}

bool NONS_LabelLog::check(const std::wstring &string){
	size_t a=0;
	for (;string[a]=='*';a++);
	std::wstring copy(string,a);
	bool ret=(this->log.find(copy)!=this->log.end());
	return ret;
}
#endif
