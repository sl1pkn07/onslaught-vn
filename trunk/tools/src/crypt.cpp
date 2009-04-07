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
#include <Encryption.cpp>
#include <IO_System/FileIO.cpp>

const char *methods[][2]={
	{"none","Do nothing."},
	{"xor84","Perform an XOR 0x84 on the target."},
	{"varxor","Perform XORs 0x79, 0x57, 0x0d, 0x80, and 0x04 on the target"},
	{"table","Perform a table-based transform and then an XOR 0x84. Unsupported for practical\n"
		"    reasons."},
	{0,0},
};

void usage();

void version(){
	std::cout <<"crypt v1.05\n\n"
		"Copyright (c) 2008, 2009, Helios (helios.vmg@gmail.com)\n"
		"All rights reserved."<<std::endl;
	exit(0);
}

int main(int argc,char **argv){
	if (argc<2)
		usage();
	if (!strcmp(argv[1],"--version"))
		version();
	if (argc<4 ||!strcmp(argv[1],"-h") || !strcmp(argv[1],"-?") || !strcmp(argv[1],"--help"))
		usage();
	char *ifile=argv[1];
	char *meth=argv[2];
	char *ofile=argv[3];
	long l;
	char *buffer=(char *)readfile(ifile,&l);
	if (!buffer){
		std::cout <<"File not found."<<std::endl;
		return 0;
	}
	long method=-1;
	for (ulong a=0;methods[a][0] && method==-1;a++)
		if (!strcmp(meth,methods[a][0]))
			method=a;
	if (method==-1){
		std::cout <<"Could not make sense of argument. Method defaults to xor84."<<std::endl;
		method=XOR84_ENCRYPTION;
	}
	if (argc>4 && !strcmp(argv[4],"-l")){
		char *rpointer=buffer,
			*wpointer=buffer;
		for (;*rpointer;rpointer++,wpointer++){
			if (*rpointer==13){
				*wpointer=10;
				if (rpointer[1]==10)
					rpointer++;
			}else
				*wpointer=*rpointer;
		}
		l-=rpointer-wpointer;
	}
	inPlaceDecryption(buffer,l,method);
	if (writefile(ofile,buffer,l))
		std::cout <<"Writing to file failed."<<std::endl;
	delete[] buffer;
	return 0;
}

void usage(){
	std::cout <<"Usage: crypt <input file> <method> <output file> [-l]\n"
		"\n"
		"Available methods:\n"<<std::endl;
	for (short a=0;methods[a][0];a++)
		std::cout <<methods[a][0]<<" - "<<methods[a][1]<<std::endl;
	std::cout <<"\nXOR encryption is symmetric, so the same algorithm is used both for encryption\n"
		"and decryption.\n\n"
		"-l option:\n"
		"    Convert all newlines to LF (UNIX style). The following are recognized as\n"
		"    newlines: LF, CR (Mac style), CRLF (DOS style).\n"
		"    USE ONLY WHEN ENCRYPTING. USING WHILE DECRYPTING MAY PRODUCE DATA LOSS.\n"
		"    This option is included solely for backwards compatibility with an incorrect\n"
		"    behavior in NSDEC.exe. See http://forums.novelnews.net/showpost.php?p=61427&postcount=55\n"
		"    for details."<<std::endl;
 	exit(0);
}

