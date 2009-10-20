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
#include "../Globals.h"
#include "../Functions.h"
#include <SDL/SDL.h>
#include <stack>
#include <cstdlib>

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
			if (!CHECK_FLAG(topError.error,NONS_NO_ERROR_FLAG)){
				if (topError.caller.size()>0)
					o_stderr <<topError.caller<<"(): ";
				if (CHECK_FLAG(topError.error,NONS_INTERNAL_ERROR))
					o_stderr <<"Internal error. ";
				else{
					if (CHECK_FLAG(topError.error,NONS_FATAL_ERROR))
						o_stderr <<"Fatal error";
					else if (CHECK_FLAG(topError.error,NONS_WARNING))
						o_stderr <<"Warning";
					else
						o_stderr <<"Error";
					if (topError.original_line!=ULONG_MAX)
						o_stderr <<" near line "<<topError.original_line<<". ";
					else
						o_stderr <<". ";
				}
				if (CHECK_FLAG(topError.error,NONS_UNDEFINED_ERROR))
					o_stderr <<"Unspecified error.\n";
				else
					o_stderr <<"("<<ulong(topError.error&0xFFFF)<<") "<<errorMessages[topError.error&0xFFFF]<<"\n";
				if (topError.extraInfo.size())
					o_stderr <<"    Extra information: "<<topError.extraInfo<<"\n";
			}
			currentQueue->second.pop();
		}
	}
	if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
		if (caller)
			o_stderr <<caller<<"(): ";
		if (CHECK_FLAG(error,NONS_INTERNAL_ERROR))
			o_stderr <<"Internal error. ";
		else{
			if (CHECK_FLAG(error,NONS_FATAL_ERROR))
				o_stderr <<"Fatal error";
			else if (CHECK_FLAG(error,NONS_WARNING))
				o_stderr <<"Warning";
			else
				o_stderr <<"Error";
			if (original_line>0)
				o_stderr <<" near line "<<original_line<<". ";
			else
				o_stderr <<". ";
		}
		if (CHECK_FLAG(error,NONS_UNDEFINED_ERROR))
			o_stderr <<"Unspecified error.\n";
		else
			o_stderr <<errorMessages[error&0xFFFF]<<"\n";
		if (extraInfo.size())
			o_stderr <<"    Extra information: "<<extraInfo<<"\n";
	}
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
	bool detach=queue==0;
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
