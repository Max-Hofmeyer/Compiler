//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

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

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/24/2024" << "\n";

	/* setup */
	CliConfig::ParseCli(argc, argv);
	if (CliConfig::hasError) return EXIT_FAILURE;
	assignLogLevel();
	if (!CliConfig::LoadFile()) return EXIT_FAILURE;

	/* part 1 */
	Scanner scanner(std::move(CliConfig::fileContents));

	/* part 2 */
	Parser parser(scanner);
	auto program = parser.begin();
	if (program == nullptr || parser.hasError) return EXIT_FAILURE;
	if (CliConfig::dumpAST || CliConfig::demo) program->print(std::cout);

	/* part 3 */
	SymbolTable table;
	AnalyseSemantics checkAST(table, program);

	//check semantics
	checkAST.traverseTree();
	if (checkAST.hasError) return EXIT_FAILURE;
	if (CliConfig::dumpST || CliConfig::demo) table.dumpTable();

	//generate jasmin code
	GenerateJasminCode outputAST(table, program);
	outputAST.generateCode();
	return EXIT_SUCCESS;
}
