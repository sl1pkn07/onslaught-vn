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

#ifndef NONS_ENCRYPTION_CPP
#define NONS_ENCRYPTION_CPP

#include "Encryption.h"

#ifndef BARE_FILE
#include "Globals.h"
#include "IO_System/FileIO.h"
#endif

ErrorCode inPlaceDecryption(char *buffer,long length,ulong mode,char *filename){
	switch (mode){
		case NO_ENCRYPTION:
		default:
			return NONS_NO_ERROR;
		case XOR84_ENCRYPTION:
			for (long a=0;a<length;a++)
				buffer[a]^=0x84;
			return NONS_NO_ERROR;
		case VARIABLE_XOR_ENCRYPTION:
			{
				char magic_numbers[5]={0x79,0x57,0x0d,0x80,0x04};
				ulong index=0;
				for (long a=0;a<length;a++){
					buffer[a]^=magic_numbers[index];
					index=(index+1)%5;
				}
				return NONS_NO_ERROR;
			}
		case TRANSFORM_THEN_XOR84_ENCRYPTION:
			{
#ifndef BARE_FILE
				v_stderr
#else
				std::cerr
#endif
				<<"TRANSFORM_THEN_XOR84 (aka mode 4) encryption not implemented for a very good\n\
					reason. Which I, of course, don\'t need to explain to you. Good day to you.";
				return NONS_NOT_IMPLEMENTED;

				/*char table[256];
				char ring_buffer[256];
				for (short a=0;a<256;a++)
					table[a]=a;
				if (!filename)
					return NONS_INTERNAL_INVALID_PARAMETER;
				long l;
				uchar *filebuffer=readfile(filename,&l);
				long i,ring_start=0,ring_end=0;
				long a=0;
				for (;a<l;a++){
					i=ring_start;
					long count=0;
					while (i!=ring_end && ring_buffer[i]!=filebuffer[a]){
						count++;
						i=(i+1)%256;
					}
					if (i==ring_end && count==255)
						break;
					if (i!=ring_end)
						ring_start=(i+1)%256;
					ring_buffer[ring_end]=filebuffer[a];
					ring_end=(ring_end+1)%256;
				}
				ring_buffer[ring_end]=filebuffer[a];
				delete[] filebuffer;
				for (a=0;a<256;a++)
					table[(ulong)ring_buffer[(ring_start+a)%256]]=i;
				for (long a=0;a<length;a++)
					buffer[a]=table[(ulong)buffer[a]]^0x84;
				return NONS_NO_ERROR;*/
			}
	}
	return NONS_NO_ERROR;
}

ErrorCode inPlaceEncryption(char *buffer,long length,ulong mode,char *filename){
	return NONS_NO_ERROR;
}
#endif
