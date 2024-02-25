//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "cliConfig.h"
#include "logger.h"

int CliConfig::debugLevel = -1;
bool CliConfig::helpEnabled = false;
bool CliConfig::verboseEnabled = false;
//bool CliConfig::defaultEnabled = true;
std::string CliConfig::filePath;
std::string CliConfig::fileContents;

void CliConfig::ParseCli(int count, char** arguments) {
	for (int i = 1; i < count; ++i) {
		if (std::string arg = arguments[i]; arg == "-help") {
			helpEnabled = true;
			std::cout
				<< "Usage: parser.tc [options] toyc_source_file\n"
				<< "where options include:\n"
				<< "-help       display this usage message\n"
				<< "-debug <level>  display messages that aid in tracing the compilation process.\n"
				<< "               If level is:\n"
				<< "               0 - all messages\n"
				<< "               1 - scanner messages only\n"
				<< "-verbose    display all information\n";
		}
		else if (arg == "-debug" && i + 1 < count) {
			try {
				debugLevel = std::stoi(arguments[++i]);
				//defaultEnabled = false;
			}
			catch (const std::invalid_argument& e) {
				std::cerr << "[EXCEPTION] Invalid argument for debug!\n";
			}
		}
		else if (arg == "-verbose") {
			verboseEnabled = true;
			//defaultEnabled = false;
		}
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