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
	_newline
};

struct token {
	Tokens type;
	std::string typeString, value;
	int lineLoc;

	explicit token(Tokens type, int lineLoc, std::string typeStr, std::string value)
		: type(type), typeString(std::move(typeStr)), value(std::move(value)), lineLoc(lineLoc) {
	}
};