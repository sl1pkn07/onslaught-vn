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

#ifndef NONS_STDOUT_H
#define NONS_STDOUT_H

#include "../Common.h"
#include "../CommandLineOptions.h"
#include <fstream>
#include <SDL/SDL.h>

extern NONS_CommandLineOptions CLOptions;

struct NONS_RedirectedOutput{
	std::ofstream *file;
	std::ostream &cout;
	NONS_RedirectedOutput(std::ostream &a);
	~NONS_RedirectedOutput();
	template <typename T> NONS_RedirectedOutput &operator<<(const T &a){
		if (CLOptions.override_stdout && this->file)
			*this->file <<a;
		else
			this->cout <<a;
		return *this;
	}
	void redirect();
	std::ostream &getstream();
};

template <> NONS_RedirectedOutput &NONS_RedirectedOutput::operator<< <wchar_t *>(wchar_t * const &a);
template <> NONS_RedirectedOutput &NONS_RedirectedOutput::operator<< <std::wstring>(const std::wstring &a);
#endif