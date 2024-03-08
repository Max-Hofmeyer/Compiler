//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "logger.h"

Logger::Level Logger::logLevel = Logger::Level::Default;
bool Logger::hasError = false;

void Logger::setLogLevel(const Level level) {
	logLevel = level;
}

void Logger::scanner(const std::string& message) {
	if (logLevel == Level::Scanner || logLevel == Level::Verbose) {
		std::cout << "[SCANNER] " << message << "\n";
	}
}

void Logger::parser(const std::string& message) {
	if (logLevel == Level::Parser || logLevel == Level::Verbose) {
		std::cout << "[PARSER] " << message << "\n";
	}
}

void Logger::debug(const std::string& message) {
	if (logLevel <= Level::Debug || logLevel <= Level::Verbose) {
		std::cout << "[DEBUG] " << message << "\n";
	}
}

void Logger::codeGenerator(const std::string& message) {
	if (logLevel <= Level::CodeGenerator || logLevel <= Level::Verbose) {
		std::cout << "[GENERATOR] " << message << "\n";
	}
}

void Logger::warning(const std::string& message) {
	std::cout << "[WARNING] " << message << "\n";
}

void Logger::error(const std::string& message) {
	std::cout << "[ERROR] " << message << "\n";
	
}

