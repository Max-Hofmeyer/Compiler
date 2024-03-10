//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "tokens.h"

class Logger {
public:
    enum class Level {
        Default = 4,
        CodeGenerator = 3,
        Parser = 2,
        Scanner = 1,
        Debug = 0,
        Verbose = -1
    };
    static void setLogLevel(Level level);
    static void scanner(const std::string& message);
    static void parser(const std::string& message);
    static void outputToken(const token t);
    static void outputTokens(std::vector <token> t);
    static void parserEnter(const std::string& message);
    static void parserCreate(const std::string& message);
    static void parserExit(const std::string& message);
    static void codeGenerator(const std::string& message);
    static void debug(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static Level logLevel;
};
