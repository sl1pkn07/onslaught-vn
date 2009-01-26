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

#ifndef NONS_SAVEFILE_H
#define NONS_SAVEFILE_H

#include "../Common.h"
#include "../ErrorCodes.h"
#include "../Processing/Variable.h"
#include <vector>
#include <ctime>
#include <SDL/SDL.h>
#include "../UTF.h"

#define NONS_SAVEFILE_VERSION 1

std::vector<tm *> *existing_files(char *location="./");
std::vector<tm *> *existing_files(wchar_t *location=L"./");
char *getConfigLocation();
char *getSaveLocation(unsigned hash[5]);

struct NONS_SaveFile{
	char format;
	ushort version;
	ErrorCode error;
	bool boldFont;
	bool fontShadow;
	bool rmode;
	SDL_Color windowTextColor;
	wchar_t *arrowCursorString;
	bool arrowCursorAbs;
	long arrowCursorX,
		arrowCursorY;
	wchar_t *pageCursorString;
	bool pageCursorAbs;
	long pageCursorX,
		pageCursorY;
	ulong windowTransition;
	ulong windowTransitionDuration;
	wchar_t *windowTransitionRule;
	SDL_Rect windowFrame;
	ulong windowFrameColumns,
		windowFrameRows;
	ulong windowFontWidth,
		windowFontHeight;
	ulong spacing;
	short lineSkip;
	SDL_Color windowColor;
	bool transparentWindow;
	bool hideWindow;
	bool hideWindow2;
	ulong textSpeed;
	SDL_Rect textWindow;
	wchar_t *unknownString_000;
	wchar_t *background;
	wchar_t *leftChar;
	wchar_t *centChar;
	wchar_t *righChar;
	struct Sprite{
		wchar_t *string;
		long x,y;
		bool visibility;
		uchar alpha;
		ulong animOffset;
		Sprite();
		~Sprite();
	};
	std::vector<Sprite *> sprites;
	std::vector<NONS_Variable *> variables;
	ushort fontSize;
	struct stackEl{
		bool type;
		wchar_t *label;
		ulong offset;
		ulong variable;
		long to;
		long step;
		wchar_t *leftovers;
		stackEl();
		~stackEl();
	};
	std::vector<stackEl *> stack;
	bool monochrome;
	SDL_Color monochromeColor;
	bool negative;
	wchar_t *midi;
	wchar_t *wav;
	wchar_t *music;
	long musicTrack;
	bool loopMidi,
		loopWav,
		playOnce,
		loopMp3,
		saveMp3;
	wchar_t *btnDef;
	wchar_t *loopBGM0,
		*loopBGM1;
	std::vector<wchar_t *> logPages;
	ulong currentLine;
	ulong currentSubline;
	wchar_t *currentLabel;
	ulong currentOffset;
	unsigned hash[5];
	std::vector<wchar_t *> arraynames;
	std::vector<NONS_Variable *> arrays;
	wchar_t *currentBuffer;
	ushort textX,
		textY;
	SDL_Color bgColor;
	ulong spritePriority;
	uchar musicVolume;
	struct Channel{
		wchar_t *name;
		bool loop;
		uchar volume;
		Channel();
		~Channel();
	};
	std::vector<Channel *> channels;
	NONS_SaveFile();
	~NONS_SaveFile();
	void load(char *filename);
	bool save(char *filename);
};
#endif
