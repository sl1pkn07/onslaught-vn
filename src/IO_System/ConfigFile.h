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
*     * Helios' name may not be used to endorse or promote products derived from
*       this software without specific prior written permission.
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
#include "../strCmpT.h"
#include "../UTF.h"
#include "../enums.h"
#include "FileIO.h"

struct ConfigFile{
	std::map<wchar_t *,std::vector<wchar_t *> *,wstrCmpCI> entries;
	ConfigFile();
	ConfigFile(char *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	ConfigFile(wchar_t *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	~ConfigFile();
	wchar_t *getWString(const wchar_t *index,long subindex=0);
	long getInt(const wchar_t *index,long subindex=0);
	void assignWString(const wchar_t *var,const wchar_t *val,long subindex=0);
	void assignInt(const wchar_t *var,long val,long subindex=0);
	void writeOut(char *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	std::string *writeOut(ENCODINGS encoding=ISO_8859_1_ENCODING);
	bool exists(const wchar_t *var);
private:
	void init(char *filename,ENCODINGS encoding);
};
#endif