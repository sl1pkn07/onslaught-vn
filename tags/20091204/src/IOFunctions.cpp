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

#include "IOFunctions.h"
#include "CommandLineOptions.h"
#include <SDL/SDL.h>
#include <iostream>
#include <ctime>
#if NONS_SYS_WINDOWS
#include <windows.h>
#endif

NONS_InputObserver InputObserver;
NONS_RedirectedOutput o_stdout(std::cout);
NONS_RedirectedOutput o_stderr(std::cerr);
//NONS_RedirectedOutput o_stdlog(std::clog);

NONS_EventQueue::NONS_EventQueue(){
	InputObserver.attach(this);
}

NONS_EventQueue::~NONS_EventQueue(){
	InputObserver.detach(this);
}

void NONS_EventQueue::push(SDL_Event a){
	NONS_MutexLocker ml(this->mutex);
	this->data.push(a);
}

SDL_Event NONS_EventQueue::pop(){
	NONS_MutexLocker ml(this->mutex);
	SDL_Event ret=this->data.front();
	this->data.pop();
	return ret;
}

bool NONS_EventQueue::emptify(){
	NONS_MutexLocker ml(this->mutex);
	bool ret=0;
	while (!this->data.empty()){
		if (this->data.front().type==SDL_QUIT)
			ret=1;
		this->data.pop();
	}
	return ret;
}

bool NONS_EventQueue::empty(){
	NONS_MutexLocker ml(this->mutex);
	bool ret=this->data.empty();
	return ret;
}

void NONS_EventQueue::WaitForEvent(int delay){
	while (this->data.empty())
		SDL_Delay(delay);
}

NONS_InputObserver::NONS_InputObserver(){
	this->data.reserve(50);
}

NONS_InputObserver::~NONS_InputObserver(){}

void NONS_InputObserver::attach(NONS_EventQueue *what){
	NONS_MutexLocker ml(this->mutex);
	ulong pos=this->data.size();
	for (ulong a=0;a<pos;a++)
		if (!this->data[a])
			pos=a;
	if (pos==this->data.size())
		this->data.push_back(what);
	else
		this->data[pos]=what;
}
void NONS_InputObserver::detach(NONS_EventQueue *what){
	NONS_MutexLocker ml(this->mutex);
	for (ulong a=0;a<this->data.size();a++){
		if (this->data[a]==what){
			this->data[a]=0;
			break;
		}
	}
}
void NONS_InputObserver::notify(SDL_Event *event){
	NONS_MutexLocker ml(this->mutex);
	for (ulong a=0;a<this->data.size();a++)
		if (!!this->data[a])
			this->data[a]->push(*event);
}

struct reportedError{
	ErrorCode error;
	long original_line;
	std::string caller;
	std::wstring extraInfo;
	reportedError(ErrorCode error,long original_line,const char *caller,std::wstring &extra){
		this->error=error;
		this->original_line=original_line;
		this->caller=caller;
		this->extraInfo=extra;
	}
	reportedError(const reportedError &b){
		this->error=b.error;
		this->original_line=b.original_line;
		this->caller=b.caller;
		this->extraInfo=b.extraInfo;
	}
};

typedef std::map<Uint32,std::queue<reportedError> > errorManager;

void printError(NONS_RedirectedOutput &stream,ErrorCode error,ulong original_line,const std::string &caller,const std::wstring &extraInfo){
	if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
		if (caller.size()>0)
			stream <<caller<<"(): ";
		if (CHECK_FLAG(error,NONS_INTERNAL_ERROR))
			stream <<"Internal error. ";
		else{
			if (CHECK_FLAG(error,NONS_FATAL_ERROR))
				stream <<"Fatal error";
			else if (CHECK_FLAG(error,NONS_WARNING))
				stream <<"Warning";
			else
				stream <<"Error";
			if (original_line!=ULONG_MAX)
				stream <<" near line "<<original_line<<". ";
			else
				stream <<". ";
		}
		if (CHECK_FLAG(error,NONS_UNDEFINED_ERROR))
			stream <<"Unspecified error.\n";
		else
			stream <<"("<<ulong(error&0xFFFF)<<") "<<errorMessages[error&0xFFFF]<<"\n";
		if (extraInfo.size()){
			stream.indent(1);
			stream <<"Extra information: "<<extraInfo<<"\n";
			stream.indent(-1);
		}
	}
}

ErrorCode handleErrors(ErrorCode error,ulong original_line,const char *caller,bool queue,std::wstring extraInfo){
	static errorManager manager;
	Uint32 currentThread=SDL_ThreadID();
	errorManager::iterator currentQueue=manager.find(currentThread);
	/*if (error==NONS_END){
		if (currentQueue!=manager.end())
			while (!currentQueue->second.empty())
				currentQueue->second.pop();
		return error;
	}*/
	if (queue){
		(currentQueue!=manager.end()?currentQueue->second:manager[currentThread]).push(reportedError(error,original_line,caller,extraInfo));
		return error;
	}else if (CHECK_FLAG(error,NONS_NO_ERROR_FLAG) && currentQueue!=manager.end()){
		while (!currentQueue->second.empty())
			currentQueue->second.pop();
	}
	if (currentQueue!=manager.end()){
		while (!currentQueue->second.empty() && CHECK_FLAG(currentQueue->second.front().error,NONS_NO_ERROR_FLAG))
			currentQueue->second.pop();
		while (!currentQueue->second.empty()){
			reportedError &topError=currentQueue->second.front();
			printError(o_stderr,topError.error,topError.original_line,topError.caller,topError.extraInfo);
			currentQueue->second.pop();
		}
	}
	printError(o_stderr,error,original_line,caller,extraInfo);
	if (CHECK_FLAG(error,NONS_FATAL_ERROR)){
		o_stderr <<"I'll just go ahead and kill myself.\n";
		exit(error);
	}
	return error;
}

void waitUntilClick(NONS_EventQueue *queue){
	bool detach=!queue;
	if (detach)
		queue=new NONS_EventQueue;
	while (!CURRENTLYSKIPPING){
		SDL_Delay(25);
		while (!queue->empty()){
			SDL_Event event=queue->pop();
			if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_KEYDOWN){
				if (detach)
					delete queue;
				return;
			}
		}
	}
}

void waitCancellable(long delay,NONS_EventQueue *queue){
	bool detach=!queue;
	if (detach)
		queue=new NONS_EventQueue;
	while (delay>0 && !CURRENTLYSKIPPING){
		SDL_Delay(25);
		delay-=25;
		while (!queue->empty()){
			SDL_Event event=queue->pop();
			if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_KEYDOWN /*&& (event.key.keysym.sym==SDLK_LCTRL || event.key.keysym.sym==SDLK_RCTRL)*/){
				delay=0;
				break;
			}
		}
	}
	if (detach)
		delete queue;
}

void waitNonCancellable(long delay){
	while (delay>0 && !CURRENTLYSKIPPING){
		SDL_Delay(10);
		delay-=10;
	}
}

Uint8 getCorrectedMousePosition(NONS_VirtualScreen *screen,int *x,int *y){
	int x0,y0;
	Uint8 r=SDL_GetMouseState(&x0,&y0);
	x0=screen->unconvertX(x0);
	y0=screen->unconvertY(y0);
	*x=x0;
	*y=y0;
	return r;
}

NONS_RedirectedOutput::NONS_RedirectedOutput(std::ostream &a)
		:cout(a){
	this->file=0;
	this->indentation=0;
	this->addIndentationNext=1;
}

NONS_RedirectedOutput::~NONS_RedirectedOutput(){
	if (this->file)
		this->file->close();
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(ulong a){
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	if (this->addIndentationNext)
		for (ulong a=0;a<this->indentation;a++)
			stream <<INDENTATION_STRING;
	this->addIndentationNext=0;
	stream <<a;
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::outputHex(ulong a,ulong w){
	std::stringstream s;
	s.width(w);
	s <<s.hex<<a;
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	stream <<s.str();
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(long a){
	return *this <<(ulong)a;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(wchar_t a){
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	if (this->addIndentationNext)
		for (ulong a=0;a<this->indentation;a++)
			stream <<INDENTATION_STRING;
	this->addIndentationNext=0;
	stream <<a;
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(const char *a){
	return *this <<std::string(a);
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(const std::string &a){
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	for (ulong b=0;b<a.size();b++){
		char c=a[b];
		if (this->addIndentationNext)
			for (ulong d=0;d<this->indentation;d++)
				stream <<INDENTATION_STRING;
		if (c==UNICODE_LINE_FEED)
			this->addIndentationNext=1;
		else
			this->addIndentationNext=0;
		stream <<c;
	}
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(const std::wstring &a){
	return *this <<UniToUTF8(a);
}

void NONS_RedirectedOutput::redirect(){
	if (!!this->file)
		delete this->file;
	if (!CLOptions.override_stdout){
		this->file=0;
		return;
	}
	if (this->cout==std::cout)
		this->file=new std::ofstream("stdout.txt",CLOptions.reset_redirection_files?std::ios::trunc:std::ios::app);
	else if (this->cout==std::cerr)
		this->file=new std::ofstream("stderr.txt",CLOptions.reset_redirection_files?std::ios::trunc:std::ios::app);
	else
		this->file=new std::ofstream("stdlog.txt",CLOptions.reset_redirection_files?std::ios::trunc:std::ios::app);
	if (!this->file->is_open())
		delete this->file;
	else if (!CLOptions.reset_redirection_files){
		time_t secs=time(0);
		tm *t=localtime(&secs);
		std::string str=
			itoa<char>(t->tm_year+1900,4)+"-"+
			itoa<char>(t->tm_mon+1,4)+"-"+
			itoa<char>(t->tm_mday,4)+" "+
			itoa<char>(t->tm_hour,4)+":"+
			itoa<char>(t->tm_min,4)+":"+
			itoa<char>(t->tm_sec,4);
		*this->file <<"\n\n"
			"--------------------------------------------------------------------------------\n"
			"Session "<<str<<"\n"
			"--------------------------------------------------------------------------------"<<std::endl;
	}
}

void NONS_RedirectedOutput::indent(long a){
	if (!a)
		return;
	if (a<0){
		if (ulong(-a)>this->indentation)
			this->indentation=0;
		else
			this->indentation-=-a;
	}else
		this->indentation+=a;
}

NONS_File::NONS_File(const std::wstring &path,bool read){
	this->is_open=0;
#if NONS_SYS_WINDOWS
	this->file=INVALID_HANDLE_VALUE;
#endif
	this->open(path,read);
}

void NONS_File::open(const std::wstring &path,bool open_for_read){
	this->close();
	this->opened_for_read=open_for_read;
#if NONS_SYS_WINDOWS
	this->file=CreateFile(
		&path[0],
		(open_for_read?FILE_READ_DATA:FILE_WRITE_DATA),
		(open_for_read?FILE_SHARE_READ:FILE_SHARE_WRITE),
		0,
		(open_for_read?OPEN_EXISTING:TRUNCATE_EXISTING),
		0,0
	);
#else
	this->file.open(UniToUTF8(path).c_str(),std::ios::binary|(open_for_read?std::ios::in:std::ios::out));
#endif
	this->is_open=!!*this;
}

void NONS_File::close(){
	if (!this->is_open)
		return;
#if NONS_SYS_WINDOWS
	if (!!*this)
		CloseHandle(this->file);
#else
	this->file.close();
#endif
	this->is_open=0;
}
	
bool NONS_File::operator!(){
#if NONS_SYS_WINDOWS
	return this->file==INVALID_HANDLE_VALUE;
#else
	return !this->file;
#endif
}

ulong NONS_File::filesize(){
	if (!this->is_open)
		return 0;
#if NONS_SYS_WINDOWS
	return GetFileSize(this->file,0);
#else
	this->file.seekg(0,std::ios::end);
	return this->file.tellg();
#endif
}

NONS_File::type *NONS_File::read(ulong read_bytes,ulong &bytes_read,ulong offset){
	if (!this->is_open || !this->opened_for_read)
		return 0;
	ulong filesize=this->filesize();
	if (offset>=filesize)
		offset=filesize-1;
#if NONS_SYS_WINDOWS
	SetFilePointer(this->file,offset,0,FILE_BEGIN);
#else
	this->file.seekg(offset);
#endif
	if (filesize-offset<read_bytes)
		read_bytes=filesize-offset;
	NONS_File::type *buffer=new NONS_File::type[read_bytes];
#if NONS_SYS_WINDOWS
	DWORD rb=read_bytes,br=0;
	ReadFile(this->file,buffer,rb,&br,0);
#else
	this->file.read((char *)buffer,read_bytes);
#endif
	bytes_read=read_bytes;
	return buffer;
}

NONS_File::type *NONS_File::read(ulong &bytes_read){
	if (!this->is_open || !this->opened_for_read)
		return 0;
	ulong filesize=this->filesize();
#if NONS_SYS_WINDOWS
	SetFilePointer(this->file,0,0,FILE_BEGIN);
#else
	this->file.seekg(0);
#endif
	NONS_File::type *buffer=new NONS_File::type[filesize];
#if NONS_SYS_WINDOWS
	DWORD rb=filesize,br=0;
	ReadFile(this->file,buffer,rb,&br,0);
#else
	this->file.read((char *)buffer,filesize);
#endif
	bytes_read=filesize;
	return buffer;
}

bool NONS_File::write(void *buffer,ulong size){
	if (!this->is_open || this->opened_for_read)
		return 0;
#if NONS_SYS_WINDOWS
	SetFilePointer(this->file,0,0,FILE_BEGIN);
	DWORD a=size;
	WriteFile(this->file,buffer,a,&a,0);
#else
	this->file.seekp(0);
	this->file.write((char *)buffer,size);
#endif
	return 1;
}

NONS_File::type *NONS_File::read(const std::wstring &path,ulong read_bytes,ulong &bytes_read,ulong offset){
	NONS_File file(path,1);
	return file.read(read_bytes,bytes_read,offset);
}

NONS_File::type *NONS_File::read(const std::wstring &path,ulong &bytes_read){
	NONS_File file(path,1);
	return file.read(bytes_read);
}

bool NONS_File::write(const std::wstring &path,void *buffer,ulong size){
	NONS_File file(path,0);
	return file.write(buffer,size);
}

bool fileExists(const std::wstring &name){
	bool ret;
#if NONS_SYS_WINDOWS
	HANDLE file=CreateFile(&name[0],0,0,0,OPEN_EXISTING,0,0);
	ret=(file!=INVALID_HANDLE_VALUE);
	CloseHandle(file);
#else
	std::ifstream file(UniToUTF8(name).c_str());
	ret=!!file;
	file.close();
#endif
	return ret;
}
