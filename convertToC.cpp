#include <fstream>
#include <string>

int main(int argc,char **argv){
	for (argv++;*argv;argv++){
		std::ifstream ifile(*argv);
		if (!ifile)
			continue;
		std::string ofilename(*argv);
		ofilename.append(".cpp");
		std::ofstream ofile(ofilename.c_str());
		bool lastwasnewline;
		while (1){
			char c=ifile.get();
			if (!ifile.good())
				break;
			if (lastwasnewline)
				ofile <<"\"";
			lastwasnewline=0;
			switch (c){
				case '\"':
					ofile <<"\\\"";
					break;
				case '\t':
					ofile <<"\\t";
					break;
				case '\\':
					ofile <<"\\\\";
					break;
				case '\n':
					ofile <<"\\n\"\n";
					lastwasnewline=1;
					break;
				default:
					ofile <<c;
			}
		}
	}
}
