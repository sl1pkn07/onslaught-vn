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
#ifndef TOOLS_BARE_FILE
#ifndef TOOLS_NSAIO
#include "../Globals.h"
#endif
#include "../UTF.h"
#endif

uchar *readfile(const std::wstring &filename,ulong &len,ulong offset){
#ifdef NONS_SYS_WINDOWS
	HANDLE file=CreateFile(&filename[0],FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (file==INVALID_HANDLE_VALUE)
		return 0;
	uchar *ret=readfile(file,len,offset);
	CloseHandle(file);
	return ret;
#else
	std::ifstream file(UniToUTF8(filename).c_str(),std::ios::binary);
	if (!file)
		return 0;
	return readfile(file,len,offset);
#endif
}

#ifdef NONS_SYS_WINDOWS
uchar *readfile(HANDLE file,ulong &len,ulong offset){
	if (file==INVALID_HANDLE_VALUE)
		return 0;
	DWORD filesize=GetFileSize(file,0),
		originalPosition=SetFilePointer(file,0,0,FILE_CURRENT);

	if (offset>=filesize)
		offset=filesize-1;

	SetFilePointer(file,offset,0,FILE_BEGIN);

	filesize=filesize-offset>=len?len:filesize-offset;

	uchar *buffer=new uchar[filesize];
	ReadFile(file,buffer,filesize,&filesize,0);

	len=filesize;
	return buffer;
}
#else
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
#endif

uchar *readfile(const std::wstring &name,ulong &len){
#ifdef NONS_SYS_WINDOWS
	HANDLE file=CreateFile(&name[0],FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
	if (file==INVALID_HANDLE_VALUE)
		return 0;
	DWORD filesize;
	filesize=GetFileSize(file,0);
	
	uchar *buffer=new uchar[filesize];
	ReadFile(file,buffer,filesize,&filesize,0);
	CloseHandle(file);

	len=filesize;
	return buffer;
#else
	std::ifstream file(UniToUTF8(name).c_str(),std::ios::binary|std::ios::ate);
	if (!file)
		return 0;
	ulong pos=file.tellg();
	len=pos;
	file.seekg(0,std::ios::beg);
	uchar *buffer=new uchar[pos];
	file.read((char *)buffer,pos);
	file.close();
	return buffer;
#endif
}

char writefile(const std::wstring &name,char *buffer,ulong size){
#ifdef NONS_SYS_WINDOWS
	HANDLE file=CreateFile(&name[0],GENERIC_WRITE,0,0,TRUNCATE_EXISTING,0,0);
	if (file==INVALID_HANDLE_VALUE)
		file=CreateFile(&name[0],GENERIC_WRITE,0,0,CREATE_NEW,0,0);
	if (file==INVALID_HANDLE_VALUE)
		return 1;
	WriteFile(file,buffer,size,&size,0);
	CloseHandle(file);
	return 0;
#else
	std::ofstream file(UniToUTF8(name).c_str(),std::ios::binary);
	if (!file)
		return 1;
	file.write(buffer,size);
	file.close();
	return 0;
#endif
}

bool fileExists(const std::wstring &name){
	bool ret;
#ifdef NONS_SYS_WINDOWS
	HANDLE file=CreateFile(&name[0],FILE_READ_DATA,0,0,OPEN_EXISTING,0,0);
	ret=(file!=INVALID_HANDLE_VALUE);
	CloseHandle(file);
#else
	std::ifstream file(UniToUTF8(name).c_str());
	ret=!!file;
	file.close();
#endif
	return ret;
}
#endif
