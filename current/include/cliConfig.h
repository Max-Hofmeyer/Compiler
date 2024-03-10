//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "logger.h"

class CliConfig {
public:
    static void ParseCli(int count, char** arguments);
    static bool LoadFile();
    static int debugLevel;
    static bool verboseEnabled, dumpAST, hasError;
    static std::string filePath, fileContents;
private:
    static void OutputHelp();
    static void CheckForFile(const std::string& arg);
};