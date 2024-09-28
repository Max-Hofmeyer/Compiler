#pragma once
#include <optional>
#include "ast.hpp"
#include "scanner.h"
#include "cliConfig.h"

class Parser{
public:
	explicit Parser(Scanner& scanner) : _scanner(scanner) {}
	std::unique_ptr<NodeToyCProgram> begin();
	bool hasError = false;
	bool dumpAST = false;
private:

	/* Syntax definitions */
	std::unique_ptr<NodeToyCProgram> parseToyCProgram();
	std::unique_ptr<NodeDefinition> parseDefinition();
	token parseType();
	std::unique_ptr<NodeFunctionDefinition> parseFunctionDefinition();
	std::unique_ptr<NodeFunctionHeader> parseFunctionHeader();
	std::unique_ptr<NodeFunctionBody> parseFunctionBody();
	std::unique_ptr<NodeFormalParamList> parseFormalParamList();
	std::unique_ptr<NodeStatement> parseStatement();
	std::unique_ptr<NodeExpressionStatement> parseExpressionStatement();
	std::unique_ptr<NodeBreakStatement> parseBreakStatement();
	std::unique_ptr<NodeCompoundStatement> parseCompoundStatement();
	std::unique_ptr<NodeIfStatement> parseIfStatement();
	std::unique_ptr<NodeNullStatement> parseNullStatement();
	std::unique_ptr<NodeReturnStatement> parseReturnStatement();
	std::unique_ptr<NodeWhileStatement> parseWhileStatement();
	std::unique_ptr<NodeReadStatement> parseReadStatement();
	std::unique_ptr<NodeWriteStatement> parseWriteStatement();
	std::unique_ptr<NodeNewLineStatement> parseNewLineStatement();
	std::unique_ptr<NodeExpression> parseExpression();
	std::unique_ptr<NodeRelopExpression> parseRelopExpression();
	std::unique_ptr<NodeSimpleExpression> parseSimpleExpression();
	std::unique_ptr<NodeTerm> parseTerm();
	std::unique_ptr<NodePrimary> parsePrimary();
	std::unique_ptr<NodeFunctionCall> parseFunctionCall();
	std::unique_ptr<NodeActualParameters> parseActualParameters();
	token parseIdentifier();

	/* Class variables */
	Scanner& _scanner;
	std::vector<token> _tokenBuffer;
	std::vector<token> _errorBuffer;
	size_t _index = 0;
	int scope = 0;
	bool parsingStarted = false;

	/* Syntax check helpers */
	bool isTypeLit(const token& t);
	bool isStartingExpression(const token& t);

	/* Token buffer checkers/manipulators */
	std::optional<token> peek(int offset = 0) const;
	token peekSafe(int offset = 0) const;
	token eat();
	std::optional<token> eatCurrentToken(Tokens type);
	bool checkAndEatToken(Tokens type);
	void addTokenToBuffer();

	/* Error reporting and debugging */
	token returnBadToken() const;
	bool areTokensEqual(token t1, token t2);
	void reportError(const std::string& message);
};