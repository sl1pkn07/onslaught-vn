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

#ifndef NONS_SCRIPTINTERPRETER_H
#define NONS_SCRIPTINTERPRETER_H

#include "../Common.h"
#include "../ErrorCodes.h"
#include "ParsedLine.h"
#include "VariableStore.h"
#include "../Everything.h"
#include "../IO_System/Script/Script.h"
#include "../IO_System/Graphics/GFX.h"
#include "../IO_System/INIfile.h"
#include "../IO_System/SAR/Image_Loader/ImageLoader.h"
#include "../IO_System/Graphics/Cursor.h"
#include "../IO_System/Graphics/Menu.h"
#include "../IO_System/SaveFile.h"
#include "../IO_System/ConfigFile.h"
#include "../enums.h"
#include <set>
#include <stack>
#include <cmath>
#include <cstdlib>
#include <ctime>
//#include <SDL/smpeg.h>

#include "../version.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define _GETINTVALUE(dst,src,extra) _HANDLE_POSSIBLE_ERRORS(this->store->getIntValue(line.parameters[(src)],dst),extra)
//#define _GETSTRVALUE(dst,src,extra) _HANDLE_POSSIBLE_ERRORS(this->store->getStrValue(line.parameters[(src)],&(dst)),extra)
#define _GETWCSVALUE(dst,src,extra) _HANDLE_POSSIBLE_ERRORS(this->store->getWcsValue(line.parameters[(src)],dst),extra)
#define _GETVARIABLE(varName,src,extra){\
	ErrorCode error;\
	(varName)=this->store->retrieve(line.parameters[(src)],&error);\
	if (!(varName)){\
		extra\
		return error;\
	}\
	if ((varName)->isConstant()){\
		extra\
		return NONS_EXPECTED_VARIABLE;\
	}\
	if ((varName)->getType()==INTEGER_ARRAY){\
		extra\
		return NONS_EXPECTED_SCALAR;\
	}\
}
#define _GETINTVARIABLE(varName,src,extra){\
	_GETVARIABLE(varName,src,extra)\
	if ((varName)->getType()!=INTEGER){\
		extra\
		return NONS_EXPECTED_NUMERIC_VARIABLE;\
	}\
}
#define _GETSTRVARIABLE(varName,src,extra){\
	_GETVARIABLE(varName,src,extra)\
	if ((varName)->getType()!=STRING){\
		extra\
		return NONS_EXPECTED_STRING_VARIABLE;\
	}\
}

typedef std::map<std::string,INIfile *> INIcacheType;


struct printingPage{
	std::wstring print;
	std::wstring reduced;
	std::pair<ulong,ulong> startAt;
	//first: position in the string that is actually printed.
	//second: position in the reduced string
	std::vector<std::pair<ulong,ulong> > stops;
	printingPage();
	printingPage(const printingPage &b);
	printingPage &operator=(const printingPage &b);
};

enum{
	UNDEFINED=0,
	SUBROUTINE_CALL=1,
	FOR_NEST=2,
	TEXTGOSUB_CALL=3
};

struct NONS_StackElement{
	int type;
	ulong offset;
	//subroutine data
	std::wstring first_interpret_string;
	//for data
	NONS_VariableMember *var;
	long from,
		to,
		step;
	ulong end;
	//textgosub data
	ulong textgosubLevel;
	wchar_t textgosubTriggeredBy;
	std::vector<printingPage> pages;

	NONS_StackElement(ulong level);
	NONS_StackElement(ulong offset,const std::wstring &string,ulong level);
	NONS_StackElement(NONS_VariableMember *variable,ulong startoffset,long from,long to,long step,ulong level);
	NONS_StackElement(std::vector<printingPage> pages,wchar_t trigger,ulong level);
};

class NONS_ScriptInterpreter{
	typedef std::map<std::wstring,ErrorCode(NONS_ScriptInterpreter::*)(NONS_ParsedLine &),stdStringCmpCI<wchar_t> > commandListType;
	bool Printer_support(std::vector<printingPage> &pages,ulong *totalprintedchars,bool *justTurnedPage,ErrorCode *error);
	ErrorCode Printer(const std::wstring &line);
	void reduceString(const std::wstring &src,std::wstring &dst,std::set<NONS_VariableMember *> *visited=0,std::vector<std::pair<std::wstring,NONS_VariableMember *> > *stack=0);
	void uninit();
	void init();

	ulong interpreter_position;
	ulong previous_interpreter_position;
	commandListType commandList;
	//std::set<ulong> errored_lines;
	std::set<ulong> printed_lines;
	std::set<std::wstring> implementationErrors;
	ulong interpreter_mode;
	std::string nsadir;
	std::vector<NONS_StackElement *> callStack;
	bool mp3_save;
	bool mp3_loop;
	bool wav_loop;
	long default_speed;
	long default_speed_slow;
	long default_speed_med;
	long default_speed_fast;
	char current_speed_setting;
	int defaultx;
	int defaulty;
	int defaultfs;
	bool legacy_set_window;
	INIcacheType INIcache;
	NONS_Cursor *arrowCursor;
	NONS_Cursor *pageCursor;
	bool hideTextDuringEffect;
	SDL_Color selectOn;
	SDL_Color selectOff;
	std::wstring selectVoiceEntry;
	std::wstring selectVoiceMouseOver;
	std::wstring selectVoiceClick;
	NONS_EventQueue *inputQueue;
	long trapLabel;
	std::wstring clickStr;
	ulong autoclick;
	ulong timer;
	NONS_Menu *menu;
	NONS_ButtonLayer *imageButtons;
	ulong current_line;
	bool new_if;
	ulong btnTimer;
	ulong imageButtonExpiration;
	NONS_SaveFile* saveGame;
	std::wstring currentBuffer;
	std::wstring textgosub;
	bool textgosubRecurses;

	ErrorCode command_caption(NONS_ParsedLine &line);
	ErrorCode command_alias(NONS_ParsedLine &line);
	ErrorCode command_game(NONS_ParsedLine &line);
	ErrorCode command_nsa(NONS_ParsedLine &line);
	ErrorCode command_nsadir(NONS_ParsedLine &line);
	ErrorCode command_goto(NONS_ParsedLine &line);
	ErrorCode command_globalon(NONS_ParsedLine &line);
	ErrorCode command_gosub(NONS_ParsedLine &line);
	ErrorCode command_return(NONS_ParsedLine &line);
	ErrorCode command_if(NONS_ParsedLine &line);
	ErrorCode command_mov(NONS_ParsedLine &line);
	ErrorCode command_add(NONS_ParsedLine &line);
	ErrorCode command_inc(NONS_ParsedLine &line);
	ErrorCode command_cmp(NONS_ParsedLine &line);
	ErrorCode command_skip(NONS_ParsedLine &line);
	ErrorCode command_len(NONS_ParsedLine &line);
	ErrorCode command_rnd(NONS_ParsedLine &line);
	ErrorCode command_play(NONS_ParsedLine &line);
	ErrorCode command_playstop(NONS_ParsedLine &line);
	ErrorCode command_wave(NONS_ParsedLine &line);
	ErrorCode command_wavestop(NONS_ParsedLine &line);
	ErrorCode command_itoa(NONS_ParsedLine &line);
	ErrorCode command_intlimit(NONS_ParsedLine &line);
	ErrorCode command_wait(NONS_ParsedLine &line);
	ErrorCode command_end(NONS_ParsedLine &line);
	ErrorCode command_date(NONS_ParsedLine &line);
	ErrorCode command_mp3vol(NONS_ParsedLine &line);
	ErrorCode command_getmp3vol(NONS_ParsedLine &line);
	ErrorCode command_effect(NONS_ParsedLine &line);
	ErrorCode command_mid(NONS_ParsedLine &line);
	ErrorCode command_dim(NONS_ParsedLine &line);
	ErrorCode command_movl(NONS_ParsedLine &line);
	ErrorCode command_time(NONS_ParsedLine &line);
	ErrorCode command_jumpf(NONS_ParsedLine &line);
	ErrorCode command_atoi(NONS_ParsedLine &line);
	ErrorCode command_getversion(NONS_ParsedLine &line);
	ErrorCode command_dwave(NONS_ParsedLine &line);
	ErrorCode command_dwaveload(NONS_ParsedLine &line);
	//ErrorCode command_dwavefree(NONS_ParsedLine &line);
	ErrorCode command_mp3fadeout(NONS_ParsedLine &line);
	ErrorCode command_stop(NONS_ParsedLine &line);
	ErrorCode command_mpegplay(NONS_ParsedLine &line);
	ErrorCode command_getini(NONS_ParsedLine &line);
	ErrorCode command_setwindow(NONS_ParsedLine &line);
	ErrorCode command_new_set_window(NONS_ParsedLine &line);
	ErrorCode command_set_default_font_size(NONS_ParsedLine &line);
	ErrorCode command_bg(NONS_ParsedLine &line);
	ErrorCode command_setcursor(NONS_ParsedLine &line);
	ErrorCode command_br(NONS_ParsedLine &line);
	ErrorCode command_ld(NONS_ParsedLine &line);
	ErrorCode command_cl(NONS_ParsedLine &line);
	ErrorCode command_tal(NONS_ParsedLine &line);
	ErrorCode command_undocumented(NONS_ParsedLine &line);
	ErrorCode command_unimplemented(NONS_ParsedLine &line);
	ErrorCode command_lsp(NONS_ParsedLine &line);
	ErrorCode command_csp(NONS_ParsedLine &line);
	ErrorCode command_unalias(NONS_ParsedLine &line);
	ErrorCode command_vsp(NONS_ParsedLine &line);
	ErrorCode command_windoweffect(NONS_ParsedLine &line);
	ErrorCode command_literal_print(NONS_ParsedLine &line);
	ErrorCode command_print(NONS_ParsedLine &line);
	ErrorCode command_delay(NONS_ParsedLine &line);
	ErrorCode command_monocro(NONS_ParsedLine &line);
	ErrorCode command_nega(NONS_ParsedLine &line);
	ErrorCode command_textonoff(NONS_ParsedLine &line);
	ErrorCode command_erasetextwindow(NONS_ParsedLine &line);
	ErrorCode command_for(NONS_ParsedLine &line);
	ErrorCode command_next(NONS_ParsedLine &line);
	ErrorCode command_break(NONS_ParsedLine &line);
	ErrorCode command_effectblank(NONS_ParsedLine &line);
	ErrorCode command_select(NONS_ParsedLine &line);
	ErrorCode command_selectcolor(NONS_ParsedLine &line);
	ErrorCode command_selectvoice(NONS_ParsedLine &line);
	ErrorCode command_trap(NONS_ParsedLine &line);
	ErrorCode command_defaultspeed(NONS_ParsedLine &line);
	ErrorCode command_clickstr(NONS_ParsedLine &line);
	ErrorCode command_click(NONS_ParsedLine &line);
	ErrorCode command_autoclick(NONS_ParsedLine &line);
	ErrorCode command_textclear(NONS_ParsedLine &line);
	ErrorCode command_locate(NONS_ParsedLine &line);
	ErrorCode command_textspeed(NONS_ParsedLine &line);
	ErrorCode command_repaint(NONS_ParsedLine &line);
	ErrorCode command_resettimer(NONS_ParsedLine &line);
	ErrorCode command_waittimer(NONS_ParsedLine &line);
	ErrorCode command_gettimer(NONS_ParsedLine &line);
	ErrorCode command_rmenu(NONS_ParsedLine &line);
	ErrorCode command_menusetwindow(NONS_ParsedLine &line);
	//ErrorCode command_kidokuskip(NONS_ParsedLine &line);
	ErrorCode command_menuselectcolor(NONS_ParsedLine &line);
	ErrorCode command_savename(NONS_ParsedLine &line);
	ErrorCode command_menuselectvoice(NONS_ParsedLine &line);
	ErrorCode command_rmode(NONS_ParsedLine &line);
	//ErrorCode command_skipoff(NONS_ParsedLine &line);
	ErrorCode command_savenumber(NONS_ParsedLine &line);
	ErrorCode command_systemcall(NONS_ParsedLine &line);
	ErrorCode command_reset(NONS_ParsedLine &line);
	ErrorCode command_btndef(NONS_ParsedLine &line);
	ErrorCode command_btn(NONS_ParsedLine &line);
	ErrorCode command_btnwait(NONS_ParsedLine &line);
	ErrorCode command_msp(NONS_ParsedLine &line);
	ErrorCode command_use_new_if(NONS_ParsedLine &line);
	ErrorCode command_getbtntimer(NONS_ParsedLine &line);
	ErrorCode command_btntime(NONS_ParsedLine &line);
	ErrorCode command_humanz(NONS_ParsedLine &line);
	ErrorCode command_sinusoidal_quake(NONS_ParsedLine &line);
	ErrorCode command_quake(NONS_ParsedLine &line);
	ErrorCode command_filelog(NONS_ParsedLine &line);
	ErrorCode command_lookbackbutton(NONS_ParsedLine &line);
	ErrorCode command_lookbackcolor(NONS_ParsedLine &line);
	ErrorCode command_lookbackflush(NONS_ParsedLine &line);
	ErrorCode command_savegame(NONS_ParsedLine &line);
	ErrorCode command_loadgame(NONS_ParsedLine &line);
	ErrorCode command_centerh(NONS_ParsedLine &line);
	ErrorCode command_centerv(NONS_ParsedLine &line);
	ErrorCode command_blt(NONS_ParsedLine &line);
	ErrorCode command_fileexist(NONS_ParsedLine &line);
	ErrorCode command_menu_full(NONS_ParsedLine &line);
	ErrorCode command_savefileexist(NONS_ParsedLine &line);
	ErrorCode command_savescreenshot(NONS_ParsedLine &line);
	ErrorCode command_savetime(NONS_ParsedLine &line);
	ErrorCode command_savetime2(NONS_ParsedLine &line);
	ErrorCode command_split(NONS_ParsedLine &line);
	ErrorCode command_isdown(NONS_ParsedLine &line);
	ErrorCode command_isfull(NONS_ParsedLine &line);
	ErrorCode command_getcursorpos(NONS_ParsedLine &line);
	ErrorCode command_textgosub(NONS_ParsedLine &line);
	ErrorCode command_ispage(NONS_ParsedLine &line);
	ErrorCode command_labellog(NONS_ParsedLine &line);
	ErrorCode command_underline(NONS_ParsedLine &line);
	/*ErrorCode command_(NONS_ParsedLine &line);
	ErrorCode command_(NONS_ParsedLine &line);
	ErrorCode command_(NONS_ParsedLine &line);
	ErrorCode command_(NONS_ParsedLine &line);
	ErrorCode command_(NONS_ParsedLine &line);
	ErrorCode command_(NONS_ParsedLine &line);*/
	//Not a command!:
	ErrorCode bad_select(NONS_ParsedLine &line);
public:
	NONS_Script *script;
	NONS_VariableStore *store;
	NONS_GFXstore *gfx_store;
	NONS_Everything *everything;
	NONS_ScriptInterpreter(NONS_Everything *everything);
	~NONS_ScriptInterpreter();
	bool interpretNextLine();
	NONS_Font *main_font;
	ErrorCode interpretString(const std::wstring &string);
	ulong totalCommands();
	ulong implementedCommands();
	bool load(int file);
	bool save(int file);
	void convertParametersToString(NONS_ParsedLine &line,std::wstring &string);
	ulong getCurrentTextgosubLevel();
	ulong insideTextgosub();
	bool goto_label(const std::wstring &label);
	bool gosub_label(const std::wstring &label);
};

ulong countLines(const std::wstring &buffer,ulong byte_pos);
#endif
