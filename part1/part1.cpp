//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

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
	ID,
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
	comma,
	colon,
	eof,
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
		: type(type), typeString(std::move(typeStr)), lineLoc(lineLoc), value(std::move(value)) {
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
		if (logLevel <= Level::Scanner) {
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

	static void warning(const std::string& message) {
		std::cout << "[WARNING] " << message << "\n";
	}

	static void error(const std::string& message) {
		std::cout << "[ERROR] " << message << "\n";
	}
};

Logger::Level Logger::logLevel = Level::Default;


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

int CliConfig::debugLevel = 2;
bool CliConfig::helpEnabled = false;
bool CliConfig::verboseEnabled = false;
bool CliConfig::defaultEnabled = false;
std::string CliConfig::filePath;
std::string CliConfig::fileContents;

class Scanner {
public:
	explicit Scanner(std::string source) : source_(std::move(source)) {
	}

	std::vector<token> tokenize() {
		std::string buffer;
		std::vector<token> tokens;
		bool error = false;
		int line = 1;

		while (peek().has_value() && !error) {
			//keywords
			if (std::isalpha(peek().value())) {
				buffer.push_back(eat());
				while (peek().has_value() && std::isalnum(peek().value())) { buffer.push_back(eat()); }
				Logger::debug(buffer);

				if (buffer == "return") {
					tokens.emplace_back(Tokens::_return, line, "RETURN", buffer);
					buffer.clear();
				}
				else if (buffer == "int") {
					tokens.emplace_back(Tokens::_int, line, "INT", buffer);
					buffer.clear();
				}
				else if (buffer == "char") {
					tokens.emplace_back(Tokens::_char, line, "CHAR", buffer);
					buffer.clear();
				}
				else if (buffer == "if") {
					tokens.emplace_back(Tokens::_if, line, "IF", buffer);
					buffer.clear();
				}
				else if (buffer == "else") {
					tokens.emplace_back(Tokens::_else, line, "ELSE", buffer);
					buffer.clear();
				}
				else if (buffer == "for") {
					tokens.emplace_back(Tokens::_for, line, "FOR", buffer);
					buffer.clear();
				}
				else if (buffer == "do") {
					tokens.emplace_back(Tokens::_do, line, "DO", buffer);
					buffer.clear();
				}
				else if (buffer == "while") {
					tokens.emplace_back(Tokens::_while, line, "WHILE", buffer);
					buffer.clear();
				}
				else if (buffer == "switch") {
					tokens.emplace_back(Tokens::_switch, line, "SWITCH", buffer);
					buffer.clear();
				}
				else if (buffer == "case") {
					tokens.emplace_back(Tokens::_case, line, "CASE", buffer);
					buffer.clear();
				}
				else if (buffer == "default") {
					tokens.emplace_back(Tokens::_default, line, "DEFAULT", buffer);
					buffer.clear();
				}
				else if (buffer == "write") {
					tokens.emplace_back(Tokens::_write, line, "WRITE", buffer);
					buffer.clear();
				}
				else if (buffer == "read") {
					tokens.emplace_back(Tokens::_read, line, "READ", buffer);
					buffer.clear();
				}
				else if (buffer == "continue") {
					tokens.emplace_back(Tokens::_continue, line, "CONTINUE", buffer);
					buffer.clear();
				}
				else if (buffer == "break") {
					tokens.emplace_back(Tokens::_break, line, "BREAK", buffer);
					buffer.clear();
				}
				else {
					tokens.emplace_back(Tokens::ID, line, "ID", buffer);
					buffer.clear();
				}
			}
			//numbers
			else if (std::isdigit(peek().value())) {
				buffer.push_back(eat());
				while (std::isdigit(peek().value())) { buffer.push_back(eat()); }
				//fraction
				if(peek().value() == '.') {
					buffer.push_back(eat());
					if(!std::isdigit(peek().value())) {
						Logger::error("No value after decimal at line: " + std::to_string(line));
						error = true;
					}
					while (std::isdigit(peek().value())) { buffer.push_back(eat()); }
				}

				//exponent
				if(peek().value() == 'E') {
					buffer.push_back(eat());
					if (peek().value() == '+' || peek().value() == '-') buffer.push_back(eat());
					if (!std::isdigit(peek().value())) {
						Logger::error("No value after exponent at line: " + std::to_string(line));
					}
					while (std::isdigit(peek().value())) { buffer.push_back(eat()); }
				}
				tokens.emplace_back(Tokens::number, line, "NUMBER", buffer);
				buffer.clear();
			}
			//inline comments
			else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
				eat();
				eat();
				while (peek().has_value() && peek().value() != '\n') {
					eat();
				}
			}
			//block comments
			else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
				eat();
				eat();
				int nestedCommentCount = 1;

				//handling nested blocks
				while (nestedCommentCount > 0) {
					if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
						eat();
						eat();
						nestedCommentCount++;
					}
					else if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/') {
						eat();
						eat();
						nestedCommentCount--;
					}
					else eat();
				}
			}
			//newline
			else if (peek().value() == '\n') {
				eat();
				line++;
			}
			//strings
			else if (peek().value() == '\"') {
				buffer.push_back(eat());
				while (peek().value() != '\"' && peek().value() != '\n') { buffer.push_back(eat()); }
				if(peek().value() == '\"') {
					buffer.push_back(eat());
					tokens.emplace_back(Tokens::string, line, "STRING", buffer);
				} else {
					Logger::error("String not terminated at line: " + std::to_string(line));
					error = true;
				}
				buffer.clear();
			}
			//characters
			else if (peek().value() == '\'') {
				buffer.push_back(eat());
				//empty
				if (peek().value() == '\'') {
					buffer.push_back(eat());
					tokens.emplace_back(Tokens::charliteral, line, "CHARLITERAL", buffer);
				}
				//value
				else if (peek(1).has_value() && peek(1).value() == '\'') {
					buffer.push_back(eat());
					buffer.push_back(eat());
					tokens.emplace_back(Tokens::charliteral, line, "CHARLITERAL", buffer);
				}
				else {
					Logger::error("Char not terminated at line: " + std::to_string(line));
					error = true;
				}
				buffer.clear();
			}
			//tokens
			else {
				//LPAREN (()
				if (peek().value() == '(') {
					tokens.emplace_back(Tokens::lparen, line, "LPAREN", "(");
					eat();
				}
				//RPAREN ())
				else if (peek().value() == ')') {
					tokens.emplace_back(Tokens::rparen, line, "RPAREN", ")");
					eat();
				}
				//LCURLY ({)
				else if (peek().value() == '{') {
					tokens.emplace_back(Tokens::lcurly, line, "LCURLY", "{");
					eat();
				}
				//RCURLY (})
				else if (peek().value() == '}') {
					tokens.emplace_back(Tokens::rcurly, line, "RCURLY", "}");
					eat();
				}
				//LBRACKET([)
				else if (peek().value() == '[') {
					tokens.emplace_back(Tokens::lbracket, line, "LBRACKET", "[");
					eat();
				}
				//RBRACKET (])
				else if (peek().value() == ']') {
					tokens.emplace_back(Tokens::rbracket, line, "RBRACKET", "]");
					eat();
				}
				//COMMA (,)
				else if (peek().value() == ',') {
					tokens.emplace_back(Tokens::comma, line, "COMMA", ",");
					eat();
				}
				//SEMICOLON (;)
				else if (peek().value() == ';') {
					tokens.emplace_back(Tokens::semicolon, line, "SEMICOLON", ";");
					eat();
				}
				//RELOP(!=) or NOT(!)
				else if (peek().value() == '!') {
					if (peek(1).has_value() && peek(1).value() == '=') {
						tokens.emplace_back(Tokens::relop, line, "RELOP", "!=");
						eat();
						eat();
					}
					else {
						// confirm if this works
						tokens.emplace_back(Tokens::_not, line, "NOT", "!");
						eat();
					}
				}
				//RELOP (<= or <)
				else if (peek().value() == '<') {
					if (peek(1).has_value() && peek(1).value() == '=') {
						tokens.emplace_back(Tokens::relop, line, "RELOP", "<=");
						eat();
						eat();
					}
					else {
						// could simplify theoretically (should ask)
						tokens.emplace_back(Tokens::relop, line, "RELOP", "<");
						eat();
					}
				}
				//RELOP (>= or >)
				else if (peek().value() == '>') {
					if (peek(1).has_value() && peek(1).value() == '=') {
						tokens.emplace_back(Tokens::relop, line, "RELOP", ">=");
						eat();
						eat();
					}
					else {
						// could simplify theoretically (should ask)
						tokens.emplace_back(Tokens::relop, line, "RELOP", ">");
						eat();
					}
				}
				//COLON (:)
				else if (peek().value() == ':') {
					tokens.emplace_back(Tokens::colon, line, "COLON", ":");
					eat();
				}
				//ADDOP (-)
				else if (peek().value() == '-') {
					tokens.emplace_back(Tokens::addop, line, "ADDOP", "-");
					eat();
				}
				//ADDOP (+)
				else if (peek().value() == '+') {
					tokens.emplace_back(Tokens::addop, line, "ADDOP", "+");
					eat();
				}
				//ADDOP (||)
				else if (peek().value() == '|') {
					//may cause error
					if (peek(1).has_value() && peek(1).value() == '|') {
						tokens.emplace_back(Tokens::addop, line, "ADDOP", "||");
						eat();
						eat();
					}
				}
				//MULOP (*)
				else if (peek().value() == '*') {
					tokens.emplace_back(Tokens::mulop, line, "MULOP", "*");
					eat();
				}
				//MULOP (%)
				else if (peek().value() == '%') {
					tokens.emplace_back(Tokens::mulop, line, "MULOP", "%");
					eat();
				}
				//MULOP (/)
				else if (peek().value() == '/') {
					tokens.emplace_back(Tokens::mulop, line, "MULOP", "/");
					eat();
				}
				//MULOP (&&)
				else if (peek().value() == '&') {
					//may cause error
					if (peek(1).has_value() && peek(1).value() == '&') {
						tokens.emplace_back(Tokens::mulop, line, "MULOP", "&&");
						eat();
						eat();
					}
				}
				//ASSIGNOP (=)
				else if (peek().value() == '=') {
					if (peek(1).has_value() && peek(1).value() == '=') {
						//may cause error
						tokens.emplace_back(Tokens::relop, line, "RELOP", "==");
						eat();
						eat();
					}
					else {
						tokens.emplace_back(Tokens::assignop, line, "ASSIGNOP", "=");
						eat();
					}
				}
				//whitespace
				else if (std::isspace(peek().value())) eat();
				//anything else must be illegal...
				else {
				std::string illegalChar = std::string(1, peek().value());
					Logger::warning("Illegal character(" + illegalChar + ") at line: "+ std::to_string(line));
				}
			}
		}

		if (!error) tokens.emplace_back(Tokens::eof, line, "EOF", "EOF");
		_index = 0;
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
		//a guard against ourselves, wont throw an exception if we forget to assign a value
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
		print_tokens(tokens);
	}

	if (!CliConfig::LoadFile()) {
		Logger::error("Failed to load file");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
