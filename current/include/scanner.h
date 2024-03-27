//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <optional>
#include "tokens.h"
#include "logger.h"

class Scanner {
public:
    explicit Scanner(std::string source) : source_(std::move(source)) {}
    token getNextToken(bool display = true);
    bool hasError;

private:
    std::string source_;
    int line = 1;
    size_t _index = 0;
	bool _error = false;
    bool _display = true;
    int repeat = 0;
    void checkWhiteSpace();
    bool checkComments();
    std::optional<char> peek(int offset = 0) const;
    token createToken(Tokens tokenType, int line, const std::string& buffer, const std::string& value, bool isKeyword);
    char eat();
    token reportError(const std::string& message);
};