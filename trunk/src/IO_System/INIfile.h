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

#ifndef NONS_INIsection_H
#define NONS_INIsection_H

#include "../Common.h"
#include "../UTF.h"
#include "../strCmpT.h"
#include "../ErrorCodes.h"
#include "../enums.h"
#include <map>

class INIvalue{
	long intValue;
	wchar_t *strValue;
	char type;
public:
	INIvalue(long a);
	INIvalue(char *a);
	INIvalue(wchar_t *a,bool takeOwnership=0);
	INIvalue(const char *a);
	INIvalue(const wchar_t *a);
	~INIvalue();
	void setIntValue(long a);
	void setStrValue(wchar_t *a,bool takeOwnership=0);
	void setStrValue(const char *a);
	void setStrValue(const wchar_t *a);
	char getType();
	long getIntValue();
	char *getStrValue();
	wchar_t *getWStrValue(bool getCopy=1);
};

class INIsection{
	std::map<wchar_t *,INIvalue *,wstrCmp> variables;
public:
	INIsection();
	INIsection(const wchar_t *buffer,long *offset,long l);
	~INIsection();
	void readFile(const wchar_t *buffer,long *offset,long l);
	void setIntValue(const char    *index,long     a);
	void setIntValue(const wchar_t *index,long     a);
	void setStrValue(const char    *index,wchar_t *a,bool takeOwnership=0);
	void setStrValue(const wchar_t *index,wchar_t *a,bool takeOwnership=0);
	void setStrValue(const char    *index,const char    *a);
	void setStrValue(const wchar_t *index,const char    *a);
	void setStrValue(const char    *index,const wchar_t *a);
	void setStrValue(const wchar_t *index,const wchar_t *a);
	char getType(const char *index);
	char getType(const wchar_t *index);
	long getIntValue(const char *index);
	long getIntValue(const wchar_t *index);
	char *getStrValue(const char *index);
	char *getStrValue(const wchar_t *index);
	wchar_t *getWStrValue(const char *index,bool getCopy=1);
	wchar_t *getWStrValue(const wchar_t *index,bool getCopy=1);
	INIvalue *getValue(const char *index);
	INIvalue *getValue(const wchar_t *index);
};

class INIfile{
	std::map<wchar_t *,INIsection *,wstrCmp> sections;
public:
	INIfile();
	INIfile(const char *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	INIfile(const wchar_t *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	INIfile(const char *buffer,long l,ENCODINGS encoding=ISO_8859_1_ENCODING);
	INIfile(const wchar_t *buffer,long l);
	~INIfile();
	ErrorCode readFile(const char *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	ErrorCode readFile(const wchar_t *filename,ENCODINGS encoding=ISO_8859_1_ENCODING);
	void readFile(const char *buffer,long l,ENCODINGS encoding=ISO_8859_1_ENCODING);
	void readFile(const wchar_t *buffer,long l);
	INIsection *getSection(const char *index);
	INIsection *getSection(const wchar_t *index);
};
#endif
