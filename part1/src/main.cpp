//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "logger.h"
#include "cliConfig.h"
#include "scanner.h"
#include <iostream>

void print_tokens(const std::vector<token>& tokens) {
	for (const auto& t : tokens) {
		//guard against trying to output unassigned optional values
		std::string valueStr = t.value.has_value() ? t.value.value() : t.typeString;
		Logger::scanner("(<" + t.typeString + ">,\"" + valueStr + "\")");
	}
	Logger::scanner("Total tokens: " + std::to_string(tokens.size()));
}

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024" << "\n";

	CliConfig::ParseCli(argc, argv);

	if (CliConfig::verboseEnabled) Logger::setLogLevel(Logger::Level::Verbose);
	if (CliConfig::debugLevel == 0) Logger::setLogLevel(Logger::Level::Debug);
	if (CliConfig::debugLevel == 1) Logger::setLogLevel(Logger::Level::Scanner);
	if (CliConfig::defaultEnabled) Logger::setLogLevel(Logger::Level::Default);

	if (CliConfig::LoadFile()) {
		Logger::debug("ToyC File location: " + CliConfig::filePath);
		Scanner scanner(std::move(CliConfig::fileContents));
		const std::vector<token> tokens = scanner.tokenize();
		if(!scanner.hasError()) print_tokens(tokens);
		
	}

	if (!CliConfig::LoadFile()) {
		Logger::error("Failed to load file");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
