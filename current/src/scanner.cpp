//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "scanner.h"

token Scanner::getNextToken(bool display) {
	_display = display;
	hasError = false;
	std::string buffer;
	if (peek().has_value()) {
		checkWhiteSpace();
		if (checkComments()) return createToken(Tokens::eof, line, "EOF", "EOF", false);

		//keywords
		if (std::isalpha(peek().value())) {
			buffer.push_back(eat());
			while (peek().has_value() && std::isalnum(peek().value())) { buffer.push_back(eat()); }

			if (buffer == "return") {
				return createToken(Tokens::_return, line, "RETURN",  buffer, true);
			}
			if (buffer == "int") {
				return createToken(Tokens::_int, line, "INT", buffer, true);
			}
			if (buffer == "char") {
				return createToken(Tokens::_char, line, "CHAR", buffer, true);
			}
			if (buffer == "if") {
				return createToken(Tokens::_if, line, "IF", buffer, true);
			}
			if (buffer == "else") {
				return createToken(Tokens::_else, line, "ELSE", buffer, true);
			}
			if (buffer == "for") {
				return createToken(Tokens::_for, line, "FOR", buffer, true);
			}
			if (buffer == "do") {
				return createToken(Tokens::_do, line, "DO", buffer, true);
			}
			if (buffer == "while") {
				return createToken(Tokens::_while, line, "WHILE", buffer, true);
			}
			if (buffer == "switch") {
				return createToken(Tokens::_switch, line, "SWITCH", buffer, true);
			}
			if (buffer == "case") {
				return createToken(Tokens::_case, line, "CASE", buffer, true);
			}
			if (buffer == "default") {
				return createToken(Tokens::_default, line, "DEFAULT", buffer, true);
			}
			if (buffer == "write") {
				return createToken(Tokens::_write, line, "WRITE", buffer, true);
			}
			if (buffer == "read") {
				return createToken(Tokens::_read, line, "READ", buffer, true);
			}
			if (buffer == "continue") {
				return createToken(Tokens::_continue, line, "CONTINUE", buffer, true);
			}
			if (buffer == "break") {
				return createToken(Tokens::_break, line, "BREAK", buffer, true);
			}
			if (buffer == "newline") {
				return createToken(Tokens::_newline, line, "NEWLINE", buffer, true);
			}

			return createToken(Tokens::ID, line, "ID", buffer, true);
		}
		//numbers
		while (std::isdigit(peek().value())) {
			buffer.push_back(eat());
			while (peek().has_value() && std::isdigit(peek().value())) { buffer.push_back(eat()); }
			//fraction
			if (peek().has_value() && peek().value() == '.') {
				buffer.push_back(eat());
				if (!std::isdigit(peek().value())) {
					return reportError("No value after decimal at line: " + std::to_string(line));
				}
				while (peek().has_value() && std::isdigit(peek().value())) { buffer.push_back(eat()); }
			}
			//exponent
			if (peek().has_value() && peek().value() == 'E') {
				buffer.push_back(eat());
				if (peek().value() == '+' || peek().value() == '-') buffer.push_back(eat());
				if (!std::isdigit(peek().value())) {
					return reportError("No value after exponent at line: " + std::to_string(line));
				}
				while (peek().has_value() && std::isdigit(peek().value())) { buffer.push_back(eat()); }
			}

			return createToken(Tokens::number, line, "NUMBER", buffer, false);
		}
		//strings
		if (peek().value() == '\"') {
			buffer.push_back(eat());
			while (peek().value() != '\"' && peek().value() != '\n') { buffer.push_back(eat()); }
			if (peek().value() == '\"') {
				buffer.push_back(eat());
				return createToken(Tokens::string, line, "STRING", buffer, false);
			}

			return reportError("Illegal string at line: " + std::to_string(line));
		}
		//characters
		if (peek().value() == '\'') {
			buffer.push_back(eat());
			//empty
			if (peek().value() == '\'' && peek().value() != '\n') {
				buffer.push_back(eat());
				return createToken(Tokens::charliteral, line, "CHARLITERAL", buffer, false);
			}
			//value
			if (peek(1).has_value() && peek(1).value() == '\'' && peek().value() != '\n') {
				buffer.push_back(eat());
				buffer.push_back(eat());
				return createToken(Tokens::charliteral, line, "CHARLITERAL", buffer, false);
			}
			Logger::error("Illegal char at line: " + std::to_string(line));
			hasError = true;
		}

		/* Tokens */
		//LPAREN (()
		if (peek().value() == '(') {
			eat();
			return createToken(Tokens::lparen, line, "LPAREN", "(", false);
		}
		//RPAREN ())
		else if (peek().value() == ')') {
			eat();
			return createToken(Tokens::rparen, line, "RPAREN", ")", false);
		}
		//LCURLY ({)
		else if (peek().value() == '{') {
			eat();
			return createToken(Tokens::lcurly, line, "LCURLY", "{", false);
		}
		//RCURLY (})
		else if (peek().value() == '}') {
			eat();
			return createToken(Tokens::rcurly, line, "RCURLY", "}", false);
		}
		//LBRACKET([)
		else if (peek().value() == '[') {
			eat();
			return createToken(Tokens::lbracket, line, "LBRACKET", "[", false);
		}
		//RBRACKET (])
		else if (peek().value() == ']') {
			eat();
			return createToken(Tokens::rbracket, line, "RBRACKET", "]", false);
		}
		//COMMA (,)
		else if (peek().value() == ',') {
			eat();
			return createToken(Tokens::comma, line, "COMMA", ",", false);
		}
		//SEMICOLON (;)
		else if (peek().value() == ';') {
			eat();
			return createToken(Tokens::semicolon, line, "SEMICOLON", ";", true);
		}
		//RELOP(!=) or NOT(!)
		else if (peek().value() == '!') {
			if (peek(1).has_value() && peek(1).value() == '=') {
				eat();
				eat();
				return createToken(Tokens::relop, line, "RELOP", "!=", false);
			}
			eat();
			return createToken(Tokens::_not, line, "NOT", "!", false);
		}
		//RELOP (<= or <)
		else if (peek().value() == '<') {
			if (peek(1).has_value() && peek(1).value() == '=') {
				eat();
				eat();
				return createToken(Tokens::relop, line, "RELOP", "<=", false);
			}
			eat();
			return createToken(Tokens::relop, line, "RELOP", "<", false);
		}
		//RELOP (>= or >)
		else if (peek().value() == '>') {
			if (peek(1).has_value() && peek(1).value() == '=') {
				eat();
				eat();
				return createToken(Tokens::relop, line, "RELOP", ">=", false);
			}
			eat();
			return createToken(Tokens::relop, line, "RELOP", ">", false);
		}
		//COLON (:)
		else if (peek().value() == ':') {
			eat();
			return createToken(Tokens::colon, line, "COLON", ":", false);
		}
		//ADDOP (-)
		else if (peek().value() == '-') {
			eat();
			return createToken(Tokens::addop, line, "ADDOP", "-", false);
		}
		//ADDOP (+)
		else if (peek().value() == '+') {
			eat();
			return createToken(Tokens::addop, line, "ADDOP", "+", false);
		}
		//ADDOP (||)
		else if (peek().value() == '|') {
			if (peek(1).has_value() && peek(1).value() == '|') {
				eat();
				eat();
				return createToken(Tokens::addop, line, "ADDOP", "||", false);
			}
		}
		//MULOP (*)
		else if (peek().value() == '*') {
			eat();
			return createToken(Tokens::mulop, line, "MULOP", "*", false);
		}
		//MULOP (%)
		else if (peek().value() == '%') {
			eat();
			return createToken(Tokens::mulop, line, "MULOP", "%", false);
		}
		//MULOP (/)
		else if (peek().value() == '/') {
			eat();
			return createToken(Tokens::mulop, line, "MULOP", "/", false);
		}
		//MULOP (&&)
		else if (peek().value() == '&') {
			if (peek(1).has_value() && peek(1).value() == '&') {
				eat();
				eat();
				return createToken(Tokens::mulop, line, "MULOP", "&&", false);
			}
		}
		//ASSIGNOP (=)
		else if (peek().value() == '=') {
			if (peek(1).has_value() && peek(1).value() == '=') {
				eat();
				eat();
				return createToken(Tokens::relop, line, "RELOP", "==", false);
			}
			eat();
			return createToken(Tokens::assignop, line, "ASSIGNOP", "=", false);
		}
		else {
			if (isspace(peek().value())) {
				checkWhiteSpace();
				return getNextToken();
			}
			auto illegalChar = std::string(1, peek().value());
			Logger::warning("Illegal character (" + illegalChar + ") at line: " + std::to_string(line));
			eat();
			return token{ Tokens::ILLEGAL, -1, "ILLEGAL TOKEN", illegalChar, false };
		}
	}
	return createToken(Tokens::eof, line, "EOF", "EOF", false);
	//return 
}

void Scanner::checkWhiteSpace() {
	if (peek().has_value()) {
		//space
		while (peek().has_value() && std::isspace(peek().value()) && peek().value() != '\n') {
			eat();
		}
		while(peek().has_value() && peek().value() == '\n') {
			line++;
			eat();
		}
	}
}

bool Scanner::checkComments() {
	if (peek().has_value()) {
		if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
			eat();
			eat();
			//the simple way to handle block comments
			int nestedCommentCount = 1;
			while (nestedCommentCount > 0) {
				//if comment doesn't end before eof is reached
				if (!peek().has_value()) return true;
				if (peek().value() == '\n') line++;
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
	}
	else return true;
	return false;
}

//looks at a character from the source string, default offset is 0
//offset allows us to look ahead in the string to check for multi-character
//tokens. Will return null if peeking exceeds length of source string
std::optional<char> Scanner::peek(int offset) const {
	if (_index + offset >= source_.length()) return {};
	char temp = source_.at(_index + offset);
	return temp;
}

//returns the current character and increments the index
char Scanner::eat() {
	if (_index >= source_.length()) return ' ';
	return source_.at(_index++);
}


token Scanner::createToken(Tokens tokenType, int line, const std::string& buffer, const std::string& value,
                           const bool isKeyword) {
	const token t{tokenType, line, buffer, value, isKeyword};
	if(_display) Logger::scanner("(<" + t.typeString + ">,\"" + value + "\")");
	return t;
}

token Scanner::reportError(const std::string& message) {
	Logger::error(message);
	hasError = true;
	return token{Tokens::ERROR, -1, "ERROR TOKEN", "ERROR VALUE", false};
}
