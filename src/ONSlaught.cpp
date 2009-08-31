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

#include <fstream>
#include <cstdlib>
#include <csignal>

#include "Common.h"
#include "Functions.h"
#include "UTF.h"
#include "Everything.h"
#include "Globals.h"
#include "ErrorCodes.h"
#include "Processing/ScriptInterpreter.h"
#include "IO_System/IOFunctions.h"
#include "IO_System/FileIO.h"
#include "version.h"

#define NONS_PARALLELIZE

#if defined(NONS_SYS_WINDOWS)
#include <windows.h>
#endif

int mainThread(void *nothing);

bool useArgumentsFile(const char *filename,const std::vector<std::wstring> &argc){
	std::ifstream file(filename);
	if (!file)
		return 0;
	std::string str;
	std::getline(file,str);
	std::wstring copy=UniFromUTF8(str);
	std::vector<std::wstring> vec=getParameterList(copy,0);
	vec.insert(vec.end(),argc.begin(),argc.end());
	CLOptions.parse(vec);
	return 1;
}

SDL_Thread *dbgThread=0;
SDL_Thread *thread=0;

NONS_Everything *everything=0;

void enditall(){
	SDL_LockMutex(exitMutex);
	SDL_KillThread(thread);
	if (!!dbgThread)
		SDL_KillThread(dbgThread);
	if (gScriptInterpreter)
		delete gScriptInterpreter;
#ifdef NONS_SYS_LINUX
	/*
	This deserves some explanation.
	There's a segmentation fault I haven't been able to fix that occurs only on
	Linux. It occurs when trying to free the audio. Most likely, the error comes
	from much deeper, which is why I'm unable to fix it.
	All user data was written to disk when the script interpreter was deleted,
	so exitting now is perfectly safe.
	*/
	exit(0);
#endif
	if (everything)
		delete everything;
	if (ImageLoader)
		delete ImageLoader;
#if defined(NONS_SYS_WINDOWS) && defined(_CONSOLE)
	if (CLOptions.noconsole){
		HWND console=GetConsoleWindow();
		if (!!console){
			ShowWindow(console,SW_SHOW);
		}
	}
#endif
	exit(0);
}

void handle_SIGTERM(int){
	o_stdout <<"SIGTERM received. Terminating properly.";
	enditall();
}

void handle_SIGINT(int){
	o_stderr <<"SIGINT received. Terminating harshly.";
	exit(0);
}

bool stopEventHandling=0;

void handleInputEvent(SDL_Event &event){
	long x,y;
	switch(event.type){
		case SDL_QUIT:
			while (exitLocked)
				SDL_Delay(50);
			enditall();
			break;
		case SDL_KEYDOWN:
			{
				bool notify=0,
					full=0;
				switch (event.key.keysym.sym){
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						ctrlIsPressed=1;
						break;
					case SDLK_PERIOD:
						ctrlIsPressed=!ctrlIsPressed;
						break;
					case SDLK_f:
						full=1;
						break;
					case SDLK_s:
						if (everything->audio)
							everything->audio->toggleMute();
						break;
					case SDLK_RETURN:
						if (!!(event.key.keysym.mod&KMOD_ALT))
							full=1;
						else
							notify=1;
						break;
					case SDLK_F12:
						o_stdout <<"Screenshot saved to \""<<everything->screen->screen->takeScreenshot()<<"\".\n";
						break;
					case SDLK_NUMLOCK:
					case SDLK_CAPSLOCK:
					case SDLK_SCROLLOCK:
					case SDLK_RSHIFT:
					case SDLK_LSHIFT:
					case SDLK_RALT:
					case SDLK_LALT:
					case SDLK_RMETA:
					case SDLK_LMETA:
					case SDLK_LSUPER:
					case SDLK_RSUPER:
					case SDLK_MODE:
					case SDLK_COMPOSE:
						break;
					default:
						notify=1;
				}
				if (full && everything->screen)
					everything->screen->screen->toggleFullscreen();
				if (notify)
					InputObserver.notify(&event);
			}
			break;
		case SDL_KEYUP:
			InputObserver.notify(&event);
			if (event.key.keysym.sym==SDLK_LCTRL || event.key.keysym.sym==SDLK_RCTRL)
				ctrlIsPressed=0;
			break;
		case SDL_MOUSEMOTION:
			x=event.motion.x;
			y=event.motion.y;
			if (everything->screen){
				x=everything->screen->screen->unconvertX(x);
				y=everything->screen->screen->unconvertY(y);
				event.motion.x=x;
				event.motion.y=y;
			}
			if (x>0 && y>0)
				InputObserver.notify(&event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			x=event.button.x;
			y=event.button.y;
			if (everything->screen){
				x=everything->screen->screen->unconvertX(x);
				y=everything->screen->screen->unconvertY(y);
				event.button.x=x;
				event.button.y=y;
			}
			if (x>0 && y>0)
				InputObserver.notify(&event);
			lastClickX=event.button.x;
			lastClickY=event.button.y;
			break;
		default:
			InputObserver.notify(&event);
	}
}

std::vector<std::wstring> getArgumentsVector(char **argv){
	std::vector<std::wstring> ret;
	for (argv++;*argv;argv++)
		ret.push_back(UniFromUTF8(std::string(*argv)));
	return ret;
}

std::vector<std::wstring> getArgumentsVector(wchar_t **argv){
	std::vector<std::wstring> ret;
	for (argv++;*argv;argv++)
		ret.push_back(std::wstring(*argv));
	return ret;
}

#ifdef main
#undef main
#endif

#include "IO_System/Script/MacroParser.h"

int main(int argc,char **argv){
	std::cout <<"ONSlaught: An ONScripter clone with Unicode support."<<std::endl;
	if (ONSLAUGHT_BUILD_VERSION<99999999)
		std::cout <<"Build "<<ONSLAUGHT_BUILD_VERSION<<", ";
	std::cout <<ONSLAUGHT_BUILD_VERSION_STR"\n\n"
		"Copyright (c) "ONSLAUGHT_COPYRIGHT_YEAR_STR", Helios (helios.vmg@gmail.com)\n"
		"All rights reserved.\n\n"<<std::endl;

	signal(SIGTERM,handle_SIGTERM);
	signal(SIGINT,handle_SIGINT);

	std::vector<std::wstring> cmdl_arg=getArgumentsVector(argv);
	if (!useArgumentsFile("arguments.txt",cmdl_arg))
		CLOptions.parse(cmdl_arg);

	if (CLOptions.override_stdout){
		o_stdout.redirect();
		o_stderr.redirect();
		o_stderr.redirect();
		std::cout <<"Redirecting."<<std::endl;
	}
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
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	//exitMutex=SDL_CreateMutex();
	screenMutex=SDL_CreateMutex();
	config_directory=getConfigLocation();

#if defined(NONS_SYS_WINDOWS) && defined(_CONSOLE)
	if (CLOptions.noconsole){
		HWND console=GetConsoleWindow();
		if (!!console){
			ShowWindow(console,SW_HIDE);
		}
	}
#endif
	everything=new NONS_Everything();
	SDL_WM_SetCaption("ONSlaught ("ONSLAUGHT_BUILD_VERSION_STR")",0);
#ifdef NONS_SYS_WINDOWS
	findMainWindow(L"ONSlaught ("ONSLAUGHT_BUILD_VERSION_WSTR L")");
#endif

#ifdef LOOKUP_BLEND_CONSTANT
	for (ulong y=0;y<256;y++){
		for (ulong x=0;x<256;x++){
			ulong a=INTEGER_MULTIPLICATION(x^0xFF,y^0xFF)^0xFF;
			a=(y<<8)/a;
			if (a>255)
				a=255;
			blendData[x+(y<<8)]=a;
		}
	}
#endif

	ErrorCode error=NONS_NO_ERROR;
	if (CLOptions.scriptPath.size())
		error=everything->init_script(CLOptions.scriptPath,CLOptions.scriptencoding,CLOptions.scriptEncryption);
	else
		error=everything->init_script(CLOptions.scriptencoding);
	if (error!=NONS_NO_ERROR){
		handleErrors(error,-1,"mainThread",0);
		exit(error);
	}
	labellog.init(L"NScrllog.dat",L"nonsllog.dat");
	ImageLoader=new NONS_ImageLoader(everything->archive,CLOptions.cacheSize);
	o_stdout <<"Global files go in \""<<config_directory<<"\".\n";
	o_stdout <<"Local files go in \""<<save_directory<<"\".\n";
	if (CLOptions.musicDirectory.size())
		error=everything->init_audio(CLOptions.musicDirectory);
	else
		error=everything->init_audio();
	if (error!=NONS_NO_ERROR){
		handleErrors(error,-1,"mainThread",0);
		exit(error);
	}
	if (CLOptions.musicFormat.size())
		everything->audio->musicFormat=CLOptions.musicFormat;
	everything->init_screen();
	NONS_ScriptInterpreter *interpreter=new NONS_ScriptInterpreter(everything);
	gScriptInterpreter=interpreter;
	thread=SDL_CreateThread(mainThread,0);

	SDL_Event event;
	while (!stopEventHandling){
		while (SDL_WaitEvent(&event)>=0 && !stopEventHandling)
			handleInputEvent(event);
		SDL_Delay(100);
	}
	return 0;
}

int debugThread(void *nothing);

int mainThread(void *nothing){
	if (CLOptions.debugMode)
		dbgThread=SDL_CreateThread(debugThread,0);
	while (gScriptInterpreter->interpretNextLine());
	if (CLOptions.debugMode)
		SDL_KillThread(dbgThread);
	stopEventHandling=1;

	delete gScriptInterpreter;
	delete everything;
	delete ImageLoader;
#if defined(NONS_SYS_WINDOWS) && defined(_CONSOLE)
	if (CLOptions.noconsole){
		HWND console=GetConsoleWindow();
		if (!!console){
			ShowWindow(console,SW_SHOW);
		}
	}
#endif
	exit(0);
	exitLocked=0;
	return 0;
}

int debugThread(void *nothing){
	while (1){
		std::string input;
		std::getline(std::cin,input);
		if (!stdStrCmpCI(input,"exit") || !stdStrCmpCI(input,"quit"))
			return 0;
		std::wstring winput=UniFromUTF8(input);
		ErrorCode error;
		NONS_VariableMember *var=gScriptInterpreter->store->retrieve(winput,&error);
		if (var){
			if (var->getType()==INTEGER)
				std::cout <<"intValue: "<<var->getInt()<<std::endl;
			else if (var->getType()==STRING)
				std::cout <<"UTF-8 Value: \""<<UniToUTF8(var->getWcs())<<"\""<<std::endl;
			else
				std::cout <<"Scalar value."<<std::endl;
		}else if (error!=NONS_NO_ERROR)
			handleErrors(error,-1,"debugThread",0);
		else
			handleErrors(gScriptInterpreter->interpretString(winput,0,0),-1,"debugThread",0);
	}
}
