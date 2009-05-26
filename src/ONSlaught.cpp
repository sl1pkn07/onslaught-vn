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
#include "Everything.h"
#include "Globals.h"
#include "ErrorCodes.h"
#include "Processing/ScriptInterpreter.h"
#include "IO_System/IOFunctions.h"
#include "IO_System/FileIO.h"

#if defined(NONS_SYS_WINDOWS)
#include <tchar.h>
#include <windows.h>
#define COMMAND_LINE_ARGUMENT_TYPE wchar_t
#else
#define COMMAND_LINE_ARGUMENT_TYPE char
#endif

int mainThread(void *nothing);

void usage(){
	o_stdout <<"Usage: ONSlaught [options]\n"
		"Options:\n"
		"  -h\n"
		"  -?\n"
		"  --help\n"
		"      Display this message.\n"
		"  --version\n"
		"      Display version number.\n"
		"  -verbosity <number>\n"
		"      Set log verbosity level. 0 by default.\n"
		"  -save-directory <directory name>\n"
		"      Override automatic save game directory selection.\n"
		"      See the documentation for more information.\n"
		"  -f\n"
		"      Start in fullscreen.\n"
		"  -r <virtual width> <virtual height> <real width> <real height>\n"
		"      Sets the screen resolution. The first two numbers are width and height of\n"
		"      the virtual screen. The second two numbers are width and height of the\n"
		"      physical screen or window graphical output will go to.\n"
		"      See the documentation for more information.\n"
		"  -script {auto|<path> {0|1|2|3}}\n"
		"      Select the path and encryption method used by the script.\n"
		"      Default is \'auto\'. On auto, this is the priority order for files:\n"
		"          1. \"0.txt\", method 0\n"
		"          2. \"00.txt\", method 0\n"
		"          3. \"nscr_sec.dat\", method 2\n"
		"          4. \"nscript.___\", method 3\n"
		"          5. \"nscript.dat\", method 1\n"
		"      The documentation contains a detailed description on each of the modes.\n"
		"  -encoding {auto|sjis|iso-8859-1|utf8|ucs2}\n"
		"      Select the encoding to be used for the script.\n"
		"      Default is \'auto\'.\n"
		"  -s\n"
		"      No sound.\n"
		"  -music-format {auto|ogg|mp3|it|xm|s3m|mod}}\n"
		"      Select the music format to be used.\n"
		"      Default is \'auto\'.\n"
		"  -music-directory <directory>\n"
		"      Set where to look for music files.\n"
		"      Default is \"./CD\"\n"
		"  -image-cache-size <size>\n"
		"      Set the size for the image cache. -1 is infinite, 0 is do not use.\n"
		"      Default to -1.\n"
		"  -debug\n"
		"      Enable debug mode.\n"
		"      If -output-to-file has been used, it is disabled.\n"
		"      See the documentation for details.\n"
#ifdef NONS_SYS_WINDOWS
		"  -no-console\n"
		"      Hide the console.\n"
#endif
		"  -redirect\n"
		"      Redirect stdout and stderr to \"stdout.txt\" and \"stderr.txt\"\n"
		"      correspondingly.\n"
		"      If -debug has been used, it is disabled.\n"
		"      By default, output is redirected.\n"
		"  -!redirect\n"
		"      Sends the output to the console instead of the file system.\n"
		"      See \"-redirect\" for more info.\n"
		"  -!reset-out-files\n"
		"      Only used with \"-redirect\".\n"
		"      Keeps the contents of stdout.txt, stderr.txt, and stdlog.txt when it opens\n"
		"      them and puts the date and time as identification.\n"
		"   -stop-on-first-error\n"
		"      Stops executing the script when the first error occurs. \"Unimplemented\n"
		"      command\" (when the command will not be implemented) errors don't count.\n";
	exit(0);
}

template <typename T0,typename T1>
int strcmp2(T0 *str0,T1 *str1){
	for (;*str0 || *str1;str0++,str1++){
		if (*str0<*str1)
			return -1;
		if (*str0>*str1)
			return 1;
	}
	return 0;
}

template <typename T>
int atoi2(T *str){
	char *temp=copyString(str);
	int res=atoi(temp);
	delete[] temp;
	return res;
}

std::ofstream textDumpFile;

template <typename T>
void parseCommandLine(int argc,T **argv){
	if (argc==1)
		return;
	static const char *options[]={
		"--help",
		"-script",
		"-encoding",
		"-music-format",
		"-music-directory",
		"",
		"",
		"-image-cache-size",
		"-debug",
		"-redirect",
		"--version",
		"-implementation",
		"-no-console",
		"-dump-text",
		"-f",
		"-r",
		"-verbosity",
		"-sdebug",
		"-s",
		"-h",
		"-?",
		"-save-directory",
		"-!reset-out-files",
		"-!redirect",
		"-stop-on-first-error",
		0
	};
	for (long argument=1;argument<argc;argument++){
		long option=-1;
		for (long a=0;options[a] && option<0;a++)
			if (!strcmp2(argv[argument],options[a]))
				option=a;
		switch(option){
			case 0: //--help
			case 19: //-h
			case 20: //-?
				usage();
			case 1: //-script
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				if (!strcmp2(argv[++argument],"auto"))
					break;
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument-1]<<"\""<<std::endl;
					break;
				}
				if (CLOptions.scriptPath)
					delete[] CLOptions.scriptPath;
				CLOptions.scriptPath=copyString(argv[argument]);
				switch (atoi2(argv[++argument])){
					case 0:
						CLOptions.scriptEncryption=NO_ENCRYPTION;
						break;
					case 1:
						CLOptions.scriptEncryption=XOR84_ENCRYPTION;
						break;
					case 2:
						CLOptions.scriptEncryption=VARIABLE_XOR_ENCRYPTION;
						break;
					case 3:
						CLOptions.scriptEncryption=TRANSFORM_THEN_XOR84_ENCRYPTION;
						break;
				}
				break;
			case 2: //-encoding
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				argument++;
				if (!strcmp2(argv[argument],"sjis")){
					CLOptions.scriptencoding=DETECT_ENCODING;
					break;
				}
				if (!strcmp2(argv[argument],"sjis")){
					CLOptions.scriptencoding=SJIS_ENCODING;
					break;
				}
				if (!strcmp2(argv[argument],"iso-8859-1")){
					CLOptions.scriptencoding=ISO_8859_1_ENCODING;
					break;
				}
				if (!strcmp2(argv[argument],"utf8")){
					CLOptions.scriptencoding=UTF8_ENCODING;
					break;
				}
				if (!strcmp2(argv[argument],"ucs2")){
					CLOptions.scriptencoding=UCS2_ENCODING;
					break;
				}
				std::cerr <<"Unrecognized encoding: \""<<argv[argument]<<"\""<<std::endl;
				break;
			case 3: //-music-format
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				if (!strcmp2(argv[++argument],"auto") || !!CLOptions.musicFormat){
					delete[] CLOptions.musicFormat;
					CLOptions.musicFormat=0;
					break;
				}
				if (!strcmp2(argv[argument],"ogg") ||
					!strcmp2(argv[argument],"mp3") ||
					!strcmp2(argv[argument],"it") ||
					!strcmp2(argv[argument],"xm") ||
					!strcmp2(argv[argument],"s3m") ||
					!strcmp2(argv[argument],"mod")){
					if (CLOptions.musicFormat)
						delete[] CLOptions.musicFormat;
					CLOptions.musicFormat=copyString(argv[argument]);
					break;
				}
				std::cerr <<"Unrecognized music format: \""<<argv[argument]<<"\""<<std::endl;
				break;
			case 4: //-music-directory
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				if (CLOptions.musicDirectory)
					delete[] CLOptions.musicDirectory;
				CLOptions.musicDirectory=copyString(argv[++argument]);
				break;
			case 5: //-transparency-method-layer
				break;
			case 6: //-transparency-method-anim
				break;
			case 7: //-image-cache-size
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				CLOptions.cacheSize=atoi2(argv[++argument]);
				break;
			case 8: //-debug
				CLOptions.debugMode=1;
				CLOptions.override_stdout=0;
				CLOptions.noconsole=0;
				break;
			case 9: //-redirect
				CLOptions.override_stdout=1;
				CLOptions.debugMode=0;
				break;
			case 10: //--version
				{
					delete new NONS_ScriptInterpreter(0);
				}
				exit(0);
			case 11: //-implementation
				break;
			case 12: //-no-console
#ifdef NONS_SYS_WINDOWS
				CLOptions.noconsole=1;
				CLOptions.debugMode=0;
				CLOptions.override_stdout=1;
#endif
				break;
			case 13: //-dump-text
				{
					if (argument+1>=argc){
						std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
						break;
					}
					char *copy=copyString(argv[++argument]);
					textDumpFile.open(copy,std::ios::app);
					delete[] copy;
				}
				break;
			case 14: //-f
				CLOptions.startFullscreen=1;
				break;
			case 15: //-r
				if (argument+4>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				CLOptions.virtualWidth=atoi2(argv[++argument]);
				CLOptions.virtualHeight=atoi2(argv[++argument]);
				CLOptions.realWidth=atoi2(argv[++argument]);
				CLOptions.realHeight=atoi2(argv[++argument]);
				break;
			case 16: //-verbosity
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
					break;
				}
				CLOptions.verbosity=atoi2(argv[++argument]);
				break;
			case 18: //-s
				CLOptions.no_sound=1;
				break;
			case 21: //-save-directory
				if (argument+1>=argc){
					std::cerr <<"Invalid argument syntax: \""<<argv[argument]<<"\""<<std::endl;
				}else{
					char *str=copyString(argv[++argument]);
					ulong l=strlen(str);
					for (l--;str[l]=='/' || str[l]=='\\';l--)
						str[l]=0;
					if (!strlen(str))
						delete[] str;
					else{
						if (CLOptions.savedir)
							delete[] CLOptions.savedir;
						CLOptions.savedir=str;
					}
				}
				break;
			case 22: //-!reset-out-files
				CLOptions.reset_redirection_files=0;
				break;
			case 23: //-!redirect
				CLOptions.override_stdout=0;
				break;
			case 24: //-stop-on-first-error
				CLOptions.stopOnFirstError=1;
				break;
			case 17://-sdebug
			default:
				std::cout <<"Unrecognized command line option: \""<<argv[argument]<<"\""<<std::endl;
		}
	}
}

bool useArgumentsFile(const char *filename){
	std::ifstream file(filename);
	if (!file)
		return 0;
	std::string str;
	std::getline(file,str);
	std::vector<char *> *vec=getParameterList(str.c_str());
	char **argv=new char*[vec->size()+1];
	for (ulong a=0;a<vec->size();a++)
		argv[a+1]=(*vec)[a];
	parseCommandLine(vec->size()+1,argv);
	delete[] argv;
	for (ulong a=0;a<vec->size();a++)
		delete[] (*vec)[a];
	delete vec;
	return 1;
}

SDL_Thread *dbgThread=0;
SDL_Thread *thread=0;

NONS_Everything *everything=0;

void enditall(){
	SDL_LockMutex(exitMutex);
	SDL_KillThread(thread);
	if (gScriptInterpreter)
		delete (NONS_ScriptInterpreter *)gScriptInterpreter;
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

void handleInputEvent(SDL_Event &event,bool onlyHandleQuit=0){
	if (onlyHandleQuit && event.type!=SDL_QUIT || stopEventHandling)
		return;
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

void writeEvent(SDL_Event *event,uchar *buffer){
	*buffer++=event->type;
	switch(event->type){
		case SDL_QUIT:
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			*buffer++=event->key.type;
			*buffer++=event->key.which;
			*buffer++=event->key.state;
			*buffer++=event->key.keysym.scancode;
			{
				Uint32 x=event->key.keysym.sym;
				for (int a=0;a<4;a++){
					*buffer++=x&0xFF;
					x>>=8;
				}
			}
			{
				Uint32 x=event->key.keysym.mod;
				for (int a=0;a<4;a++){
					*buffer++=x&0xFF;
					x>>=8;
				}
			}
			{
				Uint16 x=event->key.keysym.unicode;
				for (int a=0;a<2;a++){
					*buffer++=x&0xFF;
					x>>=8;
				}
			}
			break;
		case SDL_MOUSEMOTION:
			*buffer++=event->motion.type;
			*buffer++=event->motion.which;
			*buffer++=event->motion.state;
			*buffer++=event->motion.x&0xFF;
			*buffer++=event->motion.x>>8;
			*buffer++=event->motion.y&0xFF;
			*buffer++=event->motion.y>>8;
			*buffer++=event->motion.xrel&0xFF;
			*buffer++=event->motion.xrel>>8;
			*buffer++=event->motion.yrel&0xFF;
			*buffer++=event->motion.yrel>>8;
			break;
		case SDL_MOUSEBUTTONDOWN:
			*buffer++=event->button.type;
			*buffer++=event->button.which;
			*buffer++=event->button.button;
			*buffer++=event->button.state;
			*buffer++=event->button.x&0xFF;
			*buffer++=event->button.x>>8;
			*buffer++=event->button.y&0xFF;
			*buffer++=event->button.y>>8;
			break;
		default:
			break;
	}
}

int
#ifdef _MSC_VER
	_tmain(int argc,wchar_t **argv)
#else
	main(int argc,char **argv)
#endif
{
	std::cout <<"ONSlaught ";
	if (ONSLAUGHT_BUILD_VERSION<99999999)
		std::cout <<ONSLAUGHT_BUILD_VERSION<<" ";
	std::cout <<ONSLAUGHT_BUILD_VERSION_STR": An ONScripter clone with Unicode support.\n\n"
		"Copyright (c) "ONSLAUGHT_COPYRIGHT_YEAR_STR", Helios (helios.vmg@gmail.com)\n"
		"All rights reserved.\n\n"
		"\"Make it right before you make it faster.\"\n\n"<<std::endl;
	signal(SIGTERM,handle_SIGTERM);
	signal(SIGINT,handle_SIGINT);
	if (argc>1 && (
			!strcmp2(argv[1],"-?") || 
			!strcmp2(argv[1],"-h") || 
			!strcmp2(argv[1],"--help") || 
			!strcmp2(argv[1],"--version")
			) || !useArgumentsFile("arguments.txt"))
		parseCommandLine(argc,argv);
	if (CLOptions.override_stdout){
		o_stdout.redirect();
		o_stderr.redirect();
		o_stderr.redirect();
		std::cout <<"Redirecting."<<std::endl;
	}
#ifdef NONS_PARALLELIZE
	//get CPU count
#if defined(NONS_SYS_WINDOWS)
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	cpu_count=siSysInfo.dwNumberOfProcessors;
#elif defined(NONS_SYS_LINUX)
	{
		/*std::ifstream cpuinfo("/proc/cpuinfo");
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
		cpuinfo.close();*/
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
	o_stdout <<"Parallelization disabled."<<std::endl;
#endif
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	//exitMutex=SDL_CreateMutex();
	screenMutex=SDL_CreateMutex();

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
	ErrorCode error=NONS_NO_ERROR;
	if (CLOptions.scriptPath)
		error=everything->init_script(CLOptions.scriptPath,CLOptions.scriptencoding,CLOptions.scriptEncryption);
	else
		error=everything->init_script(CLOptions.scriptencoding);
	if (error!=NONS_NO_ERROR){
		handleErrors(error,-1,"mainThread",0);
		exit(error);
	}
	labellog.init("NScrllog.dat","nonsllog.dat");
	ImageLoader=new NONS_ImageLoader(everything->archive,CLOptions.cacheSize);
	if (!config_directory)
		config_directory=getConfigLocation();
	o_stdout <<"Global files go in \""<<config_directory<<"\".\n";
	o_stdout <<"Local files go in \""<<save_directory<<"\".\n";
	if (CLOptions.musicDirectory)
		error=everything->init_audio(CLOptions.musicDirectory);
	else
		error=everything->init_audio();
	if (error!=NONS_NO_ERROR){
		handleErrors(error,-1,"mainThread",0);
		exit(error);
	}
	if (CLOptions.musicFormat)
		everything->audio->musicFormat=CLOptions.musicFormat;
	everything->init_screen();
	NONS_ScriptInterpreter *interpreter=new NONS_ScriptInterpreter(everything);
	gScriptInterpreter=(void *)interpreter;
	thread=SDL_CreateThread(mainThread,0);

	SDL_Event event;
	while (1){
		while (SDL_WaitEvent(&event)>=0)
			handleInputEvent(event);
		SDL_Delay(100);
	}
	return 0;
}

int debugThread(void *nothing);

int mainThread(void *nothing){
	NONS_ScriptInterpreter *interpreter=(NONS_ScriptInterpreter *)gScriptInterpreter;
	if (CLOptions.debugMode)
		dbgThread=SDL_CreateThread(debugThread,0);
	while (interpreter->interpretNextLine());
	if (CLOptions.debugMode)
		SDL_KillThread(dbgThread);
	stopEventHandling=1;

	delete interpreter;
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
	NONS_ScriptInterpreter *interpreter=(NONS_ScriptInterpreter *)gScriptInterpreter;
	while (1){
		char *command=inputstr();
		NONS_tolower(command);
		if (!strcmp(command,"exit") || !strcmp(command,"quit"))
			return 0;
		wchar_t *wcommand=copyWString(command);
		ErrorCode error;
		NONS_VariableMember *var=interpreter->store->retrieve(wcommand,&error);
		if (var){
			if (var->getType()==INTEGER)
				std::cout <<"intValue: "<<var->getInt()<<std::endl;
			else if (var->getType()==STRING){
				if (var->getWcs()){
					char *copy=WChar_to_UTF8(var->getWcs());
					std::cout <<"UTF-8 Value: \""<<copy<<"\""<<std::endl;
					delete[] copy;
				}else
					std::cout <<"UTF-8 Value: \"\""<<std::endl;
			}else
				std::cout <<"Scalar value."<<std::endl;
		}else if (error!=NONS_NO_ERROR)
			handleErrors(error,-1,"debugThread",0);
		else
			handleErrors(interpreter->interpretString(wcommand),-1,"debugThread",0);
		delete[] wcommand;
		delete[] command;
	}
}
