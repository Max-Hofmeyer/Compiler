//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <vector>
#include <optional>
#include "tokens.h"
#include "logger.h"
#include "subject.h"

class Scanner : public Subject{
public:
    explicit Scanner(std::string source) : source_(std::move(source)) {}
    //std::vector<token> tokenize();
	void scan();

    [[nodiscard]] bool hasError() const { return _error; }

private:
    std::string source_;
    size_t _index = 0;
	bool _error = false;
    std::optional<char> peek(int offset = 0) const;
    void sendToken(Tokens tokenType, int line, const std::string& value, const std::string& buffer);
    char eat();
};