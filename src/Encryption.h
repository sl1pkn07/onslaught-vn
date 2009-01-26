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

#ifndef NONS_ENCRYPTION_H
#define NONS_ENCRYPTION_H

//This macro is defined when an external program includes this header.
#ifndef BARE_FILE
#include "Common.h"
#include "ErrorCodes.h"
#else
#include <iostream>
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef ulong ErrorCode;

enum ENCRYPTION{
	NO_ENCRYPTION=0,
	XOR84_ENCRYPTION=1,
	VARIABLE_XOR_ENCRYPTION=2,
	TRANSFORM_THEN_XOR84_ENCRYPTION=3
};

#define NONS_FATAL_ERROR							0x40000000
#define NONS_BREAK_WORTHY_ERROR						0x20000000
#define NONS_NO_ERROR_FLAG							0x10000000
#define NONS_INTERNAL_ERROR							(0x08000000|NONS_FATAL_ERROR)
#define NONS_SYNTAX_ERROR							0x04000000
#define NONS_WARNING								0x02000000
#define NONS_END									0x01000000
#define NONS_UNDEFINED_ERROR						0x00010000

#define NONS_NO_ERROR								(0|NONS_NO_ERROR_FLAG)
#define NONS_NOT_IMPLEMENTED						4
#define NONS_INTERNAL_INVALID_PARAMETER				(22|NONS_INTERNAL_ERROR)
#endif

ErrorCode inPlaceDecryption(char *buffer,long length,ulong mode,char *filename=0);
ErrorCode inPlaceEncryption(char *buffer,long length,ulong mode,char *filename=0);
#endif
