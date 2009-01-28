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

#define BARE_FILE
#include <UTF.cpp>
#include <iostream>

enum{
	AUTO_ENCODING=0,
	UCS2_ENCODING=1,
	UCS2L_ENCODING=2,
	UCS2B_ENCODING=3,
	UTF8_ENCODING=4,
	ISO_8859_1_ENCODING=5,
	SJIS_ENCODING=6
};

const char *encodings[][2]={
	{"auto","Attempt to automatically determine the encoding"},
	{"ucs2","UCS-2 (auto endianness)"},
	{"ucs2l","UCS-2 (little endian)"},
	{"ucs2b","UCS-2 (big endian)"},
	{"utf8","UTF-8"},
	{"iso-8859-1","ISO-8859-1"},
	{"sjis","Shift JIS"},
	{0,0},
};

void usage();

int main(int argc,char **argv){
	if (argc>1 && !strcmp(argv[1],"--help") || argc<5)
		usage();
	char *ienc=argv[1];
	char *ifile=argv[2];
	char *oenc=argv[3];
	char *ofile=argv[4];
	long inputEncoding=-1,outputEncoding=-1;
	for (ulong a=0;encodings[a][0] && inputEncoding==-1;a++)
		if (!strcmp(ienc,encodings[a][0]))
			inputEncoding=a;
	if (inputEncoding==-1){
		std::cout <<"Could not make sense of argument. Input encoding defaults to auto."<<std::endl;
		inputEncoding=AUTO_ENCODING;
	}
	long l;
	char *buffer=(char *)readfile(ifile,&l);
	if (!buffer){
		std::cout <<"File not found."<<std::endl;
		return 0;
	}
	wchar_t *middleBuffer=0;
switchInputEncoding:
	if (inputEncoding!=AUTO_ENCODING){
		if ((uchar)buffer[0]==BOM8A && (uchar)buffer[1]==BOM8B && (uchar)buffer[2]==BOM8C && inputEncoding!=UTF8_ENCODING)
			std::cout <<"WARNING: The file appears to be a UTF-8."<<std::endl;
		else if ((uchar)buffer[0]==BOM16BA && (uchar)buffer[1]==BOM16BB && inputEncoding==UCS2L_ENCODING)
			std::cout <<"WARNING: The file appears to be a big endian UCS-2."<<std::endl;
		else if ((uchar)buffer[0]==BOM16LA && (uchar)buffer[1]==BOM16LB && inputEncoding==UCS2B_ENCODING)
			std::cout <<"WARNING: The file appears to be a little endian UCS-2."<<std::endl;
	}
	switch (inputEncoding){
		case AUTO_ENCODING:
			if ((uchar)buffer[0]==BOM16BA && (uchar)buffer[1]==BOM16BB || (uchar)buffer[0]==BOM16LA && (uchar)buffer[1]==BOM16LB){
				inputEncoding=UCS2_ENCODING;
				goto switchInputEncoding;
			}
			if ((uchar)buffer[0]==BOM8A && (uchar)buffer[1]==BOM8B && (uchar)buffer[2]==BOM8C){
				inputEncoding=UTF8_ENCODING;
				goto switchInputEncoding;
			}
			std::cout <<"Could not determine input encoding. Terminating."<<std::endl;
			return 0;
		case UCS2_ENCODING:
			middleBuffer=UCS2_to_WChar(buffer,l,&l);
			break;
		case UCS2L_ENCODING:
			middleBuffer=UCS2_to_WChar(buffer,l,&l,NONS_LITTLE_ENDIAN);
			break;
		case UCS2B_ENCODING:
			middleBuffer=UCS2_to_WChar(buffer,l,&l,NONS_BIG_ENDIAN);
			break;
		case UTF8_ENCODING:
			middleBuffer=UTF8_to_WChar(buffer,l,&l);
			break;
		case ISO_8859_1_ENCODING:
			middleBuffer=ISO88591_to_WChar(buffer,l,&l);
			break;
		case SJIS_ENCODING:
			middleBuffer=SJIS_to_WChar(buffer,l,&l);
			break;
	}
	delete[] buffer;
	for (ulong a=0;encodings[a][0] && outputEncoding==-1;a++)
		if (!strcmp(oenc,encodings[a][0]))
			outputEncoding=a;
	if (outputEncoding==-1){
		std::cout <<"Could not make sense of argument. Output encoding defaults to auto."<<std::endl;
		outputEncoding=AUTO_ENCODING;
	}
switchOutputEncoding:
	switch (outputEncoding){
		case AUTO_ENCODING:
			{
				bool canbeISO=1;
				for (long a=0;a<l && canbeISO;a++)
					if (middleBuffer[a]>0xFF)
						canbeISO=0;
				long UTF8size=getUTF8size(middleBuffer,l);
				long ucs2size=l*2+2;
				if (canbeISO && float(UTF8size)/float(l)>1.25){
					outputEncoding=ISO_8859_1_ENCODING;
					goto switchOutputEncoding;
				}else if (UTF8size<ucs2size){
					outputEncoding=UTF8_ENCODING;
					goto switchOutputEncoding;
				}
				outputEncoding=UCS2_ENCODING;
				goto switchOutputEncoding;
			}
		case UCS2_ENCODING:
			buffer=WChar_to_UCS2(middleBuffer,l,&l);
			break;
		case UCS2L_ENCODING:
			buffer=WChar_to_UCS2(middleBuffer,l,&l,NONS_LITTLE_ENDIAN);
			break;
		case UCS2B_ENCODING:
			buffer=WChar_to_UCS2(middleBuffer,l,&l,NONS_BIG_ENDIAN);
			break;
		case UTF8_ENCODING:
			buffer=WChar_to_UTF8(middleBuffer,l,&l);
			break;
		case ISO_8859_1_ENCODING:
			buffer=WChar_to_ISO88591(middleBuffer,l,&l);
			break;
		case SJIS_ENCODING:
			buffer=WChar_to_SJIS(middleBuffer,l,&l);
			break;
	}
	delete[] middleBuffer;
	if (writefile(ofile,buffer,l))
		std::cout <<"Writing to file failed."<<std::endl;
	delete[] buffer;
	return 0;
}

void usage(){
	std::cout <<"Usage: recoder <input encoding> <input file> <output encoding> <output file>\n"
	            "\n"
	            "Available encodings:\n"<<std::endl;
	for (short a=0;encodings[a][0];a++)
		std::cout <<encodings[a][0]<<" - "<<encodings[a][1]<<std::endl;
	exit(0);
}
