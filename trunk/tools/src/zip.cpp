/*
* Copyright (c) 2009, Helios (helios.vmg@gmail.com)
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

#include <iostream>
#include <vector>
#include <boost/cstdint.hpp>
#include <bzlib.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "LZMA.h"
#include "Archive.h"

#define NONS_SYS_WINDOWS (defined _WIN32 || defined _WIN64)
#define NONS_SYS_UNIX (defined __unix__ || defined __unix)

#define COMPRESSION_NONE	0
#define COMPRESSION_BZ2		12
#define COMPRESSION_LZMA	14
#define COMPRESSION_AUTO	-1
#define COMPRESSION_AUTO_MT	(COMPRESSION_AUTO-1)
#define COMPRESSION_DEFAULT	COMPRESSION_AUTO_MT

ulong parseSize(const std::string &s){
	static const char *prefixes="kmg";
	ulong multiplier=1;
	switch (s[s.size()-1]){
		case 'K':
		case 'k':
			multiplier=1024;
			break;
		case 'M':
		case 'm':
			multiplier=1024*1024;
			break;
		case 'G':
		case 'g':
			multiplier=1024*1024*1024;
			break;
	}
	std::stringstream stream(s);
	ulong res;
	stream >>res;
	return res*multiplier;
}

struct Options{
	bool good;
	char mode;
	std::wstring outputFilename;
	ulong compressionType;
	boost::int32_t split;
	std::vector<std::pair<std::wstring,bool> > inputFilenames;
	Options(char **argv){
		static std::pair<std::wstring,ulong> compressions[]={
			std::make_pair(L"none",COMPRESSION_NONE),
			std::make_pair(L"bz2",COMPRESSION_BZ2),
			std::make_pair(L"lzma",COMPRESSION_LZMA),
			std::make_pair(L"auto",COMPRESSION_AUTO),
			std::make_pair(L"automt",COMPRESSION_AUTO_MT)
		};
		static const char *options[]={
			"-h",
			"-?",
			"--help",
			"--version",
			"-o",
			"-c",
			"-r",
			"-s",
			0
		};
		this->compressionType=COMPRESSION_DEFAULT;
		this->split=2147483647; //2 GiB - 1 B
		this->good=0;
		if (!++argv)
			return;
		this->mode='c';
		bool nextIsOutput=0,
			nextIsCompression=0,
			nextIsSkip=0,
			nextIsSplit=0;
		while (*argv){
			long option=-1;
			for (ulong a=0;options[a] && option<0;a++)
				if (!strcmp(*argv,options[a]))
					option=a;
			switch (option){
				case 0: //-h
				case 1: //-?
				case 2: //--help
					this->mode='h';
					this->good=1;
					return;
				case 3: //--version
					this->mode='v';
					this->good=1;
					return;
				case 4: //-o
					if (!(nextIsOutput || nextIsCompression || nextIsSplit || nextIsSkip)){
						nextIsOutput=1;
						break;
					}
				case 5: //-c
					if (!(nextIsOutput || nextIsCompression || nextIsSplit || nextIsSkip)){
						nextIsCompression=1;
						break;
					}
				case 6: //-r
					if (!(nextIsOutput || nextIsCompression || nextIsSplit || nextIsSkip)){
						nextIsSkip=1;
						break;
					}
				case 7: //-s
					if (!(nextIsOutput || nextIsCompression || nextIsSplit || nextIsSkip)){
						nextIsSplit=1;
						break;
					}
				default:
					if (nextIsOutput){
						this->outputFilename=UniFromUTF8(*argv);
						nextIsOutput=0;
					}else if (nextIsCompression){
						std::wstring s=UniFromUTF8(*argv);
						for (ulong a=0,b=4;b;a++,b--){
							if (s==compressions[a].first){
								this->compressionType=compressions[a].second;
								break;
							}
							if (b==1){
								std::cerr <<"Unrecognized compression mode."<<std::endl;
								return;
							}
						}
						nextIsCompression=0;
					}else if (nextIsSplit){
						this->split=parseSize(*argv);
						if (this->split<0)
							this->split=-this->split;
						nextIsSplit=0;
					}else{
						this->inputFilenames.push_back(std::make_pair(UniFromUTF8(*argv),nextIsSkip));
						nextIsSkip=0;
					}
			}
			argv++;
		}
		this->good=1;
	}
};

class CRC32{
	boost::uint32_t crc32;
	static boost::uint32_t CRC32lookup[];
public:
	CRC32(){
		this->Reset();
	}
	void Reset(){
		this->crc32^=~this->crc32;
	}
	void Input(const void *message_array,size_t length){
		for (const uchar *array=(const uchar *)message_array;length;length--,array++)
			this->Input(*array);
	}
	void Input(uchar message_element){
		this->crc32=(this->crc32>>8)^CRC32lookup[message_element^(this->crc32&0xFF)];
	}
	boost::uint32_t Result(){
		return ~this->crc32;
	}
};

boost::uint32_t CRC32::CRC32lookup[]={
	0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
	0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91, 
	0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7, 
	0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5, 
	0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B, 
	0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59, 
	0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F, 
	0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D, 
	0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433, 
	0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01, 
	0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457, 
	0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65, 
	0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB, 
	0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9, 
	0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F, 
	0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD, 
	0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683, 
	0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1, 
	0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7, 
	0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5, 
	0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B, 
	0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79, 
	0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F, 
	0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D, 
	0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713, 
	0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21, 
	0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777, 
	0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45, 
	0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB, 
	0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9, 
	0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF, 
	0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
};

char *compressBuffer_BZ2(void *src,unsigned long srcl,unsigned long &dstl){
	unsigned long l=srcl,
		realsize=l;
	char *dst=new char[l];
	while (BZ2_bzBuffToBuffCompress(dst,(unsigned int *)&l,(char *)src,srcl,9,0,30)==BZ_OUTBUFF_FULL){
		delete[] dst;
		l*=2;
		realsize=l;
		dst=new char[l];
	}
	if (l!=realsize){
		char *temp=new char[l];
		memcpy(temp,dst,l);
		delete[] dst;
		dst=temp;
	}
	dstl=l;
	return dst;
}

char *compressLZMA(void *src,ulong srcl,ulong &dstl){
	size_t l=srcl,
		realsize=l,
		propsSize;
	char *dst=new char[l];
	uchar props[9]={0,0,5,0};
	while (LzmaCompress((uchar *)dst,&l,(uchar *)src,srcl,props+4,&propsSize,5,0,0,0,0,0,2)==SZ_ERROR_OUTPUT_EOF){
		delete[] dst;
		l*=2;
		realsize=l;
		dst=new char[l];
	}
	{
		char *temp=new char[9+l];
		memcpy(temp,props,9);
		memcpy(temp+9,dst,l);
		delete[] dst;
		dst=temp;
	}
	dstl=l+9;
	return dst;
}

class ZipArchive:public Archive<char>{
	std::vector<std::string> central_header;
	boost::filesystem::ofstream output_file;
	ulong current_file;
	void write_buffer(const char *buffer,size_t size);
	ulong write_header(const std::string &buffer);
	void go_to_next();
	ulong total,
		progress;
public:
	ulong split,
		compression;
	std::wstring outputFilename;
	ZipArchive():split(0x7FFFFFFF),compression(COMPRESSION_DEFAULT){}
	~ZipArchive(){}
	void write();
	void write(const Path &src,const std::wstring &dst,bool dir);
};

void ZipArchive::write(){
	this->current_file=0;
	if (!this->outputFilename.size())
		this->outputFilename=L"output";
	this->output_file.open(this->outputFilename+L".z01",std::ios::binary);
	this->central_header.clear();
	this->total=this->root.count(1);
	this->progress=0;
	this->root.write(this,L"",L"");
	ulong central_size=0,
		central_start_disk,
		central_start_offset,
		entries_on_this_disk=0;
	for (ulong a=0;a<this->central_header.size();a++){
		ulong temp=this->write_header(this->central_header[a]);
		if (!a){
			central_start_disk=this->current_file;
			central_start_offset=temp;
		}
		if (!temp)
			entries_on_this_disk=1;
		else
			entries_on_this_disk++;
		central_size+=this->central_header[a].size();
	}

	std::string buffer;
	writeLittleEndian(4,buffer,0x06054b50);
	writeLittleEndian(2,buffer,this->current_file+((this->split-this->output_file.tellp()<22)?1:0));
	writeLittleEndian(2,buffer,central_start_disk);
	writeLittleEndian(2,buffer,entries_on_this_disk);
	writeLittleEndian(2,buffer,this->central_header.size());
	this->central_header.clear();
	writeLittleEndian(4,buffer,central_size);
	writeLittleEndian(4,buffer,central_start_offset);
	writeLittleEndian(2,buffer,0);
	this->write_header(buffer);
	this->output_file.close();

	{
		std::wstring temp=this->outputFilename+L".zip";
		if (boost::filesystem::exists(temp))
			boost::filesystem::remove(temp);
		boost::filesystem::rename(this->outputFilename+L".z"+itoa<wchar_t>(++this->current_file,2),temp);
	}
	std::cout <<"Done."<<std::endl;
}

void compression_helper(char *(*f)(void *,ulong,ulong &),char **r,void *a,ulong b,ulong *c){
	*r=f(a,b,*c);
}

#define ZIP_FLAG_UTF8 0x800

void ZipArchive::write(const Path &src,const std::wstring &dst,bool dir){
	if (!boost::filesystem::exists(src))
		return;
	(std::cout <<'(').width(2);
	std::cout <<(this->progress++*100)/this->total<<"%) "<<UniToUTF8(dst)<<"..."<<std::endl;
	std::string buffer;
	writeLittleEndian(4,buffer,0x04034B50);
	writeLittleEndian(2,buffer,10);
	writeLittleEndian(2,buffer,ZIP_FLAG_UTF8);
	long compression=COMPRESSION_NONE;
	char *raw=0;
	size_t raw_l=0;
	char *compressed=0;
	ulong compressed_l=0;
	CRC32 crc;
	if (!dir){
		boost::filesystem::ifstream file(src,std::ios::binary|std::ios::ate);
		raw_l=file.tellg();
		if (raw_l)
			compression=this->compression;
		file.seekg(0);
		raw=new char[raw_l];
		file.read(raw,raw_l);
		crc.Input(raw,raw_l);
		switch (compression){
			case COMPRESSION_NONE:
				compressed=raw;
				compressed_l=raw_l;
				break;
			case COMPRESSION_BZ2:
				compressed=compressBuffer_BZ2(raw,raw_l,compressed_l);
				delete[] raw;
				break;
			case COMPRESSION_LZMA:
				compressed=compressLZMA(raw,raw_l,compressed_l);
				delete[] raw;
				break;
			case COMPRESSION_AUTO:
			case COMPRESSION_AUTO_MT:
				{
					ulong bz2=0,
						lzma=0;
					char *bz2_buffer,
						*lzma_buffer;
					if (compression==COMPRESSION_AUTO_MT){
						boost::thread thread(boost::bind(compression_helper,compressLZMA,&lzma_buffer,raw,raw_l,&lzma));
						bz2_buffer=compressBuffer_BZ2(raw,raw_l,bz2);
						thread.join();
					}else{
						bz2_buffer=compressBuffer_BZ2(raw,raw_l,bz2);
						lzma_buffer=compressLZMA(raw,raw_l,lzma);
					}
					if (raw_l<=bz2 && raw_l<=lzma){
						compressed=raw;
						compressed_l=raw_l;
						delete[] bz2_buffer;
						delete[] lzma_buffer;
						compression=COMPRESSION_NONE;
					}else{
						delete[] raw;
						if (bz2<raw_l && bz2<=lzma){
							compressed=bz2_buffer;
							compressed_l=bz2;
							delete[] lzma_buffer;
							compression=COMPRESSION_BZ2;
						}else{
							compressed=lzma_buffer;
							compressed_l=lzma;
							delete[] bz2_buffer;
							compression=COMPRESSION_LZMA;
						}
					}
				}
				break;
		}
	}
	writeLittleEndian(2,buffer,compression);
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(4,buffer,crc.Result());
	writeLittleEndian(4,buffer,compressed_l);
	writeLittleEndian(4,buffer,raw_l);
	std::string utf8=UniToUTF8(dst);
	writeLittleEndian(2,buffer,utf8.size());
	writeLittleEndian(2,buffer,0);
	buffer.append(utf8.substr(0,0x10000));
	ulong offset=this->output_file.tellp();
	this->write_buffer(&buffer[0],buffer.size());
	buffer.clear();

	writeLittleEndian(4,buffer,0x02014b50);
	writeLittleEndian(2,buffer,10);
	writeLittleEndian(2,buffer,10);
	writeLittleEndian(2,buffer,ZIP_FLAG_UTF8);
	writeLittleEndian(2,buffer,compression);
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(4,buffer,crc.Result());
	writeLittleEndian(4,buffer,compressed_l);
	writeLittleEndian(4,buffer,raw_l);
	writeLittleEndian(2,buffer,utf8.size());
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(2,buffer,this->current_file);
	writeLittleEndian(2,buffer,0);
	writeLittleEndian(4,buffer,0);
	writeLittleEndian(4,buffer,offset);
	buffer.append(utf8.substr(0,0x10000));
	this->central_header.push_back(buffer);

	if (compressed){
		this->write_buffer(compressed,compressed_l);
		delete[] compressed;
	}
}

void ZipArchive::go_to_next(){
	this->output_file.close();
	this->output_file.open(this->outputFilename+L".z"+itoa<wchar_t>(++this->current_file+1,2),std::ios::binary);
}

void ZipArchive::write_buffer(const char *buffer,size_t size){
	while (this->split-this->output_file.tellp()<size){
		ulong diff=this->split-this->output_file.tellp();
		this->output_file.write(buffer,diff);
		buffer+=diff;
		size-=diff;
		this->go_to_next();
	}
	this->output_file.write(buffer,size);
}

ulong ZipArchive::write_header(const std::string &buffer){
	ulong diff=this->split-this->output_file.tellp();
	ulong ret;
	if (diff<buffer.size()){
		char *temp=new char[diff];
		memset(temp,0,diff);
		this->output_file.write(temp,diff);
		this->go_to_next();
		ret=0;
	}else
		ret=this->output_file.tellp();
	this->write_buffer(&buffer[0],buffer.size());
	return ret;
}

void version(){
	std::cout <<"zip v1.0\n"
		"Copyright (c) 2009, Helios (helios.vmg@gmail.com)\n"
		"All rights reserved.\n\n";
}

void usage(){
	version();
	std::cout <<
		"zip <options> <input files>\n"
		"\n"
		"OPTIONS:\n"
		"    -o <filename>\n"
		"        Output filename.\n"
		"    -c <compression>\n"
		"        Set compression.\n"
		"    -s <file size>\n"
		"        Split archive into pieces no bigger than this.\n"
		"\n"
		"See the documentation for details.\n";
}

void initialize_conversion_tables();

int main(int,char **argv){
	initialize_conversion_tables();
	Options options(argv);
	if (!options.good)
		return 1;
	switch (options.mode){
		case 'h':
			usage();
			break;
		case 'v':
			version();
			break;
		case 'c':
			{
				boost::posix_time::ptime t0=boost::posix_time::microsec_clock::local_time();
				{
					ZipArchive archive;
					for (ulong a=0;a<options.inputFilenames.size();a++)
						archive.add(options.inputFilenames[a].first,options.inputFilenames[a].second);
					archive.split=options.split;
					archive.outputFilename=options.outputFilename;
					archive.compression=options.compressionType;
					archive.write();
				}
				boost::posix_time::ptime t1=boost::posix_time::microsec_clock::local_time();
				std::cout <<"Elapsed time: "<<double((t1-t0).total_milliseconds())/1000.0<<'s'<<std::endl;
			}
			break;
	}
	return 0;
}
