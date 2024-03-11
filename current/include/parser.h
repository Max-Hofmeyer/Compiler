//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/23/2024

#pragma once
#include <optional>
#include "logger.h"
#include "subscriber.h"

class Parser : public SubscriberInterface {
public:
	void Update(const token& t);
	
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


	/* Third section */
	void parseStatement();

	/* Helper methods & variables */
	std::vector<token> _tokenBuffer;
	size_t _index = 0;
	int currentLine = 0;
	bool parsingStarted, hasError = false;
	bool checkAndEatToken(Tokens type);
	std::optional<token> peek(int offset = 0) const;
	std::optional<token> eatCurrentToken(Tokens type);
	token eat();
	token previousToken(int offset = 1);
	void outputTokenLine() const;
	void reportError(const std::string& message);
	//const std::string& name, const std::string& data = ""

	//void parseTerminatedStatement();
	//void parseBreakStatement();
	//void parseIntExpression();
};