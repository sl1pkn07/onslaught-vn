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

#ifndef CONFIGFILE_CPP
#define CONFIGFILE_CPP

#include "ConfigFile.h"
#include "../Functions.h"

template <typename T>
T DEC2HEX(T x){
	return x<10?'0'+x:'A'+x-10;
}

template <typename T>
T HEX2DEC(T x){
	return x<='9'?x-'0':(x<='F'?x-'A'+10:x-'a'+10);
}

void getMembers(wchar_t *src,wchar_t **var,wchar_t **val){
	wchar_t *temp[]={0,0};
	long equals=-1;
	for (wchar_t z=0;z<2;z++){
		long start=-1,end=-1;
		for (long a=equals+1;src[a]!=13 && src[a]!=10;a++){
			if (iswhitespace(src[a]))
				continue;
			start=a;
			break;
		}
		if (start==-1)
			goto getMembers_fail;
		for (long a=start;src[a]!=13 && src[a]!=10;a++){
			if (iswhitespace(src[a]))
				continue;
			if (src[a]=='='){
				equals=a;
				break;
			}
			end=a;
		}
		if (end==-1)
			goto getMembers_fail;
		temp[z]=copyWString(src+start,end-start+1);
	}
	*var=temp[0];
	*val=temp[1];
	return;
getMembers_fail:
	if (temp[0])
		delete[] temp[0];
	if (temp[1])
		delete[] temp[1];
}

/*std::vector<char *> *getParameterList(char *string,char delim=' '){
	std::vector<char *> *res=new std::vector<char *>;
	ulong len=strlen(string);
	char tempDelim=delim;
	for (ulong start=0;start<len;){
		ulong end;
		if (string[start]=='\"')//{
			delim='\"';
			/*start++;
		}*//*
		for (end=start+1;string[end] && string[end]!=delim;end++);
		if (delim=='\"')
			end++;
		ulong pl=end-start;
		char *el=new char[pl+1];
		el[pl]=0;
		for (ulong a=start;a<end;a++)
			el[a-start]=string[a];
		res->push_back(el);
		delim=tempDelim;
		for (start=end+1;string[start]==delim;start++);
	}
	return res;
}*/

//0=str, 1=dec, 2=hex, 3=bin
char getDataType(wchar_t *string,long len=0){
	if (!len)
		len=wcslen(string);
	if (*string=='\"')
		return 0;
	if (*string=='0' && string[1]=='x')
		return 2;
	if (string[len-1]=='b')
		return 3;
	return 1;
}

ConfigFile::ConfigFile(){
}

ConfigFile::ConfigFile(char *filename,ENCODINGS encoding){
	this->init(filename,encoding);
}

ConfigFile::ConfigFile(wchar_t *filename,ENCODINGS encoding){
	char *temp=copyString(filename,encoding);
	this->init(temp,encoding);
	delete[] temp;
}

void ConfigFile::init(char *filename,ENCODINGS encoding){
	long l;
	char *buffer=(char *)readfile(filename,&l);
	if (!buffer)
		return;
	wchar_t *decoded;
	switch (encoding){
		case ISO_8859_1_ENCODING:
			decoded=ISO88591_to_WChar(buffer,l,&l);
			break;
		case UCS2_ENCODING:
			decoded=UCS2_to_WChar(buffer,l,&l);
			break;
		case UTF8_ENCODING:
			decoded=UTF8_to_WChar(buffer,l,&l);
			break;
		case SJIS_ENCODING:
			decoded=SJIS_to_WChar(buffer,l,&l);
	}
	delete[] buffer;
	if (decoded[l-1]!=13 && decoded[l-1]!=10){
		wchar_t *temp=new wchar_t[l+1];
		memcpy(temp,decoded,l*sizeof(wchar_t));
		temp[l]=10;
		delete[] decoded;
		decoded=temp;
	}
	for (long pos=0;pos<l;){
		wchar_t *var,*val;
		getMembers(decoded+pos,&var,&val);
		tolower(var);
		this->entries[var]=getParameterList(val);
		for (;pos<l && (decoded[pos]!=13 && decoded[pos]!=10);pos++);
		for (;pos<l && (decoded[pos]==13 || decoded[pos]==10);pos++);
		delete[] val;
	}
	delete[] decoded;
}

ConfigFile::~ConfigFile(){
	for (std::map<wchar_t *,std::vector<wchar_t *> *,wstrCmpCI>::iterator i=this->entries.begin();i!=this->entries.end();i++){
		delete[] i->first;
		for (std::vector<wchar_t *>::iterator i2=i->second->begin();i2!=i->second->end();i2++)
			delete[] *i2;
		delete i->second;
	}
}

wchar_t *ConfigFile::getWString(const wchar_t *index,long subindex){
	if (this->entries.find((wchar_t *)index)==this->entries.end() || getDataType((*this->entries[(wchar_t *)index])[subindex]))
		return 0;
	return copyWString((*this->entries[(wchar_t *)index])[subindex]+1,wcslen((*this->entries[(wchar_t *)index])[subindex]+1)-1);
}

template <typename T>
int atoi2(T *str){
	char *temp=copyString(str);
	int res=atoi(temp);
	delete[] temp;
	return res;
}

long ConfigFile::getInt(const wchar_t *index,long subindex){
	if (this->entries.find((wchar_t *)index)==this->entries.end())
		return 0;
	wchar_t *in=(*this->entries[(wchar_t *)index])[subindex];
	char type=getDataType(in);
	long out=0;
	switch (type){
		case 1:
			return atoi2(in);
		case 2:
			in+=2;
			for (;*in;in++){
				out<<=4;
				out+=HEX2DEC(*in);
			}
			return out;
		case 3:
			for (;*in!='b';in++){
				out<<=1;
				out+=*in-'0';
			}
			return out;
		default:
			return 0;
	}
}

void ConfigFile::assignWString(const wchar_t *var,const wchar_t *val,long subindex){
	ulong len=wcslen(val);
	wchar_t *temp=new wchar_t[len+3];
#ifndef __MINGW32__
	swprintf(temp,len+3,L"\"%s\"",val);
#else
	swprintf(temp,L"\"%s\"",val);
#endif
	if (this->exists(var))
		if (subindex<0 || subindex>=this->entries.size())
			this->entries[(wchar_t *)var]->push_back(temp);
		else
			(*this->entries[(wchar_t *)var])[subindex]=temp;
	else{
		std::vector<wchar_t *> *temp2=new std::vector<wchar_t *>;
		temp2->push_back(temp);
		this->entries[copyWString(var)]=temp2;
	}
}

void ConfigFile::assignInt(const wchar_t *var,long val,long subindex){
	wchar_t *temp=new wchar_t[50];
#ifndef __MINGW32__
	swprintf(temp,12,L"%d",val);
#else
	swprintf(temp,L"%d",val);
#endif
	if (this->exists(var))
		if (subindex<0 || subindex>=this->entries.size())
			this->entries[(wchar_t *)var]->push_back(temp);
		else
			(*this->entries[(wchar_t *)var])[subindex]=temp;
	else{
		std::vector<wchar_t *> *temp3=new std::vector<wchar_t *>;
		temp3->push_back(temp);
		this->entries[copyWString(var)]=temp3;
	}
}

void ConfigFile::writeOut(char *filename,ENCODINGS encoding){
	std::ofstream file(filename,std::ios::binary);
	std::string *temp=this->writeOut(encoding);
	file.write(temp->c_str(),temp->size());
	delete temp;
}

std::string *ConfigFile::writeOut(ENCODINGS encoding){
	std::wstring buffer;
	for(std::map<wchar_t *,std::vector<wchar_t *> *,wstrCmpCI>::iterator iter=this->entries.begin();iter!=this->entries.end();iter++){
		buffer.append(iter->first);
		buffer.push_back('=');
		for (long a=0;;){
			buffer.append((*(iter->second))[a++]);
			if (a<iter->second->size())
				buffer.push_back(' ');
			else{
				buffer.push_back(13);
				buffer.push_back(10);
				break;
			}
		}
	}
	long l;
	char *string;
	switch (encoding){
		case ISO_8859_1_ENCODING:
			string=WChar_to_ISO88591(buffer.c_str(),buffer.size(),&l);
			break;
		case UCS2_ENCODING:
			string=WChar_to_UCS2(buffer.c_str(),buffer.size(),&l);
			break;
		case UTF8_ENCODING:
			string=WChar_to_UTF8(buffer.c_str(),buffer.size(),&l);
			break;
		case SJIS_ENCODING:
			string=WChar_to_SJIS(buffer.c_str(),buffer.size(),&l);
	}
	std::string *ret=new std::string;
	ret->insert(ret->end(),string,string+l);
	delete[] string;
	return ret;
}

bool ConfigFile::exists(const wchar_t *var){
	return ((this->entries.find((wchar_t *)var))!=(this->entries.end()));
}
#endif
