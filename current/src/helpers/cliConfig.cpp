//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "cliConfig.h"
#include "logger.h"

void CliConfig::ParseCli(int count, char** arguments) {
	for (int i = 1; i < count; ++i) {
		if (std::string arg = arguments[i]; arg == "-help") {
			helpEnabled = true;
			std::cout
				<< "Options:\n"
				<< "  -help       Display this usage message\n"
				<< "  -output <file>    Specifies target file name\n"
				<< "  -class <file>     Specifies class file name\n"
				<< "  -debug <level>    Display messages that aid in tracing the compilation process.\n"
				<< "                    If level is:\n"
				<< "                    0 - all messages\n"
				<< "                    1 - scanner messages only\n"
				<< "                    2 - parser messages only\n"
				<< "                    3 - code generation messages only\n"
				<< "  -abstract   Dump the abstract syntax tree\n"
				<< "  -symbol     Dump the symbol table(s)\n"
				<< "  -code       Dump the generated program\n"
				<< "  -verbose    Display all information\n"
				<< "  -version    Display the program version\n";
		}
		else if (arg == "-debug" && i + 1 < count) {
			try { debugLevel = std::stoi(arguments[++i]); }
			catch (const std::invalid_argument& e) {
				std::cerr << "[EXCEPTION] Invalid argument for debug!\n";
			}
		}
		else if (arg == "-output") {
			//todo: implement logic
		}
		else if(arg == "-class") {
			//todo: implement logic
		}
		else if (arg == "-verbose") verboseEnabled = true;
		else if (arg == "-abstract") dumpAST = true;
		else if (arg == "-code") dumpCode = true;
		else {
			filePath = arg;
		}
	}
}

//bool so we can check later if it worked or not
bool CliConfig::LoadFile() {
	if (filePath.empty()) return false;
	std::ifstream f(filePath, std::ios::in);
	if (!f) return false;

	std::stringstream s;
	s << f.rdbuf();
	f.close();
	fileContents = s.str();
	return true;
}