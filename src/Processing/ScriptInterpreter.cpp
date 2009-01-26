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

#ifndef NONS_SCRIPTINTERPRETER_CPP
#define NONS_SCRIPTINTERPRETER_CPP

#include "ScriptInterpreter.h"
#include "../Functions.h"
#include "../Globals.h"
#include "../IO_System/FileIO.h"
#include "../IO_System/IOFunctions.h"
//#include "../UTF.h"

NONS_StackElement::NONS_StackElement(){
	this->type=UNDEFINED;
	this->offset=0;
	this->first_interpret_string=0;
	this->var=0;
	this->from=0;
	this->to=0;
	this->step=0;
	this->end=0;
}

NONS_StackElement::NONS_StackElement(ulong offset,wchar_t *string){
	this->type=SUBROUTINE_CALL;
	this->offset=offset;
	this->first_interpret_string=copyWString(string);
}

NONS_StackElement::NONS_StackElement(NONS_Variable *variable,ulong startoffset,long from,long to,long step){
	this->type=FOR_NEST;
	this->var=variable;
	this->offset=startoffset;
	this->from=from;
	this->to=to;
	this->step=step;
	this->end=this->offset;
	this->first_interpret_string=0;
}

NONS_StackElement::~NONS_StackElement(){
	if (this->first_interpret_string)
		delete[] this->first_interpret_string;
}

void NONS_ScriptInterpreter::init(){
	this->script=everything->script;
	this->interpreter_position=0;
	this->store=new NONS_VariableStore();
	this->interpreter_mode=DEFINE;
	this->nsadir=copyString("./");
	this->language_extensions=0;
	this->default_speed=0;
	this->default_speed_slow=0;
	this->default_speed_med=0;
	this->default_speed_fast=0;

	char *settings_filename=addStrings(this->NONS_config_dir,"settings.cfg");
	ConfigFile settings(settings_filename);
	if (settings.exists(L"textSpeedMode"))
		this->current_speed_setting=settings.getInt(L"textSpeedMode");
	else
		this->current_speed_setting=1;
	delete[] settings_filename;

	srand(time(0));
	this->defaultx=640;
	this->defaulty=480;
	this->defaultfs=18;
	this->legacy_set_window=1;
	this->arrowCursor=new NONS_Cursor();
	this->pageCursor=new NONS_Cursor();
	this->gfx_store=this->everything->screen->gfx_store;
	this->hideTextDuringEffect=1;
	this->selectOn.r=0xFF;
	this->selectOn.g=0xFF;
	this->selectOn.b=0xFF;
	this->selectOff.r=0xA9;
	this->selectOff.g=0xA9;
	this->selectOff.b=0xA9;
	this->selectVoiceEntry=0;
	this->selectVoiceMouseOver=0;
	this->selectVoiceClick=0;
	this->inputQueue=InputObserver.attach();
	this->clickStr=0;
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
	this->errored_lines.clear();
	this->printed_lines.clear();
}

NONS_ScriptInterpreter::NONS_ScriptInterpreter(NONS_Everything *everything){
	this->NONS_config_dir=getConfigLocation();
	this->nsadir=0;
	this->arrowCursor=0;
	this->pageCursor=0;
	this->selectVoiceEntry=0;
	this->selectVoiceClick=0;
	this->inputQueue=0;
	this->clickStr=0;
	this->menu=0;
	this->imageButtons=0;
	this->saveGame=0;
	this->script=0;
	this->store=0;
	this->gfx_store=0;
	this->everything=0;
	this->main_font=0;
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
	this->commandList[L"allsphide"]=0;
	this->commandList[L"allspresume"]=0;
	this->commandList[L"amsp"]=&NONS_ScriptInterpreter::command_msp;
	this->commandList[L"arc"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"atoi"]=&NONS_ScriptInterpreter::command_atoi;
	this->commandList[L"autoclick"]=&NONS_ScriptInterpreter::command_autoclick;
	this->commandList[L"automode_time"]=0;
	this->commandList[L"automode"]=0;
	this->commandList[L"avi"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"bar"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"barclear"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"bg"]=&NONS_ScriptInterpreter::command_bg;
	this->commandList[L"bgcopy"]=0;
	this->commandList[L"bgcpy"]=0;
	this->commandList[L"bgm"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"bgmonce"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"bgmstop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"bgmvol"]=&NONS_ScriptInterpreter::command_mp3vol;
	this->commandList[L"blt"]=0;
	this->commandList[L"br"]=&NONS_ScriptInterpreter::command_br;
	this->commandList[L"break"]=&NONS_ScriptInterpreter::command_break;
	this->commandList[L"btn"]=&NONS_ScriptInterpreter::command_btn;
	this->commandList[L"btndef"]=&NONS_ScriptInterpreter::command_btndef;
	this->commandList[L"btndown"]=0;
	this->commandList[L"btntime"]=&NONS_ScriptInterpreter::command_btntime;
	this->commandList[L"btntime2"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"btnwait"]=&NONS_ScriptInterpreter::command_btnwait;
	this->commandList[L"btnwait2"]=0;
	this->commandList[L"caption"]=&NONS_ScriptInterpreter::command_caption;
	this->commandList[L"cell"]=0;
	this->commandList[L"cellcheckexbtn"]=0;
	this->commandList[L"cellcheckspbtn"]=0;
	this->commandList[L"checkpage"]=0;
	this->commandList[L"chvol"]=0;
	this->commandList[L"cl"]=&NONS_ScriptInterpreter::command_cl;
	this->commandList[L"click"]=&NONS_ScriptInterpreter::command_click;
	this->commandList[L"clickstr"]=&NONS_ScriptInterpreter::command_clickstr;
	this->commandList[L"clickvoice"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"cmp"]=&NONS_ScriptInterpreter::command_cmp;
	this->commandList[L"cos"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"csel"]=0;
	this->commandList[L"cselbtn"]=0;
	this->commandList[L"cselgoto"]=0;
	this->commandList[L"csp"]=&NONS_ScriptInterpreter::command_csp;
	this->commandList[L"date"]=&NONS_ScriptInterpreter::command_date;
	this->commandList[L"dec"]=&NONS_ScriptInterpreter::command_inc;
	this->commandList[L"defaultfont"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"defaultspeed"]=&NONS_ScriptInterpreter::command_defaultspeed;
	this->commandList[L"definereset"]=0;
	this->commandList[L"defmp3vol"]=0;
	this->commandList[L"defsevol"]=0;
	this->commandList[L"defsub"]=0;
	this->commandList[L"defvoicevol"]=0;
	this->commandList[L"delay"]=&NONS_ScriptInterpreter::command_delay;
	this->commandList[L"dim"]=&NONS_ScriptInterpreter::command_dim;
	this->commandList[L"div"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"draw"]=0;
	this->commandList[L"drawbg"]=0;
	this->commandList[L"drawbg2"]=0;
	this->commandList[L"drawclear"]=0;
	this->commandList[L"drawfill"]=0;
	this->commandList[L"drawsp"]=0;
	this->commandList[L"drawsp2"]=0;
	this->commandList[L"drawsp3"]=0;
	this->commandList[L"drawtext"]=0;
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
	this->commandList[L"exec_dll"]=0;
	this->commandList[L"existspbtn"]=0;
	this->commandList[L"fileexist"]=0;
	this->commandList[L"filelog"]=&NONS_ScriptInterpreter::command_filelog;
	this->commandList[L"for"]=&NONS_ScriptInterpreter::command_for;
	this->commandList[L"game"]=&NONS_ScriptInterpreter::command_game;
	this->commandList[L"getbgmvol"]=&NONS_ScriptInterpreter::command_getmp3vol;
	this->commandList[L"getbtntimer"]=&NONS_ScriptInterpreter::command_getbtntimer;
	this->commandList[L"getcselnum"]=0;
	this->commandList[L"getcselstr"]=0;
	this->commandList[L"getcursor"]=0;
	this->commandList[L"getcursorpos"]=0;
	this->commandList[L"getenter"]=0;
	this->commandList[L"getfunction"]=0;
	this->commandList[L"getinsert"]=0;
	this->commandList[L"getlog"]=0;
	this->commandList[L"getmousepos"]=0;
	this->commandList[L"getmp3vol"]=&NONS_ScriptInterpreter::command_getmp3vol;
	this->commandList[L"getpage"]=0;
	this->commandList[L"getpageup"]=0;
	this->commandList[L"getparam"]=0;
	this->commandList[L"getreg"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"getret"]=0;
	this->commandList[L"getscreenshot"]=0;
	this->commandList[L"getsevol"]=0;
	this->commandList[L"getspmode"]=0;
	this->commandList[L"getspsize"]=0;
	this->commandList[L"gettab"]=0;
	this->commandList[L"gettag"]=0;
	this->commandList[L"gettext"]=0;
	this->commandList[L"gettimer"]=&NONS_ScriptInterpreter::command_gettimer;
	this->commandList[L"getversion"]=&NONS_ScriptInterpreter::command_getversion;
	this->commandList[L"getvoicevol"]=0;
	this->commandList[L"getzxc"]=0;
	this->commandList[L"globalon"]=&NONS_ScriptInterpreter::command_globalon;
	this->commandList[L"gosub"]=&NONS_ScriptInterpreter::command_gosub;
	this->commandList[L"goto"]=&NONS_ScriptInterpreter::command_goto;
	this->commandList[L"humanorder"]=0;
	this->commandList[L"humanz"]=&NONS_ScriptInterpreter::command_humanz;
	this->commandList[L"if"]=&NONS_ScriptInterpreter::command_if;
	this->commandList[L"inc"]=&NONS_ScriptInterpreter::command_inc;
	this->commandList[L"indent"]=0;
	this->commandList[L"input"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"insertmenu"]=0;
	this->commandList[L"intlimit"]=&NONS_ScriptInterpreter::command_intlimit;
	this->commandList[L"isdown"]=0;
	this->commandList[L"isfull"]=0;
	this->commandList[L"ispage"]=0;
	this->commandList[L"isskip"]=0;
	this->commandList[L"itoa"]=&NONS_ScriptInterpreter::command_itoa;
	this->commandList[L"itoa2"]=&NONS_ScriptInterpreter::command_itoa;
	this->commandList[L"jumpb"]=&NONS_ScriptInterpreter::command_jumpf;
	this->commandList[L"jumpf"]=&NONS_ScriptInterpreter::command_jumpf;
	this->commandList[L"kidokumode"]=0;
	this->commandList[L"kidokuskip"]=&NONS_ScriptInterpreter::command_kidokuskip;
	this->commandList[L"labellog"]=0;
	this->commandList[L"layermessage"]=0;
	this->commandList[L"ld"]=&NONS_ScriptInterpreter::command_ld;
	this->commandList[L"len"]=&NONS_ScriptInterpreter::command_len;
	this->commandList[L"linepage"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"linepage2"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"loadgame"]=&NONS_ScriptInterpreter::command_loadgame;
	this->commandList[L"loadgosub"]=0;
	this->commandList[L"locate"]=&NONS_ScriptInterpreter::command_locate;
	this->commandList[L"logsp"]=0;
	this->commandList[L"logsp2"]=0;
	this->commandList[L"lookbackbutton"]=&NONS_ScriptInterpreter::command_lookbackbutton;
	this->commandList[L"lookbackcolor"]=&NONS_ScriptInterpreter::command_lookbackcolor;
	this->commandList[L"lookbackflush"]=&NONS_ScriptInterpreter::command_lookbackflush;
	this->commandList[L"lookbacksp"]=0;
	this->commandList[L"loopbgm"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"loopbgmstop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"lr_trap"]=&NONS_ScriptInterpreter::command_trap;
	this->commandList[L"lr_trap2"]=&NONS_ScriptInterpreter::command_trap;
	this->commandList[L"lsp"]=&NONS_ScriptInterpreter::command_lsp;
	this->commandList[L"lsph"]=&NONS_ScriptInterpreter::command_lsp;
	this->commandList[L"maxkaisoupage"]=0;
	this->commandList[L"menu_automode"]=0;
	this->commandList[L"menu_full"]=0;
	this->commandList[L"menu_window"]=0;
	this->commandList[L"menuselectcolor"]=&NONS_ScriptInterpreter::command_menuselectcolor;
	this->commandList[L"menuselectvoice"]=&NONS_ScriptInterpreter::command_menuselectvoice;
	this->commandList[L"menusetwindow"]=&NONS_ScriptInterpreter::command_menusetwindow;
	this->commandList[L"mid"]=&NONS_ScriptInterpreter::command_mid;
	this->commandList[L"mod"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"mode_ext"]=0;
	this->commandList[L"mode_saya"]=0;
	this->commandList[L"monocro"]=&NONS_ScriptInterpreter::command_monocro;
	this->commandList[L"mov"]=&NONS_ScriptInterpreter::command_mov;
	this->commandList[L"mov10"]=0;
	this->commandList[L"mov3"]=0;
	this->commandList[L"mov4"]=0;
	this->commandList[L"mov5"]=0;
	this->commandList[L"mov6"]=0;
	this->commandList[L"mov7"]=0;
	this->commandList[L"mov8"]=0;
	this->commandList[L"mov9"]=0;
	this->commandList[L"movemousecursor"]=0;
	this->commandList[L"movl"]=&NONS_ScriptInterpreter::command_movl;
	this->commandList[L"mp3"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"mp3fadeout"]=&NONS_ScriptInterpreter::command_mp3fadeout;
	this->commandList[L"mp3loop"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"mp3save"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"mp3stop"]=0;
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
	this->commandList[L"ofscopy"]=0;
	this->commandList[L"ofscpy"]=0;
	this->commandList[L"play"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"playonce"]=&NONS_ScriptInterpreter::command_play;
	this->commandList[L"playstop"]=&NONS_ScriptInterpreter::command_playstop;
	this->commandList[L"pretextgosub"]=0;
	this->commandList[L"print"]=&NONS_ScriptInterpreter::command_print;
	this->commandList[L"prnum"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"prnumclear"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"puttext"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"quake"]=&NONS_ScriptInterpreter::command_quake;
	this->commandList[L"quakex"]=&NONS_ScriptInterpreter::command_sinusoidal_quake;
	this->commandList[L"quakey"]=&NONS_ScriptInterpreter::command_sinusoidal_quake;
	this->commandList[L"repaint"]=&NONS_ScriptInterpreter::command_repaint;
	this->commandList[L"reset"]=&NONS_ScriptInterpreter::command_reset;
	this->commandList[L"resetmenu"]=0;
	this->commandList[L"resettimer"]=&NONS_ScriptInterpreter::command_resettimer;
	this->commandList[L"return"]=&NONS_ScriptInterpreter::command_return;
	this->commandList[L"rmenu"]=&NONS_ScriptInterpreter::command_rmenu;
	this->commandList[L"rmode"]=&NONS_ScriptInterpreter::command_rmode;
	this->commandList[L"rnd"]=&NONS_ScriptInterpreter::command_rnd;
	this->commandList[L"rnd2"]=&NONS_ScriptInterpreter::command_rnd;
	this->commandList[L"roff"]=&NONS_ScriptInterpreter::command_rmode;
	this->commandList[L"rubyoff"]=0;
	this->commandList[L"rubyon"]=0;
	this->commandList[L"sar"]=&NONS_ScriptInterpreter::command_nsa;
	this->commandList[L"savefileexist"]=0;
	this->commandList[L"savegame"]=&NONS_ScriptInterpreter::command_savegame;
	this->commandList[L"savename"]=&NONS_ScriptInterpreter::command_savename;
	this->commandList[L"savenumber"]=&NONS_ScriptInterpreter::command_savenumber;
	this->commandList[L"saveoff"]=0;
	this->commandList[L"saveon"]=0;
	this->commandList[L"savescreenshot"]=0;
	this->commandList[L"savescreenshot2"]=0;
	this->commandList[L"savetime"]=0;
	this->commandList[L"select"]=&NONS_ScriptInterpreter::command_select;
	this->commandList[L"selectbtnwait"]=0;
	this->commandList[L"selectcolor"]=&NONS_ScriptInterpreter::command_selectcolor;
	this->commandList[L"selectvoice"]=&NONS_ScriptInterpreter::command_selectvoice;
	this->commandList[L"selgosub"]=&NONS_ScriptInterpreter::command_select;
	this->commandList[L"selnum"]=0;
	this->commandList[L"setcursor"]=&NONS_ScriptInterpreter::command_setcursor;
	this->commandList[L"setlayer"]=0;
	this->commandList[L"setwindow"]=&NONS_ScriptInterpreter::command_setwindow;
	this->commandList[L"setwindow2"]=0;
	this->commandList[L"setwindow3"]=0;
	this->commandList[L"sevol"]=0;
	this->commandList[L"shadedistance"]=0;
	this->commandList[L"sin"]=&NONS_ScriptInterpreter::command_add;
	this->commandList[L"skip"]=&NONS_ScriptInterpreter::command_skip;
	this->commandList[L"skipoff"]=&NONS_ScriptInterpreter::command_skipoff;
	this->commandList[L"soundpressplgin"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"sp_rgb_gradation"]=0;
	this->commandList[L"spbtn"]=0;
	this->commandList[L"spclclk"]=0;
	this->commandList[L"spi"]=&NONS_ScriptInterpreter::command_unimplemented;
	this->commandList[L"split"]=0;
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
	this->commandList[L"tateyoko"]=0;
	this->commandList[L"texec"]=0;
	this->commandList[L"textbtnwait"]=0;
	this->commandList[L"textclear"]=&NONS_ScriptInterpreter::command_textclear;
	this->commandList[L"textgosub"]=0;
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
	this->commandList[L"versionstr"]=&NONS_ScriptInterpreter::command_unimplemented;
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
	this->commandList[L"enable_onslaught_language_extensions"]=&NONS_ScriptInterpreter::command_onslaught_language_extensions;
	this->commandList[L"disable_onslaught_language_extensions"]=&NONS_ScriptInterpreter::command_onslaught_language_extensions;
	this->commandList[L"getini"]=&NONS_ScriptInterpreter::command_getini;
	this->commandList[L"new_set_window"]=&NONS_ScriptInterpreter::command_new_set_window;
	this->commandList[L"set_default_font_size"]=&NONS_ScriptInterpreter::command_set_default_font_size;
	this->commandList[L"unalias"]=&NONS_ScriptInterpreter::command_unalias;
	this->commandList[L"literal_print"]=&NONS_ScriptInterpreter::command_literal_print;
	this->commandList[L"use_new_if"]=&NONS_ScriptInterpreter::command_use_new_if;
	this->commandList[L"centerh"]=&NONS_ScriptInterpreter::command_centerh;
	this->commandList[L"centerv"]=&NONS_ScriptInterpreter::command_centerv;
	/*this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	this->commandList[L""]=&NONS_ScriptInterpreter::command_;
	*/
	ulong total=this->totalCommands(),implemented=this->implementedCommands();
	std::cout <<"ONSlaught script interpreter v"<<float((implemented*100)/total)/100<<std::endl;
}

void NONS_ScriptInterpreter::uninit(){
	if (this->store)
		delete this->store;
	for (INIcacheType::iterator i=this->INIcache.begin();i!=this->INIcache.end();i++){
		delete[] i->first;
		delete i->second;
	}
	delete this->arrowCursor;
	delete this->pageCursor;
	if (this->menu)
		delete this->menu;
	if (this->selectVoiceClick)
		delete[] this->selectVoiceClick;
	if (this->selectVoiceEntry)
		delete[] this->selectVoiceEntry;
	if (this->selectVoiceMouseOver)
		delete[] this->selectVoiceMouseOver;
	if (this->clickStr)
		delete[] this->clickStr;
	if (this->imageButtons)
		delete this->imageButtons;
	delete this->saveGame;
	char *settings_filename=addStrings(this->NONS_config_dir,"settings.cfg");
	ConfigFile settings;
	settings.assignInt(L"textSpeedMode",this->current_speed_setting);
	settings.writeOut(settings_filename);
	delete[] settings_filename;
}

NONS_ScriptInterpreter::~NONS_ScriptInterpreter(){
	InputObserver.detach(this->inputQueue);
	this->uninit();
	delete[] this->NONS_config_dir;
	//delete this->main_font;
}

ulong NONS_ScriptInterpreter::totalCommands(){
	return this->commandList.size();
}

ulong NONS_ScriptInterpreter::implementedCommands(){
	ulong res=0;
	for (commandListType::iterator i=this->commandList.begin();i!=this->commandList.end();i++)
		if (i->second)
			res++;
	return res;
}

ulong countLines(wchar_t *buffer,ulong byte_pos){
	ulong res=1;
	for (ulong pos=0;pos<byte_pos;pos++){
		if (buffer[pos]==13 && buffer[pos+1]==10){
			res++;
			pos++;
		}else if (buffer[pos]==13)
			res++;
		else if (buffer[pos]==10)
			res++;
	}
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
			}else if (event.key.keysym.sym==SDLK_F6){\
				this->default_speed=this->default_speed_med;\
				this->current_speed_setting=1;\
			}else if (event.key.keysym.sym==SDLK_F7){\
				this->default_speed=this->default_speed_fast;\
				this->current_speed_setting=2;\
			}\
			this->everything->screen->output->display_speed=cur/def*float(this->default_speed);\
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
				this->interpreter_position=this->trapLabel;
				this->trapLabel=0;
				trapFlag=0;
			}
		}
	}else{
		while (!this->inputQueue->data.empty()){
			SDL_Event event=this->inputQueue->pop();
			INTERPRETNEXTLINE_HANDLEKEYS
		}
	}
	this->current_line=countLines(this->script->script,this->interpreter_position);
	bool lineWillBeSkipped=(this->errored_lines.find(this->current_line)!=this->errored_lines.end());
	if (CLOptions.verbosity>=1){
		if (!lineWillBeSkipped)
			v_stdlog <<"Interpreting line "<<this->current_line<<std::endl;
		else
			v_stdlog <<"Ignoring line "<<this->current_line<<std::endl;
	}
	this->previous_interpreter_position=this->interpreter_position;
	//The NONS_ParsedLine constructor needs to be called so that the interpreter position can advance.
	NONS_ParsedLine line(this->script->script,&this->interpreter_position,this->current_line);
	if (!lineWillBeSkipped){
		this->saveGame->currentOffset=this->interpreter_position;
		this->saveGame->textX=this->everything->screen->output->x;
		this->saveGame->textY=this->everything->screen->output->y;
		switch (line.type){
			case PARSEDLINE_BLOCK:
				if (this->saveGame->currentLabel)
					delete[] this->saveGame->currentLabel;
				this->saveGame->currentLabel=copyWString(line.line);
				if (!wcscmp(line.line,L"*define"))
					this->interpreter_mode=DEFINE;
				break;
			case PARSEDLINE_JUMP:
			case PARSEDLINE_COMMENT:
				break;
			case PARSEDLINE_PRINTER:
				if (this->printed_lines.find(this->current_line)==this->printed_lines.end()){
					softwareCtrlIsPressed=0;
					this->printed_lines.insert(this->current_line);
				}
				this->Printer2(line.line);
				break;
			case PARSEDLINE_COMMAND:
				{
					commandListType::iterator i=this->commandList.find(line.line);
					if (i!=this->commandList.end()){
						ErrorCode(NONS_ScriptInterpreter::*temp)(NONS_ParsedLine &)=i->second;
						if (!temp){
							if (this->implementationErrors.find(i->first)==this->implementationErrors.end()){
								v_stderr <<"NONS_ScriptInterpreter::interpretNextLine(): Error near line "<<this->current_line<<". Command \"";
								v_stderr.writeWideString(line.line);
								v_stderr <<"\" is not implemented yet.\n"
										 <<"    Implementation errors are reported only once."<<std::endl;
								this->implementationErrors.insert(i->first);
							}
							this->errored_lines.insert(this->current_line);
							break;
						}
						ErrorCode error=(this->*temp)(line);
						if (error==NONS_END)
							return 0;
						if (!(handleErrors(error,this->current_line,"NONS_ScriptInterpreter::interpretNextLine")&NONS_NO_ERROR_FLAG)){
							v_stderr <<"\"";
							v_stderr.writeWideString(line.line);
							v_stderr <<"\""<<std::endl;
							for (ulong a=0;a<line.parameters.size();a++){
								v_stderr <<"    \"";
								v_stderr.writeWideString(line.parameters[a]);
								v_stderr <<"\""<<std::endl;
							}
							this->errored_lines.insert(this->current_line);
						}
					}else{
						v_stderr <<"NONS_ScriptInterpreter::interpretNextLine(): Error near line "<<this->current_line<<". Command \"";
						v_stderr.writeWideString(line.line);
						v_stderr <<"\" could not be recognized."<<std::endl;
						this->errored_lines.insert(this->current_line);
					}
				}
				break;
		}
	}
	if (this->interpreter_position>=this->script->length){
		this->command_end(line);
		return 0;
	}
	return 1;
}

ErrorCode NONS_ScriptInterpreter::interpretString(wchar_t *string){
	NONS_ParsedLine line(string);
	line.lineNo=-1;
	switch (line.type){
		case PARSEDLINE_COMMENT:
			break;
		case PARSEDLINE_COMMAND:
			{
				commandListType::iterator i=this->commandList.find(line.line);
				if (i!=(this->commandList.end())){
					ErrorCode(NONS_ScriptInterpreter::*temp)(NONS_ParsedLine &)=i->second;
					if (!temp){
						if (this->implementationErrors.find(i->first)!=this->implementationErrors.end()){
							v_stderr <<"NONS_ScriptInterpreter::interpretNextLine(): Error. Command \""<<line.line<<"\" is not implemented.\n"
									  <<"    Implementation errors are reported only once."<<std::endl;
							this->implementationErrors.insert(i->first);
						}
						return NONS_NOT_IMPLEMENTED;
					}
					return handleErrors((this->*temp)(line),line.lineNo,"NONS_ScriptInterpreter::interpretString");
				}else{
					v_stderr <<"NONS_ScriptInterpreter::interpretString(): Error. Command \""<<line.line<<"\" could not be recognized."<<std::endl;
					return NONS_UNRECOGNIZED_COMMAND;
				}
			}
			break;
	}
	return NONS_NO_ERROR;
}

wchar_t *insertIntoString(wchar_t *dst,long from,long l,wchar_t *src){
	long ldst=wcslen(dst),
		lsrc=wcslen(src),
		lnew=ldst-l+lsrc;
	wchar_t *res=new wchar_t[lnew+1];
	res[lnew]=0;
	memcpy(res,dst,from*sizeof(wchar_t));
	memcpy(res+from,src,lsrc*sizeof(wchar_t));
	memcpy(res+from+lsrc,dst+from+l,(ldst-(from+l))*sizeof(wchar_t));
	return res;
}

wchar_t *insertIntoString(wchar_t *dst,long from,long l,long src){
	char temp[100];
	sprintf(temp,"%d",src);
	wchar_t *representation=copyWString(temp);
	wchar_t *res=insertIntoString(dst,from,l,representation);
	delete[] representation;
	return res;
}

wchar_t *getArray(const wchar_t *string){
	long l=0;
	for (;string[l]!='?';l++);
	const char *breakers="\012\015 \t|&=!<>+-*/";
	bool breakerfound=0;
	for (;string[l] && string[l]!='[' && !iswhitespace((char)string[l]) && !breakerfound;l++)
		for (short a=0;breakers[a];a++)
			if (string[l]==breakers[a])
				breakerfound=1;
	if (breakerfound==1)
		return 0;
	for (;string[l] && iswhitespace((char)string[l]);l++);
	if (string[l]!='[')
		return 0;
	while (1){
		l++;
		ulong nesting=1;
		for (;string[l];l++){
			switch (string[l]){
				case '[':
					nesting++;
					break;
				case ']':
					nesting--;
			}
			if (!nesting)
				break;
		}
		if (nesting)
			return 0;
		l++;
		long l2=l;
		for (;string[l] && iswhitespace((char)string[l]);l++);
		if (string[l]!='['){
			l=l2;
			break;
		}
	}
	return copyWString(string,l);
}

/*void NONS_ScriptInterpreter::setDefaultWindow(){
	if (this->everything->screen)
		return;
	this->main_font=new NONS_Font("default.ttf",this->defaultfs,TTF_STYLE_NORMAL);
	this->everything->screen=new NONS_ScreenSpace(20,this->main_font,this->gfx_store);
	//this->everything->screen->output->shadeLayer->setShade(0x99,0x99,0x99);
	this->everything->screen->output->shadeLayer->Clear();
	//this->everything->screen->Background->defaultShade=0;
	this->everything->screen->Background->Clear();
	this->everything->screen->BlendAll(1);
}*/

/*ErrorCode NONS_ScriptInterpreter::Printer(wchar_t *line){
	if (!this->everything->screen)
		this->setDefaultWindow();
	this->currentBuffer=this->everything->screen->output->currentBuffer;
	bool skip=*line=='`';
	//Unused:
	//ulong olen=wcslen(line)-skip;
	wchar_t *str=addStrings(line+skip,L"\n");
	wchar_t *original=str;
	bool printed=0;
	bool justClicked=0;
	while (*str){
		switch (*str){
			case '\\':
				this->everything->screen->showText();
				if (!justClicked && this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
					goto Printer_000;
				justClicked=0;
				this->everything->screen->clearText();
				str++;
				if (*str=='\n')
					goto Printer_000;
				break;
			case '@':
				this->everything->screen->showText();
				if (!justClicked && this->arrowCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
					goto Printer_000;
				str++;
				if (*str=='\n' && !printed)
					goto Printer_000;
				break;
			case '!':
				if (!instr(str,"!sd")){
					this->everything->screen->output->display_speed=this->default_speed;
					str+=3;
					if (*str=='\n')
						goto Printer_000;
					break;
				}else{
					bool notess=!instr(str,"!s"),notdee=!instr(str,"!d"),notdu=!instr(str,"!w");
					if (notess || notdee || notdu){
						str+=2;
						ulong l=0;
						for (;isdigit(str[l]);l++);
						if (l>0){
							char *temp=copyString(str,l);
							long s=atol(temp);
							delete[] temp;
							if (notess)
								this->everything->screen->output->display_speed=s;
							else if (notdee)
								waitCancellable(s);
							else
								waitNonCancellable(s);
							str+=l;
							break;
						}else
							str-=2;
					}
				}
			case '#':
				if (*str=='#'){
					ulong len=wcslen(str+1);
					if (len>=6){
						str++;
						integer32 parsed=0;
						short a;
						for (a=0;a<6;a++){
							char hex=toupper(str[a]);
							if (!(hex>='0' && hex<='9' || hex>='A' && hex<='F'))
								break;
							parsed<<=4;
							parsed|=(hex>='0' && hex<='9')?hex-'0':hex-'A'+10;
						}
						if (a==6){
							SDL_Color color=this->everything->screen->output->foregroundLayer->fontCache->foreground;
							color.r=parsed>>16;
							color.g=(parsed&0xFF00)>>8;
							color.b=(parsed&0xFF);
							this->everything->screen->output->foregroundLayer->fontCache->foreground=color;
							str+=6;
							break;
						}
						str--;
					}
				}
			case '%':
				if (*str=='%'){
					if (isalpha(str[1]) || str[1]=='_'){
						ulong l;
						for (l=0;str[l+1] && (isalnum(str[l+1]) || str[l+1]=='_');l++);
						if (l){
							wchar_t *name=copyWString(str,l+1);
							NONS_Variable *var=this->store->retrieve(name);
							delete[] name;
							if (var){
								long current_position=str-original;
								wchar_t *newstring=insertIntoString(original,current_position,l+1,var->intValue);
								delete[] original;
								original=newstring;
								str=newstring+current_position;
							}
						}
					}
				}
			case '$':
				if (*str=='$'){
					if (isalpha(str[1]) || str[1]=='_'){
						ulong l;
						for (l=0;str[l+1] && (isalnum(str[l+1]) || str[l+1]=='_');l++);
						if (l){
							wchar_t *name=copyWString(str,l+1);
							NONS_Variable *var=this->store->retrieve(name);
							delete[] name;
							if (var){
								long current_position=str-original;
								wchar_t *newstring=insertIntoString(original,current_position,l+1,var->wcsValue);
								delete[] original;
								original=newstring;
								str=newstring+current_position;
							}
						}
					}
				}
			case '?':
				if (*str=='?'){
					wchar_t *name=getArray(str);
					if (name){
						NONS_Variable *var=this->store->retrieve(name);
						long l=wcslen(name);
						delete[] name;
						if (var){
							if (var->type=='%'){
								long current_position=str-original;
								wchar_t *newstring=insertIntoString(original,current_position,l+1,var->intValue);
								delete[] original;
								original=newstring;
								str=newstring+current_position;
							}else if (var->type=='$'){
								long current_position=str-original;
								wchar_t *newstring=insertIntoString(original,current_position,l+1,var->wcsValue);
								delete[] original;
								original=newstring;
								str=newstring+current_position;
							}
						}
					}
				}
			default:
				{
					ulong end=1,end2;
					for (;str[end] && !(multicomparison(str[end],"@\\#") || str[end]=='!' && (str[end+1]=='s' || str[end+1]=='d' || str[end+1]=='w'));end++);
					if (this->clickStr){
						for (end2=1;str[end2] && !multicomparison(str[end2],this->clickStr);end2++);
						end2++;
						if (end2<end){
							end=end2;
							justClicked=1;
						}
					}
					wchar_t *printstring=copyWString(str,end);
					this->everything->screen->showText();
					std::vector<NONS_Glyph *> *temp1=this->everything->screen->NONSOut(printstring,0);
					if (justClicked && this->arrowCursor->animate(this->everything->screen,this->menu,this->autoclick)<0){
						if (temp1)
							delete temp1;
						goto Printer_000;
					}
					printed=1;
					delete[] printstring;
					while (temp1){
						if (this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0){
							delete temp1;
							goto Printer_000;
						}
						justClicked=1;
						this->everything->screen->clearText();
						std::vector<NONS_Glyph *> *temp2=0;
						if (temp1->size())
							temp2=this->everything->screen->NONSOut(temp1);
						delete temp1;
						if (!temp2){
							justClicked=0;
							break;
						}
						temp1=temp2;
					}
					str+=end;
				}
		}
	}
Printer_000:
	delete[] original;
	return NONS_NO_ERROR;
}*/

void NONS_ScriptInterpreter::reduceString(
		const wchar_t *src,
		std::wstring &dst,
		std::set<NONS_Variable *> *visited,
		std::vector<std::pair<wchar_t *,NONS_Variable *> > *stack){
	if (!src)
		return;
	for (const wchar_t *str=src;*str;){
		switch (*str){
			case '!':
				if (!instr(str,"!nl")){
					dst.push_back('\n');
					str+=3;
					break;
				}
			case '%':
				if (*str=='%'){
						ulong l;
						for (l=0;str[l+1] && (isalnum(str[l+1]) || str[l+1]=='_');l++);
						if (l){
							wchar_t *name=copyWString(str,l+1);
							NONS_Variable *var=this->store->retrieve(name);
							delete[] name;
							if (var){
								char repr[30];
								sprintf(repr,"%d",var->intValue);
								wchar_t *copy=copyWString(repr);
								dst.append(copy);
								delete[] copy;
								str+=l+1;
								break;
							}
						}
				}
			case '$':
				if (*str=='$'){
						ulong l;
						for (l=0;str[l+1] && (isalnum(str[l+1]) || str[l+1]=='_');l++);
						if (l){
							wchar_t *name=copyWString(str,l+1);
							NONS_Variable *var=this->store->retrieve(name);
							if (!!var){
								if (!!visited && visited->find(var)!=visited->end()){
									v_stderr <<"NONS_ScriptInterpreter::reduceString(): WARNING: Infinite recursion avoided.\n"
										"    Reduction stack contents:"<<std::endl;
									for (std::vector<std::pair<wchar_t *,NONS_Variable *> >::iterator i=stack->begin();i!=stack->end();i++){
										v_stderr <<"        [";
										v_stderr.writeWideString(i->first);
										v_stderr <<"] = \"";
										v_stderr.writeWideString(i->second->wcsValue);
										v_stderr <<"\""<<std::endl;
									}
									v_stderr <<" (last) [";
									v_stderr.writeWideString(name);
									v_stderr <<"] = \"";
									v_stderr.writeWideString(var->wcsValue);
									v_stderr <<"\""<<std::endl;
									dst.append(var->wcsValue);
								}else{
									std::set<NONS_Variable *> *temp_visited;
									std::vector<std::pair<wchar_t *,NONS_Variable *> > *temp_stack;
									if (!visited)
										temp_visited=new std::set<NONS_Variable *>;
									else
										temp_visited=visited;
									temp_visited->insert(var);
									if (!stack)
										temp_stack=new std::vector<std::pair<wchar_t *,NONS_Variable *> >;
									else
										temp_stack=stack;
									temp_stack->push_back(std::pair<wchar_t *,NONS_Variable *>(name,var));
									reduceString(var->wcsValue,dst,temp_visited,temp_stack);
									if (!visited)
										delete temp_visited;
									else
										temp_visited->erase(var);
									if (!stack)
										delete temp_stack;
									else
										temp_stack->pop_back();
								}
								str+=l+1;
								delete[] name;
								break;
							}
							delete[] name;
						}
				}
			case '?':
				if (*str=='?'){
					wchar_t *name=getArray(str);
					if (name){
						NONS_Variable *var=this->store->retrieve(name);
						long l=wcslen(name);
						if (var){
							if (var->type=='%'){
								delete[] name;
								char repr[30];
								sprintf(repr,"%d",var->intValue);
								wchar_t *copy=copyWString(repr);
								dst.append(copy);
								delete[] copy;
								str+=l+1;
								break;
							}else if (var->type=='$'){
								if (instr(var->wcsValue,name)>=0){
									v_stderr <<"NONS_ScriptInterpreter::reduceString(): WARNING: Infinite recursion avoided. The variable [";
									v_stderr.writeWideString(name);
									v_stderr <<"] contained a reference to itself while trying to print it.";
									dst.append(var->wcsValue);
								}else
									reduceString(var->wcsValue,dst);
								delete[] name;
								str+=l+1;
								break;
							}
						}
						delete[] name;
					}
				}
			default:
				dst.push_back(*str);
				str++;
		}
	}
}

void findStops(const wchar_t *src,std::vector<std::pair<ulong,ulong> > &stopping_points,std::wstring &dst){
	dst.clear();
	const wchar_t *str2=src;
	for (;*str2;str2++){
		switch (*str2){
			case '\\':
			case '@':
				{
					std::pair<ulong,ulong> push(dst.size(),str2-src);
					stopping_points.push_back(push);
					continue;
				}
			case '!':
				if (!instr(str2,L"!sd")){
					std::pair<ulong,ulong> push(dst.size(),src-str2);
					stopping_points.push_back(push);
					str2+=2;
					continue;
				}else if (!instr(str2,L"!s") || !instr(str2,L"!d") || !instr(str2,L"!w")){
					std::pair<ulong,ulong> push(dst.size(),src-str2);
					stopping_points.push_back(push);
					ulong l=2;
					for (;isdigit(str2[l]);l++);
					str2+=l-1;
					continue;
				}
			case '#':
				if (*str2=='#'){
					ulong len=wcslen(str2+1);
					if (len>=6){
						str2++;
						short a;
						for (a=0;a<6;a++){
							char hex=toupper(str2[a]);
							if (!(hex>='0' && hex<='9' || hex>='A' && hex<='F'))
								break;
						}
						if (a!=6)
							str2--;
						else{
							std::pair<ulong,ulong> push(dst.size(),src-str2);
							stopping_points.push_back(push);
							str2+=5;
							continue;
						}
					}
				}
			default:
				dst.push_back(*str2);
		}
		if (*str2=='\\')
			break;
	}
	std::pair<ulong,ulong> push(dst.size(),str2-src);
	stopping_points.push_back(push);
}

struct printingPage{
	std::wstring print;
	std::wstring reduced;
	//first: position in the string that is actually printed.
	//second: position in the reduced string
	std::vector<std::pair<ulong,ulong> > stops;
	printingPage(){}
	printingPage(int a){}
};

ErrorCode NONS_ScriptInterpreter::Printer2(const wchar_t *line){
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	this->currentBuffer=this->everything->screen->output->currentBuffer;
	NONS_StandardOutput *out=this->everything->screen->output;
	if (!*line){
		if (out->NewLine()){
			if (this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
				return NONS_NO_ERROR;
			out->NewLine();
		}
		return NONS_NO_ERROR;
	}
	bool skip=*line=='`';
	wchar_t *str=copyWString(line+skip);
	bool justClicked=0,
		justTurnedPage=0;
	std::wstring reducedString;
	reduceString(str,reducedString);
	delete[] str;
	std::vector<printingPage> pages;
	ulong totalprintedchars=0;
	for (const wchar_t *str2=reducedString.c_str();*str2;){
		ulong p=instr(str2,L"\\")+1;
		wchar_t *str3=copyWString(str2,p);
		if (!p)
			str2+=wcslen(str2);
		else
			str2+=p;
		pages.push_back(0);
		printingPage &page=pages[pages.size()-1];
		page.reduced=str3;
		delete[] str3;
		findStops(page.reduced.c_str(),page.stops,page.print);
	}
	this->everything->screen->showText();
	for (std::vector<printingPage>::iterator i=pages.begin();i!=pages.end();i++){
		bool clearscr=out->prepareForPrinting(i->print.c_str());
		if (clearscr){
			if (this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
				return NONS_NO_ERROR;
			this->everything->screen->clearText();
		}
		str=(wchar_t *)i->reduced.c_str();
		for (ulong reduced=0,printed=0,stop=0;stop<i->stops.size();stop++){
			ulong printedChars=0;
			while (justClicked=out->print(printed,i->stops[stop].first,this->everything->screen->screen,&printedChars)){
				if (this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
					return NONS_NO_ERROR;
				this->everything->screen->clearText();
				justClicked=1;
			}
			if (printedChars>0){
				totalprintedchars+=printedChars;
				justTurnedPage=0;
				justClicked=0;
			}
			reduced=i->stops[stop].second;
			switch (str[reduced]){
				case '\\':
					if (!justClicked && this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
						return NONS_NO_ERROR;
					out->endPrinting();
					this->everything->screen->clearText();
					reduced++;
					justTurnedPage=1;
					break;
				case '@':
					if (!justClicked && this->arrowCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
						return NONS_NO_ERROR;
					reduced++;
					break;
				case '!':
					if (!instr(str+reduced,"!sd")){
						out->display_speed=this->default_speed;
						reduced+=3;
						break;
					}else{
						bool notess=!instr(str+reduced,"!s"),
							notdee=!instr(str+reduced,"!d"),
							notdu=!instr(str+reduced,"!w");
						if (notess || notdee || notdu){
							reduced+=2;
							ulong l=0;
							for (;isdigit(str[reduced+l]);l++);
							if (l>0){
								char *temp=copyString(str+reduced,l);
								long s=atol(temp);
								delete[] temp;
								if (notess)
									this->everything->screen->output->display_speed=s;
								else if (notdee)
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
					if (str[reduced]=='#'){
						ulong len=wcslen(str+reduced+1);
						if (len>=6){
							reduced++;
							integer32 parsed=0;
							short a;
							for (a=0;a<6;a++){
								char hex=toupper(str[reduced+a]);
								if (!(hex>='0' && hex<='9' || hex>='A' && hex<='F'))
									break;
								parsed<<=4;
								parsed|=(hex>='0' && hex<='9')?hex-'0':hex-'A'+10;
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
	if (!justTurnedPage && totalprintedchars && out->NewLine() && this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)>=0){
		this->everything->screen->clearText();
		//out->NewLine();
	}
//Printer2_000:
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::getIntValue(wchar_t *str,long *value){
	if (*str=='\"' || *str=='`' || *str=='$')
		return NONS_UNMATCHING_OPERANDS;
	if (*str=='#'){
		long res=0;
		str++;
		short a;
		for (a=0;a<6;a++){
			char hex=toupper(str[a]);
			if (!(hex>='0' && hex<='9' || hex>='A' && hex<='F'))
				break;
			res<<=4;
			res|=(hex>='0' && hex<='9')?hex-'0':hex-'A'+10;
		}
		if (a<6)
			return NONS_INVALID_HEX;
		if (!!value)
			*value=res;
	}else
		_HANDLE_POSSIBLE_ERRORS(this->store->evaluate(str,value),)
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::getStrValue(wchar_t *str,char **value){
	if (*str=='%')
		return NONS_UNMATCHING_OPERANDS;
	if (*str=='\"' || *str=='`'){
		wchar_t find[]={0,0};
		find[0]=*str;
		long endstr=instr(str+1,find);
		if (endstr<0)
			return NONS_UNMATCHED_QUOTES;
		if (*value)
			delete[] *value;
		if (!endstr)
			*value=copyString("");
		else
			*value=copyString(str+1,endstr);
	}else{
		NONS_Variable *var=0;
		if (*str=='?'){
			_HANDLE_POSSIBLE_ERRORS(this->store->resolveIndexing(str,&var),)
		}else
			var=this->store->retrieve(str);
		if (!var)
			return NONS_UNDEFINED_VARIABLE;
		if (*value)
			delete[] *value;
		if (!var->wcsValue)
			*value=copyString("");
		else
			*value=copyString(var->wcsValue);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::getWcsValue(wchar_t *str,wchar_t **value){
	if (*str=='%')
		return NONS_UNMATCHING_OPERANDS;
	if (*str=='\"' || *str=='`'){
		wchar_t find[]={0,0};
		find[0]=*str;
		long endstr=instr(str+1,find);
		if (endstr<0)
			return NONS_UNMATCHED_QUOTES;
		if (*value)
			delete[] *value;
		if (!endstr)
			*value=copyWString("");
		else
			*value=copyWString(str+1,endstr);
	}else{
		NONS_Variable *var=0;
		if (*str=='?'){
			_HANDLE_POSSIBLE_ERRORS(this->store->resolveIndexing(str,&var),)
		}else
			var=this->store->retrieve(str);
		if (!var)
			return NONS_UNDEFINED_VARIABLE;
		if (*value)
			delete[] *value;
		if (!var->wcsValue)
			*value=copyWString("");
		else
			*value=copyWString(var->wcsValue);
	}
	return NONS_NO_ERROR;
}

void NONS_ScriptInterpreter::convertParametersToString(NONS_ParsedLine &line,std::wstring &string){
	string.clear();
	for (ulong a=0;a<line.parameters.size();a++){
		NONS_Variable *var=this->store->retrieve(line.parameters[a]);
		if (!var){
			wchar_t *str=line.parameters[a];
			for (str+=(*str=='"')?1:0;*str && *str!='"';str++){
				if (*str=='\\'){
					str++;
					switch (*str){
						case '"':
						case '\\':
							string.push_back(*str);
							break;
						case 'r':
						case 'n':
							string.push_back('\n');
							break;
						case 'x':
							{
								str++;
								wchar_t n=0;
								short b;
								for (b=0;b<4;b++){
									char hex=toupper(str[b]);
									if (!(hex>='0' && hex<='9' || hex>='A' && hex<='F'))
										break;
									n<<=4;
									n|=(hex>='0' && hex<='9')?hex-'0':hex-'A'+10;
								}
								if (b<4)
									goto convertParametersToString_000;
								str+=3;
								string.push_back(n);
							}
							break;
						default:
							goto convertParametersToString_000;
					}
				}else
					string.push_back(*str);
			}
		}else if (var->type=='%'){
			char temp[100];
			sprintf(temp,"%d",var->intValue);
			wchar_t *representation=copyWString(temp);
			string.append(representation);
			delete[] representation;
		}else if (var->type=='$'){
			string.append(var->wcsValue);
		}else if (var->type=='?')
			continue;
convertParametersToString_000:;
	}
}
#endif
