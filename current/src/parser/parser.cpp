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
		if (t.type == Tokens::eof) {
			parseLine();
		}
		//while(!_tokenBuffer.empty()) {
		//	const token& nextT = _tokenBuffer.front();

		//}
	}
}

void Parser::parseLine() {
	Logger::outputTokens(_tokenBuffer);
	while (peekSafe().type != Tokens::eof && !_tokenBuffer.empty()) parseToyCProgram();
}

std::unique_ptr<NodeToyCProgram> Parser::parseToyCProgram() {
	if (!parsingStarted) {
		Logger::parserEnter("ToyCProgram");
		parsingStarted = true;
	}
	auto program = std::make_unique<NodeToyCProgram>();
	if (peekSafe().type != Tokens::eof && !hasError) {
		auto d = parseDefinition();
		if (d) program->addRHS(std::move(d));
	}
	if (peekSafe().type == Tokens::eof) {
		Logger::parserExit("ToyCProgram");
		_tokenBuffer.clear();
		_index = 0;
	}
	return program;
}

std::unique_ptr<NodeDefinition> Parser::parseDefinition() {
	if (hasError) return nullptr;
	std::unique_ptr<NodeDeclaration> declaration = nullptr;
	std::unique_ptr<NodeFunctionDefinition> function = nullptr;

	Logger::parserEnter("Definition");
	if (isTypeLit(peekSafe())) {
		token type = parseType();
		if (peekSafe().type == Tokens::ID) {
			token id = parseIdentifier();
			declaration = std::make_unique<NodeDeclaration>(type, id);
			if (peekSafe().type == Tokens::lparen) function = parseFunctionDefinition();
			else if (peekSafe().type != Tokens::semicolon) reportError("Expected a ; in definition");
			else checkAndEatToken(Tokens::semicolon);
		}
		else reportError("Expected identifier after type");
	}
	Logger::parserExit("Definition");
	if (function) return std::make_unique<NodeDefinition>(std::move(declaration), std::move(function));
	if (declaration) return std::make_unique<NodeDefinition>(std::move(declaration));

	reportError("Returning null in parsedefinition");
	return nullptr;
}

token Parser::parseType() {
	if (hasError) return returnBadToken();
	Logger::parserEnter("Type");
	token t = peekSafe();
	if (t.type == Tokens::_int || t.type == Tokens::_char) {
		checkAndEatToken(t.type);
		Logger::parserExit("Type");
		return t;
	}
	reportError("type not found in parsetype");
	return returnBadToken();

}

std::unique_ptr<NodeFunctionDefinition> Parser::parseFunctionDefinition() {
	if (hasError) return nullptr;
	Logger::parserEnter("FunctionDefinition");
	std::unique_ptr<NodeFunctionHeader> header = parseFunctionHeader();
	std::unique_ptr<NodeFunctionBody> body = parseFunctionBody();
	Logger::parserExit("FunctionDefinition");

	return std::make_unique<NodeFunctionDefinition>(std::move(header), std::move(body));
}

std::unique_ptr<NodeFunctionHeader> Parser::parseFunctionHeader() {
	if (hasError) return nullptr;
	std::unique_ptr<NodeFormalParamList> param = nullptr;

	Logger::parserEnter("FunctionHeader");
	if (checkAndEatToken(Tokens::lparen)) {
		if (peekSafe().type != Tokens::rparen) {
			checkAndEatToken(Tokens::rparen);
			param = parseFormalParamList();
			if (checkAndEatToken(Tokens::rparen)) return std::make_unique<NodeFunctionHeader>(std::move(param));
			reportError("Expected ) in function header");
		}
		if (checkAndEatToken(Tokens::rparen)) return std::make_unique<NodeFunctionHeader>(std::move(param));
	}
	else reportError("Expected ( in function header");
	Logger::parserExit("FunctionHeader");
	return nullptr;
}

std::unique_ptr<NodeFunctionBody> Parser::parseFunctionBody() {
	if (hasError) return nullptr;
	Logger::parserEnter("FunctionBody");
	auto stmt = parseCompoundStatement();
	Logger::parserExit("FunctionBody");
	if (stmt) return std::make_unique<NodeFunctionBody>(std::move(stmt));
	return nullptr;
}

std::unique_ptr<NodeFormalParamList> Parser::parseFormalParamList() {
	if (hasError) return nullptr;
	std::unique_ptr<NodeFormalParamList> formal = nullptr;

	Logger::parserEnter("FormalParamList");
	if (isTypeLit(peekSafe())) {
		token type = parseType();
		if (peekSafe().type == Tokens::ID) {
			token id = parseIdentifier();
			auto declaration = std::make_unique<NodeDeclaration>(type, id);
			formal = std::make_unique<NodeFormalParamList>(std::move(declaration));
		}
		else reportError("Expected identifier in formalparamlist");

		while (peekSafe().type == Tokens::comma) {
			if (isTypeLit(peekSafe())) {
				token nType = parseType();
				if (peekSafe().type == Tokens::ID) {
					token nId = parseIdentifier();
					auto nDeclaration = std::make_unique<NodeDeclaration>(nType, nId);
					formal->addRHS(std::move(nDeclaration));
				}
				else reportError("Expected identifier after type in formalparamlist");
			}
		}
	}
	Logger::parserExit("FormalParamList");
	return formal;
}

std::unique_ptr<NodeStatement> Parser::parseStatement() {
	if (hasError) return nullptr;
	std::unique_ptr<NodeStatement> stmt = nullptr;
	Logger::parserEnter("Statement");
	token nextT = peekSafe();

	if (nextT.type == Tokens::_break) stmt = std::make_unique<NodeStatement>(parseBreakStatement());
	else if (nextT.type == Tokens::_if) stmt = std::make_unique<NodeStatement>(parseIfStatement());
	else if (nextT.type == Tokens::semicolon) stmt = std::make_unique<NodeStatement>(parseNullStatement());
	else if (nextT.type == Tokens::_return) stmt = std::make_unique<NodeStatement>(parseReturnStatement());
	else if (nextT.type == Tokens::_while) stmt = std::make_unique<NodeStatement>(parseWhileStatement());
	else if (nextT.type == Tokens::_read) stmt = std::make_unique<NodeStatement>(parseReadStatement());
	else if (nextT.type == Tokens::_newline) stmt = std::make_unique<NodeStatement>(parseNewLineStatement());
	else if (nextT.type == Tokens::_write) stmt = std::make_unique<NodeStatement>(parseWriteStatement());
	else if (nextT.type == Tokens::lcurly) stmt = std::make_unique<NodeStatement>(parseCompoundStatement());
	else if (isStartingExpression(nextT)) stmt = std::make_unique<NodeStatement>(parseExpressionStatement());
	else reportError("Statement unrecongnized");
	Logger::parserExit("Statement");
	return stmt;
}

std::unique_ptr<NodeExpressionStatement> Parser::parseExpressionStatement() {
	if (hasError) return nullptr;
	Logger::parserEnter("ExpressionStatement");
	auto expr = parseExpression();
	if (checkAndEatToken(Tokens::semicolon)) {
		return std::make_unique<NodeExpressionStatement>(std::move(expr));
	}
	reportError("Expected ; in expression");
	Logger::parserExit("ExpressionStatement");
	return nullptr;
}

std::unique_ptr<NodeBreakStatement> Parser::parseBreakStatement() {
	if (hasError) return nullptr;
	Logger::parserEnter("BreakStatement");
	if (checkAndEatToken(Tokens::_break)) {
		if (checkAndEatToken(Tokens::semicolon)) return std::make_unique<NodeBreakStatement>();
		reportError("Expected ; in break");
	}
	reportError("Expected break");
	Logger::parserExit("BreakStatement");
	return nullptr;
}

std::unique_ptr<NodeCompoundStatement> Parser::parseCompoundStatement() {
	if (hasError) return nullptr;
	auto compound = std::make_unique<NodeCompoundStatement>();

	Logger::parserEnter("CompoundStatement");
	if (checkAndEatToken(Tokens::lcurly)) {
		while (isTypeLit(peekSafe())) {
			token type = parseType(); 
			token id = parseIdentifier();
			auto d = std::make_unique<NodeDeclaration>(type, id);
			if (checkAndEatToken(Tokens::semicolon)) {
				if (d) compound->addDeclaration(std::move(d));
			}
			else reportError("Expected ; in compound");
		}
		while (!checkAndEatToken(Tokens::rcurly)) {
			auto s = parseStatement();
			if (s) compound->addStatement(std::move(s));
			
		}
	}
	else reportError("Expected { in compound");
	Logger::parserExit("CompoundStatement");
	return compound;
}

std::unique_ptr<NodeIfStatement> Parser::parseIfStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("IfStatement");
	if (checkAndEatToken(Tokens::_if)) {
		if (checkAndEatToken(Tokens::lparen)) {
			auto expr = parseExpression();
			if (checkAndEatToken(Tokens::rparen)) {
				auto stmt = parseStatement();
				if (checkAndEatToken(Tokens::_else)) {
					auto eStmt = parseStatement();
					return std::make_unique<NodeIfStatement>(std::move(expr), std::move(stmt), std::move(eStmt));
				}
				return std::make_unique<NodeIfStatement>(std::move(expr), std::move(stmt));
			}
			reportError("Expected ) in if");
		}
		else reportError("Expected ( in if");
	}
	else reportError("Expected if");

	return nullptr;
}

std::unique_ptr<NodeNullStatement> Parser::parseNullStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("NullStatement");
	if (checkAndEatToken(Tokens::semicolon)) return std::make_unique<NodeNullStatement>();
	reportError("Expected ; in nullstatement");
	Logger::parserExit("NullStatement");

	return nullptr;
}

std::unique_ptr<NodeReturnStatement> Parser::parseReturnStatement() {
	if (hasError) return nullptr;
	Logger::parserEnter("ReturnStatement");
	if (peekSafe().type != Tokens::_return) reportError("Expected return keyword");
	checkAndEatToken(Tokens::_return);

	if (peekSafe().type != Tokens::semicolon) {
		auto expr = parseExpression();
		if (checkAndEatToken(Tokens::semicolon)) return std::make_unique<NodeReturnStatement>(std::move(expr));
		else reportError("Expected ; in return");

	}
	else {
		checkAndEatToken(Tokens::semicolon);
		return std::make_unique<NodeReturnStatement>();
	}
	Logger::parserExit("ReturnStatement");
	return nullptr;
}

std::unique_ptr<NodeWhileStatement> Parser::parseWhileStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("WhileStatement");
	if (checkAndEatToken(Tokens::_while)) {
		if (checkAndEatToken(Tokens::lparen)) {
			auto expr = parseExpression();
			if (checkAndEatToken(Tokens::rparen)) {
				auto stmt = parseStatement();
				return std::make_unique<NodeWhileStatement>(std::move(expr), std::move(stmt));
			}
			reportError("Expected ) in while");
		}
		else reportError("Expected ( in while");
	}
	else reportError("Expected while");

	Logger::parserExit("WhileStatement");
	return nullptr;
}

std::unique_ptr<NodeReadStatement> Parser::parseReadStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("ReadStatement");
	if (checkAndEatToken(Tokens::_read)) {
		if (checkAndEatToken(Tokens::lparen)) {
			token id = parseIdentifier();
			auto read = std::make_unique<NodeReadStatement>(std::move(id));
			while (checkAndEatToken(Tokens::comma)) read->addRHS(parseIdentifier());
			if (checkAndEatToken(Tokens::rparen)) {
				if (checkAndEatToken(Tokens::semicolon)) {
					return read;
				}
				reportError("Expected ; in read");
			}
			else reportError("Expected ) in read");
		}
		else reportError("Expected ( in read");
	}
	else reportError("Expected read");

	Logger::parserExit("ReadStatement");
	return nullptr;
}

std::unique_ptr<NodeWriteStatement> Parser::parseWriteStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("WriteStatement");
	if (checkAndEatToken(Tokens::_write)) {
		if (checkAndEatToken(Tokens::lparen)) {
			auto param = parseActualParameters();
			if (checkAndEatToken(Tokens::rparen)) {
				if (checkAndEatToken(Tokens::semicolon)) return std::make_unique<NodeWriteStatement>(std::move(param));
				reportError("Expected ; in write");
			}
			else reportError("Expected )");
		}
		else reportError("Expected (");
	}
	else reportError("Expected write");

	Logger::parserExit("WriteStatement");
	return nullptr;
}

std::unique_ptr<NodeNewLineStatement> Parser::parseNewLineStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("NewLineStatement");
	if (peekSafe().type != Tokens::_newline) reportError("Expected newline keyword");
	eatCurrentToken(Tokens::_newline);

	if (peekSafe().type != Tokens::semicolon) reportError("Expected ; after newline");
	eatCurrentToken(Tokens::semicolon);

	return std::make_unique<NodeNewLineStatement>();
	Logger::parserExit("NewLineStatement");
}

std::unique_ptr<NodeExpression> Parser::parseExpression() {
	if (hasError) return nullptr;
	Logger::parserEnter("Expression");
	auto op = parseRelopExpression();
	auto expr = std::make_unique<NodeExpression>(std::move(op));

	while(peekSafe().type == Tokens::assignop) {
		token t = peekSafe();
		eatCurrentToken(Tokens::assignop);
		auto nOp = parseRelopExpression();
		expr->addRHS(t,std::move(nOp));
	}
	Logger::parserExit("Expression");
	return expr;
}

std::unique_ptr<NodeRelopExpression> Parser::parseRelopExpression() {
	if (hasError) return nullptr;
	Logger::parserEnter("RelopExpression");\

	std::cout << "relop index:  " << _index << "\n";
	auto op = parseSimpleExpression();
	auto expr = std::make_unique<NodeRelopExpression>(std::move(op));

	while (peekSafe().type == Tokens::relop) {
		token t = peekSafe();
		eatCurrentToken(Tokens::relop);
		auto nOp = parseSimpleExpression();
		expr->addRHS(t, std::move(nOp));
	}
	Logger::parserExit("RelopExpression");
	return expr;
}

std::unique_ptr<NodeSimpleExpression> Parser::parseSimpleExpression() {
	if (hasError) return nullptr;
	Logger::parserEnter("SimpleExpression");
	std::cout << "simple index:  " << _index << "\n";

	auto op = parseTerm();
	auto expr = std::make_unique<NodeSimpleExpression>(std::move(op));
	while (peekSafe().type == Tokens::addop) {
		token t = peekSafe();
		eatCurrentToken(Tokens::addop);
		auto nOp = parseTerm();
		expr->addRHS(t, std::move(nOp));
	}
	Logger::parserExit("SimpleExpression");
	return expr;
}

std::unique_ptr<NodeTerm> Parser::parseTerm() {
	if (hasError) return nullptr;
	Logger::parserEnter("Term");

	auto op = parsePrimary();
	auto expr = std::make_unique<NodeTerm>(std::move(op));
	while (peekSafe().type == Tokens::mulop) {
		token t = peekSafe();
		eatCurrentToken(Tokens::mulop);
		auto nOp = parsePrimary();
		expr->addRHS(t, std::move(nOp));
	}
	Logger::parserExit("Term");
	return expr;
}

std::unique_ptr<NodePrimary> Parser::parsePrimary() {
	if (hasError) return nullptr;
	Logger::parserEnter("Primary");

	if (peekSafe().type == Tokens::ID) {
		token id = parseIdentifier();
		if (peekSafe().type == Tokens::lparen) {
			auto func = parseFunctionCall();
			return std::make_unique<NodePrimary>(id, std::move(func));
		}
		return std::make_unique<NodePrimary>(id);
	}
	//number
	token t = peekSafe();
	if (checkAndEatToken(Tokens::number)) return std::make_unique<NodePrimary>(t);

	//charliteral
	if (checkAndEatToken(Tokens::charliteral)) return std::make_unique<NodePrimary>(t);

	//string
	if (checkAndEatToken(Tokens::string)) return std::make_unique<NodePrimary>(t);

	//expression
	if (checkAndEatToken(Tokens::lparen)) {
		auto expr = parseExpression();
		if (checkAndEatToken(Tokens::rparen)) return std::make_unique<NodePrimary>(std::move(expr));
		reportError("Expected ) in primary");
	}

	//primary (-)
	if (t.type == Tokens::addop && t.value == "-") {
		eatCurrentToken(Tokens::addop);
		auto p = parsePrimary();
		return std::make_unique<NodePrimary>(std::move(t), std::move(p));
	}

	//primary (not)
	if (checkAndEatToken(Tokens::_not)) {
		auto p = parsePrimary();
		return std::make_unique<NodePrimary>(std::move(t), std::move(p));
	}

	reportError("Unexpected primary");
	Logger::parserExit("Primary");
	return nullptr;
}

std::unique_ptr<NodeFunctionCall> Parser::parseFunctionCall() {
	if (hasError) return nullptr;
	Logger::parserEnter("FunctionCall");

	if (peekSafe().type != Tokens::lparen) reportError("Expected ( in functioncall");
	checkAndEatToken(Tokens::lparen);

	if (peekSafe().type != Tokens::rparen) {
		auto param = parseActualParameters();
		if (checkAndEatToken(Tokens::rparen)) return std::make_unique<NodeFunctionCall>(std::move(param));
		reportError("Expected ) in functioncall");
	}
	else return std::make_unique<NodeFunctionCall>();
	Logger::parserExit("FunctionCall");
	return nullptr;
}

std::unique_ptr<NodeActualParameters> Parser::parseActualParameters() {
	if (hasError) return nullptr;
	Logger::parserEnter("ActualParameters");
	auto op = parseExpression();
	auto expr = std::make_unique<NodeActualParameters>(std::move(op));

	while (checkAndEatToken(Tokens::comma)) {
		auto nOp = parseExpression();
		expr->addRHS(std::move(nOp));
	}
	Logger::parserExit("ActualParameters");
	return expr;
}

token Parser::parseIdentifier() {
	Logger::parserEnter("Identifier");
	token t = peekSafe();
	if (t.type == Tokens::ID) {
		eatCurrentToken(Tokens::ID);
		Logger::parserExit("Identifier");
		return t;
	}
	reportError("Expected Identifier inside identifier");
	return returnBadToken();

}

bool Parser::isTypeLit(const token& t) {
	if (t.type == Tokens::_int || t.type == Tokens::_char) return true;
	return false;
}

//helper to determine if its worth trying to parse a statement
bool Parser::isStartingExpression(const token& t) {
	if (t.type == Tokens::number || t.type == Tokens::ID || t.type == Tokens::charliteral
		|| t.type == Tokens::string || t.type == Tokens::lparen || t.type == Tokens::_not)
		return true;
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
	return returnBadToken();
}

//returns the current token and increments the index 
token Parser::eat() {
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
//worst mistake of all time
bool Parser::checkAndEatToken(Tokens type) {
	if (eatCurrentToken(type).has_value()) return true;
	return false;
}

//returns the previous token in the index, is safe 
token Parser::previousToken(int offset) {
	if (_index > 0 && !_tokenBuffer.empty()) return _tokenBuffer.at(_index - offset);
	return _tokenBuffer.at(0);
}

token Parser::returnBadToken() const {
	return token{ Tokens::ERROR, -1, "ERROR TOKEN", "ERROR VALUE", false };
}

//for debugging, and maybe error messages
void Parser::outputTokenLine() const {
	//Logger::outputTokens(_tokenBuffer);
}

void Parser::reportError(const std::string& message) {
	hasError = true;
	//Logger::outputTokens(_tokenBuffer);
	Logger::error(message);
}
