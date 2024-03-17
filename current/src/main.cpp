//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "cliConfig.h"
#include "logger.h"
#include "scanner.h"
#include "parser.h"

void assignLogLevel() {
	if (CliConfig::verboseEnabled || CliConfig::debugLevel == 0) Logger::setLogLevel(Logger::Level::Verbose);
	else if (CliConfig::debugLevel == 1) Logger::setLogLevel(Logger::Level::Scanner);
	else if (CliConfig::debugLevel == 2) Logger::setLogLevel(Logger::Level::Parser);
	else if (CliConfig::debugLevel == 3) Logger::setLogLevel(Logger::Level::CodeGenerator);
	else Logger::setLogLevel(Logger::Level::Default);
}

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024" << "\n";

	CliConfig::ParseCli(argc, argv);
	if (CliConfig::hasError) return EXIT_FAILURE;
	assignLogLevel();

	if (CliConfig::LoadFile()) {
		Scanner scanner(std::move(CliConfig::fileContents));
		Parser parser;
		scanner.Add(&parser);

		if(!parser.hasError) scanner.scan();
	}
	else return EXIT_FAILURE;
	return EXIT_SUCCESS;
}