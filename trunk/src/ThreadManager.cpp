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

#include "ThreadManager.h"
#include "Globals.h"

#define NONS_PARALLELIZE

void NONS_Event::init(){
#ifdef NONS_SYS_WINDOWS
	this->event=CreateEvent(0,0,0,0);
#elif defined(NONS_SYS_UNIX)
	sem_init(&this->sem,0,0);
#endif
	this->initialized=1;
}

NONS_Event::~NONS_Event(){
	if (!this->initialized)
		return;
#ifdef NONS_SYS_WINDOWS
	CloseHandle(this->event);
#elif defined(NONS_SYS_UNIX)
	sem_destroy(&this->sem);
#endif
}

void NONS_Event::set(){
#ifdef NONS_SYS_WINDOWS
	SetEvent(this->event);
#elif defined(NONS_SYS_UNIX)
	sem_post(&this->sem);
#endif
}

void NONS_Event::reset(){
#ifdef NONS_SYS_WINDOWS
	ResetEvent(this->event);
#endif
}

void NONS_Event::wait(){
#ifdef NONS_SYS_WINDOWS
	WaitForSingleObject(this->event,INFINITE);
#elif defined(NONS_SYS_UNIX)
	sem_wait(&this->sem);
#endif
}

void NONS_Thread::init(ulong index){
	this->initialized=1;
	this->index=index;
	this->startCallEvent.init();
	this->callEndedEvent.init();
#ifdef NONS_SYS_WINDOWS
	this->thread=CreateThread(0,0,(LPTHREAD_START_ROUTINE)runningThread,this,0,0);
#elif defined(NONS_SYS_UNIX)
	pthread_create(&this->thread,0,runningThread,this);
#endif
	this->function=0;
	this->parameter=0;
	this->destroy=0;
}

NONS_Thread::~NONS_Thread(){
	if (!this->initialized)
		return;
	//this->wait();
	this->destroy=1;
	this->startCallEvent.set();
#ifdef NONS_SYS_WINDOWS
	WaitForSingleObject(this->thread,INFINITE);
	CloseHandle(this->thread);
#elif defined(NONS_SYS_UNIX)
	pthread_join(this->thread,0);
#endif
}

void NONS_Thread::call(NONS_ThreadedFunctionPointer f,void *p){
	this->function=f;
	this->parameter=p;
	this->startCallEvent.set();
}

void NONS_Thread::wait(){
	/*if (!this->function)
		return;*/
	this->callEndedEvent.wait();
}

NONS_ThreadManager::NONS_ThreadManager(ulong CPUs){
	this->init(CPUs);
}

void NONS_ThreadManager::init(ulong CPUs){
	this->threads.resize(CPUs-1);
	for (ulong a=0;a<this->threads.size();a++)
		this->threads[a].init(a);
}

ulong NONS_ThreadManager::call(ulong onThread,NONS_ThreadedFunctionPointer f,void *p){
	if (onThread>=this->threads.size())
		return -1;
	/*volatile NONS_ThreadedFunctionPointer *f2=&this->threads[onThread].function;
	while (!!*f2);*/
	//this->wait(onThread);
	this->threads[onThread].call(f,p);
	return onThread;
}

void NONS_ThreadManager::wait(ulong index){
	this->threads[index].wait();
}

void NONS_ThreadManager::waitAll(){
	for (ulong a=0;a<this->threads.size();a++)
		this->wait(a);
}

#ifdef NONS_SYS_WINDOWS
DWORD WINAPI 
#elif defined(NONS_SYS_UNIX)
void *
#endif
NONS_Thread::runningThread(void *p){
	NONS_Thread *t=(NONS_Thread *)p;
	while (1){
		t->startCallEvent.wait();
		if (t->destroy)
			break;
		t->function(t->parameter);
		t->parameter=0;
		t->function=0;
		t->callEndedEvent.set();
	}
	return 0;
}

void NONS_ThreadManager::setCPUcount(){
	if (!CLOptions.noThreads){
#ifdef NONS_PARALLELIZE
		//get CPU count
#if defined(NONS_SYS_WINDOWS)
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		cpu_count=si.dwNumberOfProcessors;
#elif defined(NONS_SYS_LINUX)
		{
			/*
			std::ifstream cpuinfo("/proc/cpuinfo");
			std::string line;
			std::set<unsigned> IDs;
			while (!cpuinfo.eof()){
				std::getline(cpuinfo,line);
				if (!line.size())
					continue;
				if (line.find("processor")!=0)
					continue;
				size_t start=line.find(':'),
					end;
				for (;line[start]<'0' || line[start]>'9';start++);
				for (end=start;line[end]>='0' && line[end]<='9';end++);
				line=line.substr(start,end-start);
				IDs.insert(atoi(line.c_str()));
			}
			cpu_count=IDs.size();
			cpuinfo.close();
			*/
			FILE * fp;
			char res[128];
			fp = popen("/bin/cat /proc/cpuinfo |grep -c '^processor'","r");
			fread(res, 1, sizeof(res)-1, fp);
			fclose(fp);
			cpu_count=atoi(res);
		}
#endif
		o_stdout <<"Using "<<cpu_count<<" CPU"<<(cpu_count!=1?"s":"")<<".\n";
#else
		o_stdout <<"Parallelization disabled.\n";
		cpu_count=1;
#endif
	}else{
		o_stdout <<"Parallelization disabled.\n";
		cpu_count=1;
	}
}
