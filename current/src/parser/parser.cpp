//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
#include "parser.h"

void Parser::Update(const token& t){
	//this cant stay but its a future problem to fix.. 
	if(t.type == Tokens::eof) {
		_tokenBuffer.emplace_back(t);
		parseProgram();
		Logger::scanner("Successfully parsed program");
	}
}

//also maybe bad, as the second a token is sent, were assuming its a valid program?
//extra logic needs to go here in the future
void Parser::parseProgram() {
	Logger::scanner("Entering Program");
	parseIntExpression();
	//parseBreakStatement();
	if (checkCurrentToken(Tokens::eof).has_value()) Logger::scanner("Program Identified");
	else _error = true;
}


//after I made checkCurrentToken, much better then first attempt, but would
//love to find a better way still
void Parser::parseBreakStatement() {
	Logger::scanner("Entering BreakStatement");
	if (checkCurrentToken(Tokens::_break).has_value()) {
		if(checkCurrentToken(Tokens::semicolon).has_value()) {
			Logger::parser("Breakstatement Identified");
		}
		else {
			Logger::error("bruh moment");
		}
	}
}
//bwfore I made checkCurrentToken..
//you can use it as a reference as to how the flow should be, and why
//I decided to make it

/*
void Parser::parseBreakStatement() {
	Logger::scanner("Entering Breakstatement");
	if (peek().has_value() && peek()->type == Tokens::_break) {
		eat();
		if (peek().has_value() && peek()->type == Tokens::semicolon) {
			eat();
			Logger::parser("Breakstatement");
		}
		else {
			Logger::error("Error: ';' expected after break");
		}
	}
}
*/

//can probably make it more inclusive, so chars can go here too..?
//trying to find the best style. Check for the false, or true on has_value()..
void Parser::parseIntExpression() {
    Logger::scanner("Entering parseIntDeclaration");	
    if (!checkCurrentToken(Tokens::_int).has_value()) {
        Logger::error("Expected int");
        return;
    }
    if (!checkCurrentToken(Tokens::ID).has_value()) {
        Logger::error("Expected identifier after int");
		return;
    }
    if (!checkCurrentToken(Tokens::assignop).has_value()) {
        Logger::error("Expected = after identifier");
        return;
    }
    if (!checkCurrentToken(Tokens::number).has_value()) {
        Logger::error("Expected number after =");
        return;
    }
    if (!checkCurrentToken(Tokens::semicolon).has_value()) {
        Logger::error("cmon man");
        return;
    }
}

//so we dont have to keep doing:
//if peek().value() == Token::type : eat().....
std::optional<token> Parser::checkCurrentToken(Tokens type) {
	if(_index < _tokenBuffer.size() && _tokenBuffer.at(_index).type == type) {
		return _tokenBuffer.at(_index++);
	}
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
