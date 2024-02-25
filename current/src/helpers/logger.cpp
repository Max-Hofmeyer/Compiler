//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "logger.h"

Logger::Level Logger::logLevel = Logger::Level::Default;

void Logger::setLogLevel(Level level) {
	logLevel = level;
}

void Logger::scanner(const std::string& message) {
	if (logLevel <= Level::Scanner || logLevel <= Level::Verbose) {
		std::cout << "[SCANNER] " << message << "\n";
	}
}

void Logger::parser(const std::string& message) {
	std::cout << "[PARSER] " << message << "\n";
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
}

