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

#include <cstring>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <algorithm>

#include "NONS_src/UTF.h"
#include "NONS_src/IO_System/SAR/Archive.h"
#include "NONS_src/IO_System/FileIO.h"
#include "NONS_src/Functions.h"

#if WCHAR_MAX<0xFFFF
#error "Wide characters on this platform are too narrow."
#endif

bool is_directory(const char *path){
	return boost::filesystem::is_directory(boost::filesystem::path(path));
}

bool is_directory(const wchar_t *path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::is_directory(boost::filesystem::wpath(path));
#else
	return is_directory(UniToUTF8(path));
#endif
}

bool is_directory(const std::string &path){
	return boost::filesystem::is_directory(boost::filesystem::path(path));
}

bool is_directory(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::is_directory(boost::filesystem::wpath(path));
#else
	return is_directory(UniToUTF8(path));
#endif
}

bool create_directory(const std::string &path){
	return boost::filesystem::create_directory(boost::filesystem::path(path));
}

bool create_directory(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::create_directory(boost::filesystem::wpath(path));
#else
	return create_directory(UniToUTF8(path));
#endif
}

bool file_exists(const std::string &path){
	return boost::filesystem::exists(boost::filesystem::path(path));
}

bool file_exists(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::exists(boost::filesystem::wpath(path.c_str()));
#else
	return file_exists(UniToUTF8(path));
#endif
}

boost::uintmax_t file_size(const std::string &path){
	return boost::filesystem::file_size(boost::filesystem::path(path.c_str()));
}

boost::uintmax_t file_size(const std::wstring &path){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	return boost::filesystem::file_size(boost::filesystem::wpath(path.c_str()));
#else
	return file_size(UniToUTF8(path));
#endif
}

bool createDirectory(std::wstring &name){
	if (!create_directory(name) && !file_exists(name))
		return 0;
	return 1;
}

bool output(NONS_Archive &archive,std::vector<NONS_TreeNode *> &stack,bool write=1){
	std::wstring path(L".");
	for (ulong a=0;a<stack.size();a++){
		path.push_back('/');
		path.append(stack[a]->data.name);
	}
	if (write && !createDirectory(path))
		return 0;
	std::vector<NONS_TreeNode *> &branches=stack.back()->branches;
	for (ulong a=0;a<branches.size();a++){
		NONS_TreeNode *node=branches[a];
		if (node->branches.size()){
			stack.push_back(node);
			if (!output(archive,stack,write))
				return 0;
			stack.pop_back();
		}else{
			ulong l;
			uchar *buffer;
			if (write)
				buffer=archive.getFileBuffer(node,l);
			std::wstring filename=path;
			filename.push_back('/');
			filename.append(node->data.name);
			{
				std::string temp=UniToUTF8(filename.c_str());
				if (write)
					std::cout <<"Writing ";
				std::cout <<"\""<<temp<<"\""<<std::endl;
			}
			if (write){
				writefile(filename,(char *)buffer,(long)l);
				delete[] buffer;
			}
		}
	}
	return 1;
}

enum MODE{
	LIST=0,
	EXTRACT=1,
	CREATE=2
};

long match(const char *string,const char **options){
	for (long a=0;*options;options++,a++)
		if (!strcmp(string,*options))
			return a;
	return -1;
}

void findfiles(const std::wstring &dir_path,std::vector<std::wstring> &vec){
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
	boost::filesystem::wdirectory_iterator end_itr;
	for (boost::filesystem::wdirectory_iterator itr(dir_path);itr!=end_itr;itr++){
		std::wstring filename=itr->path().string();
		if (!boost::filesystem::is_directory(filename))
			vec.push_back(filename);
		else
			findfiles(filename,vec);
	}
#else
	boost::filesystem::directory_iterator end_itr;
	char *str=copyString(dir_path.c_str());
	for (boost::filesystem::directory_iterator itr(str);itr!=end_itr;itr++){
		std::string filename=itr->path().string();
		wchar_t *wstr=copyWString(filename.c_str());
		if (!is_directory(filename))
			vec.push_back(new std::wstring(wstr));
		else{
			findfiles(std::wstring(wstr),vec);
		}
		delete[] wstr;
	}
#endif
}

void resolveFilenames(std::vector<std::wstring> &filelist,std::vector<bool> &removes){
	std::vector<std::wstring> res;
	std::vector<bool> res2;
	for (ulong a=0;a<filelist.size();a++){
		if (is_directory(filelist[a]))
			findfiles(filelist[a],res);
		else
			res.push_back(filelist[a]);
		while (res.size()>res2.size())
			res2.push_back(removes[a]);
	}
	filelist.clear();
	filelist=res;
	removes.clear();
	removes=res2;
}

template <class T0,class T1>
bool like(const T0 *string,const T1 *pattern){
	switch (pattern[0]){
		case '\0':
			return !string[0];
		case '*':
			return like(string,pattern+1) || string[0] && like(string+1,pattern);
		case '?':
			return string[0] && like(string+1,pattern+1);
		default:
			return (pattern[0] == string[0]) && like(string+1,pattern+1);
	}
}

void usage(){
	std::cout <<"nsaio <mode> <options> <files>\n"
		"\n"
		"MODES:\n"
		"    e - Extract\n"
		"    c - Create\n"
		"    l - List (same as extract but doesn't write to the file system)\n"
		"\n"
		"OPTIONS:\n"
		"    -o <file name>\n"
		"        Output file name.\n"
		"        If used for extraction, the archives will be extracted to the same\n"
		"        directory in the same order the engine would read them (arc.sar,\n"
		"        arc.nsa, arc1.nsa, ..., arc9.nsa, and then all other file names than\n"
		"        don't match).\n"
		"        If used for creation, this sets the name of the resulting archive.\n"
		"        Set to \"arc.sar\" by default.\n"
		"    -f {sar|nsa}\n"
		"        Sets the format of the created archive. If not used, the format will be\n"
		"        guessed from the file name passed to -o.\n"
		"    -nbz <wildcard pattern>\n"
		"    -lzss <wildcard pattern>\n"
		"    -spb <wildcard pattern>\n"
		"        Sets the compression type as NBZ (BZ2), LZSS, and SPB respectively for\n"
		"        all files matching the wildcard pattern.\n"
		"        This automatically sets the format to nsa. If -o was not used, it's set\n"
		"        to \"arc.nsa\".\n"
		"    -r <directory>\n"
		"        Only used for creating new archives.\n"
		"        It will cause the packer to put the files and subdirectories inside the\n"
		"        directory in the root of the archive, instead of of inside the\n"
		"        directory.\n"
		"        Example:\n"
		"        Let's say we have the following directory structure:\n"
		"            ./arc_sar/\n"
		"            ./arc_sar/0.bmp\n"
		"            ./arc_sar/icon/\n"
		"            ./arc_sar/image/\n"
		"            ./arc_sar/wave/\n"
		"        If we call the packer like this:\n"
		"        nsaio c arc_sar\n"
		"        the archive will have the following structure:\n"
		"            /arc_sar/\n"
		"            /arc_sar/0.bmp\n"
		"            /arc_sar/icon/\n"
		"            /arc_sar/image/\n"
		"            /arc_sar/wave/\n"
		"        but if we call it like this:\n"
		"        nsaio c -r arc_sar\n"
		"        the archive will have the following structure:\n"
		"            /\n"
		"            /0.bmp\n"
		"            /icon/\n"
		"            /image/\n"
		"            /wave/\n"
		"        Note that the option applies only to the directory that comes after it.\n";
	exit(0);
}

ulong compare(uchar *current,uchar *test,ulong max){
	for (ulong a=0;a<max;a++){
		if (current[a]!=test[a] || test+a>=current)
			return a;
	}
	return max;
}

void writeBits(uchar *buffer,ulong *byteOffset,ulong *bitOffset,ulong data,ushort s){
	ulong byteo=*byteOffset,
		bito=*bitOffset;
	data<<=(sizeof(data)*8-s)-bito;
	buffer+=byteo;
	for (ushort a=0;a<sizeof(data) && data;a++){
		*(buffer++)|=data>>(sizeof(data)-1)*8;
		data<<=8;
	}
	bito+=s;
	*bitOffset=bito%8;
	byteo+=bito/8;
	*byteOffset=byteo;
}

//Default:
const ulong NN=8,
	MAXS=4;

std::string encode_LZSS(char *buffer,ulong decompressedSize){
	ulong window_bits=NN,
		window_size=1<<window_bits,
		max_string_bits=MAXS,
		threshold=1+window_bits+max_string_bits,
		max_string_len=(1<<max_string_bits);
	threshold=threshold/8+!!(threshold%8);
	max_string_len+=threshold-1;
	uchar *unsigned_buffer=(uchar *)buffer;
	ulong res_size=decompressedSize+decompressedSize/8+!!(decompressedSize%8);
	uchar *res=new uchar[res_size];
	memset(res,0,res_size);
	memset(res,0,decompressedSize);
	ulong byte=0,bit=0;
	std::vector<ulong> tree[256];
	ulong starts[256];
	ulong offset=window_size-max_string_len;
	for (ulong a=0;a<decompressedSize;a++)
		tree[unsigned_buffer[a]].push_back(a);
	memset(starts,0,256*sizeof(ulong));
	for (ulong a=0;a<decompressedSize;){
		if (byte==0x3a04)
			byte=byte;
		long found=-1,max=-1;
		ulong found_size=0;
		for (std::vector<ulong>::iterator i2=tree[unsigned_buffer[a]].begin()+starts[unsigned_buffer[a]];i2!=tree[unsigned_buffer[a]].end();i2++){
			if (a>=window_size && *i2<a-window_size){
				starts[unsigned_buffer[a]]++;
				continue;
			}
			if (*i2>=a)
				break;
			found_size=compare(unsigned_buffer+a,unsigned_buffer+*i2,max_string_len);
			if (found_size<threshold)
				continue;
			if (max<0 || found_size>(ulong)max){
				found=*i2;
				max=found_size;
				if (max==max_string_len)
					break;
			}
		}
		found_size=max;
		if (found<0){
			writeBits(res,&byte,&bit,1,1);
			writeBits(res,&byte,&bit,unsigned_buffer[a],8);
			a++;
		}else{
			writeBits(res,&byte,&bit,0,1);
			ulong pos=(found+offset)%window_size;
			writeBits(res,&byte,&bit,pos,window_bits);
			writeBits(res,&byte,&bit,found_size-threshold,max_string_bits);
			a+=found_size;
		}
	}
	byte+=bit?1:0;
	std::string res_buf((const char *)res,(size_t)byte);
	delete[] res;
	return res_buf;
}

char *decode_LZSS(char *buffer,ulong compressedSize,ulong decompressedSize){
	uchar decompression_buffer[256*2];
	ulong decompresssion_buffer_offset=239;
	memset(decompression_buffer,0,239);
	uchar *res=new uchar[decompressedSize];
	ulong byteoffset=0;
	uchar bitoffset=0;
	for (ulong len=0;len<decompressedSize;){
		if (getbit((uchar *)buffer,&byteoffset,&bitoffset)){
			uchar a=getbits((uchar *)buffer,8,&byteoffset,&bitoffset);
			res[len++]=a;
			decompression_buffer[decompresssion_buffer_offset++]=a;
			decompresssion_buffer_offset&=0xFF;
		}else{
			uchar a=getbits((uchar *)buffer,8,&byteoffset,&bitoffset);
			uchar b=getbits((uchar *)buffer,4,&byteoffset,&bitoffset);
			for (long c=0;c<=b+1 && len<decompressedSize;c++){
				uchar d=decompression_buffer[(a+c)&0xFF];
				res[len++]=d;
				decompression_buffer[decompresssion_buffer_offset++]=d;
				decompresssion_buffer_offset&=0xFF;
			}
		}
	}
	return (char *)res;
}

char *decode_LZSS2(char *buffer,ulong compressedSize,ulong decompressedSize){
	ulong window_bits=NN,
		window_size=1<<window_bits,
		max_string_bits=MAXS,
		threshold=1+window_bits+max_string_bits,
		max_string_len=(1<<max_string_bits);
	threshold=threshold/8+!!(threshold%8);
	max_string_len+=threshold-1;
	ulong offset=window_size-max_string_len;

	uchar *decompression_buffer=new uchar[window_size];
	ulong decompression_buffer_offset=offset;
	memset(decompression_buffer,0,window_size);
	uchar *res=new uchar[decompressedSize];
	ulong byteoffset=0;
	uchar bitoffset=0;
	ulong len;
	for (len=0;len<decompressedSize;){
		if (getbit((uchar *)buffer,&byteoffset,&bitoffset)){
			uchar a=getbits((uchar *)buffer,8,&byteoffset,&bitoffset);
			res[len++]=a;
			decompression_buffer[decompression_buffer_offset++]=a;
			decompression_buffer_offset%=window_size;
		}else{
			ulong a=getbits((uchar *)buffer,window_bits,&byteoffset,&bitoffset);
			ulong b=getbits((uchar *)buffer,max_string_bits,&byteoffset,&bitoffset)+threshold;
			for (ulong c=0;c<b;c++){
				uchar d=decompression_buffer[(a+c)%window_size];
				res[len++]=d;
				decompression_buffer[decompression_buffer_offset++]=d;
				decompression_buffer_offset%=window_size;
			}
		}

	}
	delete[] decompression_buffer;
	return (char *)res;
}

std::string encode_NBZ(char *buffer,long decompressedSize){
	std::string res;
	writeDWordBig(decompressedSize,res);
	ulong compressedSize;
	char *compressed=compressBuffer_BZ2(buffer,decompressedSize,&compressedSize);
	res.append(compressed,compressedSize);
	return res;
}

int main(int argc,char **argv){
	if (argc<2){
		std::cout <<"Not enough arguments.";
		usage();
		return 0;
	}
	MODE mode=LIST;
	const char *options[]={
		"l",
		"e",
		"c",
		"--help",
		"-h",
		"-?",
		"-o",
		"-f",
		"-nbz",
		"-lzss",
		"-spb",
		"-r",
		0
	};
	std::vector<std::wstring> filelist;
	std::vector<bool> removeslist;
	std::set<std::wstring> NBZlist,LZSSlist,SPBlist;
	std::wstring outputfile;
	ulong outputformat=NONS_Archive::UNRECOGNIZED;
	bool forcensa=0;
	for (argv++;*argv;argv++){
		long option=match(*argv,options);
		switch (option){
			case 0:
				mode=LIST;
				break;
			case 1:
				mode=EXTRACT;
				break;
			case 2:
				mode=CREATE;
				break;
			case 3:
			case 4:
			case 5:
				usage();
			case 6:
				{
					argv++;
					outputfile=UniFromUTF8(*argv);
				}
				break;
			case 7:
				argv++;
				if (!strcmp(*argv,"nsa"))
					outputformat=NONS_Archive::NSA_ARCHIVE;
				else
					outputformat=NONS_Archive::SAR_ARCHIVE;
				break;
			case 8:
				{
					argv++;
					std::wstring wtemp(UniFromUTF8(*argv));
					NBZlist.insert(wtemp);
					std::set<std::wstring>::iterator lzss=LZSSlist.find(wtemp);
					if (lzss!=LZSSlist.end())
						LZSSlist.erase(lzss);
					std::set<std::wstring>::iterator spb=SPBlist.find(wtemp);
					if (spb!=SPBlist.end())
						SPBlist.erase(spb);
					forcensa=1;
				}
				break;
			case 9:
				{
					argv++;
					std::wstring wtemp(UniFromUTF8(*argv));
					std::set<std::wstring>::iterator nbz=NBZlist.find(wtemp);
					if (nbz!=NBZlist.end())
						NBZlist.erase(nbz);
					LZSSlist.insert(wtemp);
					std::set<std::wstring>::iterator spb=SPBlist.find(wtemp);
					if (spb!=SPBlist.end())
						SPBlist.erase(spb);
					forcensa=1;
				}
				break;
			case 10:
				{
					argv++;
					std::wstring wtemp(UniFromUTF8(*argv));
					std::set<std::wstring>::iterator nbz=NBZlist.find(wtemp);
					if (nbz!=NBZlist.end())
						NBZlist.erase(nbz);
					std::set<std::wstring>::iterator lzss=LZSSlist.find(wtemp);
					if (lzss!=LZSSlist.end())
						LZSSlist.erase(lzss);
					SPBlist.insert(wtemp);
					forcensa=1;
				}
				break;
			case 11:
				{
					argv++;
					filelist.push_back(UniFromUTF8(*argv));
					removeslist.push_back(1);
				}
				break;
			default:
				{
					filelist.push_back(UniFromUTF8(*argv));
					removeslist.push_back(0);
				}
		}
	}
	if (forcensa)
		outputformat=NONS_Archive::NSA_ARCHIVE;
	const wchar_t *archive_names[]={
		L"arc.sar",
		L"arc.nsa",
		L"arc1.nsa",
		L"arc2.nsa",
		L"arc3.nsa",
		L"arc4.nsa",
		L"arc5.nsa",
		L"arc6.nsa",
		L"arc7.nsa",
		L"arc8.nsa",
		L"arc9.nsa",
		0
	};
	switch (mode){
		case EXTRACT:
			if (outputfile.size()){
				std::vector<std::wstring> temp;
				for (ulong a=0;a<filelist.size();a++){
					temp.push_back(filelist[a]);
					tolower(temp[a]);
				}
				for (ulong a=0,pos=0;archive_names[pos] && a<filelist.size();a++){
					long m=-1;
					for (ulong b=pos;m<0 && b<filelist.size();b++)
						if (temp[b]==archive_names[pos])
							m=b;
					if (m<0)
						continue;
					std::swap(filelist[pos],filelist[m]);
					std::swap(temp[pos++],temp[m]);
				}
			}
		case LIST:
			for (ulong a=0;a<filelist.size();a++){
				NONS_Archive archive(filelist[a],0);
				if (!archive.readArchive())
					continue;
				std::vector<NONS_TreeNode *> stack;
				if (outputfile.size())
					archive.root->data.name=outputfile;
				stack.push_back(archive.root);
				output(archive,stack,mode==EXTRACT);
				stack.pop_back();
			}
			break;
		case CREATE:
			{
				if (!outputfile.size()){
					if (outputformat==NONS_Archive::SAR_ARCHIVE || outputformat==NONS_Archive::UNRECOGNIZED)
						outputfile.append(L"arc.sar");
					else
						outputfile.append(L"arc.nsa");
				}
				if (outputformat==NONS_Archive::UNRECOGNIZED){
					ulong dot=outputfile.find_last_of('.');
					if (dot<outputfile.size()){
						dot++;
						std::wstring ext=outputfile.substr(dot);
						tolower(ext);
						if (ext==L"sar")
							outputformat=NONS_Archive::SAR_ARCHIVE;
						else
							outputformat=NONS_Archive::NSA_ARCHIVE;
					}
				}
				resolveFilenames(filelist,removeslist);
				std::vector<std::wstring> translated_paths;
				for (ulong a=0;a<filelist.size();a++){
					if (!file_exists(filelist[a])){
						filelist.erase(filelist.begin()+a--);
						continue;
					}
					std::wstring str=filelist[a];
					bool dont_remove=0,
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
						remove_twice=boost::filesystem::wpath(str).has_root_path();
#else
						remove_twice=boost::filesystem::path(UniToUTF8(str)).has_root_path();
#endif
					while (!remove_twice && (!str.find(L"../") || !str.find(L"./"))){
						ulong slash=str.find_first_of('/');
						str.erase(0,slash+1);
						dont_remove=1;
					}
					if (!dont_remove && removeslist[a]){
						ulong slash=str.find_first_of('/');
						str.erase(0,slash+1);
						if (remove_twice){
							slash=str.find_first_of('/');
							str.erase(0,slash+1);
						}
					}
					translated_paths.push_back(str);
				}
				if (!filelist.size())
					break;
				std::vector<ulong> compressions(translated_paths.size(),NONS_TreeNode::NONS_ArchivedFile::NO_COMPRESSION);

				//SPB compression is unsupported.
				if (SPBlist.size())
					std::cerr <<"SPB compression is unsupported."<<std::endl;
				for (std::set<std::wstring>::iterator i=LZSSlist.begin();i!=LZSSlist.end();i++){
					for (ulong b=0;b<translated_paths.size();b++){
						ulong p=translated_paths[b].rfind('/');
						p=(p==std::string::npos)?0:p+1;
						if (like(translated_paths[b].c_str()+p,i->c_str()))
							compressions[b]=NONS_TreeNode::NONS_ArchivedFile::LZSS_COMPRESSION;
					}
				}
				for (std::set<std::wstring>::iterator i=NBZlist.begin();i!=NBZlist.end();i++){
					for (ulong b=0;b<translated_paths.size();b++){
						ulong p=translated_paths[b].rfind('/');
						p=(p==std::string::npos)?0:p+1;
						if (like(translated_paths[b].c_str()+p,i->c_str()))
							compressions[b]=NONS_TreeNode::NONS_ArchivedFile::NBZ_COMPRESSION;
					}
				}

				NONS_Archive archive(outputformat);
				//archive.root->makeDirectory();
				for (ulong a=0;a<translated_paths.size();a++){
					NONS_TreeNode *node=archive.root->getBranch(translated_paths[a],1);
					node->data.archivepath=translated_paths[a];
					node->data.filepath=filelist[a];
					node->data.original_length=file_size(filelist[a]);
					node->data.compression_type=compressions[a];
				}
				archive.root->sort();
				std::vector<NONS_TreeNode *> allnodes;
				archive.root->vectorizeFiles(allnodes);

				std::string buffer;
				writeWordBig(translated_paths.size(),buffer);
				std::vector<ulong> fillout;
				//Reserve space for the offset of the data stream start.
				fillout.push_back(buffer.size());
				writeDWordBig(0,buffer);
				if (outputformat==NONS_Archive::SAR_ARCHIVE){
					ulong start=0;
					for (ulong a=0;a<allnodes.size();a++){
						std::string path=UniToSJIS(allnodes[a]->data.archivepath);
						tobackslash(path);
						buffer.append(path);
						buffer.push_back(0);
						writeDWordBig(start,buffer);
						writeDWordBig(allnodes[a]->data.original_length,buffer);
						start+=allnodes[a]->data.original_length;
					}
					writeDWordBig(buffer.size(),buffer,fillout[0]);
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
					boost::filesystem::ofstream file(outputfile,std::ios::binary);
#else
					boost::filesystem::ofstream file(UniToUTF8(outputfile),std::ios::binary);
#endif
					file.write(buffer.c_str(),buffer.size());
					for (ulong a=0;a<filelist.size();a++){
						std::cout <<"Adding \"/"<<UniToUTF8(allnodes[a]->data.archivepath)<<"\"."<<std::endl;
						ulong size=allnodes[a]->data.original_length;
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
						boost::filesystem::ifstream tempfile(allnodes[a]->data.filepath,std::ios::binary);
#else
						boost::filesystem::ifstream tempfile(UniToUTF8(allnodes[a]->data.filepath),std::ios::binary);
#endif
						char *tempbuf=new char[size];
						tempfile.read(tempbuf,size);
						file.write(tempbuf,size);
						delete[] tempbuf;
					}
					file.close();
				}else{
					for (ulong a=0;a<allnodes.size();a++){
						std::string path=UniToSJIS(allnodes[a]->data.archivepath);
						tobackslash(path);
						buffer.append(path);
						buffer.push_back(0);
						writeByte(allnodes[a]->data.compression_type,buffer);
						//Reserve space for the offset.
						fillout.push_back(buffer.size());
						writeDWordBig(0,buffer);
						//Reserve space for the compressed size (actual size in
						//the archive).
						fillout.push_back(buffer.size());
						writeDWordBig(0,buffer);
						writeDWordBig(allnodes[a]->data.original_length,buffer);
					}
					writeDWordBig(buffer.size(),buffer,fillout[0]);
					fillout.erase(fillout.begin());
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
					boost::filesystem::ofstream file(outputfile,std::ios::binary);
#else
					boost::filesystem::ofstream file;
					{
						char *temp_copy=copyString(outputfile.c_str());
						file.open(temp_copy,std::ios::binary);
						delete[] temp_copy;
					}
#endif
					file.write(buffer.c_str(),buffer.size());
					ulong start=0;
					for (ulong a=0;a<filelist.size();a++){
						std::cout <<"Adding \"/"<<UniToUTF8(allnodes[a]->data.archivepath)<<"\"."<<std::endl;
						ulong original_size=allnodes[a]->data.original_length;
#ifndef BOOST_FILESYSTEM_NARROW_ONLY
						boost::filesystem::ifstream tempfile(allnodes[a]->data.filepath,std::ios::binary);
#else
						boost::filesystem::ifstream tempfile(UniToUTF8(allnodes[a]->data.filepath),std::ios::binary);
#endif
						char *ibuffer=new char[original_size];
						tempfile.read(ibuffer,original_size);
						{
							std::string obuffer;
							switch (allnodes[a]->data.compression_type){
								case NONS_TreeNode::NONS_ArchivedFile::NO_COMPRESSION:
									break;
								case NONS_TreeNode::NONS_ArchivedFile::NBZ_COMPRESSION:
									obuffer=encode_NBZ(ibuffer,original_size);
									delete[] ibuffer;
									break;
								case NONS_TreeNode::NONS_ArchivedFile::LZSS_COMPRESSION:
									obuffer=encode_LZSS(ibuffer,original_size);
									delete[] ibuffer;
									break;
								case NONS_TreeNode::NONS_ArchivedFile::SPB_COMPRESSION:
									break;
							}
							file.write(&obuffer[0],obuffer.size());
							writeDWordBig(start,buffer,fillout[a*2]);
							writeDWordBig(obuffer.size(),buffer,fillout[a*2+1]);
							start+=obuffer.size();
						}
						delete[] ibuffer;
					}
					file.seekp(0,std::ios::beg);
					file.write(buffer.c_str(),buffer.size());
					file.close();
				}
			}
			break;
	}
	std::cout <<"Done."<<std::endl;
	return 0;
}
