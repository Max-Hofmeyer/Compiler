//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "logger.h"

Logger::Level Logger::logLevel = Logger::Level::Default;
int Logger::spaces = 0;

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

void Logger::parserEnter(const std::string& message) {
	indent();
	if (logLevel == Level::Parser || logLevel == Level::Verbose){
		std::cout << "[PARSER] Entering " << message << "\n";
	}
}

void Logger::parserCreate(const std::string& name, const std::string& data) {
	std::cout << getIndent() << "[NODE] " << name <<" " << data << "\n";
}

void Logger::parserExit(const std::string& message) {
	outdent();
	if (logLevel == Level::Parser || logLevel == Level::Verbose){
		std::cout << "[PARSER] Exiting " << message << "\n";
	}
}

void Logger::outputToken(const token t) {
	if (logLevel == Level::Parser) {
		std::cout << "[TOKEN] " << t.typeString << " " << t.value << " " << t.lineLoc << "\n";
	}
}

void Logger::outputTokens(const std::vector<token> t) {
	std::cout << "\t" << t.front().lineLoc << ": ";
	for (auto x : t) {
		std::cout << x.value;
	}
	std::cout << "\n";
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
