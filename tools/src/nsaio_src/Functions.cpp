/*
* Copyright (c) 2008, Helios (helios.vmg@gmail.com)
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

#ifndef NONS_FUNCTIONS_CPP
#define NONS_FUNCTIONS_CPP

#include "Functions.h"
#include <bzlib.h>
#include "UTF.h"
#include <cmath>
#include <cstring>

template<typename dst,typename src>
dst *copyString_template(const src *str,ulong len){
	if (!str)
		return 0;
	if (!len)
		for (;str[len];len++);
	dst *res=new dst[len+1];
	res[len]=0;
	for (ulong a=0;a<len;a++)
		res[a]=str[a];
	return res;
}

wchar_t *copyWString(const wchar_t *str,ulong len){
	return copyString_template<wchar_t,wchar_t>(str,len);
}

wchar_t *copyWString(const char *str,ulong len){
	return copyString_template<wchar_t,char>(str,len);
}

char *copyString(const wchar_t *str,ulong len){
	return copyString_template<char,wchar_t>(str,len);
}

char *copyString(const char *str,ulong len){
	return copyString_template<char,char>(str,len);
}

char *addStrings(const char *str1,const char *str2){
	ulong len1=strlen(str1),len2=strlen(str2);
	ulong len3=len1+len2;
	char *res=new char[len3+1];
	sprintf(res,"%s%s",str1,str2);
	return res;
}

wchar_t *addStrings(const wchar_t *str1,const wchar_t *str2){
	ulong len1=wcslen(str1),len2=wcslen(str2);
	ulong len3=len1+len2;
	wchar_t *res=new wchar_t[len3+1];
	wcscpy(res,str1);
	wcscat(res,str2);
	return res;
}

template <typename dst,typename src>
void addStringsInplace_template(dst **str1,const src *str2){
	ulong len1=0,len2=0;
	for (;(*str1)[len1];len1++);
	for (;str2[len2];len2++);
	ulong len3=len1+len2;
	dst *res=new dst[len3+1];
	res[len3]=0;
	for (ulong a=0;a<len1;a++)
		res[a]=(*str1)[a];
	for (ulong a=0;a<len2;a++)
		res[a+len1]=str2[a];
	delete[] *str1;
	*str1=res;
}

void addStringsInplace(wchar_t **str1,const wchar_t *str2){
	addStringsInplace_template<wchar_t,wchar_t>(str1,str2);
}

void addStringsInplace(wchar_t **str1,const char *str2){
	addStringsInplace_template<wchar_t,char>(str1,str2);
}

void addStringsInplace(char **str1,const wchar_t *str2){
	addStringsInplace_template<char,wchar_t>(str1,str2);
}

void addStringsInplace(char **str1,const char *str2){
	addStringsInplace_template<char,char>(str1,str2);
}

std::vector<char *> *getParameterList(const char *string,char delim){
	std::vector<char *> *res=new std::vector<char *>;
	ulong len=strlen(string);
	char tempDelim=delim;
	for (ulong start=0;start<len;){
		ulong end;
		if (string[start]=='\"')
			delim='\"';
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
}

bool getbit(uchar *arr,ulong *byteoffset,uchar *bitoffset){
	bool res=(arr[*byteoffset]>>(7-*bitoffset))&1;
	(*bitoffset)++;
	if (*bitoffset>7){
		(*byteoffset)++;
		*bitoffset=0;
	}
	return res;
}

ulong getbits(uchar *arr,uchar bits,ulong *byteoffset,uchar *bitoffset){
	ulong res=0;
	if (bits>sizeof(ulong)*8)
		bits=sizeof(ulong)*8;
	for (;bits>0;bits--){
		res<<=1;
		res|=(ulong)getbit(arr,byteoffset,bitoffset);
	}
	return res;
}

template<typename T,typename T2>
long instr_template(const T *str0,const T2 *str1){
	for (long res=0;*str0;res++,str0++){
		const T *str2=str0;
		const T2 *str3=str1;
		for (;*str3;str2++,str3++){
			if (!*str2)
				return -1;
			if (*str2!=*str3)
				goto instr_000;
		}
		return res;
instr_000:;
	}
	return -1;
}

long instr(const wchar_t *str0,const wchar_t *str1){
	return instr_template<wchar_t,wchar_t>(str0,str1);
}

long instr(const wchar_t *str0,const char *str1){
	return instr_template<wchar_t,char>(str0,str1);
}

long instr(const char *str0,const wchar_t *str1){
	return instr_template<char,wchar_t>(str0,str1);
}

long instr(const char *str0,const char *str1){
	return instr_template<char,char>(str0,str1);
}

template<typename T>
long instrB_template(const T *str0,const T *str1){
	long len0,len1;
	for (len0=0;str0[len0];len0++);
	for (len1=0;str1[len1];len1++);
	str0+=len0-1;
	for (long res=len0-1;res>=0;res--,str0--){
		const T *str2=str0;
		const T *str3=str1;
		for (;*str3;str2++,str3++){
			if (!*str2)
				return -1;
			if (*str2!=*str3)
				goto instrB_000;
		}
		return res;
instrB_000:;
	}
	return -1;
}

long instrB(const wchar_t *str0,const wchar_t *str1){
	return instrB_template<wchar_t>(str0,str1);
}

long instrB(const char *str0,const char *str1){
	return instrB_template<char>(str0,str1);
}

template<typename T>
void tolower_template(T *param){
    for (;*param;param++)
		if (*param>='A' && *param<='Z')
			*param=*param|32;
}

void tolower(wchar_t *param){
	return tolower_template<wchar_t>(param);
}

void tolower(char *param){
	return tolower_template<char>(param);
}

template <typename T,typename T2>
bool multicomparison_template(T character,const T2 *characters){
	for (;*characters;characters++)
		if (character==*characters)
			return 1;
	return 0;
}

bool multicomparison(char character,const char *characters){
	return multicomparison_template<char,char>(character,characters);
}

bool multicomparison(wchar_t character,const char *characters){
	return multicomparison_template<wchar_t,char>(character,characters);
}

bool multicomparison(char character,const wchar_t *characters){
	return multicomparison_template<char,wchar_t>(character,characters);
}

bool multicomparison(wchar_t character,const wchar_t *characters){
	return multicomparison_template<wchar_t,wchar_t>(character,characters);
}

template<typename T>
void toforwardslash_template(T *param){
	for (;*param;param++)
		if (*param=='\\')
			*param='/';
}

void toforwardslash(wchar_t *param){
	toforwardslash_template(param);
}

void toforwardslash(char *param){
	toforwardslash_template(param);
}

template<typename T>
void tobackslash_template(T *param){
	for (;*param;param++)
		if (*param=='/')
			*param='\\';
}

void tobackslash(wchar_t *param){
	tobackslash_template<wchar_t>(param);
}

void tobackslash(char *param){
	tobackslash_template<char>(param);
}

bool isanumber(char *a){
	for (;*a;a++)
		if (!isdigit(*a))
			return 0;
	return 1;
}

bool isanumber(wchar_t *a){
	for (;*a;a++)
		if (!isdigit(*a))
			return 0;
	return 1;
}

char *compressBuffer_BZ2(char *src,unsigned long srcl,unsigned long *dstl){
	unsigned long l=srcl,realsize=l;
	char *dst=new char[l];
	while (BZ2_bzBuffToBuffCompress(dst,(unsigned int *)&l,src,srcl,1,0,0)==BZ_OUTBUFF_FULL){
		delete[] dst;
		l*=2;
		realsize=l;
		dst=new char[l];
	}
	if (l!=realsize){
		char *temp=new char[l];
		memcpy(temp,dst,l);
		delete[] dst;
		dst=temp;
	}
	*dstl=l;
	return dst;
}

char *decompressBuffer_BZ2(char *src,unsigned long srcl,unsigned long *dstl){
	unsigned long l=srcl,realsize=l;
	char *dst=new char[l];
	while (BZ2_bzBuffToBuffDecompress(dst,(unsigned int *)&l,src,srcl,1,0)==BZ_OUTBUFF_FULL){
		delete[] dst;
		l*=2;
		realsize=l;
		dst=new char[l];
	}
	if (l!=realsize){
		char *temp=new char[l];
		memcpy(temp,dst,l);
		delete[] dst;
		dst=temp;
	}
	*dstl=l;
	return dst;
}

template <typename T>
T *tagName_template(const T *string){
	if (!string || *string!='<')
		return 0;
	for (string++;*string && iswhitespace(*string);string++);
	if (multicomparison(*string,"\0>="))
		return 0;
	ulong l=0;
	for (;string[l] && !iswhitespace(string[l]) && string[l]!='=';l++);
	if (!string[l])
		return 0;
	return copyString_template<T>(string,l);
}

wchar_t *tagName(const wchar_t *string){
	return tagName_template<wchar_t>(string);
}

char *tagName(const char *string){
	return tagName_template<char>(string);
}

template <typename T>
T *tagValue_template(const T *string){
	if (!string || *string!='<')
		return 0;
	for (string++;;string++){
		if (!*string || *string=='>')
			return 0;
		if (*string=='=')
			break;
	}
	for (string++;*string && iswhitespace(*string);string++)
	if (!*string || *string=='>')
		return 0;
	ulong l=0;
	for (;string[l] && !iswhitespace(string[l]) && string[l]!='>';l++);
	if (!string[l])
		return 0;
	return copyString_template<T>(string,l);
}

wchar_t *tagValue(const wchar_t *string){
	return tagValue_template<wchar_t>(string);
}

char *tagValue(const char *string){
	return tagValue_template<char>(string);
}

void writeByte(integer8 a,std::string *str,long offset){
	if (offset<0)
		str->push_back(a&0xFF);
	else
		(*str)[offset]=a&0xFF;
}

void writeWord(integer16 a,std::string *str,long offset){
	if (offset<0)
		offset=str->size();
	for (char b=0;b<2;b++,offset++){
		if (str->size()>offset)
			(*str)[offset]=a&0xFF;
		else
			str->push_back(a&0xFF);
		a>>=8;
	}
}

void writeDWord(integer32 a,std::string *str,long offset){
	if (offset<0)
		offset=str->size();
	for (char b=0;b<4;b++,offset++){
		if (str->size()>offset)
			(*str)[offset]=a&0xFF;
		else
			str->push_back(a&0xFF);
		a>>=8;
	}
}

void writeString(wchar_t *a,std::string *str){
	char *utf8=WChar_to_UTF8(a);
	str->append(utf8);
	str->push_back(0);
	delete[] utf8;
}

void writeWordBig(integer16 a,std::string *str,long offset){
	if (offset<0)
		offset=str->size();
	for (char b=0;b<2;b++,offset++){
		if (str->size()>offset)
			(*str)[offset]=a>>8;
		else
			str->push_back(a>>8);
		a<<=8;
	}
}

void writeDWordBig(integer32 a,std::string *str,long offset){
	if (offset<0)
		offset=str->size();
	for (char b=0;b<4;b++,offset++){
		if (str->size()>offset)
			(*str)[offset]=a>>24;
		else
			str->push_back(a>>24);
		a<<=8;
	}
}

bool is_directory(const char *path){
	return boost::filesystem::is_directory(boost::filesystem::path(path));
}

bool is_directory(const wchar_t *path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::is_directory(boost::filesystem::wpath(path));
#else
	char *temp=copyString(path);
	bool ret=boost::filesystem::is_directory(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

bool is_directory(const std::string &path){
	return boost::filesystem::is_directory(boost::filesystem::path(path.c_str()));
}

bool is_directory(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::is_directory(boost::filesystem::wpath(path.c_str()));
#else
	char *temp=copyString(path.c_str());
	bool ret=boost::filesystem::is_directory(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

bool create_directory(const char *path){
	return boost::filesystem::create_directory(boost::filesystem::path(path));
}

bool create_directory(const wchar_t *path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::create_directory(boost::filesystem::wpath(path));
#else
	char *temp=copyString(path);
	bool ret=boost::filesystem::create_directory(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

bool create_directory(const std::string &path){
	return boost::filesystem::create_directory(boost::filesystem::path(path.c_str()));
}

bool create_directory(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::create_directory(boost::filesystem::wpath(path.c_str()));
#else
	char *temp=copyString(path.c_str());
	bool ret=boost::filesystem::create_directory(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

bool file_exists(const char *path){
	return boost::filesystem::exists(boost::filesystem::path(path));
}

bool file_exists(const wchar_t *path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::exists(boost::filesystem::wpath(path));
#else
	char *temp=copyString(path);
	bool ret=boost::filesystem::exists(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

bool file_exists(const std::string &path){
	return boost::filesystem::exists(boost::filesystem::path(path.c_str()));
}

bool file_exists(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::exists(boost::filesystem::wpath(path.c_str()));
#else
	char *temp=copyString(path.c_str());
	bool ret=boost::filesystem::exists(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

boost::uintmax_t file_size(const char *path){
	return boost::filesystem::file_size(boost::filesystem::path(path));
}

boost::uintmax_t file_size(const wchar_t *path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::file_size(boost::filesystem::wpath(path));
#else
	char *temp=copyString(path);
	bool ret=boost::filesystem::file_size(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}

boost::uintmax_t file_size(const std::string &path){
	return boost::filesystem::file_size(boost::filesystem::path(path.c_str()));
}

boost::uintmax_t file_size(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::file_size(boost::filesystem::wpath(path.c_str()));
#else
	char *temp=copyString(path.c_str());
	bool ret=boost::filesystem::file_size(boost::filesystem::path(temp));
	delete[] temp;
	return ret;
#endif
}
#endif
