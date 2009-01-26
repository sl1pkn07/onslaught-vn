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

#ifndef NONS_SAVEFILE_CPP
#define NONS_SAVEFILE_CPP

#include "SaveFile.h"
#include "FileIO.h"
#include "../Functions.h"
#include "../Globals.h"

#if defined(NONS_SYS_WINDOWS)
#include <windows.h>
#elif defined(NONS_SYS_LINUX)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

tm *getDate(char *filename){
	tm *res=new tm();
#if defined(NONS_SYS_WINDOWS)
	FILETIME time;
	SYSTEMTIME time2;
#ifdef UNICODE
	wchar_t *copy=copyWString(filename);
	HANDLE h=CreateFile(copy,FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	delete[] copy;
#else
	HANDLE h=CreateFile(filename,FILE_READ_DATA,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
#endif
	GetFileTime(h,0,0,&time);
	CloseHandle(h);
	FileTimeToSystemTime((const FILETIME *)&time,&time2);
	SystemTimeToTzSpecificLocalTime(0,&time2,&time2);
	res->tm_year=time2.wYear-1900;
	res->tm_mon=time2.wMonth-1;
	res->tm_mday=time2.wDay;
	res->tm_hour=time2.wHour;
	res->tm_min=time2.wMinute;
	res->tm_sec=time2.wSecond;
#elif defined(NONS_SYS_LINUX)
	struct stat buf;
	stat(filename,&buf);
	*res=*localtime(&(buf.st_mtime));
#else
	res->tm_year=2000;
	res->tm_mon=0;
	res->tm_mday=1;
	res->tm_hour=0;
	res->tm_min=0;
	res->tm_sec=0;
#endif
	return res;
}

std::vector<tm *> *existing_files(char *location){
	std::vector<tm *> *res=new std::vector<tm *>;
	res->reserve(20);
	char *path=copyString(location);
	toforwardslash(path);
	if (path[strlen(path)-1]!='/')
		addStringsInplace(&path,"/");
	char *filename=addStrings(path,"save01.dat");
	for (short a=1;a<21;a++){
		sprintf(filename,"%ssave%d.dat",path,a);
		if (!fileExists(filename))
			res->push_back(0);
		else
			res->push_back(getDate(filename));
	}
	delete[] filename;
	delete[] path;
	return res;
}

std::vector<tm *> *existing_files(wchar_t *location){
	char *copy=copyString(location);
	std::vector<tm *> *ret=existing_files(copy);
	delete[] copy;
	return ret;
}

#if defined(NONS_SYS_WINDOWS)
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#elif defined(NONS_SYS_LINUX)
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif

char *getConfigLocation(){
#if defined(NONS_SYS_WINDOWS)
	HKEY k;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),0,KEY_READ,&k)!=ERROR_SUCCESS)
		return copyString("./");
	DWORD type,size;
	if (RegQueryValueEx(k,TEXT("Personal"),0,&type,0,&size)!=ERROR_SUCCESS || type!=REG_SZ){
		RegCloseKey(k);
		return copyString("./");
	}
	wchar_t *path=new wchar_t[size/2];
	RegQueryValueEx(k,TEXT("Personal"),0,&type,(LPBYTE)path,&size);
	RegCloseKey(k);
	size/=2;
	toforwardslash(path);
	if (path[size-1]!='/')
		addStringsInplace(&path,"/.ONSlaught");
	else
		addStringsInplace(&path,".ONSlaught");
	if (!CreateDirectory((LPCTSTR)path,0) && GetLastError()!=ERROR_ALREADY_EXISTS){
		delete[] path;
		return copyString("./");
	}
	addStringsInplace(&path,"/");
	char *res=copyString(path);
	delete[] path;
	return res;
#elif defined(NONS_SYS_LINUX)
	passwd* pwd=getpwuid(getuid());
	if (!pwd)
		return copyString("./");
	char *res=copyString(pwd->pw_dir);
	if (res[strlen(res)-1]!='/')
		addStringsInplace(&res,"/.ONSlaught");
	if (mkdir(res,~0) && errno!=EEXIST){
		delete[] res;
		return copyString("./");
	}
	addStringsInplace(&res,"/");
	return res;
#else
	return copyString("./");
#endif
}

char *getSaveLocation(unsigned hash[5]){
	char *root=getConfigLocation();
#if defined(NONS_SYS_WINDOWS)
	wchar_t *path=copyWString(root);
#elif defined(NONS_SYS_LINUX)
	char *path=copyString(root);
#else
	return root;
#endif
	if (!CLOptions.savedir){
		char hashstring[100];
		//sprintf(hashstring,"%08X%08X%08X%08X%08X",hash[0],hash[1],hash[2],hash[3],hash[4]);
		sprintf(hashstring,"%08x %08x",hash[0],hash[1]);
		addStringsInplace(&path,hashstring);
	}else
		addStringsInplace(&path,CLOptions.savedir);
#if defined(NONS_SYS_WINDOWS)
	if (!CreateDirectory((LPCTSTR)path,0) && GetLastError()!=ERROR_ALREADY_EXISTS){
		delete[] path;
		return root;
	}
	char *res=copyString(path);
	delete[] path;
#elif defined(NONS_SYS_LINUX)
	if (mkdir(path,~0) && errno!=EEXIST){
		delete[] path;
		return root;
	}
	char *res=path;
#endif
	addStringsInplace(&res,"/");
	return res;
}

NONS_SaveFile::Sprite::Sprite(){
	this->string=0;
}

NONS_SaveFile::Sprite::~Sprite(){
	if (this->string)
		delete[] this->string;
}

NONS_SaveFile::stackEl::stackEl(){
	this->leftovers=0;
	this->label=0;
}

NONS_SaveFile::stackEl::~stackEl(){
	if (this->leftovers)
		delete[] this->leftovers;
	if (this->label)
		delete[] this->label;
}

NONS_SaveFile::Channel::Channel(){
	this->name=0;
}

NONS_SaveFile::Channel::~Channel(){
	if (this->name)
		delete[] this->name;
}

NONS_Variable *readArray(char *buffer,long *offset){
	NONS_Variable *var=new NONS_Variable();
	var->dimensionSize=readDWord(buffer,offset);
	if (var->dimensionSize){
		var->type='?';
		var->dimension=new NONS_Variable*[var->dimensionSize];
		for (ulong a=0;a<var->dimensionSize;a++)
			var->dimension[a]=readArray(buffer,offset);
	}else{
		var->type='%';
		var->intValue=readSignedDWord(buffer,offset);
		_READ_BINARY_UTF8_STRING(var->wcsValue,buffer,*offset)
	}
	return var;
}

void NONS_SaveFile::load(char *filename){
	long l;
	char *buffer=(char *)readfile(filename,&l);
	if (!buffer)
		return;
	long offset=0;
	if (!instr(buffer,"ONS")){
		this->format='O';
		offset=3;
		this->version=readByte(buffer,&offset)*100+readByte(buffer,&offset);
		if (this->version>202 || this->version<200)
			this->error=NONS_UNSUPPORTED_SAVEGAME_VERSION;
		else{
			this->error=NONS_NO_ERROR;
			//readDWord(buffer,&offset);
			offset+=4;
			this->boldFont=!!readDWord(buffer,&offset);
			this->fontShadow=!!readDWord(buffer,&offset);
			//readDWord(buffer,&offset);
			offset+=4;
			this->rmode=!!readDWord(buffer,&offset);
			this->windowTextColor.r=readDWord(buffer,&offset);
			this->windowTextColor.g=readDWord(buffer,&offset);
			this->windowTextColor.b=readDWord(buffer,&offset);
			_READ_BINARY_SJIS_STRING(this->arrowCursorString,buffer,offset)
			_READ_BINARY_SJIS_STRING(this->pageCursorString,buffer,offset)
			this->windowTransition=readDWord(buffer,&offset);
			this->windowTransitionDuration=readDWord(buffer,&offset);
			_READ_BINARY_SJIS_STRING(this->windowTransitionRule,buffer,offset)
			this->windowFrame.x=readSignedDWord(buffer,&offset);
			this->windowFrame.y=readSignedDWord(buffer,&offset);
			this->windowFrameColumns=readDWord(buffer,&offset);
			this->windowFrameRows=readDWord(buffer,&offset);
			this->windowFontWidth=readDWord(buffer,&offset);
			this->windowFontHeight=readDWord(buffer,&offset);
			this->spacing=readDWord(buffer,&offset)-this->windowFontWidth;
			this->lineSkip=readDWord(buffer,&offset);
			this->windowFrame.w=this->windowFrameColumns*(this->windowFontWidth+this->spacing);
			this->windowFrame.h=this->windowFrameRows*this->lineSkip;
			this->windowColor.r=readByte(buffer,&offset);
			this->windowColor.g=readByte(buffer,&offset);
			this->windowColor.b=readByte(buffer,&offset);
			this->transparentWindow=!!readByte(buffer,&offset);
			this->textSpeed=readDWord(buffer,&offset);
			this->textWindow.x=readDWord(buffer,&offset);
			this->textWindow.y=readDWord(buffer,&offset);
			this->textWindow.w=readDWord(buffer,&offset)-this->textWindow.x+1;
			this->textWindow.h=readDWord(buffer,&offset)-this->textWindow.y+1;
			_READ_BINARY_SJIS_STRING(unknownString_000,buffer,offset)
			this->arrowCursorAbs=!readDWord(buffer,&offset);
			this->pageCursorAbs=!readDWord(buffer,&offset);
			this->arrowCursorX=readSignedDWord(buffer,&offset);
			this->arrowCursorY=readSignedDWord(buffer,&offset);
			this->pageCursorX=readSignedDWord(buffer,&offset);
			this->pageCursorY=readSignedDWord(buffer,&offset);
			_READ_BINARY_SJIS_STRING(this->background,buffer,offset)
			_READ_BINARY_SJIS_STRING(this->leftChar,buffer,offset)
			_READ_BINARY_SJIS_STRING(this->centChar,buffer,offset)
			_READ_BINARY_SJIS_STRING(this->righChar,buffer,offset)
			/*readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);
			readSignedDWord(buffer,&offset);*/
			offset+=9*4;
			for (ulong a=0;a<1000;a++){
				Sprite *spr=new Sprite;
				_READ_BINARY_SJIS_STRING(spr->string,buffer,offset)
				spr->x=readSignedDWord(buffer,&offset);
				spr->y=readSignedDWord(buffer,&offset);
				spr->visibility=!!readDWord(buffer,&offset);
				spr->animOffset=readDWord(buffer,&offset);
				if (!*(spr->string)){
					delete spr;
					spr=0;
				}
				this->sprites.push_back(spr);
			}
			for (ulong a=0;a<200;a++){
				NONS_Variable *var=new NONS_Variable;
				var->intValue=readSignedDWord(buffer,&offset);
				_READ_BINARY_SJIS_STRING(var->wcsValue,buffer,offset)
				this->variables.push_back(var);
			}
			ulong nesting=readDWord(buffer,&offset);
			if (nesting){
				offset+=(nesting-1)*4;
				while (nesting){
					long i=readSignedDWord(buffer,&offset);
					if (i>0){
						stackEl *el=new stackEl;
						el->type=0;
						el->offset=i;
						offset-=8;
						nesting--;
						this->stack.push_back(el);
					}else{
						i=-i;
						offset-=16;
						stackEl *el=new stackEl;
						el->type=1;
						el->offset=i;
						el->variable=readDWord(buffer,&offset);
						el->to=readSignedDWord(buffer,&offset);
						el->step=readSignedDWord(buffer,&offset);
						offset-=16;
						nesting-=4;
					}
				}
				offset+=readDWord(buffer,&offset)*4;
			}
			this->monochrome=!!readDWord(buffer,&offset);
			this->monochromeColor.r=readDWord(buffer,&offset);
			this->monochromeColor.g=readDWord(buffer,&offset);
			this->monochromeColor.b=readDWord(buffer,&offset);
			this->negative=!!readDWord(buffer,&offset);
			_READ_BINARY_SJIS_STRING(this->midi,buffer,offset)
			_READ_BINARY_SJIS_STRING(this->wav,buffer,offset)
			this->musicTrack=readSignedDWord(buffer,&offset);
			this->loopMidi=!!readDWord(buffer,&offset);
			this->loopWav=!!readDWord(buffer,&offset);
			this->playOnce=!!readDWord(buffer,&offset);
			this->loopMp3=!!readDWord(buffer,&offset);
			this->saveMp3=!!readDWord(buffer,&offset);
			_READ_BINARY_SJIS_STRING(this->music,buffer,offset)
			this->hideWindow=!!readDWord(buffer,&offset);
			//Let's skip a lot of unnecessary data. The parentheses are
			//redundant, but they sure help understand what I'm doing.
			offset+=4+(100*7*4)+(100*6*4)+(3*4);
			_READ_BINARY_SJIS_STRING(this->btnDef,buffer,offset)
			offset+=4;
			this->hideWindow2=!!readByte(buffer,&offset);
			offset+=3;
			_READ_BINARY_SJIS_STRING(this->loopBGM0,buffer,offset)
			_READ_BINARY_SJIS_STRING(this->loopBGM1,buffer,offset)
			if (version>=201){
				offset+=3*4;
				char *p=readString(buffer,&offset);
				delete[] p;
			}
			ulong textL=readDWord(buffer,&offset);
			this->logPages.reserve(textL);
			for (ulong a=0;a<textL;a++){
				wchar_t *str;
				_READ_BINARY_SJIS_STRING(str,buffer,offset)
				this->logPages.push_back(str);
			}
			this->currentLine=readDWord(buffer,&offset);
			this->currentSubline=readDWord(buffer,&offset);
		}
	}else if (!instr(buffer,"NONS") || !instr(buffer,"BZh")){
		this->error=NONS_NO_ERROR;
		this->format='N';
		if (!instr(buffer,"BZh")){
			char *temp=decompressBuffer_BZ2(buffer,l,(unsigned long *)&l);
			delete[] buffer;
			buffer=temp;
		}
		offset+=5;
		this->version=readWord(buffer,&offset);
		for (ulong a=0;a<5;a++)
			this->hash[a]=readDWord(buffer,&offset);
		ulong header[4];
		for (ulong a=0;a<4;a++)
			header[a]=readDWord(buffer,&offset);
		//stack
		offset=*header;
		{
			ulong n=readDWord(buffer,&offset);
			for (ulong a=0;a<n;a++){
				stackEl *el=new stackEl();
				el->type=!!readByte(buffer,&offset);
				_READ_BINARY_UTF8_STRING(el->label,buffer,offset)
				el->offset=readDWord(buffer,&offset);
				if (!el->type)
					_READ_BINARY_UTF8_STRING(el->leftovers,buffer,offset)
				else{
					el->variable=readDWord(buffer,&offset);
					el->to=readSignedDWord(buffer,&offset);
					el->step=readSignedDWord(buffer,&offset);
				}
				this->stack.push_back(el);
			}
			_READ_BINARY_UTF8_STRING(this->currentLabel,buffer,offset)
			this->currentOffset=readDWord(buffer,&offset);
		}
		//variables
		offset=header[1];
		{
			ulong n=readDWord(buffer,&offset);
			std::vector<ulong> intervals;
			for (ulong a=0;a<n;a++){
				ulong b=readDWord(buffer,&offset);
				if (b&0x80000000){
					intervals.push_back(b&0x7FFFFFFF);
					intervals.push_back(1);
				}else{
					intervals.push_back(b);
					intervals.push_back(readDWord(buffer,&offset));
				}
			}
			for (ulong currentInterval=0;currentInterval<intervals.size();){
				ulong a=intervals[currentInterval],
					b=intervals[currentInterval+1];
				currentInterval+=2;
				if (this->variables.size()<a)
					this->variables.resize(a+b,0);
				for (ulong c=0;c<b;c++){
					NONS_Variable *var=new NONS_Variable();
					var->type='%';
					var->intValue=readSignedDWord((char *)buffer,&offset);
					_READ_BINARY_UTF8_STRING(var->wcsValue,buffer,offset)
					this->variables[a++]=var;
				}
			}
			n=readDWord(buffer,&offset);
			for (ulong a=0;a<n;a++){
				wchar_t *name;
				_READ_BINARY_UTF8_STRING(name,buffer,offset)
				this->arraynames.push_back(name);
				this->arrays.push_back(readArray(buffer,&offset));
			}
		}
		//screen
		offset=header[2];
		{
			ulong subheader[2];
			subheader[0]=readDWord(buffer,&offset);
			subheader[1]=readDWord(buffer,&offset);
			offset=*subheader;
			this->textWindow.x=readSignedWord(buffer,&offset);
			this->textWindow.y=readSignedWord(buffer,&offset);
			this->textWindow.w=readSignedWord(buffer,&offset);
			this->textWindow.h=readSignedWord(buffer,&offset);
			this->windowFrame.x=readSignedWord(buffer,&offset);
			this->windowFrame.y=readSignedWord(buffer,&offset);
			this->windowFrame.w=readSignedWord(buffer,&offset);
			this->windowFrame.h=readSignedWord(buffer,&offset);
			this->windowColor.r=readByte(buffer,&offset);
			this->windowColor.g=readByte(buffer,&offset);
			this->windowColor.b=readByte(buffer,&offset);
			this->windowTransition=readDWord(buffer,&offset);
			this->windowTransitionDuration=readDWord(buffer,&offset);
			_READ_BINARY_UTF8_STRING(this->windowTransitionRule,buffer,offset);
			this->hideWindow=!!readByte(buffer,&offset);
			this->fontSize=readWord(buffer,&offset);
			this->windowTextColor.r=readByte(buffer,&offset);
			this->windowTextColor.g=readByte(buffer,&offset);
			this->windowTextColor.b=readByte(buffer,&offset);
			this->textSpeed=readDWord(buffer,&offset);
			this->fontShadow=!!readByte(buffer,&offset);
			this->spacing=readWord(buffer,&offset);
			this->lineSkip=readSignedWord(buffer,&offset);
			ulong n=readDWord(buffer,&offset);
			for (ulong a=0;a<n;a++){
				wchar_t *str;
				_READ_BINARY_UTF8_STRING(str,buffer,offset)
				this->logPages.push_back(str);
			}
			_READ_BINARY_UTF8_STRING(this->currentBuffer,buffer,offset)
			this->textX=readWord(buffer,&offset);
			this->textY=readWord(buffer,&offset);
			_READ_BINARY_UTF8_STRING(this->arrowCursorString,buffer,offset)
			this->arrowCursorAbs=!!readByte(buffer,&offset);
			this->arrowCursorX=readSignedWord(buffer,&offset);
			this->arrowCursorY=readSignedWord(buffer,&offset);
			_READ_BINARY_UTF8_STRING(this->pageCursorString,buffer,offset)
			this->pageCursorAbs=!!readByte(buffer,&offset);
			this->pageCursorX=readSignedWord(buffer,&offset);
			this->pageCursorY=readSignedWord(buffer,&offset);

			offset=subheader[1];
			_READ_BINARY_UTF8_STRING(this->background,buffer,offset)
			if (!this->background){
				this->bgColor.r=readByte(buffer,&offset);
				this->bgColor.g=readByte(buffer,&offset);
				this->bgColor.b=readByte(buffer,&offset);
			}
			_READ_BINARY_UTF8_STRING(this->leftChar,buffer,offset)
			_READ_BINARY_UTF8_STRING(this->centChar,buffer,offset)
			_READ_BINARY_UTF8_STRING(this->righChar,buffer,offset)
			n=readDWord(buffer,&offset);
			std::vector<ulong> intervals;
			for (ulong a=0;a<n;a++){
				ulong b=readDWord(buffer,&offset);
				if (b&0x80000000){
					intervals.push_back(b&0x7FFFFFFF);
					intervals.push_back(1);
				}else{
					intervals.push_back(b);
					intervals.push_back(readDWord(buffer,&offset));
				}
			}
			for (ulong currentInterval=0;currentInterval<intervals.size();){
				ulong a=intervals[currentInterval],
					b=intervals[currentInterval+1];
				currentInterval+=2;
				if (this->sprites.size()<a)
					this->sprites.resize(a+b,0);
				for (ulong c=0;c<b;c++){
					Sprite *spr=new Sprite();
					_READ_BINARY_UTF8_STRING(spr->string,buffer,offset)
					spr->x=readSignedDWord(buffer,&offset);
					spr->y=readSignedDWord(buffer,&offset);
					spr->visibility=!!readByte(buffer,&offset);
					spr->alpha=readByte(buffer,&offset);
					this->sprites[a++]=spr;
				}
			}
			this->spritePriority=readDWord(buffer,&offset);
			this->monochrome=!!readByte(buffer,&offset);
			this->monochromeColor.r=readByte(buffer,&offset);
			this->monochromeColor.g=readByte(buffer,&offset);
			this->monochromeColor.b=readByte(buffer,&offset);
			this->negative=!!readByte(buffer,&offset);
		}
		//audio
		offset=header[3];
		{
			this->musicTrack=(char)readByte(buffer,&offset);
			if (this->musicTrack<0)
				_READ_BINARY_UTF8_STRING(this->music,buffer,offset)
			char vol=readByte(buffer,&offset);
			this->musicVolume=(vol&0x7F);
			this->loopMp3=!!(vol&0x80);
			this->channels.resize(readWord(buffer,&offset),0);
			for (ushort a=0;a<this->channels.size();a++){
				Channel *b=new Channel();
				_READ_BINARY_UTF8_STRING(b->name,buffer,offset)
				char vol=readByte(buffer,&offset);
				b->volume=(vol&0x7F);
				b->loop=!!(vol&0x80);
				this->channels[a]=b;
			}
		}
	}else
		this->error=NONS_UNSUPPORTED_SAVEGAME_VERSION;
	delete[] buffer;
}

NONS_SaveFile::NONS_SaveFile(){
	this->arrowCursorString=0;
	this->pageCursorString=0;
	this->windowTransitionRule=0;
	this->unknownString_000=0;
	this->background=0;
	this->leftChar=0;
	this->centChar=0;
	this->righChar=0;
	this->midi=0;
	this->wav=0;
	this->music=0;
	this->btnDef=0;
	this->loopBGM0=0;
	this->loopBGM1=0;
	this->currentLabel=0;
	memset(this->hash,0,sizeof(unsigned)*5);
	this->currentBuffer=0;
}

NONS_SaveFile::~NONS_SaveFile(){
	if (this->arrowCursorString)
		delete[] this->arrowCursorString;
	if (this->pageCursorString)
		delete[] this->pageCursorString;
	if (this->windowTransitionRule)
		delete[] this->windowTransitionRule;
	if (this->unknownString_000)
		delete[] this->unknownString_000;
	if (this->background)
		delete[] this->background;
	if (this->leftChar)
		delete[] this->leftChar;
	if (this->centChar)
		delete[] this->centChar;
	if (this->righChar)
		delete[] this->righChar;
	if (this->midi)
		delete[] this->midi;
	if (this->wav)
		delete[] this->wav;
	if (this->music)
		delete[] this->music;
	if (this->btnDef)
		delete[] this->btnDef;
	if (this->loopBGM0)
		delete[] this->loopBGM0;
	if (this->loopBGM1)
		delete[] this->loopBGM1;
	for (ulong a=0;a<this->sprites.size();a++)
		if (this->sprites[a])
			delete this->sprites[a];
	if (this->currentLabel)
		delete[] this->currentLabel;
	for (ulong a=0;a<this->stack.size();a++)
		if (this->stack[a])
			delete this->stack[a];
	for (ulong a=0;a<this->variables.size();a++)
		if (this->variables[a])
			delete this->variables[a];
	for (ulong a=0;a<this->arraynames.size();a++){
		if (this->arraynames[a])
			delete[] this->arraynames[a];
		if (this->arrays[a])
			delete this->arrays[a];
	}
	for (ulong a=0;a<this->logPages.size();a++)
		if (this->logPages[a])
			delete[] this->logPages[a];
	if (this->currentBuffer)
		delete[] this->currentBuffer;
	for (ulong a=0;a<this->channels.size();a++)
		if (this->channels[a])
			delete this->channels[a];
}

void writeArray(NONS_Variable *var,std::string *buffer){
	writeDWord(var->dimensionSize,buffer);
	if (var->dimensionSize){
		for (ulong a=0;a<var->dimensionSize;a++)
			writeArray(var->dimension[a],buffer);
	}else{
		writeDWord(var->intValue,buffer);
		writeString(var->wcsValue,buffer);
	}
}

bool NONS_SaveFile::save(char *filename){
	if (this->format!='N')
		return 0;
	std::string buffer("NONS");
	buffer.push_back(0);
	writeWord(NONS_SAVEFILE_VERSION,&buffer);
	for (ulong a=0;a<5;a++)
		writeDWord(this->hash[a],&buffer);
	ulong header[4];
	for (ulong a=0;a<4;a++){
		header[a]=buffer.size();
		writeDWord(0,&buffer);
	}
	//stack
	writeDWord(buffer.size(),&buffer,*header);
	{
		writeDWord(this->stack.size(),&buffer);
		for (ulong a=0;a<this->stack.size();a++){
			stackEl *el=this->stack[a];
			writeByte(el->type,&buffer);
			writeString(el->label,&buffer);
			writeDWord(el->offset,&buffer);
			if (!el->type){
				writeString(el->leftovers,&buffer);
			}else{
				writeDWord(el->variable,&buffer);
				writeDWord(el->to,&buffer);
				writeDWord(el->step,&buffer);
			}
		}
		writeString(this->currentLabel,&buffer);
		writeDWord(this->currentOffset,&buffer);
	}
	//variables
	writeDWord(buffer.size(),&buffer,header[1]);
	{
		std::vector<ulong> intervals;
		ulong last;
		bool set=0;
		for (ulong a=0;a<this->variables.size() && !set;a++){
			if (!!this->variables[a]){
				last=a;
				set=1;
			}
		}
		if (set){
			intervals.push_back(last++);
			for (ulong a=last;a<this->variables.size();a++){
				if (!this->variables[a]){
					intervals.push_back(last-intervals[intervals.size()-1]);
					for (a++;a<this->variables.size() && !this->variables[a];a++);
					if (a>=this->variables.size())
						break;
					intervals.push_back(a);
					last=a+1;
				}else
					last++;
			}
			if (intervals.size()%2)
				intervals.push_back(last-intervals[intervals.size()-1]);
			writeDWord(intervals.size()/2,&buffer);
			for (ulong a=0;a<intervals.size();){
				long start=intervals[a++];
				ulong size=intervals[a++];
				if (size==1)
					writeDWord(start|0x80000000,&buffer);
				else{
					writeDWord(start,&buffer);
					writeDWord(size,&buffer);
				}
			}
			for (ulong a=0;a<this->variables.size();a++){
				NONS_Variable *var=this->variables[a];
				if (!var)
					continue;
				writeDWord(var->intValue,&buffer);
				writeString(var->wcsValue,&buffer);
			}
		}else
			writeDWord(0,&buffer);
		writeDWord(this->arraynames.size(),&buffer);
		for (ulong a=0;a<this->arraynames.size();a++){
			writeString(this->arraynames[a],&buffer);
			writeArray(this->arrays[a],&buffer);
		}
	}
	//screen
	writeDWord(buffer.size(),&buffer,header[2]);
	{
		ulong screenHeader[2];
		for (ulong a=0;a<2;a++){
			screenHeader[a]=buffer.size();
			writeDWord(0,&buffer);
		}
		//window
		writeDWord(buffer.size(),&buffer,*screenHeader);
		writeWord(this->textWindow.x,&buffer);
		writeWord(this->textWindow.y,&buffer);
		writeWord(this->textWindow.w,&buffer);
		writeWord(this->textWindow.h,&buffer);
		writeWord(this->windowFrame.x,&buffer);
		writeWord(this->windowFrame.y,&buffer);
		writeWord(this->windowFrame.w,&buffer);
		writeWord(this->windowFrame.h,&buffer);
		writeByte(this->windowColor.r,&buffer);
		writeByte(this->windowColor.g,&buffer);
		writeByte(this->windowColor.b,&buffer);
		writeDWord(this->windowTransition,&buffer);
		writeDWord(this->windowTransitionDuration,&buffer);
		writeString(this->windowTransitionRule,&buffer);
		writeByte(this->hideWindow,&buffer);
		writeWord(this->fontSize,&buffer);
		writeByte(this->windowTextColor.r,&buffer);
		writeByte(this->windowTextColor.g,&buffer);
		writeByte(this->windowTextColor.b,&buffer);
		writeDWord(this->textSpeed,&buffer);
		writeByte(this->fontShadow,&buffer);
		writeWord(this->spacing,&buffer);
		writeWord(this->lineSkip,&buffer);
		writeDWord(this->logPages.size(),&buffer);
		for (ulong a=0;a<this->logPages.size();a++)
			writeString(this->logPages[a],&buffer);
		writeString(this->currentBuffer,&buffer);
		writeWord(this->textX,&buffer);
		writeWord(this->textY,&buffer);
		writeString(this->arrowCursorString,&buffer);
		writeByte(this->arrowCursorAbs,&buffer);
		writeWord(this->arrowCursorX,&buffer);
		writeWord(this->arrowCursorY,&buffer);
		writeString(this->pageCursorString,&buffer);
		writeByte(this->pageCursorAbs,&buffer);
		writeWord(this->pageCursorX,&buffer);
		writeWord(this->pageCursorY,&buffer);
		//graphics
		writeDWord(buffer.size(),&buffer,screenHeader[1]);
		writeString(this->background,&buffer);
		if (!this->background){
			writeByte(this->bgColor.r,&buffer);
			writeByte(this->bgColor.g,&buffer);
			writeByte(this->bgColor.b,&buffer);
		}
		writeString(this->leftChar,&buffer);
		writeString(this->centChar,&buffer);
		writeString(this->righChar,&buffer);
		std::vector<ulong> intervals;
		ulong last;
		bool set=0;
		for (ulong a=0;a<this->sprites.size() && !set;a++){
			if (!!this->sprites[a]){
				last=a;
				set=1;
			}
		}
		if (set){
			intervals.push_back(last++);
			for (ulong a=last;a<this->sprites.size();a++){
				if (!this->sprites[a]){
					intervals.push_back(last-intervals[intervals.size()-1]);
					for (a++;a<this->sprites.size() && !this->sprites[a];a++);
					if (a>=this->sprites.size())
						break;
					intervals.push_back(a);
					last=a+1;
				}else
					last++;
			}
			if (intervals.size()%2)
				intervals.push_back(last-intervals[intervals.size()-1]);
			writeDWord(intervals.size()/2,&buffer);
			for (ulong a=0;a<intervals.size();){
				long start=intervals[a++];
				ulong size=intervals[a++];
				if (size==1)
					writeDWord(start|0x80000000,&buffer);
				else{
					writeDWord(start,&buffer);
					writeDWord(size,&buffer);
				}
			}
			for (ulong a=0;a<this->sprites.size();a++){
				Sprite *spr=this->sprites[a];
				if (!spr)
					continue;
				writeString(spr->string,&buffer);
				writeDWord(spr->x,&buffer);
				writeDWord(spr->y,&buffer);
				writeByte(spr->visibility,&buffer);
				writeByte(spr->alpha,&buffer);
			}
		}else
			writeDWord(0,&buffer);
		writeDWord(this->spritePriority,&buffer);
		writeByte(this->monochrome,&buffer);
		writeByte(this->monochromeColor.r,&buffer);
		writeByte(this->monochromeColor.g,&buffer);
		writeByte(this->monochromeColor.b,&buffer);
		writeByte(this->negative,&buffer);
	}
	//audio
	writeDWord(buffer.size(),&buffer,header[3]);
	{
		writeByte(this->musicTrack,&buffer);
		if (this->musicTrack<0)
			writeString(this->music,&buffer);
		writeByte(this->musicVolume|(this->loopMp3?0:0x80),&buffer);
		writeWord(this->channels.size(),&buffer);
		for (ulong a=0;a<this->channels.size();a++){
			Channel *c=this->channels[a];
			writeString(c->name,&buffer);
			writeByte(c->volume|(c->loop?0:0x80),&buffer);
		}
	}
	
	ulong l;
	char *writebuffer=compressBuffer_BZ2((char *)buffer.c_str(),buffer.size(),&l);
	bool ret=!writefile(filename,writebuffer,l);
	delete[] writebuffer;
	return ret;
	//return !writefile(filename,(char *)buffer.c_str(),buffer.size());
}
#endif
