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

#ifndef NONS_THREADMANAGER_H
#define NONS_THREADMANAGER_H

#include "Common.h"
#include <vector>
typedef void (*NONS_ThreadedFunctionPointer)(void *);

#ifdef NONS_SYS_WINDOWS
#include <windows.h>
#elif defined(NONS_SYS_UNIX)
#include <pthread.h>
#include <semaphore.h>
#endif

#define USE_THREAD_MANAGER

class NONS_Event{
	bool initialized;
#ifdef NONS_SYS_WINDOWS
	HANDLE event;
#elif defined(NONS_SYS_UNIX)
	sem_t sem;
#endif
public:
	NONS_Event():initialized(0){}
	void init();
	~NONS_Event();
	void set();
	void reset();
	void wait();
};

class NONS_Thread{
	bool initialized;
#ifdef NONS_SYS_WINDOWS
	HANDLE thread;
#elif defined(NONS_SYS_UNIX)
	pthread_t thread;
#endif
	ulong index;
	volatile bool destroy;
	void *parameter;
public:
	NONS_Event startCallEvent,
		callEndedEvent;
	volatile NONS_ThreadedFunctionPointer function;
	NONS_Thread():initialized(0){}
	~NONS_Thread();
	void init(ulong index);
	void call(NONS_ThreadedFunctionPointer f,void *p);
	void wait();
#ifdef NONS_SYS_WINDOWS
	static DWORD WINAPI runningThread(void *);
#elif defined(NONS_SYS_UNIX)
	static void *runningThread(void *);
#endif
};

class NONS_ThreadManager{
	std::vector<NONS_Thread> threads;
public:
	NONS_ThreadManager(){}
	NONS_ThreadManager(ulong CPUs);
	void init(ulong CPUs);
	ulong call(ulong onThread,NONS_ThreadedFunctionPointer f,void *p);
	void wait(ulong index);
	void waitAll();
	static void setCPUcount();
};
#endif
