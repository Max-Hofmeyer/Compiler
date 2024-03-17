//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/23/2024

#pragma once
#include <optional>
#include "logger.h"
#include "subscriber.h"

class Parser : public SubscriberInterface {
public:
	void Update(const token& t);
	bool hasError = false;

private: 
	/* First section */
	void parseLine();
	void parsePrimary();
	void parseType();
	void parseIdentifier();
	void parseTerm();
	void parseSimpleExpression();
	void parseRelopExpression();
	void parseExpression();
	void parseActualParameters();
	void parseFunctionCall();

	/* Second section */
	void parseNewLineStatement();
	void parseWriteStatement();
	void parseReadStatement();
	void parseWhileStatement();
	void parseReturnStatement();
	void parseNullStatement();
	void parseIfStatement();
	void parseBreakStatement();
	void parseExpressionStatement();
	void parseCompoundStatement();
	void parseStatement();

	/* Third section */
	void parseToyCProgram();
	void parseDefinition();
	void parseFunctionDefinition();
	void parseFunctionHeader();
	void parseFunctionBody();
	void parseFormalParamList();

	/* Global class variables */
	std::vector<token> _tokenBuffer;
	size_t _index = 0;
	int currentLine = 0;
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
	token previousToken(int offset = 1);

	/* Error reporting and debugging */
	void outputTokenLine() const;
	void reportError(const std::string& message);

	//const std::string& name, const std::string& data = ""
	//void parseTerminatedStatement();
	//void parseBreakStatement();
	//void parseIntExpression();
};