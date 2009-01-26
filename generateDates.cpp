#include <iostream>
#include <fstream>

std::string getISODateCompact(){
	time_t t0=time(0);
	tm *t=gmtime(&t0);
	char str[100];
	sprintf(str,"%04d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
	return std::string(str);
}

std::string getISODateHuman(){
	time_t t0=time(0);
	tm *t=gmtime(&t0);
	char str[100];
	sprintf(str,"%04d-%02d-%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
	return std::string(str);
}

std::string getISODateCommaSeparated(){
	time_t t0=time(0);
	tm *t=gmtime(&t0);
	char str[100];
	sprintf(str,"%04d, %02d, %02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
	return std::string(str);
}

int main(){
	std::ofstream rc("c:/Documents and Settings/root/My Documents/ONSlaught/onslaught.rc");
	rc <<"1 VERSIONINFO\n"
		"FILEVERSION "<<getISODateCommaSeparated()<<", 0\n"
		"PRODUCTVERSION "<<getISODateCommaSeparated()<<", 0\n"
		"#ifdef _DEBUG\n"
		"FILEFLAGS VS_FF_DEBUG\n"
		"#else\n"
		"FILEFLAGS 32\n"
		"#endif\n"
		"FILEOS 4\n"
		"FILETYPE 1\n"
		"{\n"
		"\tBLOCK \"StringFileInfo\" {\n"
		"\t\tBLOCK \"040904B0\" {\n"
		"\t\t\tVALUE \"FileDescription\", \"ONSlaught\"\n"
		"\t\t\tVALUE \"OriginalFilename\", \"ONSlaught.exe\"\n"
		"\t\t\tVALUE \"CompanyName\", \"Helios\"\n"
		"\t\t\tVALUE \"FileVersion\", \""<<getISODateHuman()<<"\"\n"
		"\t\t\tVALUE \"LegalCopyright\", \"Copyright (c) 2008, 2009, Helios\"\n"
		"\t\t\tVALUE \"ProductName\", \"ONSlaught: An ONScripter clone with Unicode support.\"\n"
		"\t\t\tVALUE \"ProductVersion\", \""<<getISODateHuman()<<"\"\n"
		"\t\t}\n"
		"\t}\n"
		"\tBLOCK \"VarFileInfo\" {\n"
		"\t\tVALUE \"Translation\", 0x0409, 0x04B0\n"
		"\t}\n"
		"}\n";
	std::ofstream version("c:/Documents and Settings/root/My Documents/ONSlaught/src/version.h");
	version <<"#define ONSLAUGHT_BUILD_VERSION "<<getISODateCompact()<<"\n"
		"#define ONSLAUGHT_BUILD_VERSION_WSTR L\"Beta "<<getISODateHuman()<<"\"\n"
		"#define ONSLAUGHT_BUILD_VERSION_STR \"Beta "<<getISODateHuman()<<"\"\n";
	return 0;
}

