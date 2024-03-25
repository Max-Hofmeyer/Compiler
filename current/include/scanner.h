//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <optional>
#include "tokens.h"
#include "logger.h"
#include "subject.h"

class Scanner : public Subject{
public:
    explicit Scanner(std::string source) : source_(std::move(source)) {}
	void scan();
    bool hasError;
    token getNextToken();

private:
    std::string source_;
    int line = 1;
    size_t _index = 0;
	bool _error = false;
    std::optional<char> peek(int offset = 0) const;
    token createToken(Tokens tokenType, int line, const std::string& value, const std::string& buffer, const bool isKeyword);
    char eat();
    token reportError(const std::string& message);
};