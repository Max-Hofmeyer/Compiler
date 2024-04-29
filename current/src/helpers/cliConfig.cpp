//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "cliConfig.h"

int CliConfig::debugLevel = -1;
bool CliConfig::verboseEnabled = false;
bool CliConfig::hasError = false;
bool CliConfig::dumpAST = false;
bool CliConfig::dumpST = false;
bool CliConfig::dumpCode = false;
bool CliConfig::customClass = false;
bool CliConfig::customOutput = false;
bool CliConfig::demo = false;

std::string CliConfig::filePath;
std::string CliConfig::fileContents;
std::string CliConfig::fileName;
std::string CliConfig::className;
std::string CliConfig::outputName;

std::filesystem::path CliConfig::jasminFileLocation;

void CliConfig::ParseCli(int count, char** arguments) {
	for (int i = 1; i < count; ++i) {
		std::string arg = arguments[i];
		if (arg == "-debug" && i + 1 < count) {
			try { debugLevel = std::stoi(arguments[++i]); }
			catch (const std::invalid_argument& e) {
				std::cerr << "Invalid argument for debug, referring to default\n";
			}
		}
		else if (arg == "-class") {
			if (i + 1 < count && arguments[i + 1][0] != '-') {
				className = arguments[++i];
				customClass = true;
			}
		}
		else if (arg == "-output") {
			//prevents the rest of the arguments from being consumed after -output
			if (i + 1 < count && arguments[i + 1][0] != '-') {
				outputName = arguments[++i];
				customOutput = true;
			}
		}
		else if (arg == "-help") OutputHelp();
		else if (arg == "-verbose") verboseEnabled = true;
		else if (arg == "-abstract") dumpAST = true;
		else if (arg == "-symbol") dumpST = true;
		else if (arg == "-code") dumpCode = true;
		else if (arg == "-demoMode") demo = true;
		else CheckForFile(arg);
	}
	if (verboseEnabled) {
		dumpCode = true;
		dumpAST = true;
		dumpST = true;
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
	const std::filesystem::path workingDir = std::filesystem::current_path().parent_path().parent_path();
	if (checkValid.extension() == ".tc") {
		filePath = arg;
		fileName = checkValid.filename().string();
		if (!customClass) className = checkValid.stem().string();
		if (!customOutput) outputName = checkValid.stem().string() + ".j";
		if (dumpCode) {
			jasminFileLocation = workingDir / outputName;
			std::ofstream(jasminFileLocation).close();
		}
	}
	else {
		Logger::error("Command line error - \'" + arg + "\' is an unrecognized command line option\n");
		OutputHelp();
	}
}

void CliConfig::OutputHelp() {
	std::cout
		<< "\n<< Help Menu >>\n"
		<< "Usage: ./part3 [options] toyc_source_file.tc\n"
		<< "\nWhere options include:\n"
		<< "  -help			Display this usage message\n"
		<< "  -output <file>	Specifies target file name\n"
		<< "  -class <file>		Specifies class file name\n"
		<< "  -debug <level>	Display messages that aid in tracing the compilation process.\n"
		<< "			If level is:\n"
		<< "				0 - all messages\n"
		<< "				1 - scanner messages only\n"
		<< "				2 - parser messages only\n"
		<< "				3 - code generation messages only\n"
		<< "  -abstract		Dump the abstract syntax tree\n"
		<< "  -symbol		Dump the symbol table(s)\n"
		<< "  -code			Dump the generated program\n"
		<< "  -verbose		Display all information\n";
}

