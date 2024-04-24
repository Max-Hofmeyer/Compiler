//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/8/2024

#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <map>
#include <stack>

#include "ast.hpp"
#include "tokens.h"

class Symbol {
public:
    std::string id;
    token type;
    int scopeLevel;
    int index = 0;
    bool inUse = false;
    std::optional<std::vector<token>> parameters;
    int children = 0;

    explicit Symbol(std::string id, token type, int scopeLevel, int index)
        : id(std::move(id)), type(std::move(type)), scopeLevel(scopeLevel), index(index) {}

	bool isFunction() const { return parameters.has_value(); }
};

class SymbolTable {
public:
    void enterScope() { _scope++; }
    void exitScope() { _scope--; }
    void formatTableIndex();
    //void exitScope();
   // bool insertSymbol(const std::string& id, const token& type, int scope = 0);
    bool insertSymbol(const std::string& id, const token& type, int scope = 0, const std::vector<token>& params = {}, bool isFunc = false);
    bool checkForSymbol(const std::string& id);
    int getScopeCount(const std::string& id);
    //returns a pointer to the symbol from the table if found, null if not found
    Symbol* lookupSymbol(const std::string& id);

    //returns the ID of the most recent emplacement on the historical table
    std::string getLastSymbol();
    std::string getFirstSymbol();

    void dumpTable();
    int _scope = 0;
    int _index = 0;

private:
    std::unordered_map<std::string, std::stack<Symbol>> _table;
    std::vector<std::pair<std::string, Symbol>> _historicalTable;
};