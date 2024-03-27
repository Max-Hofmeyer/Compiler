//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "cliConfig.h"

int CliConfig::debugLevel = -1;
bool CliConfig::verboseEnabled = false;
bool CliConfig::dumpAST = false;
bool CliConfig::hasError = false;

std::string CliConfig::filePath;
std::string CliConfig::fileContents;
std::string CliConfig::fileName;

void CliConfig::ParseCli(int count, char** arguments) {
	for (int i = 1; i < count; ++i) {
		std::string arg = arguments[i];
		if (arg == "-debug" && i + 1 < count) {
			try { debugLevel = std::stoi(arguments[++i]); }
			catch (const std::invalid_argument& e) {
				std::cerr << "Invalid argument for debug, referring to default\n";
			}
		}
		else if (arg == "-help") OutputHelp();
		else if (arg == "-verbose") verboseEnabled = true;
		else if (arg == "-abstract") dumpAST = true;
		else CheckForFile(arg);
	}
}

//bool so we can check if it worked or not
bool CliConfig::LoadFile() {
	if (hasError || filePath.empty()) {
		//technically should never occur but better to check
		Logger::error("Empty file passed");
		return false;
	}
	std::ifstream f(filePath, std::ios::in);
	if (!f) {
		Logger::error("File couldn't be opened");
		return false;
	}
	std::stringstream s;
	s << f.rdbuf();
	f.close();
	fileContents = s.str();
	return true;
}

void CliConfig::CheckForFile(const std::string& arg) {
	const std::filesystem::path checkValid = arg;
	if (checkValid.extension() == ".tc") {
		filePath = arg;
		fileName = checkValid.filename().string();
	}
	else {
		Logger::error("Command line error - \'" + arg + "\' is an unrecognized command line option\n");
		OutputHelp();
	}
}

void CliConfig::OutputHelp() {
	std::cout
		<< "Usage: ./part2 [options] input_file.tc"
		<< "\n"
		<< "where options include:\n"
		<< "  -help       Display this usage message\n"
		<< "  -debug <level>    Display messages that aid in tracing the compilation process.\n"
		<< "                    If level is:\n"
		<< "                    0 - all messages\n"
		<< "                    1 - scanner messages only\n"
		<< "                    2 - parser messages only\n"
		<< "  -abstract   Dump the abstract syntax tree\n"
		<< "  -verbose    Display all information\n";
}


//we may need this later... 
//if (!hasError) {
//	if (showOutputFile) std::cout << "output file: " << filePath << "\n";
//	if (showClassFile) std::cout << "class file: " << filePath << "\n";
//}
////else if (arg == "-output") showOutputFile = true;
//else if (arg == "-class") showClassFile = true;
//else if (arg == "-code") dumpCode = true;