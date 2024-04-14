//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "cliConfig.h"
#include "logger.h"
#include "scanner.h"
#include "parser.h"
#include "traverseAST.h"

void assignLogLevel() {
	if (CliConfig::verboseEnabled || CliConfig::debugLevel == 0) Logger::setLogLevel(Logger::Level::Verbose);
	else if (CliConfig::debugLevel == 1) Logger::setLogLevel(Logger::Level::Scanner);
	else if (CliConfig::debugLevel == 2) Logger::setLogLevel(Logger::Level::Parser);
	else if (CliConfig::debugLevel == 3) Logger::setLogLevel(Logger::Level::CodeGenerator);
	else Logger::setLogLevel(Logger::Level::Default);
}

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik | EGRE 591 | 03/27/2024" << "\n";

	//setup
	CliConfig::ParseCli(argc, argv);
	if (CliConfig::hasError) return EXIT_FAILURE;
	assignLogLevel();
	if (!CliConfig::LoadFile()) return EXIT_FAILURE;

	//part 1
	Scanner scanner(std::move(CliConfig::fileContents));

	//part 2
	Parser parser(scanner);
	auto program = parser.begin();
	if (program == nullptr) return EXIT_FAILURE;
	if (CliConfig::dumpAST) program->print(std::cout);

	//part 3
	SymbolTable table;
	TraverseAST ast(table, program);
	ast.analyzeSemantics();
	if(!ast.hasError && CliConfig::dumpST) table.dumpTable();
}
