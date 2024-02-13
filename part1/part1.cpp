//Max Hofmeyer & Ahmed Malik

#include "part1.h"
#include <optional>
#include <string>
#include <utility>
#include <vector>

//questions:
//are the debug comments graded format wise

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
	int lineLoc;
	std::optional<std::string> value;

	explicit token(Tokens type, int lineLoc, std::optional<std::string> value = std::nullopt)
		: type(type), lineLoc(lineLoc), value(std::move(value)) {
	}
};

struct cli_input {
	std::string input_file;
	std::optional<int> debug_level;
	bool help_enabled = false;
	bool verbose_enabled = false;
};


class Scanner {
public: //transfer instead of duplicate
	explicit Scanner(std::string source) : source_(std::move(source)) {
	}

	std::vector<token> tokenize() {
		std::string buffer;
		std::vector<token> tokens;
		int line = 1;

		while (peek().has_value()) {
			if (std::isalpha(peek().value())) {
				buffer.push_back(eat());
				while (peek().has_value() && std::isalnum(peek().value())) {
					buffer.push_back(eat());
				}

				if (buffer == "return") {
					tokens.emplace_back(Tokens::_return, line);
					buffer.clear();
				}
				else if (buffer == "int") {
					tokens.emplace_back(Tokens::_int, line);
					buffer.clear();
				}
				else if (buffer == "char") {
					tokens.emplace_back(Tokens::_char, line);
					buffer.clear();
				}
				else if (buffer == "if") {
					tokens.emplace_back(Tokens::_if, line);
				}
			}
		}
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
	for (int i = 1; i < count; i++) {
		std::string arg = arguments[i];
		if (arg == "-help") {
			config.help_enabled = true;
		}
		else if (arg == "-debug" && i + 1 < count) {
			config.debug_level = std::stoi(arguments[i++]);
		}
		else if (arg == "-verbose") {
			config.verbose_enabled = true;
		}
		else {
			config.input_file = arg;
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
	if (input.debug_level.has_value()) {
		std::cout << "debug level is" << input.debug_level.value() << "\n";
	}
	else {
		std::cout << "default" << "\n";
	}
}

int main(int argc, char** argv) {
	std::cout << "Max Hofmeyer & Ahmed Malik" << "\n";

	const cli_input input = assign_command_line_values(argc, argv);
	verify_input(input);

	//TODO: Command line arguments --DONE
	//TODO: Finish enum --DONE
	//TODO: Tokenize keywords
	//TODO: Tokenize tokens
	//TODO: Debug printing


	return EXIT_SUCCESS;
}
