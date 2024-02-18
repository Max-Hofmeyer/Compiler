//Max Hofmeyer & Ahmed Malik

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "part1.h"

//cd build | cmake .. | cmake --build . | ./part1

enum class Tokens {
	id,
	number,
	charliteral,
	string,
	relop,
	addop,
	mulop,
	assignop,
	lparen,
	rparen,
	lcurly,
	rcurly,
	lbracket,
	rbracket,
	semicolon,
	_not,
	_return,
	_int,
	_char,
	_if,
	_else,
	_for,
	_do,
	_while,
	_switch,
	_case,
	_default,
	_write,
	_read,
	_continue,
	_break,
	_newline
};


struct token {
	Tokens type;
	std::string typeString;
	int lineLoc;
	std::optional<std::string> value;

	explicit token(Tokens type, int lineLoc, std::string typeStr, std::optional<std::string> value = std::nullopt)
		: type(type), lineLoc(lineLoc), typeString(std::move(typeStr)), value(std::move(value)) {
	}
};

//allows us to not worry about when to print based on command line args
//just use the logger wherever a print is needed, and the flag will determine
//if it needs to be printed.
class Logger {
public:
	enum class Level {
		Default = 2,
		Scanner = 1,
		Debug = 0,
		Verbose = -1
	};
	static Level logLevel;
	static void setLogLevel(Level level) {
		logLevel = level;
	}

	static void scanner(const std::string& message) {
		if(logLevel <= Level::Scanner) {
			std::cout << "[SCANNER] " << message << "\n";
		}
	}
	static void debug(const std::string& message) {
		if (logLevel <= Level::Debug) {
			std::cout << "[DEBUG] " << message << "\n";
		}
	}
	static void verbose(const std::string& message) {
		if (logLevel <= Level::Verbose) {
			std::cout << "[VERBOSE] " << message << "\n";
		}
	}
};
Logger::Level Logger::logLevel = Logger::Level::Default;

//clearer implementation then the cli_config struct. This allows us to
//do checks of an object rather than pass by reference with a struct...
class CliConfig {
public:
	static int debugLevel;
	static bool helpEnabled, verboseEnabled, defaultEnabled;
	static std::string filePath;
	static std::string fileContents;

	static void ParseCli(int count, char** arguments) {
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
				}
				catch (const std::invalid_argument& e) {
					std::cerr << "[EXCEPTION] Invalid argument for debug!\n";
				}
			}
			else if (arg == "-verbose") {
				verboseEnabled = true;
			}
			else {
				filePath = arg;
			}
			if (debugLevel == 2 && !verboseEnabled) defaultEnabled = true;
		}
	}
	//bool so we can check later if it worked or not
	static bool LoadFile() {
		if (filePath.empty()) return false;
		std::ifstream f(filePath, std::ios::in);
		if (!f) return false;
		
		std::stringstream s;
		s << f.rdbuf();
		f.close();
		fileContents = s.str();
		return true;
	}
};
//c++ static declaration is so lame....
int CliConfig::debugLevel = 2;
bool CliConfig::helpEnabled = false;
bool CliConfig::verboseEnabled = false;
bool CliConfig::defaultEnabled = false;
std::string CliConfig::filePath;
std::string CliConfig::fileContents;

class Scanner {
public:
	explicit Scanner(std::string source) : source_(std::move(source)) {}

	std::vector<token> tokenize() {
		std::string buffer;
		std::vector<token> tokens;
		int line = 1;

		//keywords clear the buffer, tokens eat the buffer
		while (peek().has_value()) {
			if (std::isalpha(peek().value())) {
				buffer.push_back(eat());
				while (peek().has_value() && std::isalnum(peek().value())) { buffer.push_back(eat()); }
				Logger::debug("Buffer: " + buffer);
				if (buffer == "return") {
					tokens.emplace_back(Tokens::_return, line, buffer);
					buffer.clear();
				}
				else if (buffer == "int") {
					tokens.emplace_back(Tokens::_int, line, buffer);
					buffer.clear();
				}
				else if (buffer == "char") {
					tokens.emplace_back(Tokens::_char, line, buffer);
					buffer.clear();
				}
				else if (buffer == "if"){ tokens.emplace_back(Tokens::_if, line, buffer); }
				else {
					std::cout << buffer;
					buffer.clear();
				}
			}
			else {
				eat();
			}
		}
		return tokens;
	}

private:
	const std::string source_;
	size_t _index = 0;

	[[nodiscard]] std::optional<char> peek(int offset = 0) const {
		if (_index + offset >= source_.length()) return {};
		return source_.at(_index + offset);
	}

	char eat() {
		return source_.at(_index++);
	}
};

void print_tokens(const std::vector<token>& tokens) {
	for (const auto& t : tokens) {
		//since we use std::optional, keywords wont have a value, and cannot be printed otherwise 
		std::string valueStr = t.value.has_value() ? t.value.value() : t.typeString;
		Logger::scanner("(<" + t.typeString + ">,\"" + valueStr + "\")");
	}
	Logger::scanner("Total tokens: " + std::to_string(tokens.size()));
	
}

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik" << "\n";

	CliConfig::ParseCli(argc, argv);
	if (CliConfig::verboseEnabled) Logger::setLogLevel(Logger::Level::Verbose);
	if (CliConfig::debugLevel == 0) Logger::setLogLevel(Logger::Level::Debug);
	if (CliConfig::debugLevel == 1) Logger::setLogLevel(Logger::Level::Scanner);
	if (CliConfig::defaultEnabled) Logger::setLogLevel(Logger::Level::Default);

	if (CliConfig::LoadFile()) {
		Logger::debug("ToyC file location: " + CliConfig::filePath);
		Scanner scanner(std::move(CliConfig::fileContents));
		const std::vector<token> tokens = scanner.tokenize();
		print_tokens(tokens);
	}

	if(!CliConfig::LoadFile()) {
		std::cerr << "[ERROR] No file found\n";
		return EXIT_FAILURE;
	}

	//TODO: Tokenize keywords
	//TODO: Tokenize tokens

	return EXIT_SUCCESS;
}
