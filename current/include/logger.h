//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <iostream>

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
    static bool hasError;
    static void setLogLevel(Level level);
    static void scanner(const std::string& message);
    static void parser(const std::string& message);
    static void codeGenerator(const std::string& message);
    static void debug(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static Level logLevel;
};