//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
#include "parser.h"

//subscribed to the scanner. Logic inside buffers the token line by line.
void Parser::Update(const token& t){
	//since we need to start parsing after a complete line
	if (!hasError) {
		if (currentLine == 0) currentLine = t.lineLoc;

		//current token is on the next line, so start parsing
		if (t.lineLoc != currentLine || t.type == Tokens::eof) {
			if (!_tokenBuffer.empty()) {
				parseLine();
				_tokenBuffer.clear();
				_index = 0;
			}
			//makes sure the current token is added 
			if (t.type != Tokens::eof) {
				_tokenBuffer.emplace_back(t);
				currentLine = t.lineLoc;
			}
			//edge case of an empty program
			else {
				_tokenBuffer.emplace_back(t);
				parseLine();
			}
		}
		else _tokenBuffer.emplace_back(t);
	}
}

void Parser::parseLine() {
	//first line conditions
	if (!parsingStarted) {
		Logger::parserEnter("Program");
		parsingStarted = true;
	}

	//instead of calling peek(0) & peek(1), just declare it and call it once instead
	auto currentT = peek(0);
	if (!currentT.has_value()) reportError("Expected a value in the line");
	auto nextT = peek(1);

	//if eof its the end of the program
	if(currentT.value().type == Tokens::eof) {
		stubASTNode();
		Logger::parserCreate("Program");
		Logger::parserExit("Program");
		return;
	}

	//parse according to BNF hierarchy
	if (!hasError){
		if (currentT.value().type == Tokens::_char || currentT.value().type == Tokens::_int) {
			parseType();
			parseIdentifier();
			if(nextT.has_value()) {
				if (nextT.value().type == Tokens::lparen) parseFunctionCall();
				else if (nextT.value().type == Tokens::assignop) parseExpression();
			}
		}
		else if(currentT.value().type == Tokens::ID) {
			parseIdentifier();
			if(nextT.has_value()) {
				if (nextT.value().type == Tokens::assignop) parseExpression();
				else if (nextT.value().type == Tokens::lparen) parseFunctionCall();
			}
		}
		else if(currentT.value().isKeyword) {
			//todo: statements
			Logger::parser("Stub statement");
		}
		if (!checkAndEatToken(Tokens::semicolon) && !checkAndEatToken(Tokens::eof)) reportError("Expected semicolon");
		if (checkAndEatToken(Tokens::eof)) Logger::parserExit("Program");
	}
}

void Parser::parsePrimary() {
	Logger::parserEnter("Primary");
	if (checkAndEatToken(Tokens::ID)) {
		parseIdentifier();
		//since parseFunctionCall is the one that eats the lparen, just check for it
		if (peek().has_value() && peek().value().type == Tokens::lparen) parseFunctionCall();
		else reportError("Expected (");
	}

	//number
	else if (checkAndEatToken(Tokens::number)) {
		stubASTNode();
		Logger::parserCreate("number");
	}

	//charliteral
	else if (checkAndEatToken(Tokens::charliteral)) {
		stubASTNode();
		Logger::parserCreate("char");
	}

	//string
	else if (checkAndEatToken(Tokens::string)) {
		stubASTNode();
		Logger::parserCreate("string");
	}

	//expression
	else if (checkAndEatToken(Tokens::lparen)) {
		parseExpression();
		if (!checkAndEatToken(Tokens::rparen)) reportError("expected (");
	}

	//primary (-)
	else if (checkAndEatToken(Tokens::addop)) {
		//might use peek() instead
		if (previousToken().value == "-") {
			Logger::parserEnter("subtraction primary");
			parsePrimary();
			stubASTNode();
			Logger::parserCreate("subtraction primary");
			Logger::parserExit("subtraction primary");
		}
		else reportError("Expected '-' addop for primary");
	}

	//primary (not)
	else if (checkAndEatToken(Tokens::_not)) {
		Logger::parserEnter("not primary");
		parsePrimary();
		Logger::parserExit("not primary");
	}

	else reportError("Unexpected primary");
	Logger::parserExit("Primary");
}

void Parser::parseType() {
	Logger::parserEnter("Type");
	if (checkAndEatToken(Tokens::_int)) {
		stubASTNode();
		Logger::parserCreate("Int");
	}
	if (checkAndEatToken(Tokens::_char)) {
		stubASTNode();
		Logger::parserCreate("Char");
	}
	Logger::parserExit("Type");
}

void Parser::parseIdentifier() {
	Logger::parserEnter("Identifier");
	if (checkAndEatToken(Tokens::ID)) {
		stubASTNode();
		Logger::parserCreate("Identifier");
	}
	Logger::parserExit("Identifier");
}

void Parser::parseTerm() {
	Logger::parserEnter("Term");
	parsePrimary();
	if(checkAndEatToken(Tokens::mulop)) {
		parsePrimary();
		stubASTNode();
		Logger::parserCreate("Term");
	}
	else reportError("Expected mulop term");
	Logger::parserExit("Term");
}

void Parser::parseSimpleExpression() {
	Logger::parserEnter("SimpleExpression");
	parseTerm();

	if(checkAndEatToken(Tokens::mulop)) {
		parseTerm();
		stubASTNode();
		Logger::parserCreate("SimpleExpression");

	}
	else reportError("Expected mulop expression");
	Logger::parserExit("SimpleExpression");
}

void Parser::parseRelopExpression() {
	Logger::parserEnter("RelopExpression");
	parseSimpleExpression();

	if (checkAndEatToken(Tokens::relop)) {
		parseSimpleExpression();
		stubASTNode();
		Logger::parserCreate("RelopExpression");

	}
	else reportError("Expected relop expression");
	Logger::parserExit("RelopExpression");
}

void Parser::parseExpression() {
	Logger::parserEnter("Expression");
	parseRelopExpression();

	if (checkAndEatToken(Tokens::assignop)) {
		parseRelopExpression();
		stubASTNode();
		Logger::parserCreate("Expression");
	}
	else reportError("Expected assignop expression");
	Logger::parserExit("Expression");
}

void Parser::parseActualParameters() {
	Logger::parserEnter("ActualParameters");
	parseExpression();
	while (checkAndEatToken(Tokens::comma)) {
		parseExpression();
		stubASTNode();
		Logger::parserCreate("ActualParameters");
	}
	Logger::parserExit("ActualParameters");
}

void Parser::parseFunctionCall() {
	Logger::parserEnter("FunctionCall");
	if(checkAndEatToken(Tokens::lparen)) {
		if(!checkAndEatToken(Tokens::rparen)) {
			parseActualParameters();
			if(!checkAndEatToken(Tokens::rparen)) reportError("Expected )");
			else {
				stubASTNode();
				Logger::parserCreate("FunctionCall");
			}
		}
	}
	else reportError("Expected (");

	Logger::parserExit("FunctionCall");
}

//used for convenience, I didnt want to do "if eatCurrentToken().has_value()" everywhere..
bool Parser::checkAndEatToken(Tokens type) {
	if (eatCurrentToken(type).has_value()) return true;
	return false;
}

//returns null if the current token in the buffer doesnt match 
std::optional<token> Parser::eatCurrentToken(Tokens type) {
	if (_index < _tokenBuffer.size() && _tokenBuffer.at(_index).type == type) return eat();
	return {};
}

std::optional<token> Parser::peek(int offset) const {
	if (_index + offset >= _tokenBuffer.size()) return {};
	return _tokenBuffer.at(_index + offset);
}

//returns the current token and increments the index 
token Parser::eat() { return _tokenBuffer.at(_index++); }

//returns the last token checkAndEatToken ate
token Parser::previousToken() {
	if(_index > 0 && !_tokenBuffer.empty()) return _tokenBuffer.at( _index - 1);
	return _tokenBuffer.at(0);
}

//for debugging, and maybe error messages
void Parser::outputTokenLine() const { Logger::outputTokens(_tokenBuffer); }

//stub for ast, will be replaced with actual node definitions
void Parser::stubASTNode(){
	Logger::parserCreate("A node");
	//Logger::outputToken(previousToken());
}

void Parser::reportError(const std::string& message) {
	Logger::error(message);
	hasError = true;
}


//void Parser::parseStatement() {
//	Logger::parserEnter("Statement");
//	//expression
//
//	//break
//	if(checkAndEatToken(Tokens::_break)) {
//		Logger::parserEnter("Break");
//		if (checkAndEatToken(Tokens::semicolon)) {
//			Logger::parserExit("Break");
//		}
//	}
//
//	//compound
//
//	//if
//
//	//null
//	if (checkAndEatToken(Tokens::semicolon)) {
//		Logger::parser("Null statement");
//	}
//	//return
//	if (checkAndEatToken(Tokens::_return)) {
//		Logger::parser("Entering return statement");
//		if (checkAndEatToken(Tokens::semicolon)) {
//			Logger::parser("Exiting return statement");
//		}
//	}
//	//while
//	if (checkAndEatToken(Tokens::_while)) {
//		Logger::parserEnter("While");
//		if (checkAndEatToken(Tokens::lparen)) {
//			//parseExpression();
//			if (checkAndEatToken(Tokens::rparen)) {
//				//TODO: parseStatement();
//			}else {
//				reportError("missed )");
//			}
//		}else {
//			reportError("missed while");
//		}
//		Logger::parserExit("While");
//	}
//	//read
//
//	//write
//
//	//newline
//
//	//expression
//
//	//relop
//
//	//simple
//
//}

//void Parser::parseFunctionCall() {
//	return;
//	while(peek().has_value() && peek().value().type != Tokens::rparen) {
//		
//	}
//}

//void Parser::parseIntExpression() {
//    Logger::scanner("Entering parseIntDeclaration");	
//    if (!eatCurrentToken(Tokens::_int).has_value()) {
//        reportError("Expected int");
//        return;
//    }
//    if (!eatCurrentToken(Tokens::ID).has_value()) {
//        reportError("Expected identifier after int");
//		return;
//    }
//    if (!eatCurrentToken(Tokens::assignop).has_value()) {
//        reportError("Expected = after identifier");
//        return;
//    }
//    if (!eatCurrentToken(Tokens::number).has_value()) {
//        reportError("Expected number after =");
//        return;
//    }
//    if (!eatCurrentToken(Tokens::semicolon).has_value()) {
//        reportError("cmon man");
//    }
//	Logger::parser("Generated int");
//	Logger::parser("Exiting parseIntExpression");
//
//}

//void Parser::parseExpression() {
//	Logger::scanner("Entering Expression");
//	if (!checkAndEatToken(Tokens::number)) reportError("Expected number");
//	else if (checkAndEatToken(Tokens::lparen)){
//		parseExpression();
//		if (!checkAndEatToken(Tokens::rparen)) Logger::scanner("expected )");
//	}
//	else if (checkAndEatToken(Tokens::ID)) Logger::scanner("Created expression");
//	else {
//		reportError("Expected something");
//		return;
//	}
//	Logger::parser("Generated expression");
//	Logger::parserExit("Expression");
//}