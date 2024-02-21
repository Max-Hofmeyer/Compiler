//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/21/2024

#pragma once
#include <string>

class CliConfig {
public:
    static void ParseCli(int count, char** arguments);
    static bool LoadFile();
    static std::string fileContents;
    static int debugLevel;
    static bool helpEnabled, verboseEnabled, defaultEnabled;
    static std::string filePath;
};