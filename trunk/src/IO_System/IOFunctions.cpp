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

#ifndef NONS_IOFUNCTIONS_CPP
#define NONS_IOFUNCTIONS_CPP

#include "IOFunctions.h"
#include "../Globals.h"
#include <cstdlib>

ErrorCode handleErrors(ErrorCode error,long original_line,const char *caller){
	if ((error&NONS_END)==NONS_END)
		return error;
	if (!(error&NONS_NO_ERROR_FLAG)){
		if (caller)
			v_stderr <<caller<<"(): ";
		if ((error&NONS_INTERNAL_ERROR)==NONS_INTERNAL_ERROR)
			v_stderr <<"Internal error. ";
		else{
			if ((error&NONS_FATAL_ERROR)==NONS_FATAL_ERROR)
				v_stderr <<"Fatal error";
			else if ((error&NONS_WARNING)==NONS_WARNING)
				v_stderr <<"Warning";
			else
				v_stderr <<"Error";
			if (original_line>0)
				v_stderr <<" near line "<<original_line<<". ";
			else
				v_stderr <<". ";
		}
		if ((error&NONS_UNDEFINED_ERROR)==NONS_UNDEFINED_ERROR)
			v_stderr <<"Unspecified error."<<std::endl;
		else
			v_stderr <<errorMessages[error&0xFFFF]<<std::endl;
		if ((error&NONS_FATAL_ERROR)==NONS_FATAL_ERROR){
			v_stderr <<"I'll just go ahead and kill myself."<<std::endl;
			exit(error);
		}
	}
	return error;
}

void waitUntilClick(NONS_EventQueue *queue){
	bool detach=!queue;
	if (detach)
		queue=InputObserver.attach();
	while (!CURRENTLYSKIPPING){
		SDL_Delay(25);
		while (!queue->data.empty()){
			SDL_Event event=queue->pop();
			if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_KEYDOWN){
				if (detach)
					InputObserver.detach(queue);
				return;
			}
		}
	}
}

void waitCancellable(long delay,NONS_EventQueue *queue){
	bool detach=queue==0;
	if (detach)
		queue=InputObserver.attach();
	while (delay>0 && !CURRENTLYSKIPPING){
		SDL_Delay(25);
		delay-=25;
		while (!queue->data.empty()){
			SDL_Event event=queue->pop();
			if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_KEYDOWN /*&& (event.key.keysym.sym==SDLK_LCTRL || event.key.keysym.sym==SDLK_RCTRL)*/){
				delay=0;
				break;
			}
		}
	}
	if (detach)
		InputObserver.detach(queue);
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

char *inputstr(long max){
	char *txt=new char[max];
	char b;
	long a;
	for (a=0;(b=std::cin.get())!=10;a++)
		if (a<max-1)
			txt[a]=b;
	if (a>max-1)
		a=max-1;
	txt[a]=0;
	return txt;
}
#endif
