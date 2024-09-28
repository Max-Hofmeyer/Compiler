#include "cliConfig.h"
#include "logger.h"
#include "scanner.h"
#include "parser.h"
#include "semanticAnalysis.h"
#include "jasminGenerator.h"

void assignLogLevel() {
	if (CliConfig::demo) Logger::demo = true;
	if (CliConfig::verboseEnabled || CliConfig::debugLevel == 0) Logger::setLogLevel(Logger::Level::Verbose);
	else if (CliConfig::debugLevel == 1) Logger::setLogLevel(Logger::Level::Scanner);
	else if (CliConfig::debugLevel == 2) Logger::setLogLevel(Logger::Level::Parser);
	else if (CliConfig::debugLevel == 3) Logger::setLogLevel(Logger::Level::CodeGenerator);
	else Logger::setLogLevel(Logger::Level::Default);
}

int main(const int argc, char** argv) {

	//configure logger based on flags
	CliConfig::ParseCli(argc, argv);
	if (CliConfig::hasError) {
		return EXIT_FAILURE;
	}
	assignLogLevel();

	if (!CliConfig::LoadFile()) {
		return EXIT_FAILURE;
	}

	Scanner scanner(std::move(CliConfig::fileContents));

	//parser driven implementation, scanner only reads next token when requested by parser
	Parser parser(scanner);

	//program will return a pointer to the top most program node
	auto program = parser.begin();
	if (program == nullptr || parser.hasError) {
		return EXIT_FAILURE;
	}

	//program will recursively print its children
	if (CliConfig::dumpAST || CliConfig::demo) {
		program->print(std::cout);
	}

	//once ast is built, 2 passes are made:
	//1. Checks for logical / semantic errors. Stores identifiers inside a symbol table
	SymbolTable table;
	AnalyseSemantics dirty_tree(table, program);
	dirty_tree.traverseTree();
	if (dirty_tree.hasError) {
		return EXIT_FAILURE;
	}

	if (CliConfig::dumpST || CliConfig::demo) {
		table.dumpTable();
	}

	//2. Outputs jasmin assembler code in the correct order, and uses the symbol table to generate
	//labels and jumps
	GenerateJasminCode clean_tree(table, program);
	clean_tree.generateCode();
	return EXIT_SUCCESS;
}
