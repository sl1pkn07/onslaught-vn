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

#ifndef NONS_FILEIO_CPP
#define NONS_FILEIO_CPP

#include "FileIO.h"
#ifndef BARE_FILE
#include "../Globals.h"
#include "../UTF.h"
#endif

uchar *readfile(const std::wstring &filename,ulong &len,ulong offset){
	std::ifstream file(wstrToIOstr(filename).c_str(),std::ios::binary);
	if (!file)
		return 0;
	return readfile(file,len,offset);
}

uchar *readfile(std::ifstream &file,ulong &len,ulong offset){
	if (!file)
		return 0;
	ulong originalPosition=file.tellg();
	file.seekg(0,std::ios::end);
	ulong size=file.tellg();
	file.seekg(offset,std::ios::beg);
	size=size-offset>=len?len:size-offset;
	len=size;
	uchar *buffer=new uchar[size];
	file.read((char *)buffer,size);
	file.seekg(originalPosition,std::ios::beg);
	return buffer;
}

uchar *readfile(const std::wstring &name,ulong &len){
	std::ifstream file(wstrToIOstr(name).c_str(),std::ios::binary);
	if (!file)
		return 0;
	file.seekg(0,std::ios::end);
	long pos=file.tellg();
	len=pos;
	file.seekg(0,std::ios::beg);
	uchar *buffer=new uchar[pos];
	file.read((char *)buffer,pos);
	file.close();
	return buffer;
}

#if defined(NONS_SYS_WINDOWS)
#include <windows.h>
#endif

char writefile(const std::wstring &name,char *buffer,ulong size){
	std::ofstream file(wstrToIOstr(name).c_str(),std::ios::binary);
	if (!file){
#ifndef BARE_FILE
#if defined(NONS_SYS_WINDOWS)
		o_stderr <<"writefile(): "<<GetLastError()<<'\n';
#endif
#endif
		return 1;
	}
	file.write(buffer,size);
	file.close();
	return 0;
}

bool fileExists(const std::wstring &name){
	std::ifstream file(wstrToIOstr(name).c_str());
	bool ret=!!file;
	file.close();
	return ret;
}

std::string wstrToIOstr(const std::wstring &string){
#ifdef NONS_SYS_LINUX
	return UniToUTF8(string);
#else
	return UniToISO88591(string);
#endif
}
#endif
