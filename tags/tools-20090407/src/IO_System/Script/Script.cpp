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

#ifndef NONS_SCRIPT_CPP
#define NONS_SCRIPT_CPP

#include "Script.h"
#include "../FileIO.h"
#include "../../UTF.h"
#include "../../Globals.h"
#include "../SaveFile.h"
#include "sha1.h"
#include <cstring>

NONS_Script::NONS_Script(){
	this->script=0;
	this->length=0;
	memset(this->hash,0,sizeof(unsigned)*5);
}

ErrorCode NONS_Script::init(const char *scriptname,NONS_GeneralArchive *archive,ulong encoding,ulong encryption){
	long l;
	char *temp=(char *)archive->getFileBuffer(scriptname,(ulong *)&l);
	if (!temp){
		this->script=0;
		this->length=0;
		return NONS_FILE_NOT_FOUND;
	}
	{
		int error_code=inPlaceDecryption(temp,l,encryption);
		if (error_code!=NONS_NO_ERROR)
			return error_code;
	}
	wchar_t *buffer=0;
	switch (encoding){
		case DETECT_ENCODING:
			if (isValidUTF8(temp,l)){
				v_stdlog <<"The script seems to be a valid UTF-8 stream. Using it as such."<<std::endl;
				buffer=UTF8_to_WChar(temp,l,&l);
			}else if (isValidSJIS(temp,l)){
				v_stdlog <<"The script seems to be a valid Shift JIS stream. Using it as such."<<std::endl;
				buffer=SJIS_to_WChar(temp,l,&l);
			}else if (!ISO88591_or_UCS2(temp,l)){
				v_stdlog <<"The script seems to be a valid ISO-8859-1 stream. Using it as such."<<std::endl;
				buffer=ISO88591_to_WChar(temp,l,&l);
			}else{
				v_stdlog <<"The script seems to be a valid UCS-2 stream. Using it as such."<<std::endl;
				buffer=UCS2_to_WChar(temp,l,&l);
			}
			break;
		case ISO_8859_1_ENCODING:
			buffer=ISO88591_to_WChar(temp,l,&l);
			break;
		case UCS2_ENCODING:
			if (l%2)
				v_stderr <<"WARNING: input text has odd length. It may not be valid UCS-2 text."<<std::endl;
			buffer=UCS2_to_WChar(temp,l,&l);
			break;
		case UTF8_ENCODING:
			buffer=UTF8_to_WChar(temp,l,&l);
			break;
		case SJIS_ENCODING:
			buffer=SJIS_to_WChar(temp,l,&l);
			break;
		default:
			break;
	}
	delete[] temp;
	this->script=buffer;
	this->length=l;
	//NOTE: In order to avoid a segmentation fault during parsing, it is
	//important to make sure that the script ends with a newline. The newline
	//can of course be of any format (CR, LF, or CRLF).
	if (this->script[this->length-1]!=13 && this->script[this->length-1]!=10){
		wchar_t *temp=new wchar_t[this->length+1];
		memcpy(temp,this->script,this->length*sizeof(wchar_t));
		temp[this->length++]=10;
		delete[] this->script;
		this->script=temp;
		buffer=temp;
	}
	for (long a=0;a<l;){
		if (buffer[a]=='*'){
			NONS_ScriptBlock *p=new NONS_ScriptBlock(buffer,a);
			this->blocks.push_back(p);
		}
		else if (buffer[a]=='~')
			this->jumps.push_back(a);
		for (;a<l && buffer[a]!=13 && buffer[a]!=10;a++);
		for (;a<l && (buffer[a]==13 || buffer[a]==10);a++);
	}
	if (!this->offsetFromBlock(L"*define"))
		return NONS_NO_DEFINE_LABEL;
	SHA1 hash;
	for (ulong a=0;a<this->blocks.size();a++){
		wchar_t *b=this->blocks[a]->name;
		ulong l=wcslen(b);
		Uint16 *temp=new Uint16[l];
		for (Uint16 *c=temp;*b;b++,c++)
			*c=*b;
		hash.Input((char *)temp,l*2);
		delete[] temp;
	}
	//hash.Input((char *)buffer,l*sizeof(wchar_t));
	hash.Result(this->hash);
	save_directory=getSaveLocation(this->hash);
	return NONS_NO_ERROR;
}

NONS_Script::~NONS_Script(){
	if (this->script)
		delete[] this->script;
	for (ulong a=0;a<this->blocks.size();a++)
		delete this->blocks[a];
}

long NONS_Script::offsetFromBlock(const wchar_t *name){
	for (ulong a=0;a<this->blocks.size();a++){
		if (!wcscmp(name,this->blocks[a]->name)){
			this->blocks[a]->used=1;
			return this->blocks[a]->offset;
		}
	}
	return -1;
}

wchar_t *NONS_Script::blockFromOffset(long offset){
	for (long a=this->blocks.size()-1;a>=0;a--){
		NONS_ScriptBlock *b=this->blocks[a];
		if (b->offset<offset)
			return b->name;
	}
	return 0;
}
#endif
