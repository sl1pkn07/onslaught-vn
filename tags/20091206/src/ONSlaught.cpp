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
#include <iostream>

#include "Common.h"
#include "ErrorCodes.h"
#include "ScriptInterpreter.h"
#include "IOFunctions.h"
#include "ThreadManager.h"
#include "CommandLineOptions.h"
#include "version.h"

#if NONS_SYS_WINDOWS
#include <windows.h>
#endif

void mainThread(void *);

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

void enditall(bool stop_thread){
	if (stop_thread)
		gScriptInterpreter->stop();
#if 0
	if (!!dbgThread)
		SDL_KillThread(dbgThread);
	if (gScriptInterpreter)
		delete gScriptInterpreter;
#if NONS_SYS_LINUX
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
	if (ImageLoader)
		delete ImageLoader;
#if NONS_SYS_WINDOWS && defined _CONSOLE
	if (CLOptions.noconsole){
		HWND console=GetConsoleWindow();
		if (!!console){
			ShowWindow(console,SW_SHOW);
		}
	}
#endif
	exit(0);
#endif
}

void handle_SIGTERM(int){
	enditall(1);
}

void handle_SIGINT(int){
	exit(0);
}

volatile bool stopEventHandling=0;

int lastClickX=0;
int lastClickY=0;
bool useDebugMode=0,
	video_playback=0;

void handleInputEvent(SDL_Event &event){
	long x,y;
	switch(event.type){
		case SDL_QUIT:
			enditall(1);
			InputObserver.notify(&event);
			break;
		case SDL_KEYDOWN:
			if (useDebugMode){
				bool notify=0,
					full=0;
				switch (event.key.keysym.sym){
					case SDLK_RETURN:
						if (!!(event.key.keysym.mod&KMOD_ALT))
							full=1;
						else
							notify=1;
						break;
					case SDLK_F12:
						o_stdout <<"Screenshot saved to \""<<gScriptInterpreter->screen->screen->takeScreenshot()<<"\".\n";
						break;
					case SDLK_LCTRL:
					case SDLK_RCTRL:
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
				if (full && gScriptInterpreter->screen)
					gScriptInterpreter->screen->screen->toggleFullscreen();
				if (notify)
					InputObserver.notify(&event);
			}else{
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
						if (!video_playback)
							full=1;
						else
							notify=1;
						break;
					case SDLK_s:
						if (gScriptInterpreter->audio)
							gScriptInterpreter->audio->toggleMute();
						break;
					case SDLK_RETURN:
						if (CHECK_FLAG(event.key.keysym.mod,KMOD_ALT) && !video_playback)
							full=1;
						else
							notify=1;
						break;
					case SDLK_F12:
						if (!video_playback)
							o_stdout <<"Screenshot saved to \""<<gScriptInterpreter->screen->screen->takeScreenshot()<<"\".\n";
						else
							notify=1;
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
				if (full && gScriptInterpreter->screen)
					gScriptInterpreter->screen->screen->toggleFullscreen();
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
			if (gScriptInterpreter->screen){
				x=gScriptInterpreter->screen->screen->unconvertX(x);
				y=gScriptInterpreter->screen->screen->unconvertY(y);
				event.motion.x=(Uint16)x;
				event.motion.y=(Uint16)y;
			}
			if (x>0 && y>0)
				InputObserver.notify(&event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			x=event.button.x;
			y=event.button.y;
			if (gScriptInterpreter->screen){
				x=gScriptInterpreter->screen->screen->unconvertX(x);
				y=gScriptInterpreter->screen->screen->unconvertY(y);
				event.button.x=(Uint16)x;
				event.button.y=(Uint16)y;
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

extern ConfigFile settings;

int main(int argc,char **argv){
	std::cout <<"ONSlaught: An ONScripter clone with Unicode support."<<std::endl;
#if ONSLAUGHT_BUILD_VERSION<99999999
		std::cout <<"Build "<<ONSLAUGHT_BUILD_VERSION<<", ";
#endif
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
		std::cout <<"Redirecting."<<std::endl;
	}
	threadManager.setCPUcount();
#ifdef USE_THREAD_MANAGER
	threadManager.init(cpu_count);
#endif
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(250,20);
	config_directory=getConfigLocation();
	settings.init(config_directory+settings_filename,UTF8_ENCODING);

	
#if NONS_SYS_WINDOWS && defined _CONSOLE
	if (CLOptions.noconsole)
		FreeConsole();
#endif

	NONS_ScriptInterpreter *interpreter=gScriptInterpreter=new NONS_ScriptInterpreter;
	if (CLOptions.debugMode)
		console.init(interpreter->archive);
	SDL_WM_SetCaption("ONSlaught ("ONSLAUGHT_BUILD_VERSION_STR")",0);
#if NONS_SYS_WINDOWS
	findMainWindow(L"ONSlaught ("ONSLAUGHT_BUILD_VERSION_WSTR L")");
#endif
	NONS_Thread thread(mainThread,0);

	SDL_Event event;
	while (!stopEventHandling){
		while (SDL_PollEvent(&event) && !stopEventHandling)
			handleInputEvent(event);
		SDL_Delay(10);
	}
	delete gScriptInterpreter;
	delete ImageLoader;
	return 0;
}

void mainThread(void *){
	if (CLOptions.play.size())
		gScriptInterpreter->generic_play(CLOptions.play,CLOptions.play_from_archive);
	else
		while (gScriptInterpreter->interpretNextLine());
	stopEventHandling=1;
}
