//Max Hofmeyer & Ahmed Malik

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include "part1.h"


//cd build | cmake .. | cmake --build . | ./part1

enum class Tokens {
	id,
	number,
	charliteral, //not done
	string, //not done
	relop,
	addop,
	mulop, // (/) not done
	assignop,
	lparen,
	rparen,
	lcurly,
	rcurly,
	lbracket,
	rbracket,
	semicolon,
	COMMA, //added comma 
	COLON,//added Colon
	_eof, //added end of file token (discussed in class)
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

				//std::unordered_map<std::string, std::vector<token>> tokenize_list = { //in progress: need to add to constructor (confirm with max)
				//	{"return", tokens.emplace_back(Tokens::_return, line, buffer)}
				//};


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
				else if (buffer == "if") {
					tokens.emplace_back(Tokens::_if, line, buffer);
					buffer.clear();
				}
				else if (buffer == "else") {
					tokens.emplace_back(Tokens::_else, line, buffer);
					buffer.clear();
				}
				else if (buffer == "for") {
					tokens.emplace_back(Tokens::_for, line, buffer);
					buffer.clear();
				}
				else if (buffer == "do") {
					tokens.emplace_back(Tokens::_do, line, buffer);
					buffer.clear();
				}
				else if (buffer == "while") {
					tokens.emplace_back(Tokens::_while, line, buffer);
					buffer.clear();
				}
				else if (buffer == "switch") {
					tokens.emplace_back(Tokens::_switch, line, buffer);
					buffer.clear();
				}
				else if (buffer == "case") {
					tokens.emplace_back(Tokens::_case, line, buffer);
					buffer.clear();
				}
				else if (buffer == "default") {
					tokens.emplace_back(Tokens::_default, line, buffer);
					buffer.clear();
				}
				else if (buffer == "write") {
					tokens.emplace_back(Tokens::_write, line, buffer);
					buffer.clear();
				}
				else if (buffer == "read") {
					tokens.emplace_back(Tokens::_read, line, buffer);
					buffer.clear();
				}
				else if (buffer == "continue") {
					tokens.emplace_back(Tokens::_continue, line, buffer);
					buffer.clear();
				}
				else if (buffer == "break") {
					tokens.emplace_back(Tokens::_break, line, buffer);
					buffer.clear();
				}
				else {
					//confirm with max: this should basically be ID token
					tokens.emplace_back(Tokens::id, line, buffer);
					std::cout << buffer; //is this for debugging?
					buffer.clear();
				}
			}
			else if (std::isdigit(peek().value())) { //finish digit
				while (peek().has_value() && std::isdigit(peek(1).value())) { 
					buffer.push_back(eat()); //buffer eats numbers until no numbers are left
					// gotta add stuff that takes in a exponent
				}
				tokens.emplace_back(Tokens::number, line, buffer);
			}
			else if (peek().value() == '\"') { 
				while (peek().has_value() && peek(1).value() != '\"') { //this may not work with empty strings
					if (peek().value() == EOF) {
						std::cerr << "[ERROR] String not ended\n";

					}
					else {
						buffer.push_back(eat());
					}
				}
				tokens.emplace_back(Tokens::string, line, buffer);
				buffer.clear();
			}
			else if (peek().value() == '\'') { //this may not work
				buffer.push_back(eat());
				if (peek().value() == '\'') {
					buffer.push_back(eat());
					tokens.emplace_back(Tokens::charliteral, line, buffer);
				}
				else if (peek().has_value() && peek(1).value() == '\'') {
					buffer.push_back(eat());
					buffer.push_back(eat());
					tokens.emplace_back(Tokens::charliteral, line, buffer);
				}
				else { std::cerr << "[ERROR] Charliteral not ended \n"; }
			}
			else{
				if (buffer == "(") {
					tokens.emplace_back(Tokens::lparen, line, buffer);
					eat();
				}
				else if (buffer == ")") {
					tokens.emplace_back(Tokens::rparen, line, buffer);
					eat();
				}
				else if (buffer == "{") {
					tokens.emplace_back(Tokens::lcurly, line, buffer);
					eat();
				}
				else if (buffer == "}") {
					tokens.emplace_back(Tokens::rcurly, line, buffer);
					eat();
				}
				else if (buffer == "[") {
					tokens.emplace_back(Tokens::lbracket, line, buffer);
					eat();
				}
				else if (buffer == "]") {
					tokens.emplace_back(Tokens::rbracket, line, buffer);
					eat();
				}
				else if (buffer == ",") {
					tokens.emplace_back(Tokens::COMMA, line, buffer);
					eat();
				}
				else if (buffer == ";") {
					tokens.emplace_back(Tokens::semicolon, line, buffer);
					eat();
				}
				else if (buffer == "!") {
					if (peek(1).value() == '=') {
						tokens.emplace_back(Tokens::relop, line, buffer);
						eat();
						eat();
					}
					else { // confirm if this works
						tokens.emplace_back(Tokens::_not, line, buffer);
						eat();
					}
				}
				else if (buffer == "<") {
					if (peek(1).value() == '=') {
						tokens.emplace_back(Tokens::relop, line, buffer);
						eat();
						eat();
					}
					else { // could simplify theoretically (should ask)
						tokens.emplace_back(Tokens::relop, line, buffer);
						eat();
					}
				}
				else if (buffer == ">") {
					if (peek(1).value() == '=') {
						tokens.emplace_back(Tokens::relop, line, buffer);
						eat();
						eat();
					}
					else { // could simplify theoretically (should ask)
						tokens.emplace_back(Tokens::relop, line, buffer);
						eat();
					}
				}
				else if (buffer == ":") {
					tokens.emplace_back(Tokens::COLON, line, buffer);
					eat();
				} 
				else if (buffer == "+" || buffer == "-") {
					tokens.emplace_back(Tokens::addop, line, buffer);
					eat();
				}
				else if (buffer == "|") { //may cause error
					if (peek(1).value() == '|') {
						tokens.emplace_back(Tokens::addop, line, buffer);
						eat();
						eat();
					}
					else { std::cerr << "[ERROR] Invalid token (|) \n"; }
				}
				else if (buffer == "*" || buffer == "%") { //WAIT ON / for comments from max
					tokens.emplace_back(Tokens::mulop, line, buffer);
					eat();
				}
				else if (buffer == "&") {
					if (peek(1).value() == '&') { //verify through testing
						tokens.emplace_back(Tokens::mulop, line, buffer); 
						eat();
						eat();
					}
					else { std::cerr << "[ERROR] Invalid Token (&)\n"; }
				} 
				else if (buffer == "=") {
					if (peek(1).value() == '=') { //may cause error
						tokens.emplace_back(Tokens::relop, line, buffer);
						eat();
						eat();
					}
					else {
						tokens.emplace_back(Tokens::assignop, line, buffer);
						eat();
					}
				}
				else if (std::isspace(peek().value())) eat(); //whitespace

				else if (peek().value() == EOF) { //confirm if this works
					tokens.emplace_back(Tokens::_eof, line, EOF);
					eat();
				}

				else { std::cerr << "[ERROR] Invalid token\n"; } //boogaloo (error) (Confirm if this is write
				
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

	//TODO: Command line arguments --DONE
		//TODO: Make it not bad --DONE
	//TODO: Finish enum --DONE
	//TODO: Tokenize keywords --DONE (baseline solution)
	//TODO: Tokenize tokens --Almost done
	//TODO: Debug printing


	return EXIT_SUCCESS;
}
