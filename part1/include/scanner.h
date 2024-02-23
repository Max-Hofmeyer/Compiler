//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <vector>
#include <optional>

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

class Scanner {
public:
    explicit Scanner(std::string source);
    std::vector<token> tokenize();
	bool hasError() const { return _error; }

private:
    std::string source_;
    size_t _index = 0;
	bool _error = false;

    std::optional<char> peek(int offset = 0) const;
    char eat();
};