//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
#include "parser.h"

void Parser::begin() {
	_scanner.hasError = false;
	addTokenToBuffer();
	auto program = std::make_unique<NodeToyCProgram>();
	if (peekSafe().type != Tokens::eof && !_tokenBuffer.empty() && !hasError) {
		auto prog = parseToyCProgram();
		if (!hasError && (dumpAST || CliConfig::verboseEnabled)) {
			prog->print(std::cout);
		}
	}
}

std::unique_ptr<NodeToyCProgram> Parser::parseToyCProgram() {
	if (!parsingStarted) {
		Logger::parserEnter("ToyCProgram");
		parsingStarted = true;
	}
	auto program = std::make_unique<NodeToyCProgram>();
	while (peekSafe().type != Tokens::eof && !hasError) {
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
		if (type.type == Tokens::ERROR) reportError("'type' token");
		if (peekSafe().type == Tokens::ID) {
			token id = parseIdentifier();
			declaration = std::make_unique<NodeDeclaration>(type, id);
			if (declaration) _table.insertSymbol(id.value, type);
			if (peekSafe().type == Tokens::lparen) function = parseFunctionDefinition();
			else if (peekSafe().type != Tokens::semicolon) {
				reportError("';'");
			}
			else checkAndEatToken(Tokens::semicolon);
		}
		//else reportError("'identifier' token");
	}
	Logger::parserExit("Definition");
	if (function) return std::make_unique<NodeDefinition>(std::move(declaration), std::move(function));
	if (declaration) return std::make_unique<NodeDefinition>(std::move(declaration));
	reportError("'Definition'");
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
	reportError("'type' token");
	return returnBadToken();
}

std::unique_ptr<NodeFunctionDefinition> Parser::parseFunctionDefinition() {
	if (hasError) return nullptr;

	Logger::parserEnter("FunctionDefinition");
	std::unique_ptr<NodeFunctionHeader> header = parseFunctionHeader();
	_table.enterScope();
	std::unique_ptr<NodeFunctionBody> body = parseFunctionBody();
	_table.exitScope();
	Logger::parserExit("FunctionDefinition");

	if (header && body) return std::make_unique<NodeFunctionDefinition>(std::move(header), std::move(body));
	return nullptr;
}

std::unique_ptr<NodeFunctionHeader> Parser::parseFunctionHeader() {
	if (hasError) return nullptr;
	std::unique_ptr<NodeFormalParamList> param = nullptr;

	Logger::parserEnter("FunctionHeader");
	if (checkAndEatToken(Tokens::lparen)) {
		if (peekSafe().type != Tokens::rparen) {
			checkAndEatToken(Tokens::rparen);
			param = parseFormalParamList();
			if (checkAndEatToken(Tokens::rparen) && param) {
				Logger::parserExit("FunctionHeader");
				return std::make_unique<NodeFunctionHeader>(std::move(param));
			}
			reportError("')'");
		}
		if (checkAndEatToken(Tokens::rparen)) {
			Logger::parserExit("FunctionHeader");
			return std::make_unique<NodeFunctionHeader>(std::move(param));
		}
		reportError("')'");
	}
	else reportError("'('");
	Logger::parserExit("FunctionHeader");
	return nullptr;
}

std::unique_ptr<NodeFunctionBody> Parser::parseFunctionBody() {
	if (hasError) return nullptr;

	Logger::parserEnter("FunctionBody");
	auto stmt = parseCompoundStatement();
	Logger::parserExit("FunctionBody");

	return std::make_unique<NodeFunctionBody>(std::move(stmt));
}

std::unique_ptr<NodeFormalParamList> Parser::parseFormalParamList() {
	if (hasError) return nullptr;
	std::unique_ptr<NodeFormalParamList> formal = nullptr;

	Logger::parserEnter("FormalParamList");
	if (isTypeLit(peekSafe())) {
		token type = parseType();
		if (peekSafe().type == Tokens::ID) {
			token id = parseIdentifier();
			if (auto declaration = std::make_unique<NodeDeclaration>(type, id)) {
				_table.insertSymbol(id.value, type);
				formal = std::make_unique<NodeFormalParamList>(std::move(declaration));
			}
		}
		else reportError("'identifier' token");

		while (checkAndEatToken(Tokens::comma) && !hasError) {
			if (isTypeLit(peekSafe())) {
				token nType = parseType();
				if (peekSafe().type == Tokens::ID) {
					token nId = parseIdentifier();
					if(auto nDeclaration = std::make_unique<NodeDeclaration>(nType, nId)) {
						_table.insertSymbol(nId.value, nType);
						formal->addRHS(std::move(nDeclaration));
					}
				}
				else reportError("'identifier' token");
			}
		}
	}
	else reportError("'type' token");
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
	Logger::parserExit("Statement");
	return stmt;
}

std::unique_ptr<NodeExpressionStatement> Parser::parseExpressionStatement() {
	if (hasError) return nullptr;
	Logger::parserEnter("ExpressionStatement");
	auto expr = parseExpression();
	if (checkAndEatToken(Tokens::semicolon)) {
		Logger::parserExit("ExpressionStatement");
		return std::make_unique<NodeExpressionStatement>(std::move(expr));
	}
	reportError("';'");
	return nullptr;
}

std::unique_ptr<NodeBreakStatement> Parser::parseBreakStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("BreakStatement");
	if (checkAndEatToken(Tokens::_break)) {
		if (checkAndEatToken(Tokens::semicolon)) {
			Logger::parserExit("BreakStatement");
			return std::make_unique<NodeBreakStatement>();
		}
		reportError("';'");
	}
	reportError("'break'");

	return nullptr;
}

std::unique_ptr<NodeCompoundStatement> Parser::parseCompoundStatement() {
	if (hasError) return nullptr;
	_table.enterScope();
	auto compound = std::make_unique<NodeCompoundStatement>();

	Logger::parserEnter("CompoundStatement");
	if (checkAndEatToken(Tokens::lcurly)) {
		while (isTypeLit(peekSafe()) && !hasError) {
			token type = parseType();
			token id = parseIdentifier();
			if (peekSafe().type != Tokens::semicolon) reportError("';'");
			auto d = std::make_unique<NodeDeclaration>(type, id);
			if (d) _table.insertSymbol(id.value, type);

			if (checkAndEatToken(Tokens::semicolon)) {
				if (d) compound->addDeclaration(std::move(d));
			}
		}
		if (peekSafe().type == Tokens::eof) {
			reportError("'}'");
		}
		while (peekSafe().type != Tokens::rcurly && !hasError) {
			auto s = parseStatement();
			if (s == nullptr) break;
			compound->addStatement(std::move(s));
			if (peekSafe().type == Tokens::eof) {
				reportError("'}'");
			}
		}
		if (checkAndEatToken(Tokens::rcurly));
		else if (!compound->rhs.empty()) reportError("'}'");
	}
	else reportError("'{'");
	Logger::parserExit("CompoundStatement");
	_table.exitScope();
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
					Logger::parserExit("IfStatement");
					return std::make_unique<NodeIfStatement>(std::move(expr), std::move(stmt), std::move(eStmt));
				}
				Logger::parserExit("IfStatement");
				return std::make_unique<NodeIfStatement>(std::move(expr), std::move(stmt));
			}
			reportError("')'");
		}
		else reportError("'('");
	}
	else reportError("'if'");
	return nullptr;
}

std::unique_ptr<NodeNullStatement> Parser::parseNullStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("NullStatement");
	if (checkAndEatToken(Tokens::semicolon)) {
		Logger::parserExit("NullStatement");
		return std::make_unique<NodeNullStatement>();
	}
	reportError("';'");
	return nullptr;
}

std::unique_ptr<NodeReturnStatement> Parser::parseReturnStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("ReturnStatement");
	if (peekSafe().type != Tokens::_return) reportError("'return'");
	checkAndEatToken(Tokens::_return);
	if (peekSafe().type != Tokens::semicolon) {
		auto expr = parseExpression();
		if (checkAndEatToken(Tokens::semicolon)) {
			Logger::parserExit("ReturnStatement");
			return std::make_unique<NodeReturnStatement>(std::move(expr));
		}
		reportError("';'");
	}
	else {
		checkAndEatToken(Tokens::semicolon);
		Logger::parserExit("ReturnStatement");
		return std::make_unique<NodeReturnStatement>();
	}

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
				Logger::parserExit("WhileStatement");
				return std::make_unique<NodeWhileStatement>(std::move(expr), std::move(stmt));
			}
			reportError("')'");
		}
		else reportError("'('");
	}
	else reportError("'while'");
	return nullptr;
}

std::unique_ptr<NodeReadStatement> Parser::parseReadStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("ReadStatement");
	if (checkAndEatToken(Tokens::_read)) {
		if (checkAndEatToken(Tokens::lparen)) {
			token id = parseIdentifier();
			auto read = std::make_unique<NodeReadStatement>(std::move(id));
			while (checkAndEatToken(Tokens::comma) && !hasError) read->addRHS(parseIdentifier());
			if (checkAndEatToken(Tokens::rparen)) {
				if (checkAndEatToken(Tokens::semicolon)) {
					Logger::parserExit("ReadStatement");
					return read;
				}
				reportError("';'");
			}
			else reportError("')'");
		}
		else reportError("'('");
	}
	else reportError("'read'");
	return nullptr;
}

std::unique_ptr<NodeWriteStatement> Parser::parseWriteStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("WriteStatement");
	if (checkAndEatToken(Tokens::_write)) {
		if (checkAndEatToken(Tokens::lparen)) {
			auto param = parseActualParameters();
			if (checkAndEatToken(Tokens::rparen)) {
				if (checkAndEatToken(Tokens::semicolon)) {
					Logger::parserExit("WriteStatement");
					return std::make_unique<NodeWriteStatement>(std::move(param));
				}
				reportError("';'");
			}
			else reportError("')'");
		}
		else reportError("'('");
	}
	else reportError("'write'");

	return nullptr;
}

std::unique_ptr<NodeNewLineStatement> Parser::parseNewLineStatement() {
	if (hasError) return nullptr;

	Logger::parserEnter("NewLineStatement");
	if (peekSafe().type != Tokens::_newline) reportError("'newline'");
	eatCurrentToken(Tokens::_newline);

	if (peekSafe().type != Tokens::semicolon) reportError("';'");
	eatCurrentToken(Tokens::semicolon);

	Logger::parserExit("NewLineStatement");
	return std::make_unique<NodeNewLineStatement>();
}

std::unique_ptr<NodeExpression> Parser::parseExpression() {
	if (hasError) return nullptr;
	Logger::parserEnter("Expression");
	auto op = parseRelopExpression();
	auto expr = std::make_unique<NodeExpression>(std::move(op));

	while (peekSafe().type == Tokens::assignop && !hasError) {
		token t = peekSafe();
		if (eatCurrentToken(Tokens::assignop)) {
			auto nOp = parseRelopExpression();
			expr->addRHS(t, std::move(nOp));
		}
		else reportError("'='");
	}
	Logger::parserExit("Expression");
	return expr;
}

std::unique_ptr<NodeRelopExpression> Parser::parseRelopExpression() {
	if (hasError) return nullptr;
	Logger::parserEnter("RelopExpression");
	auto op = parseSimpleExpression();
	auto expr = std::make_unique<NodeRelopExpression>(std::move(op));

	while (peekSafe().type == Tokens::relop && !hasError) {
		token t = peekSafe();
		if (eatCurrentToken(Tokens::relop)) {
			auto nOp = parseSimpleExpression();
			expr->addRHS(t, std::move(nOp));
		}
		else reportError("'relop' token");
	}
	Logger::parserExit("RelopExpression");
	return expr;
}

std::unique_ptr<NodeSimpleExpression> Parser::parseSimpleExpression() {
	if (hasError) return nullptr;
	Logger::parserEnter("SimpleExpression");

	auto op = parseTerm();
	auto expr = std::make_unique<NodeSimpleExpression>(std::move(op));
	while (peekSafe().type == Tokens::addop && !hasError) {
		token t = peekSafe();
		if (checkAndEatToken(Tokens::addop)) {
			auto nOp = parseTerm();
			expr->addRHS(t, std::move(nOp));
		}
		else reportError("'addop' token");
	}
	Logger::parserExit("SimpleExpression");
	return expr;
}

std::unique_ptr<NodeTerm> Parser::parseTerm() {
	if (hasError) return nullptr;
	Logger::parserEnter("Term");

	auto op = parsePrimary();
	auto expr = std::make_unique<NodeTerm>(std::move(op));
	while (peekSafe().type == Tokens::mulop && !hasError) {
		token t = peekSafe();
		if (eatCurrentToken(Tokens::mulop)) {
			auto nOp = parsePrimary();
			expr->addRHS(t, std::move(nOp));
		}
		else reportError("'mulop' token");
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
			Logger::parserExit("Primary");
			return std::make_unique<NodePrimary>(id, std::move(func));
		}
		Logger::parserExit("Primary");
		return std::make_unique<NodePrimary>(id);
	}
	//number
	token t = peekSafe();
	if (checkAndEatToken(Tokens::number)) {
		Logger::parserExit("Primary");
		return std::make_unique<NodePrimary>(t);
	}

	//charliteral
	if (checkAndEatToken(Tokens::charliteral)) {
		Logger::parserExit("Primary");
		return std::make_unique<NodePrimary>(t);
	}

	//string
	if (checkAndEatToken(Tokens::string)) {
		Logger::parserExit("Primary");
		return std::make_unique<NodePrimary>(t);
	}

	//expression
	if (checkAndEatToken(Tokens::lparen)) {
		auto expr = parseExpression();
		if (checkAndEatToken(Tokens::rparen)) {
			Logger::parserExit("Primary");
			return std::make_unique<NodePrimary>(std::move(expr));
		}
		reportError("')'");
	}

	//primary (-)
	if (t.type == Tokens::addop && t.value == "-") {
		eatCurrentToken(Tokens::addop);
		auto p = parsePrimary();
		Logger::parserExit("Primary");
		return std::make_unique<NodePrimary>(std::move(t), std::move(p));
	}

	//primary (not)
	if (checkAndEatToken(Tokens::_not)) {
		auto p = parsePrimary();
		Logger::parserExit("Primary");
		return std::make_unique<NodePrimary>(std::move(t), std::move(p));
	}

	reportError("'primary'");
	return nullptr;
}

std::unique_ptr<NodeFunctionCall> Parser::parseFunctionCall() {
	if (hasError) return nullptr;
	Logger::parserEnter("FunctionCall");

	if (peekSafe().type != Tokens::lparen) reportError("'('");
	checkAndEatToken(Tokens::lparen);

	if (peekSafe().type != Tokens::rparen) {
		auto param = parseActualParameters();
		if (checkAndEatToken(Tokens::rparen)) return std::make_unique<NodeFunctionCall>(std::move(param));
		reportError("')'");
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
		if (hasError) break;
		auto nOp = parseExpression();
		expr->addRHS(std::move(nOp));
	}
	Logger::parserExit("ActualParameters");
	return expr;
}

token Parser::parseIdentifier() {
	if (hasError) return returnBadToken();
	Logger::parserEnter("Identifier");
	token t = peekSafe();
	if (t.type == Tokens::ID) {
		eatCurrentToken(Tokens::ID);
		Logger::parserExit("Identifier");
		return t;
	}
	reportError("'identifier' token");
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
	addTokenToBuffer();
	return _tokenBuffer.at(_index++);
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

//safer way then just calling _tokenBuffer.emplace_back(_scanner.getNextToken())
void Parser::addTokenToBuffer() {
	try {
		token t = _scanner.getNextToken();
		if (t.type != Tokens::ILLEGAL) _tokenBuffer.emplace_back(t);
		else addTokenToBuffer();
	}
	catch (std::bad_optional_access) {

	}
}


token Parser::returnBadToken() const {
	return token{Tokens::ERROR, -1, "ERROR TOKEN", "ERROR VALUE", false};
}

//useful to match the error token in the buffer, otherwise useless
bool Parser::areTokensEqual(token t1, token t2) {
	if (t1.type == t2.type && t1.value == t2.value && t1.lineLoc == t2.lineLoc) return true;
	return false;
}

//grabs the line the error occured at, and formats the spaces for the error, then sends
//it to the logger
void Parser::reportError(const std::string& message) {
	hasError = true;
	int position = 0;
	int sub = 1;
	if (_tokenBuffer.size() <= 1) sub = 0;
	token currentT = _tokenBuffer.at(_index);
	token errorT = _tokenBuffer.at(_index - sub);

	for (auto x : _tokenBuffer) {
		if (errorT.lineLoc == x.lineLoc) {
			_errorBuffer.emplace_back(x);
			//keep adding the length of the string until the error token
			if (!areTokensEqual(errorT, x)) position += x.value.length();
		}
	}
	//accounts for the line count and semicolon
	int errorTLength = errorT.value.length();
	if (errorTLength < 2) errorTLength *= 3;
	position += std::to_string(errorT.lineLoc).length() + errorTLength + 2;

	//gets the remaining tokens on the line
	while(true) {
		token t = _scanner.getNextToken(false);
		if (t.type == Tokens::eof) break;
		if (t.lineLoc != errorT.lineLoc) break;
		_errorBuffer.emplace_back(t);
	}

	Logger::parserError(message + " expected", errorT.lineLoc, _errorBuffer, position);
}
