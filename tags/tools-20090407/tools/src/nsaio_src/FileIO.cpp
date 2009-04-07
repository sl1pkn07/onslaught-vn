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
#include "Functions.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

uchar *readfile(const char *filename,long *len,long offset){
	if (!filename)
		return 0;
	std::ifstream file(filename,std::ios::binary);
	if (!file){
		if (len)
			*len=-1;
		return 0;
	}
	return readfile(&file,len,offset);
}

uchar *readfile(std::ifstream *file,long *len,long offset){
	if (!*file){
		if (len)
			*len=-1;
		return 0;
	}
	long originalPosition=file->tellg();
	file->seekg(0,std::ios::end);
	long size=file->tellg();
	file->seekg(offset,std::ios::beg);
	size=size-offset>=*len?*len:size-offset;
	if (len)
		*len=size;
	uchar *buffer=new uchar[size];
	file->read((char *)buffer,size);
	file->seekg(originalPosition,std::ios::beg);
	return buffer;
}

uchar *readfile(const char *name,long *len){
	std::ifstream file(name,std::ios::binary);
	if (!file){
		if (len)
			*len=-1;
		return 0;
	}
	file.seekg(0,std::ios::end);
	long pos=file.tellg();
	if (len)
		*len=pos;
	file.seekg(0,std::ios::beg);
	uchar *buffer=new uchar[pos];
	file.read((char *)buffer,pos);
	file.close();
	return buffer;
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

char writefile(const char *name,char *buffer,long size){
	std::ofstream file(name,std::ios::binary);
	if (!file){
#if defined(_WIN32) || defined(_WIN64)
		std::cerr <<"writefile():"<<GetLastError()<<std::endl;
#endif
		return 1;
	}
	file.write(buffer,size);
	file.close();
	return 0;
}

char writefile(const wchar_t *name,char *buffer,long size){
	char *temp=copyString(name);
	boost::filesystem::ofstream file(boost::filesystem::path(temp),std::ios::binary);
	delete[] temp;
	if (!file){
#if defined(_WIN32) || defined(_WIN64)
		std::cerr <<"writefile():"<<GetLastError()<<std::endl;
#endif
		return 1;
	}
	file.write(buffer,size);
	file.close();
	return 0;
}

char writefile(const std::wstring &name,char *buffer,long size){
	return writefile(name.c_str(),buffer,size);
}

bool fileExists(const char *name){
	std::ifstream file(name);
	bool ret=!!file;
	file.close();
	return ret;
}
#endif
