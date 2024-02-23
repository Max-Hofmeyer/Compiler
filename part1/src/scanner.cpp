//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#include "logger.h"
#include "scanner.h"

Scanner::Scanner(std::string source) : source_(std::move(source)), _index(0) {}

std::vector<token> Scanner::tokenize() {
	std::string buffer;
	std::vector<token> tokens;
	int line = 1;

	while (peek().has_value() && !_error) {
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
			if (peek().value() == '.') {
				buffer.push_back(eat());
				if (!std::isdigit(peek().value())) {
					Logger::error("No value after decimal at line: " + std::to_string(line));
					_error = true;
				}
				while (std::isdigit(peek().value())) { buffer.push_back(eat()); }
			}
			//exponent
			if (peek().value() == 'E') {
				buffer.push_back(eat());
				if (peek().value() == '+' || peek().value() == '-') buffer.push_back(eat());
				if (!std::isdigit(peek().value())) {
					Logger::error("No value after exponent at line: " + std::to_string(line));
					_error = true;
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
			//the simple way to handle block comments
			int nestedCommentCount = 1;
			while (nestedCommentCount > 0) {
				//if comment doesn't end before eof is reached
				if (!peek().has_value()) break;
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
			line++;
			eat();
		}
		//strings
		else if (peek().value() == '\"') {
			buffer.push_back(eat());
			while (peek().value() != '\"' && peek().value() != '\n') { buffer.push_back(eat()); }
			if (peek().value() == '\"') {
				buffer.push_back(eat());
				tokens.emplace_back(Tokens::string, line, "STRING", buffer);
			}
			else {
				Logger::error("Illegal string at line: " + std::to_string(line));
				_error = true;
			}
			buffer.clear();
		}
		//characters
		else if (peek().value() == '\'') {
			buffer.push_back(eat());
			//empty
			if (peek().value() == '\'' && peek().value() != '\n') {
				buffer.push_back(eat());
				tokens.emplace_back(Tokens::charliteral, line, "CHARLITERAL", buffer);
			}
			//value
			else if (peek(1).has_value() && peek(1).value() == '\'' && peek().value() != '\n'){
				buffer.push_back(eat());
				buffer.push_back(eat());
				tokens.emplace_back(Tokens::charliteral, line, "CHARLITERAL", buffer);
			}
			else {
				Logger::error("Illegal char at line: " + std::to_string(line));
				_error = true;
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
				if (peek(1).has_value() && peek(1).value() == '&') {
					tokens.emplace_back(Tokens::mulop, line, "MULOP", "&&");
					eat();
					eat();
				}
			}
			//ASSIGNOP (=)
			else if (peek().value() == '=') {
				if (peek(1).has_value() && peek(1).value() == '=') {
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
			//anything else must be illegal
			else {
				std::string illegalChar = std::string(1, peek().value());
				Logger::warning("Illegal character (" + illegalChar + ") at line: " + std::to_string(line));
				eat();
			}
		}
	}

	if (!_error) tokens.emplace_back(Tokens::eof, line, "EOF", "EOF");
	_index = 0;
	return tokens;
}

//looks at a character from the source string, default offset is 0
//offset allows us to look ahead in the string to check for multi-character
//tokens. Will return null if peeking exceeds length of source string
std::optional<char> Scanner::peek(int offset) const {
	if (_index + offset >= source_.length()) return {};
	return source_.at(_index + offset);
}

//returns the current character and increments the index
char Scanner::eat() {
	return source_.at(_index++);
}