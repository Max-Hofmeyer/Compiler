 

#include "logger.h"

#include <algorithm>

Logger::Level Logger::logLevel = Logger::Level::Default;
bool Logger::hasError = false;
bool Logger::demo = false;

void Logger::setLogLevel(const Level level) {
	logLevel = level;
}

void Logger::debug(const std::string& message) {
	if (logLevel <= Level::Debug || logLevel <= Level::Verbose) {
		std::cout << "[DEBUG] " << message << "\n";
	}
}

void Logger::warning(const std::string& message) {
	std::cout << "[WARNING] " << message << "\n";
}

void Logger::error(const std::string& message) {
	std::cout << "[ERROR] " << message << "\n";
	hasError = true;
}

void Logger::scanner(const std::string& message) {
	if (logLevel == Level::Scanner || logLevel == Level::Verbose &&
		!hasError) {
		std::cout << "[SCANNER] " << message << "\n";
	}
}

void Logger::parser(const std::string& message) {
	if (logLevel == Level::Parser || logLevel == Level::Verbose
		&& !hasError) {
		std::cout << "[PARSER] " << message << "\n";
	}
}

void Logger::parserEnter(const std::string& message) {
	if (logLevel == Level::Parser || logLevel == Level::Verbose
		&& !hasError) {
		std::cout << "[PARSER] Entering " << message << "\n";
	}
}

void Logger::parserExit(const std::string& message) {
	if (logLevel == Level::Parser || logLevel == Level::Verbose
		&& !hasError) {
		std::cout << "[PARSER] Exiting " << message << "\n";
	}
}

void Logger::outputTokens(const std::vector<token> t) {
	for (const auto& x : t) std::cout << x.value << " ";
	std::cout << "\n";
}

void Logger::parserError(const std::string& message, const int lc, const std::vector<token> t, const int spaces) {
	if (!hasError) {
		std::cout << lc << ": ";
		for (const auto& x : t) {
			if (x.type == Tokens::eof) std::cout << " ";
			else std::cout << x.value << " ";
		}
		std::cout << "\n";
		for (int i = 0; i < spaces; ++i) std::cout << " ";
		std::cout << "^ " << message << "\n";
		hasError = true;
	}
}

void Logger::symbolTable(const std::string& message) {
	if (logLevel == Level::CodeGenerator && !hasError || logLevel <= Level::Verbose) {
		std::cout << "[SYMBOL TABLE] Inserting " << message << "\n";
	}
}

void Logger::symbolTableWarning(const std::string& message) {
	if (logLevel == Level::CodeGenerator && !hasError || logLevel <= Level::Verbose) {
		std::cout << "[WARNING] Failed to insert " << message << "\n";
	}
}

void Logger::semanticAnalyzer(const std::string& message) {
	if (logLevel == Level::CodeGenerator && !hasError || logLevel <= Level::Verbose) {
		std::cout << "[ANALYZER] Checking " << message << "\n";
	}
}

void Logger::codeGenerator(const std::string& message) {
	if (logLevel == Level::CodeGenerator || logLevel == Level::Verbose && !hasError) {
		std::cout << "[GENERATOR] Generating " << message << "\n";
	}
}

void Logger::codeGeneratorStream(const std::string& message) {
	if (logLevel == Level::CodeGenerator || logLevel == Level::Verbose && !hasError) {
		std::cout << "[JASMIN STREAM] " << message << "\n";
	}
}
