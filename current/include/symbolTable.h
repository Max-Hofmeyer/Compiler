//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/8/2024

#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <stack>

#include "ast.hpp"
#include "tokens.h"

class Symbol {
public:
    std::string id;
    token type;
    int scopeLevel;

    explicit Symbol(std::string id, token type, int scopeLevel)
        : id(std::move(id)), type(std::move(type)), scopeLevel(scopeLevel) {}
};

class SymbolTable {
public:
    void enterScope() { _scope++; }
    void exitScope() { _scope--;  }
	bool insertSymbol(std::string id, const token& type, int scope = 0);
    bool checkForSymbol(const std::string& id);
    Symbol* retrieveSymbol(const std::string& id);
    void dumpTable();
    int _scope = 0;

private:
    std::unordered_map<std::string, std::stack<Symbol>> _table;
    std::vector<std::pair<std::string, Symbol>> _historicalTable;
};