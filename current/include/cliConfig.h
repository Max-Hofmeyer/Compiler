//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

class CliConfig {
public:
    static void ParseCli(int count, char** arguments);
    static bool LoadFile();
    static int debugLevel;
    static bool helpEnabled, verboseEnabled, dumpAST, dumpCode;
    static bool defaultEnabled;
    static std::string filePath, outputFileName, fileContents, className;
};