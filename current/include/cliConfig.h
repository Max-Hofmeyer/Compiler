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
    static bool verboseEnabled, hasError, dumpAST, dumpST, dumpCode, demo, customClass, customOutput;
    static std::string filePath, fileContents, fileName, className, outputName;

private:
    static void OutputHelp();
    static void CheckForFile(const std::string& arg);
};