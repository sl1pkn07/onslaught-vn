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

#ifndef NONS_SCRIPTINTERPRETER_CPP
#define NONS_SCRIPTINTERPRETER_CPP

#include "ScriptInterpreter.h"
#include "../Functions.h"
#include "../Globals.h"
#include "../IO_System/FileIO.h"
#include "../IO_System/IOFunctions.h"
//#include "../UTF.h"
#include <sstream>

#undef ABS
#include "../IO_System/Graphics/SDL_bilinear.h"

SDL_Surface *(*rotationFunction)(SDL_Surface *,double)=SDL_RotateSmooth;
SDL_Surface *(*resizeFunction)(SDL_Surface *,int,int)=SDL_ResizeSmooth;

printingPage::printingPage(){
}

printingPage::printingPage(const printingPage &b){
	*this=b;
}

printingPage &printingPage::operator=(const printingPage &b){
	this->print=b.print;
	this->reduced=b.reduced;
	this->stops=b.stops;
	return *this;
}


NONS_StackElement::NONS_StackElement(ulong level){
	this->type=UNDEFINED;
	this->var=0;
	this->from=0;
	this->to=0;
	this->step=0;
	this->textgosubLevel=level;
	this->textgosubTriggeredBy=0;
}

NONS_StackElement::NONS_StackElement(const std::pair<ulong,ulong> &returnTo,const NONS_ScriptLine &interpretAtReturn,ulong beginAtStatement,ulong level)
		:interpretAtReturn(interpretAtReturn,beginAtStatement){
	this->returnTo.line=returnTo.first;
	this->returnTo.statement=returnTo.second;
	this->type=SUBROUTINE_CALL;
	this->textgosubLevel=level;
	this->textgosubTriggeredBy=0;
}

NONS_StackElement::NONS_StackElement(NONS_VariableMember *variable,const std::pair<ulong,ulong> &startStatement,long from,long to,long step,ulong level){
	this->returnTo.line=startStatement.first;
	this->returnTo.statement=startStatement.second;
	this->type=FOR_NEST;
	this->var=variable;
	this->from=from;
	this->to=to;
	this->step=step;
	this->end=this->returnTo;
	this->textgosubLevel=level;
	this->textgosubTriggeredBy=0;
}

NONS_StackElement::NONS_StackElement(const std::vector<printingPage> &pages,wchar_t trigger,ulong level){
	this->type=TEXTGOSUB_CALL;
	this->textgosubLevel=level;
	this->pages=pages;
	this->textgosubTriggeredBy=trigger;
}

void NONS_ScriptInterpreter::init(){
	this->script=everything->script;
	//this->interpreter_position=0;
	this->thread=new NONS_ScriptThread(this->script);
	this->store=new NONS_VariableStore();
	this->interpreter_mode=DEFINE;
	this->nsadir="./";
	this->default_speed=0;
	this->default_speed_slow=0;
	this->default_speed_med=0;
	this->default_speed_fast=0;

	{
		std::wstring settings_filename=config_directory+L"settings.cfg";
		ConfigFile settings(settings_filename);
		if (settings.exists(L"textSpeedMode"))
			this->current_speed_setting=settings.getInt(L"textSpeedMode");
		else
			this->current_speed_setting=1;
	}

	srand(time(0));
	this->defaultx=640;
	this->defaulty=480;
	this->defaultfs=18;
	this->legacy_set_window=1;
	this->arrowCursor=new NONS_Cursor(L":l/3,160,2;cursor0.bmp",0,0,0,this->everything->screen);
	if (!this->arrowCursor->data){
		delete this->arrowCursor;
		this->arrowCursor=new NONS_Cursor(this->everything->screen);
	}
	this->pageCursor=new NONS_Cursor(L":l/3,160,2;cursor1.bmp",0,0,0,this->everything->screen);
	if (!this->pageCursor->data){
		delete this->pageCursor;
		this->pageCursor=new NONS_Cursor(this->everything->screen);
	}
	this->gfx_store=this->everything->screen->gfx_store;
	this->hideTextDuringEffect=1;
	this->selectOn.r=0xFF;
	this->selectOn.g=0xFF;
	this->selectOn.b=0xFF;
	this->selectOff.r=0xA9;
	this->selectOff.g=0xA9;
	this->selectOff.b=0xA9;
	this->inputQueue=InputObserver.attach();
	this->autoclick=0;
	this->timer=SDL_GetTicks();
	//this->setDefaultWindow();
	this->main_font=this->everything->screen->output->foregroundLayer->fontCache->font;
	this->menu=new NONS_Menu(this);
	this->imageButtons=0;
	this->new_if=0;
	this->btnTimer=0;
	this->imageButtonExpiration=0;
	this->saveGame=new NONS_SaveFile();
	this->saveGame->format='N';
	memcpy(this->saveGame->hash,this->script->hash,sizeof(unsigned)*5);
	this->printed_lines.clear();
	this->everything->screen->char_baseline=this->everything->screen->screen->inRect.h-1;
}

NONS_ScriptInterpreter::NONS_ScriptInterpreter(NONS_Everything *everything){
	this->arrowCursor=0;
	this->pageCursor=0;
	this->inputQueue=0;
	this->menu=0;
	this->imageButtons=0;
	this->saveGame=0;
	this->script=0;
	this->store=0;
	this->gfx_store=0;
	this->everything=0;
	this->main_font=0;
	this->thread=0;
	if (everything){
		if (!everything->script){
			this->everything=0;
			this->script=0;
			this->store=0;
			return;
		}
		this->everything=everything;
		this->init();
	}

	this->commandList[L"abssetcursor"]=&NONS_ScriptInterpreter::command_setcursor;
	this->commandList[L"add"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"allsphide"]=&NONS_ScriptInterpreter::command_allsphide;
	this->commandList[L"allspresume"]=&NONS_ScriptInterpreter::command_allsphide;
	this->commandList[L"amsp"]=&NONS_ScriptInterpreter::command_msp;
	this->commandList[L"arc"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"atoi"]=&NONS_ScriptInterpreter::command_atoi;
	this->commandList[L"autoclick"]=&NONS_ScriptInterpreter::command_autoclick;
	this->commandList[L"automode_time"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"automode"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"avi"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"bar"]=0;
	this->commandList[L"barclear"]=0;
	this->commandList[L"bg"]=&NONS_ScriptInterpreter::command_bg;
	this->commandList[L"bgcopy"]=&NONS_ScriptInterpreter::command_bgcopy;
	this->commandList[L"bgcpy"]=&NONS_ScriptInterpreter::command_bgcopy;
	this->commandList[L"bgm"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"bgmonce"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"bgmstop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"bgmvol"]=&NONS_ScriptInterpreter::command_mp3vol;
	this->commandList[L"blt"]=&NONS_ScriptInterpreter::command_blt;
	this->commandList[L"br"]=&NONS_ScriptInterpreter::command_br;
	this->commandList[L"break"]=&NONS_ScriptInterpreter::command_break;
	this->commandList[L"btn"]=&NONS_ScriptInterpreter::command_btn;
	this->commandList[L"btndef"]=&NONS_ScriptInterpreter::command_btndef;
	this->commandList[L"btndown"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"btntime"]=&NONS_ScriptInterpreter::command_btntime;
	this->commandList[L"btntime2"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"btnwait"]=&NONS_ScriptInterpreter::command_btnwait;
	this->commandList[L"btnwait2"]=&NONS_ScriptInterpreter::command_btnwait;
	this->commandList[L"caption"]=&NONS_ScriptInterpreter::command_caption;
	this->commandList[L"cell"]=&NONS_ScriptInterpreter::command_cell;
	this->commandList[L"cellcheckexbtn"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"cellcheckspbtn"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"checkpage"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"chvol"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"cl"]=&NONS_ScriptInterpreter::command_cl;
	this->commandList[L"click"]=&NONS_ScriptInterpreter::command_click;
	this->commandList[L"clickstr"]=&NONS_ScriptInterpreter::command_clickstr;
	this->commandList[L"clickvoice"]=0;
	this->commandList[L"cmp"]=&NONS_ScriptInterpreter::command_cmp;
	this->commandList[L"cos"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"csel"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"cselbtn"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"cselgoto"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"csp"]=&NONS_ScriptInterpreter::command_csp;
	this->commandList[L"date"]=&NONS_ScriptInterpreter::command_date;
	this->commandList[L"dec"]=&NONS_ScriptInterpreter::command_inc;
	this->commandList[L"defaultfont"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"defaultspeed"]=&NONS_ScriptInterpreter::command_defaultspeed;
	this->commandList[L"definereset"]=&NONS_ScriptInterpreter::command_reset;
	this->commandList[L"defmp3vol"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"defsevol"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"defsub"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"defvoicevol"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"delay"]=&NONS_ScriptInterpreter::command_delay;
	this->commandList[L"deletescreenshot"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"dim"]=&NONS_ScriptInterpreter::command_dim;
	this->commandList[L"div"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"draw"]=&NONS_ScriptInterpreter::command_draw;
	this->commandList[L"drawbg"]=&NONS_ScriptInterpreter::command_drawbg;
	this->commandList[L"drawbg2"]=&NONS_ScriptInterpreter::command_drawbg;
	this->commandList[L"drawclear"]=&NONS_ScriptInterpreter::command_drawclear;
	this->commandList[L"drawfill"]=&NONS_ScriptInterpreter::command_drawfill;
	this->commandList[L"drawsp"]=&NONS_ScriptInterpreter::command_drawsp;
	this->commandList[L"drawsp2"]=&NONS_ScriptInterpreter::command_drawsp;
	this->commandList[L"drawsp3"]=&NONS_ScriptInterpreter::command_drawsp;
	this->commandList[L"drawtext"]=&NONS_ScriptInterpreter::command_drawtext;
	this->commandList[L"dwave"]=&NONS_ScriptInterpreter::command_dwave;
	this->commandList[L"dwaveload"]=&NONS_ScriptInterpreter::command_dwaveload;
	this->commandList[L"dwaveloop"]=&NONS_ScriptInterpreter::command_dwave;
	this->commandList[L"dwaveplay"]=0;
	this->commandList[L"dwaveplayloop"]=0;
	this->commandList[L"dwavestop"]=0;
	this->commandList[L"effect"]=&NONS_ScriptInterpreter::command_effect;
	this->commandList[L"effectblank"]=&NONS_ScriptInterpreter::command_effectblank;
	this->commandList[L"effectcut"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"end"]=&NONS_ScriptInterpreter::command_end;
	this->commandList[L"erasetextwindow"]=&NONS_ScriptInterpreter::command_erasetextwindow;
	this->commandList[L"exbtn_d"]=0;
	this->commandList[L"exbtn"]=0;
	this->commandList[L"exec_dll"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"existspbtn"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"fileexist"]=&NONS_ScriptInterpreter::command_fileexist;
	this->commandList[L"filelog"]=&NONS_ScriptInterpreter::command_filelog;
	this->commandList[L"for"]=&NONS_ScriptInterpreter::command_for;
	this->commandList[L"game"]=&NONS_ScriptInterpreter::command_game;
	this->commandList[L"getbgmvol"]=&NONS_ScriptInterpreter::command_getmp3vol;
	this->commandList[L"getbtntimer"]=&NONS_ScriptInterpreter::command_getbtntimer;
	this->commandList[L"getcselnum"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getcselstr"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getcursor"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getcursorpos"]=&NONS_ScriptInterpreter::command_getcursorpos;
	this->commandList[L"getenter"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getfunction"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getinsert"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getlog"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getmousepos"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getmp3vol"]=&NONS_ScriptInterpreter::command_getmp3vol;
	this->commandList[L"getpage"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getpageup"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getparam"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getreg"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"getret"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getscreenshot"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getsevol"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getspmode"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getspsize"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"gettab"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"gettag"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"gettext"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"gettimer"]=&NONS_ScriptInterpreter::command_gettimer;
	this->commandList[L"getversion"]=&NONS_ScriptInterpreter::command_getversion;
	this->commandList[L"getvoicevol"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"getzxc"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"globalon"]=&NONS_ScriptInterpreter::command_globalon;
	this->commandList[L"gosub"]=&NONS_ScriptInterpreter::command_gosub;
	this->commandList[L"goto"]=&NONS_ScriptInterpreter::command_goto;
	this->commandList[L"humanorder"]=&NONS_ScriptInterpreter::command_humanorder;
	this->commandList[L"humanz"]=&NONS_ScriptInterpreter::command_humanz;
	this->commandList[L"if"]=&NONS_ScriptInterpreter::command_if;
	this->commandList[L"inc"]=&NONS_ScriptInterpreter::command_inc;
	this->commandList[L"indent"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"input"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"insertmenu"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"intlimit"]=&NONS_ScriptInterpreter::command_intlimit;
	this->commandList[L"isdown"]=&NONS_ScriptInterpreter::command_isdown;
	this->commandList[L"isfull"]=&NONS_ScriptInterpreter::command_isfull;
	this->commandList[L"ispage"]=&NONS_ScriptInterpreter::command_ispage;
	this->commandList[L"isskip"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"itoa"]=&NONS_ScriptInterpreter::command_itoa;
	this->commandList[L"itoa2"]=&NONS_ScriptInterpreter::command_itoa;
	this->commandList[L"jumpb"]=&NONS_ScriptInterpreter::command_jumpf;
	this->commandList[L"jumpf"]=&NONS_ScriptInterpreter::command_jumpf;
	this->commandList[L"kidokumode"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"kidokuskip"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"labellog"]=&NONS_ScriptInterpreter::command_labellog;
	this->commandList[L"layermessage"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"ld"]=&NONS_ScriptInterpreter::command_ld;
	this->commandList[L"len"]=&NONS_ScriptInterpreter::command_len;
	this->commandList[L"linepage"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"linepage2"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"loadgame"]=&NONS_ScriptInterpreter::command_loadgame;
	this->commandList[L"loadgosub"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"locate"]=&NONS_ScriptInterpreter::command_locate;
	this->commandList[L"logsp"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"logsp2"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"lookbackbutton"]=&NONS_ScriptInterpreter::command_lookbackbutton;
	this->commandList[L"lookbackcolor"]=&NONS_ScriptInterpreter::command_lookbackcolor;
	this->commandList[L"lookbackflush"]=&NONS_ScriptInterpreter::command_lookbackflush;
	this->commandList[L"lookbacksp"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"loopbgm"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"loopbgmstop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"lr_trap"]=&NONS_ScriptInterpreter::command_trap;
	this->commandList[L"lr_trap2"]=&NONS_ScriptInterpreter::command_trap;
	this->commandList[L"lsp"]=&NONS_ScriptInterpreter::command_lsp;
	//this->commandList[L"lsp2"]=&NONS_ScriptInterpreter::command_lsp2;
	this->commandList[L"lsph"]=&NONS_ScriptInterpreter::command_lsp;
	this->commandList[L"maxkaisoupage"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"menu_automode"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"menu_full"]=&NONS_ScriptInterpreter::command_menu_full;
	this->commandList[L"menu_window"]=&NONS_ScriptInterpreter::command_menu_full;
	this->commandList[L"menuselectcolor"]=&NONS_ScriptInterpreter::command_menuselectcolor;
	this->commandList[L"menuselectvoice"]=&NONS_ScriptInterpreter::command_menuselectvoice;
	this->commandList[L"menusetwindow"]=&NONS_ScriptInterpreter::command_menusetwindow;
	this->commandList[L"mid"]=&NONS_ScriptInterpreter::command_mid;
	this->commandList[L"mod"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"mode_ext"]=0;
	this->commandList[L"mode_saya"]=0;
	this->commandList[L"monocro"]=&NONS_ScriptInterpreter::command_monocro;
	this->commandList[L"mov"]=&NONS_ScriptInterpreter::command_mov;
	this->commandList[L"mov3"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov4"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov5"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov6"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov7"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov8"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov9"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"mov10"]=&NONS_ScriptInterpreter::command_movN;
	this->commandList[L"movemousecursor"]=0;
	this->commandList[L"movl"]=&NONS_ScriptInterpreter::command_movl;
	this->commandList[L"mp3"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"mp3fadeout"]=&NONS_ScriptInterpreter::command_mp3fadeout;
	this->commandList[L"mp3loop"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"mp3save"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"mp3stop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"mp3vol"]=&NONS_ScriptInterpreter::command_mp3vol;
	this->commandList[L"mpegplay"]=&NONS_ScriptInterpreter::command_mpegplay;
	this->commandList[L"msp"]=&NONS_ScriptInterpreter::command_msp;
	this->commandList[L"mul"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"nega"]=&NONS_ScriptInterpreter::command_nega;
	this->commandList[L"next"]=&NONS_ScriptInterpreter::command_next;
	this->commandList[L"notif"]=&NONS_ScriptInterpreter::command_if;
	this->commandList[L"nsa"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"ns2"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"ns3"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"nsadir"]=&NONS_ScriptInterpreter::command_nsadir;
	this->commandList[L"numalias"]=&NONS_ScriptInterpreter::command_alias;
	this->commandList[L"ofscopy"]=&NONS_ScriptInterpreter::command_ofscopy;
	this->commandList[L"ofscpy"]=&NONS_ScriptInterpreter::command_ofscopy;
	this->commandList[L"play"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"playonce"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"playstop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"pretextgosub"]=&NONS_ScriptInterpreter::command_undocumented;
	this->commandList[L"print"]=&NONS_ScriptInterpreter::command_print;
	this->commandList[L"prnum"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"prnumclear"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"puttext"]=&NONS_ScriptInterpreter::command_literal_print;
	this->commandList[L"quake"]=&NONS_ScriptInterpreter::command_quake;
	this->commandList[L"quakex"]=&NONS_ScriptInterpreter::command_sinusoidal_quake;
	this->commandList[L"quakey"]=&NONS_ScriptInterpreter::command_sinusoidal_quake;
	this->commandList[L"repaint"]=&NONS_ScriptInterpreter::command_repaint;
	this->commandList[L"reset"]=&NONS_ScriptInterpreter::command_reset;
	this->commandList[L"resetmenu"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"resettimer"]=&NONS_ScriptInterpreter::command_resettimer;
	this->commandList[L"return"]=&NONS_ScriptInterpreter::command_return;
	this->commandList[L"rmenu"]=&NONS_ScriptInterpreter::command_rmenu;
	this->commandList[L"rmode"]=&NONS_ScriptInterpreter::command_rmode;
	this->commandList[L"rnd"]=&NONS_ScriptInterpreter::command_rnd;
	this->commandList[L"rnd2"]=&NONS_ScriptInterpreter::command_rnd;
	this->commandList[L"roff"]=&NONS_ScriptInterpreter::command_rmode;
	this->commandList[L"rubyoff"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"rubyon"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"sar"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"savefileexist"]=&NONS_ScriptInterpreter::command_savefileexist;
	this->commandList[L"savegame"]=&NONS_ScriptInterpreter::command_savegame;
	this->commandList[L"savename"]=&NONS_ScriptInterpreter::command_savename;
	this->commandList[L"savenumber"]=&NONS_ScriptInterpreter::command_savenumber;
	this->commandList[L"saveoff"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"saveon"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"savescreenshot"]=&NONS_ScriptInterpreter::command_savescreenshot;
	this->commandList[L"savescreenshot2"]=&NONS_ScriptInterpreter::command_savescreenshot;
	this->commandList[L"savetime"]=&NONS_ScriptInterpreter::command_savetime;
	this->commandList[L"savetime2"]=&NONS_ScriptInterpreter::command_savetime2;
	this->commandList[L"select"]=&NONS_ScriptInterpreter::command_select;
	this->commandList[L"selectbtnwait"]=0;
	this->commandList[L"selectcolor"]=&NONS_ScriptInterpreter::command_selectcolor;
	this->commandList[L"selectvoice"]=&NONS_ScriptInterpreter::command_selectvoice;
	this->commandList[L"selgosub"]=&NONS_ScriptInterpreter::command_select;
	this->commandList[L"selnum"]=&NONS_ScriptInterpreter::command_select;
	this->commandList[L"setcursor"]=&NONS_ScriptInterpreter::command_setcursor;
	this->commandList[L"setlayer"]=0;
	this->commandList[L"setwindow"]=&NONS_ScriptInterpreter::command_setwindow;
	this->commandList[L"setwindow2"]=0;
	this->commandList[L"setwindow3"]=0;
	this->commandList[L"sevol"]=0;
	this->commandList[L"shadedistance"]=0;
	this->commandList[L"sin"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"skip"]=&NONS_ScriptInterpreter::command_skip;
	this->commandList[L"skipoff"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"soundpressplgin"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"sp_rgb_gradation"]=0;
	this->commandList[L"spbtn"]=0;
	this->commandList[L"spclclk"]=0;
	this->commandList[L"spi"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"split"]=&NONS_ScriptInterpreter::command_split;
	this->commandList[L"splitstring"]=0;
	this->commandList[L"spreload"]=0;
	this->commandList[L"spstr"]=0;
	this->commandList[L"stop"]=&NONS_ScriptInterpreter::command_stop;
	this->commandList[L"stralias"]=&NONS_ScriptInterpreter::command_alias;
	this->commandList[L"strsp"]=0;
	this->commandList[L"sub"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"systemcall"]=&NONS_ScriptInterpreter::command_systemcall;
	this->commandList[L"tablegoto"]=0;
	this->commandList[L"tal"]=&NONS_ScriptInterpreter::command_tal;
	this->commandList[L"tan"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"tateyoko"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"texec"]=0;
	this->commandList[L"textbtnwait"]=0;
	this->commandList[L"textclear"]=&NONS_ScriptInterpreter::command_textclear;
	this->commandList[L"textgosub"]=&NONS_ScriptInterpreter::command_textgosub;
	this->commandList[L"texthide"]=0;
	this->commandList[L"textoff"]=&NONS_ScriptInterpreter::command_textonoff;
	this->commandList[L"texton"]=&NONS_ScriptInterpreter::command_textonoff;
	this->commandList[L"textshow"]=0;
	this->commandList[L"textspeed"]=&NONS_ScriptInterpreter::command_textspeed;
	this->commandList[L"time"]=&NONS_ScriptInterpreter::command_time;
	this->commandList[L"transmode"]=0;
	this->commandList[L"trap"]=&NONS_ScriptInterpreter::command_trap;
	this->commandList[L"trap2"]=&NONS_ScriptInterpreter::command_trap;
	this->commandList[L"underline"]=0;
	this->commandList[L"useescspc"]=0;
	this->commandList[L"usewheel"]=0;
	this->commandList[L"versionstr"]=&NONS_ScriptInterpreter::command_versionstr;
	this->commandList[L"voicevol"]=0;
	this->commandList[L"vsp"]=&NONS_ScriptInterpreter::command_vsp;
	this->commandList[L"wait"]=&NONS_ScriptInterpreter::command_wait;
	this->commandList[L"waittimer"]=&NONS_ScriptInterpreter::command_waittimer;
	this->commandList[L"wave"]=&NONS_ScriptInterpreter::command_wave;
	this->commandList[L"waveloop"]=&NONS_ScriptInterpreter::command_wave;
	this->commandList[L"wavestop"]=&NONS_ScriptInterpreter::command_wavestop;
	this->commandList[L"windowback"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"windoweffect"]=&NONS_ScriptInterpreter::command_windoweffect;
	this->commandList[L"zenkakko"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"date2"]=&NONS_ScriptInterpreter::command_date;
	this->commandList[L"getini"]=&NONS_ScriptInterpreter::command_getini;
	this->commandList[L"new_set_window"]=&NONS_ScriptInterpreter::command_new_set_window;
	this->commandList[L"set_default_font_size"]=&NONS_ScriptInterpreter::command_set_default_font_size;
	this->commandList[L"unalias"]=&NONS_ScriptInterpreter::command_unalias;
	this->commandList[L"literal_print"]=&NONS_ScriptInterpreter::command_literal_print;
	this->commandList[L"use_new_if"]=&NONS_ScriptInterpreter::command_use_new_if;
	this->commandList[L"centerh"]=&NONS_ScriptInterpreter::command_centerh;
	this->commandList[L"centerv"]=&NONS_ScriptInterpreter::command_centerv;
	this->commandList[L"killmenu"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"command_syntax_example"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"stdout"]=&NONS_ScriptInterpreter::command_stdout;
	this->commandList[L"stderr"]=&NONS_ScriptInterpreter::command_stdout;
	/*this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	*/
	ulong total=this->totalCommands(),
		implemented=this->implementedCommands();
	std::cout <<"ONSlaught script interpreter v"<<float(implemented*100/total)/100<<std::endl;
	command_list.clear();
	command_list.reserve(this->commandList.size());
	for (commandListType::iterator i=this->commandList.begin(),end=this->commandList.end();i!=end;i++)
		command_list.push_back(i->first);
}

void NONS_ScriptInterpreter::uninit(){
	if (this->store)
		delete this->store;
	for (INIcacheType::iterator i=this->INIcache.begin();i!=this->INIcache.end();i++)
		delete i->second;
	delete this->thread;
	this->INIcache.clear();
	delete this->arrowCursor;
	delete this->pageCursor;
	if (this->menu)
		delete this->menu;
	this->selectVoiceClick.clear();
	this->selectVoiceEntry.clear();
	this->selectVoiceMouseOver.clear();
	this->clickStr.clear();
	if (this->imageButtons)
		delete this->imageButtons;
	delete this->saveGame;
	if (config_directory.size()){
		std::wstring settings_filename=config_directory+L"settings.cfg";
		ConfigFile settings;
		settings.assignInt(L"textSpeedMode",this->current_speed_setting);
		settings.writeOut(settings_filename);
	}
	this->textgosub.clear();
}

NONS_ScriptInterpreter::~NONS_ScriptInterpreter(){
	InputObserver.detach(this->inputQueue);
	this->uninit();
	//delete this->main_font;
}

ulong NONS_ScriptInterpreter::totalCommands(){
	return this->commandList.size();
}

ulong NONS_ScriptInterpreter::implementedCommands(){
	ulong res=0;
	for (commandListType::iterator i=this->commandList.begin();i!=this->commandList.end();i++)
		if (i->second && i->second!=&NONS_ScriptInterpreter::command_undocumented)
			res++;
	return res;
}

#define INTERPRETNEXTLINE_HANDLEKEYS switch (event.type){\
	case SDL_KEYDOWN:\
		{\
			float def=this->default_speed,\
				cur=this->everything->screen->output->display_speed;\
			if (event.key.keysym.sym==SDLK_F5){\
				this->default_speed=this->default_speed_slow;\
				this->current_speed_setting=0;\
				this->everything->screen->output->display_speed=cur/def*float(this->default_speed);\
			}else if (event.key.keysym.sym==SDLK_F6){\
				this->default_speed=this->default_speed_med;\
				this->current_speed_setting=1;\
				this->everything->screen->output->display_speed=cur/def*float(this->default_speed);\
			}else if (event.key.keysym.sym==SDLK_F7){\
				this->default_speed=this->default_speed_fast;\
				this->current_speed_setting=2;\
				this->everything->screen->output->display_speed=cur/def*float(this->default_speed);\
			}\
			break;\
		}\
}

bool NONS_ScriptInterpreter::interpretNextLine(){
	if (trapFlag){
		if (!CURRENTLYSKIPPING || (CURRENTLYSKIPPING && !(trapFlag>2))){
			bool end=0;
			while (!this->inputQueue->data.empty() && !end){
				SDL_Event event=this->inputQueue->pop();
				if (event.type==SDL_MOUSEBUTTONDOWN && (event.button.which=SDL_BUTTON_LEFT || !(trapFlag%2)))
					end=1;
				else{
					INTERPRETNEXTLINE_HANDLEKEYS
				}
			}
			if (end){
				this->thread->gotoLabel(this->trapLabel);
				this->thread->advanceToNextStatement();
				this->trapLabel.clear();
				trapFlag=0;
			}
		}
	}else{
		while (!this->inputQueue->data.empty()){
			SDL_Event event=this->inputQueue->pop();
			INTERPRETNEXTLINE_HANDLEKEYS
		}
	}

	NONS_Statement *stmt=this->thread->getCurrentStatement();
	if (!stmt)
		return 0;
	stmt->parse(this->everything->script);
	if (CLOptions.verbosity>=1)
		o_stderr <<"Interpreting line "<<stmt->lineOfOrigin->lineNumber<<"\n";
	if (CLOptions.verbosity>=3 && stmt->type==NONS_Statement::STATEMENT_COMMAND){
		o_stderr <<"\""<<stmt->commandName<<"\" ";
		if (stmt->parameters.size()){
			o_stderr <<"(\n";
			o_stderr.indent(1);
			for (ulong a=0;;a++){
				o_stderr <<"\""<<stmt->parameters[a]<<"\"";
				if (a==stmt->parameters.size()-1){
					o_stderr <<"\n";
					break;
				}
				o_stderr <<",\n";
			}
			o_stderr.indent(-1);
			o_stderr <<")\n";
		}else
			o_stderr <<"{NO PARAMETERS}\n";
	}
	this->saveGame->textX=this->everything->screen->output->x;
	this->saveGame->textY=this->everything->screen->output->y;
	switch (stmt->type){
		case NONS_Statement::STATEMENT_BLOCK:
			this->saveGame->currentLabel=stmt->commandName;
			if (!stdStrCmpCI(stmt->commandName,L"define"))
				this->interpreter_mode=DEFINE;
			break;
		case NONS_Statement::STATEMENT_JUMP:
		case NONS_Statement::STATEMENT_COMMENT:
			break;
		case NONS_Statement::STATEMENT_PRINTER:
			if (this->printed_lines.find(stmt->lineOfOrigin->lineNumber)==this->printed_lines.end()){
				//softwareCtrlIsPressed=0;
				this->printed_lines.insert(stmt->lineOfOrigin->lineNumber);
			}
			this->Printer(stmt->stmt);
			break;
		case NONS_Statement::STATEMENT_COMMAND:
			{
				commandListType::iterator i=this->commandList.find(stmt->commandName);
				if (i!=this->commandList.end()){
					commandFunctionPointer function=i->second;
					if (!function){
						if (this->implementationErrors.find(i->first)==this->implementationErrors.end()){
							o_stderr <<"NONS_ScriptInterpreter::interpretNextLine(): ";
							if (stmt->lineOfOrigin->lineNumber==ULONG_MAX)
								o_stderr <<"Error";
							else
								o_stderr <<"Error near line "<<stmt->lineOfOrigin->lineNumber;
							o_stderr <<". Command \""<<stmt->commandName<<"\" is not implemented yet.\n"
								"    Implementation errors are reported only once.\n";
							this->implementationErrors.insert(i->first);
						}
						if (CLOptions.stopOnFirstError)
							return 0;
						break;
					}
					ErrorCode error=(this->*function)(*stmt);
					bool there_was_an_error=!CHECK_FLAG(error,NONS_NO_ERROR_FLAG);
					if (there_was_an_error){
						o_stderr <<"{\n";
						o_stderr.indent(1);
						o_stderr <<"Line "<<stmt->lineOfOrigin->lineNumber<<": \n"
							"\""<<stmt->commandName<<"\" ";
						if (stmt->parameters.size()){
							o_stderr <<"(\n";
							o_stderr.indent(1);
							for (ulong a=0;;a++){
								o_stderr <<"\""<<stmt->parameters[a]<<"\"";
								if (a==stmt->parameters.size()-1){
									o_stderr <<"\n";
									break;
								}
								o_stderr <<",\n";
							}
							o_stderr.indent(-1);
							o_stderr <<")\n";
						}else
							o_stderr <<"{NO PARAMETERS}\n";
					}
					handleErrors(error,stmt->lineOfOrigin->lineNumber,"NONS_ScriptInterpreter::interpretNextLine",0);
					if (there_was_an_error){
						o_stderr.indent(-1);
						o_stderr <<"}\n";
					}
					if (CLOptions.stopOnFirstError && error!=NONS_UNIMPLEMENTED_COMMAND || error==NONS_END)
						return 0;
				}else{
					o_stderr <<"NONS_ScriptInterpreter::interpretNextLine(): ";
					if (stmt->lineOfOrigin->lineNumber==ULONG_MAX)
						o_stderr <<"Error";
					else
						o_stderr <<"Error near line "<<stmt->lineOfOrigin->lineNumber;
					o_stderr <<". Command \""<<stmt->commandName<<"\" could not be recognized.\n";
					if (CLOptions.stopOnFirstError)
						return 0;
				}
			}
			break;
		default:;
	}
	if (!this->thread->advanceToNextStatement()){
		this->command_end(*stmt);
		return 0;
	}
	return 1;
}

ErrorCode NONS_ScriptInterpreter::interpretString(const std::wstring &str,NONS_ScriptLine *line,ulong offset){
	NONS_ScriptLine l(0,str,0,1);
	ErrorCode ret=NONS_NO_ERROR;
	for (ulong a=0;a<l.statements.size();a++){
		ErrorCode error=this->interpretString(*l.statements[a],line,offset);
		if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG))
			ret=NONS_UNDEFINED_ERROR;
	}
	return ret;
}

ErrorCode NONS_ScriptInterpreter::interpretString(NONS_Statement &stmt,NONS_ScriptLine *line,ulong offset){
	stmt.parse(this->everything->script);
	stmt.lineOfOrigin=line;
	stmt.fileOffset=offset;
	if (CLOptions.verbosity>=3 && stmt.type==NONS_Statement::STATEMENT_COMMAND){
		o_stderr <<"String: \""<<stmt.commandName<<"\" ";
		if (stmt.parameters.size()){
			o_stderr <<"{\n";
			for (ulong a=0;;a++){
				o_stderr <<"    \""<<stmt.parameters[a]<<"\"";
				if (a==stmt.parameters.size()-1){
					o_stderr <<"\n";
					break;
				}
				o_stderr <<",\n";
			}
			o_stderr <<"}\n";
		}else
			o_stderr <<"{NO PARAMETERS}\n";
	}
	switch (stmt.type){
		case NONS_Statement::STATEMENT_COMMENT:
			break;
		case NONS_Statement::STATEMENT_PRINTER:
			if (!stmt.lineOfOrigin || this->printed_lines.find(stmt.lineOfOrigin->lineNumber)==this->printed_lines.end()){
				//softwareCtrlIsPressed=0;
				if (!!stmt.lineOfOrigin)
					this->printed_lines.insert(stmt.lineOfOrigin->lineNumber);
			}
			this->Printer(stmt.commandName);
			break;
		case NONS_Statement::STATEMENT_COMMAND:
			{
				commandListType::iterator i=this->commandList.find(stmt.commandName);
				if (i!=(this->commandList.end())){
					commandFunctionPointer function=i->second;
					if (!function){
						if (this->implementationErrors.find(i->first)!=this->implementationErrors.end()){
							o_stderr <<"NONS_ScriptInterpreter::interpretNextLine(): "
								"Error. Command \""<<stmt.commandName<<"\" is not implemented.\n"
								"    Implementation errors are reported only once.\n";
							this->implementationErrors.insert(i->first);
						}
						return NONS_NOT_IMPLEMENTED;
					}
					return handleErrors((this->*function)(stmt),(!!stmt.lineOfOrigin)?stmt.lineOfOrigin->lineNumber:0,"NONS_ScriptInterpreter::interpretString",1);
				}else{
					o_stderr <<"NONS_ScriptInterpreter::interpretString(): "
						"Error. Command \""<<stmt.commandName<<"\" could not be recognized.\n";
					return NONS_UNRECOGNIZED_COMMAND;
				}
			}
			break;
		default:;
	}
	return NONS_NO_ERROR;
}

std::wstring insertIntoString(const std::wstring &dst,ulong from,ulong l,const std::wstring &src){
	return dst.substr(0,from)+src+dst.substr(from+l);
}

std::wstring insertIntoString(const std::wstring &dst,ulong from,ulong l,long src){
	std::wstringstream temp;
	temp <<src;
	return insertIntoString(dst,from,l,temp.str());
}

std::wstring getInlineExpression(const std::wstring &string,ulong off,ulong *len){
	ulong l=off;
	while (multicomparison(string[l],"_+-*/|&%$?<>=()[]\"`") || NONS_isalnum(string[l])){
		if (string[l]=='\"' || string[l]=='`'){
			wchar_t quote=string[l];
			ulong l2=l+1;
			for (;l2<string.size() && string[l2]!=quote;l2++);
			if (string[l2]!=quote)
				break;
			else
				l=l2;
		}
		l++;
	}
	if (!!len)
		*len=l-off;
	return std::wstring(string,off,l-off);
}

void NONS_ScriptInterpreter::reduceString(
		const std::wstring &src,
		std::wstring &dst,
		std::set<NONS_VariableMember *> *visited,
		std::vector<std::pair<std::wstring,NONS_VariableMember *> > *stack){
	for (ulong off=0;off<src.size();){
		switch (src[off]){
			case '!':
				if (src.find(L"!nl",off)==off){
					dst.push_back('\n');
					off+=3;
					break;
				}
			case '%':
			case '$':
			case '?':
				{
					ulong l;
					std::wstring expr=getInlineExpression(src,off,&l);
					if (expr.size()){
						NONS_VariableMember *var=this->store->retrieve(expr,0);
						if (!!var){
							off+=l;
							if (var->getType()==INTEGER){
								std::wstringstream stream;
								stream <<var->getInt();
								dst.append(stream.str());
							}else if (var->getType()==STRING){
								std::wstring copy=var->getWcs();
								if (!!visited && visited->find(var)!=visited->end()){
									o_stderr <<"NONS_ScriptInterpreter::reduceString(): WARNING: Infinite recursion avoided.\n"
										"    Reduction stack contents:\n";
									for (std::vector<std::pair<std::wstring,NONS_VariableMember *> >::iterator i=stack->begin();i!=stack->end();i++)
										o_stderr <<"        ["<<i->first<<"] = \""<<i->second->getWcs()<<"\"\n";
									o_stderr <<" (last) ["<<expr<<"] = \""<<copy<<"\"\n";
									dst.append(copy);
								}else{
									std::set<NONS_VariableMember *> *temp_visited;
									std::vector<std::pair<std::wstring,NONS_VariableMember *> > *temp_stack;
									if (!visited)
										temp_visited=new std::set<NONS_VariableMember *>;
									else
										temp_visited=visited;
									temp_visited->insert(var);
									if (!stack)
										temp_stack=new std::vector<std::pair<std::wstring,NONS_VariableMember *> >;
									else
										temp_stack=stack;
									temp_stack->push_back(std::pair<std::wstring,NONS_VariableMember *>(expr,var));
									reduceString(copy,dst,temp_visited,temp_stack);
									if (!visited)
										delete temp_visited;
									else
										temp_visited->erase(var);
									if (!stack)
										delete temp_stack;
									else
										temp_stack->pop_back();
								}
							}
							break;
						}
					}
				}
			default:
				dst.push_back(src[off]);
				off++;
		}
	}
}

void findStops(const std::wstring &src,std::vector<std::pair<ulong,ulong> > &stopping_points,std::wstring &dst){
	dst.clear();
	for (ulong a=0,size=src.size();a<size;a++){
		switch (src[a]){
			case '\\':
			case '@':
				{
					std::pair<ulong,ulong> push(dst.size(),a);
					stopping_points.push_back(push);
					continue;
				}
			case '!':
				if (firstcharsCI(src,a,L"!sd")){
					std::pair<ulong,ulong> push(dst.size(),a);
					stopping_points.push_back(push);
					a+=2;
					continue;
				}else if (firstcharsCI(src,a,L"!s") || firstcharsCI(src,a,L"!d") || firstcharsCI(src,a,L"!w")){
					std::pair<ulong,ulong> push(dst.size(),a);
					stopping_points.push_back(push);
					ulong l=2;
					for (;isdigit(src[a+l]);l++);
					a+=l-1;
					continue;
				}
			case '#':
				if (src[a]=='#'){
					if (src.size()-a-1>=6){
						a++;
						short b;
						for (b=0;b<6 && NONS_ishexa(src[b]);b++);
						if (b!=6)
							a--;
						else{
							std::pair<ulong,ulong> push(dst.size(),a);
							stopping_points.push_back(push);
							a+=5;
							continue;
						}
					}
				}
			default:
				dst.push_back(src[a]);
		}
		if (src[a]=='\\')
			break;
	}
	std::pair<ulong,ulong> push(dst.size(),src.size());
	stopping_points.push_back(push);
}

bool NONS_ScriptInterpreter::Printer_support(std::vector<printingPage> &pages,ulong *totalprintedchars,bool *justTurnedPage,ErrorCode *error){
	NONS_StandardOutput *out=this->everything->screen->output;
	this->everything->screen->showText();
	std::wstring *str;
	bool justClicked;
	for (std::vector<printingPage>::iterator i=pages.begin();i!=pages.end();i++){
		bool clearscr=out->prepareForPrinting(i->print.c_str());
		if (clearscr){
			if (this->pageCursor->animate(this->menu,this->autoclick)<0){
				if (!!error)
					*error=NONS_NO_ERROR;
				return 1;
			}
			this->everything->screen->clearText();
		}
		str=&i->reduced;
		for (ulong reduced=0,printed=0,stop=0;stop<i->stops.size();stop++){
			ulong printedChars=0;
			while (justClicked=out->print(printed,i->stops[stop].first,this->everything->screen->screen,&printedChars)){
				if (this->pageCursor->animate(this->menu,this->autoclick)<0){
					if (!!error)
						*error=NONS_NO_ERROR;
					return 1;
				}
				this->everything->screen->clearText();
				justClicked=1;
			}
			if (printedChars>0){
				if (!!totalprintedchars)
					(*totalprintedchars)+=printedChars;
				if (!!justTurnedPage)
					*justTurnedPage=0;
				justClicked=0;
			}
			reduced=i->stops[stop].second;
			switch ((*str)[reduced]){
				case '\\':
					if (this->textgosub.size() && (this->textgosubRecurses || !this->insideTextgosub())){
						std::vector<printingPage>::iterator i2=i;
						std::vector<printingPage> temp;
						printingPage temp2(*i2);
						temp2.print.erase(temp2.print.begin(),temp2.print.begin()+temp2.stops[stop].first);
						temp2.reduced.erase(temp2.reduced.begin(),temp2.reduced.begin()+temp2.stops[stop].second+1);
						std::pair<ulong,ulong> takeOut;
						takeOut.first=temp2.stops[stop].first+1;
						takeOut.second=temp2.stops[stop].second+1;
						temp2.stops.erase(temp2.stops.begin(),temp2.stops.begin()+stop+1);
						for (std::vector<std::pair<ulong,ulong> >::iterator i=temp2.stops.begin();i<temp2.stops.end();i++){
							i->first-=takeOut.first;
							i->second-=takeOut.second;
						}
						temp.push_back(temp2);
						for (i2++;i2!=pages.end();i2++)
							temp.push_back(*i2);
						NONS_StackElement *pusher=new NONS_StackElement(temp,'\\',this->insideTextgosub()+1);
						this->callStack.push_back(pusher);
						this->goto_label(this->textgosub);
						out->endPrinting();
						if (!!error)
							*error=NONS_NO_ERROR;
						return 1;
					}else if (!justClicked && this->pageCursor->animate(this->menu,this->autoclick)<0){
						if (!!error)
							*error=NONS_NO_ERROR;
						return 1;
					}
					out->endPrinting();
					this->everything->screen->clearText();
					reduced++;
					if (!!justTurnedPage)
						*justTurnedPage=1;
					break;
				case '@':
					if (this->textgosub.size() && (this->textgosubRecurses || !this->insideTextgosub())){
						std::vector<printingPage>::iterator i2=i;
						std::vector<printingPage> temp;
						printingPage temp2(*i2);
						temp2.print.erase(temp2.print.begin(),temp2.print.begin()+temp2.stops[stop].first);
						temp2.reduced.erase(temp2.reduced.begin(),temp2.reduced.begin()+temp2.stops[stop].second+1);
						std::pair<ulong,ulong> takeOut;
						takeOut.first=temp2.stops[stop].first;
						takeOut.second=temp2.stops[stop].second+1;
						temp2.stops.erase(temp2.stops.begin(),temp2.stops.begin()+stop+1);
						for (std::vector<std::pair<ulong,ulong> >::iterator i=temp2.stops.begin();i<temp2.stops.end();i++){
							i->first-=takeOut.first;
							i->second-=takeOut.second;
						}
						temp.push_back(temp2);
						for (i2++;i2!=pages.end();i2++)
							temp.push_back(*i2);
						NONS_StackElement *pusher=new NONS_StackElement(temp,'@',this->insideTextgosub()+1);
						this->callStack.push_back(pusher);
						this->goto_label(this->textgosub);
						out->endPrinting();
						if (!!error)
							*error=NONS_NO_ERROR;
						return 1;
					}else if (!justClicked && this->arrowCursor->animate(this->menu,this->autoclick)<0){
						if (!!error)
							*error=NONS_NO_ERROR;
						return 1;
					}
					reduced++;
					break;
				case '!':
					if (firstcharsCI(*str,reduced,L"!sd")){
						out->display_speed=this->default_speed;
						reduced+=3;
						break;
					}else{
						bool notess=firstcharsCI(*str,reduced,L"!s"),
							notdee=firstcharsCI(*str,reduced,L"!d"),
							notdu=firstcharsCI(*str,reduced,L"!w");
						if (notess || notdee || notdu){
							reduced+=2;
							ulong l=0;
							for (;NONS_isdigit((*str)[reduced+l]);l++);
							if (l>0){
								long s=0;
								{
									std::wstringstream stream(str->substr(reduced,l));
									stream >>s;
								}
								if (notess){
									switch (this->current_speed_setting){
										case 0:
											this->everything->screen->output->display_speed=s*2;
											break;
										case 1:
											this->everything->screen->output->display_speed=s;
											break;
										case 2:
											this->everything->screen->output->display_speed=s/2;
									}
								}else if (notdee)
									waitCancellable(s);
								else
									waitNonCancellable(s);
								reduced+=l;
								break;
							}else
								reduced-=2;
						}
					}
				case '#':
					if ((*str)[reduced]=='#'){
						ulong len=str->size()-reduced-1;
						if (len>=6){
							reduced++;
							Uint32 parsed=0;
							short a;
							for (a=0;a<6;a++){
								int hex=(*str)[reduced+a];
								if (!NONS_ishexa(hex))
									break;
								parsed<<=4;
								parsed|=HEX2DEC(hex);
							}
							if (a==6){
								SDL_Color color=this->everything->screen->output->foregroundLayer->fontCache->foreground;
								color.r=parsed>>16;
								color.g=(parsed&0xFF00)>>8;
								color.b=(parsed&0xFF);
								this->everything->screen->output->foregroundLayer->fontCache->foreground=color;
								reduced+=6;
								break;
							}
							reduced--;
						}
					}
			}
			printed=i->stops[stop].first;
			justClicked=0;
		}
		out->endPrinting();
	}
	return 0;
}

ErrorCode NONS_ScriptInterpreter::Printer(const std::wstring &line){
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	this->currentBuffer=this->everything->screen->output->currentBuffer;
	NONS_StandardOutput *out=this->everything->screen->output;
	if (!line.size()){
		if (out->NewLine()){
			if (this->pageCursor->animate(this->menu,this->autoclick)<0)
				return NONS_NO_ERROR;
			out->NewLine();
		}
		return NONS_NO_ERROR;
	}
	bool skip=line[0]=='`';
	std::wstring str=line.substr(skip);
	bool justTurnedPage=0;
	std::wstring reducedString;
	reduceString(str,reducedString);
	std::vector<printingPage> pages;
	ulong totalprintedchars=0;
	for (ulong a=0;a<reducedString.size();){
		ulong p=reducedString.find('\\',a);
		if (p==reducedString.npos)
			p=reducedString.size();
		else
			p++;
		std::wstring str3(reducedString.begin()+a,reducedString.begin()+p);
		a=p;
		pages.push_back(printingPage());
		printingPage &page=pages.back();
		page.reduced=str3;
		findStops(page.reduced,page.stops,page.print);
	}
	ErrorCode error;
	if (this->Printer_support(pages,&totalprintedchars,&justTurnedPage,&error))
		return error;
	if (!justTurnedPage && totalprintedchars && !this->insideTextgosub() && out->NewLine() &&
			this->pageCursor->animate(this->menu,this->autoclick)>=0){
		this->everything->screen->clearText();
		//out->NewLine();
	}
	return NONS_NO_ERROR;
}

std::wstring NONS_ScriptInterpreter::convertParametersToString(NONS_Statement &stmt){
	std::wstring string;
	for (ulong a=0;a<stmt.parameters.size();a++){
		long res;
		std::wstring &str=stmt.parameters[a];
		if (CHECK_FLAG(this->store->getIntValue(str,res),NONS_NO_ERROR_FLAG)){
			string.append(itoa<wchar_t>(res));
		}else{
			std::wstring resString;
			if (CHECK_FLAG(this->store->getWcsValue(str,resString),NONS_NO_ERROR_FLAG))
				string.append(resString);
			else
				continue;
		}
	}
	return string;
}

ulong NONS_ScriptInterpreter::insideTextgosub(){
	return (this->callStack.size() && this->callStack.back()->textgosubLevel)?this->callStack.back()->textgosubLevel:0;
}

bool NONS_ScriptInterpreter::goto_label(const std::wstring &label){
	if (!this->thread->gotoLabel(label))
		return 0;
	labellog.addString(label);
	return 1;
}

bool NONS_ScriptInterpreter::gosub_label(const std::wstring &label){
	NONS_StackElement *el=new NONS_StackElement(this->thread->getNextStatementPair(),NONS_ScriptLine(),0,this->insideTextgosub());
	if (!this->goto_label(label)){
		delete el;
		return 0;
	}
	this->callStack.push_back(el);
	return 1;
}
#endif
