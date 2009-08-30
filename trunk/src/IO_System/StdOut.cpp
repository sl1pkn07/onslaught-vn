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

#ifndef NONS_STDOUT_CPP
#define NONS_STDOUT_CPP

#include "StdOut.h"
#include "../UTF.h"
#include "../Functions.h"
#include <sstream>
#include <ctime>

NONS_RedirectedOutput::NONS_RedirectedOutput(std::ostream &a)
		:cout(a){
	this->file=0;
	this->indentation=0;
	this->addIndentationNext=1;
}

NONS_RedirectedOutput::~NONS_RedirectedOutput(){
	if (this->file)
		this->file->close();
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(ulong a){
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	if (this->addIndentationNext)
		for (ulong a=0;a<this->indentation;a++)
			stream <<INDENTATION_STRING;
	this->addIndentationNext=0;
	stream <<a;
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::outputHex(ulong a,ulong w){
	std::stringstream s;
	s.width(w);
	s <<s.hex<<a;
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	stream <<s.str();
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(long a){
	return *this <<(ulong)a;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(wchar_t a){
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	if (this->addIndentationNext)
		for (ulong a=0;a<this->indentation;a++)
			stream <<INDENTATION_STRING;
	this->addIndentationNext=0;
	stream <<a;
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(const char *a){
	return *this <<std::string(a);
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(const std::string &a){
	std::ostream &stream=(CLOptions.override_stdout && this->file)?*this->file:this->cout;
	for (ulong b=0;b<a.size();b++){
		char c=a[b];
		if (this->addIndentationNext)
			for (ulong d=0;d<this->indentation;d++)
				stream <<INDENTATION_STRING;
		if (c=='\n')
			this->addIndentationNext=1;
		else
			this->addIndentationNext=0;
		stream <<c;
	}
	return *this;
}

NONS_RedirectedOutput &NONS_RedirectedOutput::operator<<(const std::wstring &a){
	return *this <<UniToUTF8(a);
}

/*std::ostream &NONS_RedirectedOutput::getstream(){
	if (CLOptions.override_stdout)
		return *(this->file);
	else
		return this->cout;
}*/

void NONS_RedirectedOutput::redirect(){
	if (!!this->file)
		delete this->file;
	if (!CLOptions.override_stdout){
		this->file=0;
		return;
	}
	if (this->cout==std::cout)
		this->file=new std::ofstream("stdout.txt",CLOptions.reset_redirection_files?std::ios::trunc:std::ios::app);
	else if (this->cout==std::cerr)
		this->file=new std::ofstream("stderr.txt",CLOptions.reset_redirection_files?std::ios::trunc:std::ios::app);
	else
		this->file=new std::ofstream("stdlog.txt",CLOptions.reset_redirection_files?std::ios::trunc:std::ios::app);
	if (!this->file->is_open())
		delete this->file;
	else if (!CLOptions.reset_redirection_files){
		time_t secs=time(0);
		tm *t=localtime(&secs);
		std::string str=
			itoa<char>(t->tm_year+1900,4)+"-"+
			itoa<char>(t->tm_mon+1,4)+"-"+
			itoa<char>(t->tm_mday,4)+" "+
			itoa<char>(t->tm_hour,4)+":"+
			itoa<char>(t->tm_min,4)+":"+
			itoa<char>(t->tm_sec,4);
		*this->file <<"\n\n"
			"--------------------------------------------------------------------------------\n"
			"Session "<<str<<"\n"
			"--------------------------------------------------------------------------------"<<std::endl;
	}
}

void NONS_RedirectedOutput::indent(long a){
	if (!a)
		return;
	if (a<0){
		if (ulong(-a)>this->indentation)
			this->indentation=0;
		else
			this->indentation-=-a;
	}else
		this->indentation+=a;
}
#endif
