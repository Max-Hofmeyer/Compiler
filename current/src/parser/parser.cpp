//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
#include "parser.h"

//subscribed to the scanner. Logic inside buffers the token line by line.
void Parser::Update(const token& t) {
	//since we need to start parsing after a complete line
	if (!hasError) {
		//if (currentLine == 0) currentLine = t.lineLoc;

		////current token is on the next line, so start parsing
		//if (t.lineLoc != currentLine || t.type == Tokens::eof) {
		//	if (!_tokenBuffer.empty()) {
		//		parseLine();
		//		if (_tokenBuffer.size() != _index) Logger::warning("Token buffer is being cleared before line can be fully parsed");
		//		_tokenBuffer.clear();
		//		_index = 0;
		//	}
		//	//makes sure the current token is added 
		//	if (t.type != Tokens::eof) {
		//		_tokenBuffer.emplace_back(t);
		//		currentLine = t.lineLoc;
		//	}
		//	//edge case of an empty program
		//	else {
		//		_tokenBuffer.emplace_back(t);
		//		parseLine();
		//	}
		//}
		//else _tokenBuffer.emplace_back(t);
		_tokenBuffer.emplace_back(t);
		if(t.type == Tokens::eof) {
			parseLine();
		}
		//while(!_tokenBuffer.empty()) {
		//	const token& nextT = _tokenBuffer.front();

		//}

	}
}

void Parser::parseLine() {
	Logger::outputTokens(_tokenBuffer);
	while (peekSafe().type != Tokens::eof) parseToyCProgram();
}

void Parser::parsePrimary() {
	Logger::parserEnter("Primary");
	if (peekSafe().type == Tokens::ID) {
		parseIdentifier();
		if (peekSafe().type == Tokens::lparen) parseFunctionCall();
	}

	//number
	else if (checkAndEatToken(Tokens::number)) {
		Logger::parserCreate("number", previousToken().value);
	}

	//charliteral
	else if (checkAndEatToken(Tokens::charliteral)) {
		Logger::parserCreate("char", previousToken().value);
	}

	//string
	else if (checkAndEatToken(Tokens::string)) {
		Logger::parserCreate("string", previousToken().value);
	}

	//expression
	else if (checkAndEatToken(Tokens::lparen)) {
		parseExpression();
		if (!checkAndEatToken(Tokens::rparen)) reportError("expected )");
	}

	//primary (-)
	else if (checkAndEatToken(Tokens::addop)) {
		//might use peek() instead
		if (previousToken().value == "-") {
			Logger::parserEnter("subtraction primary");
			parsePrimary();
			Logger::parserCreate("subtraction primary", previousToken().value);
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
		Logger::parserCreate("Int", previousToken().value);
	}
	if (checkAndEatToken(Tokens::_char)) {
		Logger::parserCreate("Char", previousToken().value);
	}
	Logger::parserExit("Type");
}

void Parser::parseIdentifier() {
	Logger::parserEnter("Identifier");
	if (checkAndEatToken(Tokens::ID)) {
		Logger::parserCreate("Identifier", previousToken().value);
	}
	Logger::parserExit("Identifier");
}

void Parser::parseTerm() {
	Logger::parserEnter("Term");
	parsePrimary();
	while (checkAndEatToken(Tokens::mulop)) {
		token t = previousToken();
		parsePrimary();
		Logger::parserCreate("Term", t.value);
	}
	Logger::parserExit("Term");
}

void Parser::parseSimpleExpression() {
	Logger::parserEnter("SimpleExpression");
	parseTerm();
	while (checkAndEatToken(Tokens::addop)) {
		token t = previousToken();
		parseTerm();
		Logger::parserCreate("SimpleExpression", t.value);
	}
	Logger::parserExit("SimpleExpression");
}

void Parser::parseRelopExpression() {
	Logger::parserEnter("RelopExpression");
	parseSimpleExpression();
	while (checkAndEatToken(Tokens::relop)) {
		token t = previousToken();
		parseSimpleExpression();
		Logger::parserCreate("RelopExpression", t.value);
	}
	Logger::parserExit("RelopExpression");
}

void Parser::parseExpression() {
	Logger::parserEnter("Expression");
	parseRelopExpression();
	if (peekSafe().type == Tokens::assignop) {
		token t = peekSafe();
		checkAndEatToken(Tokens::assignop);
		parseRelopExpression();
		Logger::parserCreate("Expression", t.value);
	}
	Logger::parserExit("Expression");
}

void Parser::parseActualParameters() {
	Logger::parserEnter("ActualParameters");
	parseExpression();
	while (checkAndEatToken(Tokens::comma)) {
		parseExpression();
		Logger::parserCreate("ActualParameters");
	}
	Logger::parserExit("ActualParameters");
}

void Parser::parseFunctionCall() {
	Logger::parserEnter("FunctionCall");
	token t = previousToken();
	if (checkAndEatToken(Tokens::lparen)) {
		if (peekSafe(1).type != Tokens::rparen) {
			parseActualParameters();
			if (checkAndEatToken(Tokens::rparen)) Logger::parserCreate("FunctionCall", t.value);
			else reportError("Expected ) in FunctionCall");
		}
		else {
			if (checkAndEatToken(Tokens::rparen)) Logger::parserCreate("FunctionCall", t.value);
		}

	}
	//else reportError("Expected (");

	Logger::parserExit("FunctionCall");
}

void Parser::parseNewLineStatement() {
	Logger::parserEnter("NewLineStatement");
	if (checkAndEatToken(Tokens::_newline)) {
		if (checkAndEatToken(Tokens::semicolon)) {
			Logger::parserCreate("NewLineStatement", " ");
		}
		else reportError("Expected ; in newline");
	}
	else reportError("Expected newline");

	Logger::parserExit("NewLineStatement");
}

void Parser::parseWriteStatement() {
	Logger::parserEnter("WriteStatement");
	if (checkAndEatToken(Tokens::_write)) {
		if (checkAndEatToken(Tokens::lparen)) {
			parseActualParameters();
			if (checkAndEatToken(Tokens::rparen)) {
				if (checkAndEatToken(Tokens::semicolon)) Logger::parserCreate("WriteStatement", previousToken().value);
				else reportError("Expected ; in write");
			}
			else reportError("Expected )");
		}
		else reportError("Expected (");
	}
	else reportError("Expected write");

	Logger::parserExit("WriteStatement");
}

void Parser::parseReadStatement() {
	Logger::parserEnter("ReadStatement");
	if (checkAndEatToken(Tokens::_read)) {
		if (checkAndEatToken(Tokens::lparen)) {
			parseIdentifier();
			while (checkAndEatToken(Tokens::comma)) {
				parseIdentifier();
			}
			if (checkAndEatToken(Tokens::rparen)) {
				if (checkAndEatToken(Tokens::semicolon)) {
					Logger::parserCreate("ReadStatement", previousToken().value);
				}
				else reportError("Expected ; in read");
			}
			else reportError("Expected ) in read");
		}
		else reportError("Expected ( in read");
	}
	else reportError("Expected read");

	Logger::parserExit("ReadStatement");
}

void Parser::parseWhileStatement() {
	Logger::parserEnter("WhileStatement");
	if (checkAndEatToken(Tokens::_while)) {
		if (checkAndEatToken(Tokens::lparen)) {
			parseExpression();
			if (checkAndEatToken(Tokens::rparen)) {
				parseStatement();
				Logger::parserCreate("WhileStatement", previousToken().value);
			}
			else reportError("Expected ) in while");
		}
		else reportError("Expected ( in while");
	}
	else reportError("Expected while");

	Logger::parserExit("WhileStatement");
}

void Parser::parseReturnStatement() {
	Logger::parserEnter("ReturnStatement");
	if (checkAndEatToken(Tokens::_return)) {
		if (peekSafe().type != Tokens::semicolon) {
			token t = peekSafe();
			parseExpression();
			if (checkAndEatToken(Tokens::semicolon)) {
				Logger::parserCreate("ReturnStatement", t.value);
			}
			else reportError("Expected ; in return");
		}
		else Logger::parserCreate("ReturnStatement", previousToken().value);
	}
	else reportError("Expected return");

	Logger::parserExit("ReturnStatement");
}

void Parser::parseNullStatement() {
	Logger::parserEnter("NullStatement");
	if (checkAndEatToken(Tokens::semicolon)) {
		Logger::parserCreate("NullStatement", previousToken().value);
	}
	else reportError("Expected ; in nullstatement");

	Logger::parserExit("NullStatement");
}

void Parser::parseIfStatement() {
	Logger::parserEnter("IfStatement");
	if (checkAndEatToken(Tokens::_if)) {
		if (checkAndEatToken(Tokens::lparen)) {
			parseExpression();
			if (checkAndEatToken(Tokens::rparen)) {
				parseStatement();
				if (peekSafe().type == Tokens::_else) checkAndEatToken(Tokens::_else);
				Logger::parserCreate("IfStatement", previousToken().value);
				//else if(checkAndEatToken(Tokens::_else)) {
				//	parseStatement();
				//	Logger::parserCreate("IfStatement", previousToken().value);
				//}
			}
			else reportError("Expected ) in if");
		}
		else reportError("Expected ( in if");
	}
	else reportError("Expected if");
}

void Parser::parseBreakStatement() {
	Logger::parserEnter("BreakStatement");
	if (checkAndEatToken(Tokens::_break)) {
		if (checkAndEatToken(Tokens::semicolon)) {
			Logger::parserCreate("BreakStatement", previousToken().value);
		}
		else reportError("Expected ; in break");
	}
	else reportError("Expected break");

	Logger::parserExit("BreakStatement");
}

void Parser::parseExpressionStatement() {
	Logger::parserEnter("ExpressionStatement");
	parseExpression();
	if (checkAndEatToken(Tokens::semicolon)) {
		Logger::parserCreate("ExpressionStatement", previousToken().value);
	}
	else reportError("Expected ; in expression");

	Logger::parserExit("ExpressionStatement");
}

void Parser::parseCompoundStatement() {
	Logger::parserEnter("CompoundStatement");
	if (checkAndEatToken(Tokens::lcurly)) {
		while(isTypeLit(peekSafe())) {
			parseType();
			token t1 = previousToken();
			parseIdentifier();
			token t2 = previousToken();

			if (checkAndEatToken(Tokens::semicolon)) Logger::parserCreate("CompoundStatement", t1.value + " " + t2.value);
			else reportError("Expected ; in compound");
		}
		while (!checkAndEatToken(Tokens::rcurly)) {
			if (peekSafe().type == Tokens::ERROR) return;
			if (!hasError) parseStatement();
		}
	}
	else reportError("Expected { in compound");
	Logger::parserExit("CompoundStatement");
}

void Parser::parseStatement() {
	Logger::parserEnter("Statement");
	token nextT = peekSafe();
	if (nextT.type == Tokens::_break) parseBreakStatement();
	else if (nextT.type == Tokens::_if) parseIfStatement();
	else if (nextT.type == Tokens::semicolon) parseNullStatement();
	else if (nextT.type == Tokens::_return) parseReturnStatement();
	else if (nextT.type == Tokens::_while) parseWhileStatement();
	else if (nextT.type == Tokens::_read) parseReadStatement();
	else if (nextT.type == Tokens::_newline) parseNewLineStatement();
	else if (nextT.type == Tokens::_write) parseWriteStatement();
	else if (nextT.type == Tokens::lcurly) parseCompoundStatement();
	else if (isStartingExpression(nextT)) parseExpressionStatement();
	else if (nextT.type == Tokens::ERROR || hasError) return;
	else reportError("Statement unrecongnized");
	Logger::parserExit("Statement");
}

void Parser::parseToyCProgram() {
	if (!parsingStarted) {
		Logger::parserEnter("ToyCProgram");
		Logger::parserCreate("Program", "ToyCProgram");
		parsingStarted = true;
	}
	if (peekSafe().type != Tokens::eof && !hasError) parseDefinition();
	if (peekSafe().type == Tokens::eof) {
		Logger::parserExit("ToyCProgram");
		_tokenBuffer.clear();
		_index = 0;
	}
}

void Parser::parseDefinition() {
	Logger::parserEnter("Definition");
	if (isTypeLit(peekSafe())) {
		parseType();
		if (peekSafe().type == Tokens::ID) {
			parseIdentifier();
			if (peekSafe().type == Tokens::lparen) parseFunctionDefinition();
			else if (peekSafe().type == Tokens::semicolon) checkAndEatToken(Tokens::semicolon);
			else reportError("Expected a ; in definition");

		}
		else reportError("Expected identifier after type");
	}
	Logger::parserExit("Definition");
}

void Parser::parseFunctionDefinition() {
	Logger::parserEnter("FunctionDefinition");
	parseFunctionHeader();
	parseFunctionBody();
	Logger::parserExit("FunctionDefinition");

}

void Parser::parseFunctionHeader() {
	Logger::parserEnter("FunctionHeader");
	token t = previousToken();
	if (checkAndEatToken(Tokens::lparen)) {
		if (peekSafe().type != Tokens::rparen) {
			checkAndEatToken(Tokens::rparen);
			parseFormalParamList();
			if (checkAndEatToken(Tokens::rparen)) Logger::parserCreate("FunctionHeader", t.value);
			else reportError("Expected ) in function header");
		}
		if (checkAndEatToken(Tokens::rparen)) Logger::parserCreate("FunctionHeader", t.value);
	}
	else reportError("Expected ( in function header");

	Logger::parserExit("FunctionHeader");
}

void Parser::parseFunctionBody() {
	Logger::parserEnter("FunctionBody");
	parseCompoundStatement();
	Logger::parserExit("FunctionBody");
}

void Parser::parseFormalParamList() {
	Logger::parserEnter("FormalParamList");
	if (isTypeLit(peekSafe())) {
		parseType();
		if (peekSafe().type == Tokens::ID) parseIdentifier();
		else reportError("Expected identifier in formalparamlist");

		while(peekSafe().type == Tokens::comma) {
			if (isTypeLit(peekSafe())) {
				parseType();
				if (peekSafe().type == Tokens::ID) parseIdentifier();
				else reportError("Expected identifier after type in formalparamlist");
			}
		}
	}
	Logger::parserExit("FormalParamList");
}

bool Parser::isTypeLit(const token& t) {
	if (t.type == Tokens::_int || t.type == Tokens::_char) return true;
	return false;
}

//helper to determine if its worth trying to parse a statement
bool Parser::isStartingExpression(const token& t) {
	if (t.type == Tokens::number || t.type == Tokens::ID || t.type == Tokens::charliteral
		|| t.type == Tokens::string || t.type == Tokens::lparen || t.type == Tokens::_not ) return true;
	if (t.type == Tokens::addop && t.value == "-") return true;
	return false;
}

//looks ahead in the buffer, can unpack null values
std::optional<token> Parser::peek(int offset) const {
	if (_index + offset >= _tokenBuffer.size()) return {};
	return _tokenBuffer.at(_index + offset);
}

//makes it impossible to unpack a null value
token Parser::peekSafe(const int offset) const {
	if (peek(offset).has_value()) return peek(offset).value();
	//Logger::error("peekSafe Unpacked Null");
	return token {Tokens::ERROR, -1, "ERROR TOKEN", "ERROR VALUE", false};
}

//returns the current token and increments the index 
token Parser::eat() {
	std::cout << "\t[Ate] " << _index << "\n";
	token t = _tokenBuffer.at(_index++);
	Logger::outputToken(t);
	return t;
}

//returns null if the current token in the buffer doesnt match 
std::optional<token> Parser::eatCurrentToken(Tokens type) {
	if (_index < _tokenBuffer.size() && _tokenBuffer.at(_index).type == type) return eat();
	return {};
}

//used for convenience, didnt want to do "if eatCurrentToken().has_value()" everywhere..
bool Parser::checkAndEatToken(Tokens type) {
	if (eatCurrentToken(type).has_value()) return true;
	return false;
}

//returns the previous token in the index, is safe 
token Parser::previousToken(int offset) {
	if (_index > 0 && !_tokenBuffer.empty()) return _tokenBuffer.at(_index - offset);
	return _tokenBuffer.at(0);
}

//for debugging, and maybe error messages
void Parser::outputTokenLine() const {Logger::outputTokens(_tokenBuffer); }

void Parser::reportError(const std::string& message) {
	hasError = true;
	Logger::outputTokens(_tokenBuffer);
	Logger::error(message);
}

