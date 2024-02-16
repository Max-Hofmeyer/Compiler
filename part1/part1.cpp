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

struct cli_input {
	std::string input_file_path;
	int debug_level = 2;
	bool help_enabled = false;
	bool verbose_enabled = false;
};

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
				
				/*std::unordered_map<std::string, std::vector<token>> tokenize_list = { //in progress: need to add to constructor (confirm with max)
					{"return", tokens.emplace_back(Tokens::_return, line, buffer)}
				};*/


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
				else if (buffer == "if"){ 
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
			else {
				if (buffer == "(") {
					tokens.emplace_back(Tokens::lparen, line, buffer);
					eat();
				}
				else{ eat(); } //whitespace
				
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

cli_input assign_command_line_values(const int count, char** arguments) {
	cli_input config;
	for (int i = 1; i < count; ++i) {
		if (std::string arg = arguments[i]; arg == "-help") {
			config.help_enabled = true;
		}
		else if (arg == "-debug" && i + 1 < count) {
			try {
				config.debug_level = std::stoi(arguments[++i]);
			} catch (const std::invalid_argument& e) {
				std::cerr << "Invalid argument for debug\n";
			}
		}
		else if (arg == "-verbose") {
			config.verbose_enabled = true;
		}
		else {
			config.input_file_path = arg;
		}
	}
	return config;
}

void verify_input(const cli_input& input) {
	if (input.help_enabled) {
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

	if (input.verbose_enabled) std::cout << "verbose active";
	if (input.debug_level != 2) {
		std::cout << "debug level is " << input.debug_level << "\n";
	}
	std::cout << "File path: " << input.input_file_path << "\n";
}

void print_tokens(const std::vector<token>& tokens) {
	for (const auto& t : tokens) {
		std::cout << t.typeString << "\n";
	}
	std::cout << tokens.size() << " tokens registered\n";
}

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik" << "\n";
	const cli_input input = assign_command_line_values(argc, argv);
	verify_input(input);

	std::string fileContents;
	{
		std::stringstream s;
		std::fstream f(input.input_file_path, std::ios::in);
		s << f.rdbuf();
		f.close();
		fileContents = s.str();
	}

	Scanner scanner(std::move(fileContents));
	std::vector<token> tokens = scanner.tokenize();
	print_tokens(tokens);

	//TODO: Command line arguments --DONE
		//TODO: Make it not bad --DONE
	//TODO: Finish enum --DONE
	//TODO: Tokenize keywords --DONE (baseline solution)
	//TODO: Tokenize tokens --in progress (got hangry)
	//TODO: Debug printing


	return EXIT_SUCCESS;
}
