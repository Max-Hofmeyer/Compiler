//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/23/2024
#pragma once
#include "logger.h"
#include "subscriber.h"
#include "ast.h"
#include <optional>

class Parser : public SubscriberInterface {
public:
	void Update(const token& t);
	[[nodiscard]] bool hasError() const {
		return _error;
	}
private: 
	std::vector<token> _tokenBuffer;
	size_t _index = 0;
	bool _error = false;
	std::optional<token> peek(int offset = 0) const;
	std::optional<token> eatCurrentToken(Tokens type);
	token eat();
	bool checkAndEatToken(Tokens type);
	void parseProgram();
	void parseStatement();
	void parseTerminatedStatement();
	void parseBreakStatement();
	void parseIntExpression();
	void parseExpression();
	void parseTerm();

	void parseType();
};