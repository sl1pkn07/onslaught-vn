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

#ifndef NONS_ARCHIVE_H
#define NONS_ARCHIVE_H

#include "../../Common.h"
#include "TreeNode.h"

#define READ_LENGTH 4096

struct NONS_Archive{
	NONS_TreeNode *root;
	std::ifstream *file;
	char *path;
	ulong archive_type;
	enum{
		UNRECOGNIZED=0,
		SAR_ARCHIVE=1,
		NSA_ARCHIVE=2,
		NS2_ARCHIVE=3,
		NS3_ARCHIVE=4
	};
	bool loaded;
	NONS_Archive(const char *filename,bool failSilently);
	~NONS_Archive();
	/*
	1 if the archive has been loaded or if it had already been loaded.
	0 if it failed to load.
	*/
	bool readArchive();
	bool readSAR();
	bool readNSA();
	uchar *getFileBuffer(NONS_TreeNode *node,ulong *buffersize);
	uchar *getFileBuffer(const wchar_t *filepath,ulong *buffersize);
};
#endif
