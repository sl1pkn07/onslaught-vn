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

#ifndef NONS_UTF_CPP
#define NONS_UTF_CPP

#include "UTF.h"
#ifndef BARE_FILE
#include "Functions.h"
#include "Globals.h"
#else
#include "SJIS.table.cpp"
#endif

extern wchar_t SJIS2Unicode[];
extern wchar_t Unicode2SJIS[];

char checkEnd(wchar_t a){
	if (a==BOM16B)
		return NONS_BIG_ENDIAN;
	else if (a==BOM16L)
		return NONS_LITTLE_ENDIAN;
	else
		return UNDEFINED_ENDIANNESS;
}

bool checkNativeEndianness(){
	ushort a=0x1234;
	bool res;
	if (*((uchar *)/*(void *)*/&a)==0x12)
		res=NONS_BIG_ENDIAN;
	else
		res=NONS_LITTLE_ENDIAN;
	return res;
}

wchar_t *UCS2_to_WChar(const char *buffer,long initialSize,long *finalSize,uchar end){
	wchar_t firstChar=buffer[0]<<8|(wchar_t(buffer[1])&0xFF);
	char realEnd=checkEnd(firstChar);
	bool usesBOM=(realEnd!=UNDEFINED_ENDIANNESS);
	initialSize/=2;
	if (usesBOM)
		initialSize--;
	else
		realEnd=NONS_BIG_ENDIAN;
	if (end==UNDEFINED_ENDIANNESS)
		end=realEnd;
	wchar_t *res=new wchar_t[initialSize];
	memcpy(res,buffer+2*usesBOM,initialSize);
	if ((uchar)checkNativeEndianness()!=end)
		for (long a=0;a<initialSize;a++)
			res[a]=(res[a]>>8)+(res[a]<<8);
	*finalSize=initialSize;
	return res;
}

wchar_t *ISO88591_to_WChar(const char *buffer,long initialSize,long *finalSize){
	wchar_t *res=new wchar_t[initialSize];
	*finalSize=initialSize;
	for (long a=0;a<initialSize;a++)
		res[a]=((wchar_t)buffer[a])&0xFF;
	return res;
}

wchar_t *UTF8_to_WChar(const char *buffer,long initialSize,long *finalSize){
	long c=0;
	long b=0,init=(uchar)buffer[0]==BOM8A && (uchar)buffer[1]==BOM8B && (uchar)buffer[2]==BOM8C?3:0;
	/*
	Predict size of resulting buffer. This is a ridiculously simple operation,
	as all it requires is to find the starting bytes of the characters.
	*/
	for (long a=init;a<initialSize;a++)
		if (uchar(buffer[a])<128 || (buffer[a]&192)==192)
			c++;
	wchar_t *res;
	res=new wchar_t[c];
	uchar *unsigned_buffer=(uchar *)buffer;
	for (long a=init;a<initialSize;a++){
		if (!(unsigned_buffer[a]&128))
			//Byte represents an ASCII character. Direct copy will do.
			res[b]=buffer[a];
		else if ((unsigned_buffer[a]&192)==128)
			//Byte is the middle of an encoded character. Ignore.
			continue;
		else if ((unsigned_buffer[a]&224)==192)
			//Byte represents the start of an encoded character in the range
			//U+0080 to U+07FF
			res[b]=(wchar_t(unsigned_buffer[a]&31)<<6)|wchar_t(unsigned_buffer[a+1]&63);
		else if ((unsigned_buffer[a]&240)==224)
			//Byte represents the start of an encoded character in the range
			//U+07FF to U+FFFF
			res[b]=(wchar_t(unsigned_buffer[a]&15)<<12)|(wchar_t(unsigned_buffer[a+1]&63)<<6)|wchar_t(unsigned_buffer[a+2]&63);
		else if ((buffer[a]&248)==240){
			//Byte represents the start of an encoded character beyond the
			//U+FFFF limit of 16-bit integer
			res[b]='?';
		}
		b++;
	}
	*finalSize=c;
	return res;
}

wchar_t *UTF8_to_WChar(const char *string){
	long b=0,
		c=0;
	if ((uchar)string[0]==BOM8A && (uchar)string[1]==BOM8B && (uchar)string[2]==BOM8C)
		string+=3;
	for (const char *a=string;*a;a++)
		if (((uchar)*a)<128 || (*a&192)==192)
			c++;
	wchar_t *res=new wchar_t[c+1];
	res[c]=0;
	for (uchar *a=(uchar*)string;*a;a++){
		if (!(*a&128))
			//Byte represents an ASCII character. Direct copy will do.
			res[b]=*a;
		else if ((*a&192)==128)
			//Byte is the middle of an encoded character. Ignore.
			continue;
		else if ((*a&224)==192)
			//Byte represents the start of an encoded character in the range
			//U+0080 to U+07FF
			res[b]=((*a&31)<<6)|a[1]&63;
		else if ((*a&240)==224)
			//Byte represents the start of an encoded character in the range
			//U+07FF to U+FFFF
			res[b]=((*a&15)<<12)|((a[1]&63)<<6)|a[2]&63;
		else if ((*a&248)==240){
			//Byte represents the start of an encoded character beyond the
			//U+FFFF limit of 16-bit integers
			res[b]='?';
		}
		b++;
	}
	return res;
}

wchar_t *SJIS_to_WChar(const char *buffer,long initialSize,long *finalSize){
	wchar_t *temp=new wchar_t[initialSize];
	ulong a=0,b=0;
	uchar *unsigned_buffer=(uchar *)buffer;
	for (;b<initialSize;a++,b++){
		if (isSJISWide(buffer[b])){
			temp[a]=(wchar_t(unsigned_buffer[b])<<8)|wchar_t(unsigned_buffer[b+1]);
			b++;
		}else
			temp[a]=wchar_t(unsigned_buffer[b]);
	}
	*finalSize=a;
	wchar_t *res=new wchar_t[a];
	for (b=0;b<a;b++){
		res[b]=SJIS2Unicode[temp[b]];
		if (res[b]=='?' && temp[b]!='?'){
#ifndef BARE_FILE
			v_stderr.getstream().width(4);
			v_stderr <<"ENCODING ERROR: Character SJIS+"<<std::hex<<temp[b]<<" is unsupported. Replacing with '?'."<<std::endl;
#else
			fprintf(stderr,"ENCODING ERROR: Character SJIS+%04x is unsupported. Replacing with '?'.\n",temp[b]);
#endif
		}
	}
	delete[] temp;
	return res;
}

wchar_t *SJIS_to_WChar(const char *string){
	ulong initialSize=strlen(string);
	wchar_t *temp=new wchar_t[initialSize];
	ulong a=0,b=0;
	uchar *unsigned_buffer=(uchar *)string;
	for (;b<initialSize;a++,b++){
		if (isSJISWide(string[b])){
			temp[a]=(wchar_t(unsigned_buffer[b])<<8)|wchar_t(string[b+1]);
			b++;
		}else
			temp[a]=wchar_t(unsigned_buffer[b]);
	}
	wchar_t *res=new wchar_t[a+1];
	res[a]=0;
	for (b=0;b<a;b++){
		res[b]=SJIS2Unicode[temp[b]];
		if (res[b]=='?' && temp[b]!='?'){
#ifndef BARE_FILE
			v_stderr.getstream().width(4);
			v_stderr <<"ENCODING ERROR: Character SJIS+"<<std::hex<<temp[b]<<" is unsupported. Replacing with '?'."<<std::endl;
#else
			fprintf(stderr,"ENCODING ERROR: Character SJIS+%04x is unsupported. Replacing with '?'.\n",temp[b]);
#endif
		}
	}
	delete[] temp;
	return res;
}

bool isSJISWide(uchar a){
	return a>=0x81 && a<=0x9F || a>=0xE0 && a<=0xEF;
}

char *WChar_to_UCS2(const wchar_t *buffer,long initialSize,long *finalSize,uchar end){
	bool useBOM=(end<UNDEFINED_ENDIANNESS);
	if (!useBOM)
		end=NONS_BIG_ENDIAN;
	char *res=new char[(initialSize+useBOM)*2];
	if (checkNativeEndianness()==NONS_BIG_ENDIAN){
		res[0]=BOM16BA;
		res[1]=BOM16BB;
	}else{
		res[1]=BOM16BA;
		res[0]=BOM16BB;
	}
	initialSize*=2;
	memcpy(res+2*(useBOM),buffer,initialSize);
	initialSize+=2*useBOM;
	if ((uchar)checkNativeEndianness()!=end){
		for (long a=0;a<initialSize;a+=2){
			char temp=res[a];
			res[a]=res[a+1];
			res[a+1]=temp;
		}
	}
	*finalSize=initialSize;
	return res;
}

char *WChar_to_ISO88591(const wchar_t *buffer,long initialSize,long *finalSize){
	*finalSize=initialSize;
	char *res=new char[initialSize];
	for (ulong a=0;a<initialSize;a++){
		if (buffer[a]>0xFF){
#ifndef BARE_FILE
			v_stderr.getstream().width(4);
			v_stderr <<"ENCODING ERROR: Character U+"<<std::hex<<buffer[a]<<" cannot be properly encoded in ISO-8859-1. Replacing with '?'."<<std::endl;
#else
			fprintf(stderr,"ENCODING ERROR: Character U+%04x cannot be properly encoded in ISO-8859-1. Replacing with '?'.\n",buffer[a]);
#endif
			res[a]='?';
		}else
			res[a]=(char)buffer[a];
	}
	return res;
}

long getUTF8size(const wchar_t *buffer,long size){
	long res=0;
	for (long a=0;a<size;a++){
		if (buffer[a]<0x80)
			res++;
		else if (buffer[a]<0x800)
			res+=2;
		else
			res+=3;
	}
	res+=3;
	return res;
}

long getUTF8size(const wchar_t *string){
	if (!string)
		return 0;
	long res=0;
	for (;*string;string++){
		if (*string<0x80)
			res++;
		else if (*string<0x800)
			res+=2;
		else
			res+=3;
	}
	return res;
}

char *WChar_to_UTF8(const wchar_t *buffer,long initialSize,long *finalSize){
	long fSize=getUTF8size(buffer,initialSize)+3;
	char *res=new char[fSize];
	long b=0;
	res[b++]=BOM8A;
	res[b++]=BOM8B;
	res[b++]=BOM8C;
	for (long a=0;a<initialSize;a++,b++){
		wchar_t character=buffer[a];
		if (character<0x80)
			res[b]=(char)character;
		else if (character<0x800){
			res[b++]=(character>>6)|192;
			res[b]=character&63|128;
		}else{
			res[b++]=(character>>12)|224;
			res[b++]=((character&4095)>>6)|128;
			res[b]=character&63|128;
		}
	}
	*finalSize=fSize;
	return res;
}

char *WChar_to_UTF8(const wchar_t *string){
	long fSize=getUTF8size(string);
	char *res=new char[fSize+1];
	res[fSize]=0;
	if (!string)
		return res;
	long b=0;
	for (;*string;string++,b++){
		if (*string<0x80)
			res[b]=(char)*string;
		else if (*string<0x800){
			res[b++]=(*string>>6)|192;
			res[b]=*string&63|128;
		}else{
			res[b++]=(*string>>12)|224;
			res[b++]=((*string&4095)>>6)|128;
			res[b]=*string&63|128;
		}
	}
	return res;
}

char *WChar_to_SJIS(const wchar_t *buffer,long initialSize,long *finalSize){
	wchar_t *temp=new wchar_t[initialSize];
	memcpy(temp,buffer,initialSize*sizeof(wchar_t));
	for (long a=0;a<initialSize;a++){
		wchar_t character=Unicode2SJIS[temp[a]];
		if (character=='?' && temp[a]!='?'){
#ifndef BARE_FILE
			v_stderr.getstream().width(4);
			v_stderr <<"ENCODING ERROR: Character U+"<<std::hex<<temp[a]<<" is unsupported. Replacing with '?'."<<std::endl;
#else
			fprintf(stderr,"ENCODING ERROR: Character U+%04x is unsupported. Replacing with '?'.\n",temp[a]);
#endif
		}
		temp[a]=character;
	}
	long fSize=0;
	for (long a=0;a<initialSize;a++)
		if (temp[a]<0x80)
			fSize++;
		else
			fSize+=2;
	*finalSize=fSize;
	char *res=new char[fSize];
	fSize=0;
	for (long a=0;a<initialSize;a++,fSize++){
		if (temp[a]<0x80)
			res[fSize]=(char)temp[a];
		else{
			res[fSize++]=char(temp[a]>>8);
			res[fSize]=char(temp[a]&0xFF);
		}
	}
	return res;
}

#ifdef BARE_FILE
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

char writefile(const char *name,char *buffer,long size){
	std::ofstream file(name,std::ios::binary);
	if (!file){
		return 1;
	}
	file.write(buffer,size);
	file.close();
	return 0;
}
#endif

bool iswhitespace(wchar_t character){
	switch (character){
		case 0x0009:
		case 0x000A:
		case 0x000B:
		case 0x000C:
		case 0x000D:
		case 0x0020:
		case 0x0085:
		case 0x00A0:
		case 0x1680:
		case 0x180E:
		case 0x2000:
		case 0x2001:
		case 0x2002:
		case 0x2003:
		case 0x2004:
		case 0x2005:
		case 0x2006:
		case 0x2007:
		case 0x2008:
		case 0x2009:
		case 0x200A:
		case 0x2028:
		case 0x2029:
		case 0x202F:
		case 0x205F:
		case 0x3000:
			return 1;
	}
	return 0;
}

bool iswhitespace(char character){
	return (character>0)?iswhitespaceASCIIe(character):0;
}

bool iswhitespaceASCIIe(char character){
	switch (character){
		case 0x0009:
		case 0x000A:
		case 0x000B:
		case 0x000C:
		case 0x000D:
		case 0x0020:
		case 0x0085:
		case 0x00A0:
			return 1;
	}
	return 0;
}

bool isbreakspace(wchar_t character){
	switch (character){
		case 0x0020:
		case 0x1680:
		case 0x180E:
		case 0x2002:
		case 0x2003:
		case 0x2004:
		case 0x2005:
		case 0x2006:
		case 0x2008:
		case 0x2009:
		case 0x200A:
		case 0x200B:
		case 0x200C:
		case 0x200D:
		case 0x205F:
		case 0x3000:
			return 1;
	}
	return 0;
}

bool isbreakspace(char character){
	return (character>0)?iswhitespaceASCIIe(character):0;
}

bool isbreakspaceASCIIe(char character){
	return character==0x20;
}
#endif
