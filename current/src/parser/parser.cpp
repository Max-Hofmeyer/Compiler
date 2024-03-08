//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
#include "parser.h"

	/*if(t.type == Tokens::eof) {
		parseProgram();
		Logger::scanner("Successfully parsed program");
	}*/
void Parser::Update(const token& t){
	if (t.lineLoc == 1) tokline = 1;
	_tokenBuffer.emplace_back(t);
	if (t.lineLoc != tokline || t.type == Tokens::eof) {
		yourmom = true;
		parseProgram();
	}
	tokline = t.lineLoc;
	Logger::debug(std::to_string(_tokenBuffer.size()) + " tokens in parser");
	
}

void Parser::parseProgram() {
	yourmom = false;
	parseType();
	parseIntExpression();
	//parseBreakStatement();
	if (checkAndEatToken(Tokens::eof)) Logger::scanner("Exiting Program");
	else Logger::error("No EOF detected/ Program error");
}

void Parser::parseType() {
	Logger::parser("Entering Type");
	if (checkAndEatToken(Tokens::_int)) Logger::scanner("Generated Int");
	if (checkAndEatToken(Tokens::_char)) Logger::scanner("Generated Char");
	Logger::parser("Exiting Type");
}

void Parser::parseStatement() {
	Logger::parser("Entering Statement");
	//expression

	//break
	if(checkAndEatToken(Tokens::_break)) {
		Logger::parser("Entering break statement");
		if (checkAndEatToken(Tokens::semicolon)) {
			Logger::parser("Exiting break statement");
		}
	}

	//compound

	//if

	//null
	if (checkAndEatToken(Tokens::semicolon)) {
		Logger::parser("Null statement");
	}
	//return
	if (checkAndEatToken(Tokens::_return)) {
		Logger::parser("Entering return statement");
		if (checkAndEatToken(Tokens::semicolon)) {
			Logger::parser("Exiting return statement");
		}
	}
	//while
	if (checkAndEatToken(Tokens::_while)) {
		Logger::parser("Entering while statement");
		if (checkAndEatToken(Tokens::lparen)) {
			parseExpression();
			if (checkAndEatToken(Tokens::rparen)) {
				//TODO: parseStatement();
				Logger::parser("Exiting while statement");
			}else {
				Logger::error("missed )");
			}
		}else {
			Logger::error("missed while");
		}
		Logger::parser("exiting while statement");
	}
	//read

	//write

	//newline

	//expression

	//relop

	//simple

}


//can probably make it more inclusive, so chars can go here too..?
//trying to find the best style. Check for the false, or true on has_value()..
void Parser::parseIntExpression() {
    Logger::scanner("Entering parseIntDeclaration");	
    if (!eatCurrentToken(Tokens::_int).has_value()) {
        Logger::error("Expected int");
        return;
    }
    if (!eatCurrentToken(Tokens::ID).has_value()) {
        Logger::error("Expected identifier after int");
		return;
    }
    if (!eatCurrentToken(Tokens::assignop).has_value()) {
        Logger::error("Expected = after identifier");
        return;
    }
    if (!eatCurrentToken(Tokens::number).has_value()) {
        Logger::error("Expected number after =");
        return;
    }
    if (!eatCurrentToken(Tokens::semicolon).has_value()) {
        Logger::error("cmon man");
    }
	Logger::parser("Generated int");
	Logger::parser("Exiting parseIntExpression");

}

void Parser::parseExpression() {
	Logger::scanner("Entering Expression");
	if (checkAndEatToken(Tokens::number)) {}
	else if (checkAndEatToken(Tokens::lparen)) {
		parseExpression();
		if (checkAndEatToken(Tokens::rparen)) {
			Logger::scanner("found )");
		}
		else {
			Logger::error("expected )");

		}
	}
	else if (checkAndEatToken(Tokens::ID)) {
		Logger::scanner("Created expression");
	}
	else {
		Logger::error("Expected something");
	}
}

//if current token matches, it gets eaten
bool Parser::checkAndEatToken(Tokens type) {
	if (eatCurrentToken(type).has_value()) return true;
	return false;
}

//returns null if the value isnt inside the buffer
std::optional<token> Parser::eatCurrentToken(Tokens type) {
	if(_index < _tokenBuffer.size() && _tokenBuffer.at(_index).type == type) return eat();
	return {};
}

//same as the scanner, we may or may not use it
std::optional<token> Parser::peek(int offset) const {
	if (_index + offset >= _tokenBuffer.size()) return {};
	return _tokenBuffer.at(_index + offset);
}

//also same as scanner
token Parser::eat() {
	return _tokenBuffer.at(_index++);
}
