 
#pragma once

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
	_newline,
	ERROR,
	ILLEGAL
};

struct token {
	Tokens type;
	std::string typeString, value;
	int lineLoc;
	bool isKeyword;

	explicit token(Tokens type, int lineLoc, std::string typeString, std::string value, bool isKeyword)
		: type(type), typeString(std::move(typeString)), value(std::move(value)), lineLoc(lineLoc), isKeyword(isKeyword) {
	}
};