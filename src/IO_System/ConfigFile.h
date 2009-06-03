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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <map>
#include <cstring>
#include "../UTF.h"
#include "../enums.h"
#include "FileIO.h"

typedef std::map<std::wstring,std::vector<std::wstring>,stdStringCmpCI<wchar_t> > config_map_t;

struct ConfigFile{
	ConfigFile();
	ConfigFile(const char *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	ConfigFile(const std::wstring &filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	~ConfigFile();
	std::wstring getWString(const std::wstring &index,ulong subindex=0);
	long getInt(const std::wstring &index,ulong subindex=0);
	void assignWString(const std::wstring &var,const std::wstring &val,ulong subindex=0);
	void assignInt(const std::wstring &var,long val,ulong subindex=0);
	void writeOut(const char *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	std::string writeOut(ENCODINGS encoding=ISO_8859_1_ENCODING);
	bool exists(const std::wstring &var);
private:
	config_map_t entries;
	void init(const char *filename,ENCODINGS encoding);
};
#endif
