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
#include "../../ErrorCodes.h"
#include <string>
#include <vector>

struct NONS_TreeNode{
	struct NONS_ArchivedFile{
		std::wstring name;
#ifdef TOOLS_NSAIO
		std::wstring archivepath,
			filepath;
#endif
		ulong compression_type;
		enum{
			NO_COMPRESSION=0,
			SPB_COMPRESSION=1,
			LZSS_COMPRESSION=2,
			NBZ_COMPRESSION=4
		};
		ulong offset;
		ulong length;
		long original_length;
		NONS_ArchivedFile(){
			this->offset=0;
			this->length=0;
			this->original_length=0;
			this->compression_type=NO_COMPRESSION;
		}
	} data;
	std::vector<NONS_TreeNode *> branches;
	NONS_TreeNode(const std::wstring &name);
	~NONS_TreeNode();
	NONS_TreeNode *getBranch(const std::wstring &name,bool createIfMissing);
	void sort();
	void vectorizeFiles(std::vector<NONS_TreeNode *> &vector);
private:
	NONS_TreeNode *newBranch(const std::wstring &name);
};

#define READ_LENGTH 4096

struct NONS_Archive{
	NONS_TreeNode *root;
	std::ifstream *file;
	std::wstring path;
	ulong archive_type;
	enum{
		UNRECOGNIZED=0,
		SAR_ARCHIVE=1,
		NSA_ARCHIVE=2,
		NS2_ARCHIVE=3,
		NS3_ARCHIVE=4
	};
	bool loaded;
	NONS_Archive(const std::wstring &filename,bool failSilently);
#ifdef TOOLS_NSAIO
	NONS_Archive(ulong archive_type);
#endif
	~NONS_Archive();
	/*
	1 if the archive has been loaded or if it had already been loaded.
	0 if it failed to load.
	*/
	bool readArchive();
	bool readSAR();
	bool readNSA();
	uchar *getFileBuffer(NONS_TreeNode *node,ulong &buffersize);
	uchar *getFileBuffer(const std::wstring &filepath,ulong &buffersize);
	bool exists(const std::wstring &filepath);
};

struct NONS_GeneralArchive{
	NONS_Archive *archive;
	std::vector<NONS_Archive *> NSAarchives;
	NONS_GeneralArchive();
	~NONS_GeneralArchive();
	ErrorCode init(const std::wstring &filename,bool which,bool failSilently);
	uchar *getFileBuffer(const std::wstring &filepath,ulong &buffersize);
	bool exists(const std::wstring &filepath);
};
#endif
