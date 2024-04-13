//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/8/2024

#include "symbolTable.h"

//void SymbolTable::exitScope() {
//	for (auto& x : _table) {
//		while (!x.second.empty() && x.second.top().scopeLevel == _scope) x.second.pop();
//	}
//	_scope--;
//}

bool SymbolTable::insertSymbol(std::string id, const token& type) {
	if (_table[id].empty() || _table[id].top().scopeLevel != _scope) {
		Symbol s(id, type, _scope);
		_table[id].emplace(s);
		_historicalTable.emplace_back(id,s);
		return true;
	}
	return false;
}

bool SymbolTable::checkForSymbol(const std::string& id) {
	if (retrieveSymbol(id) == nullptr) return false;
	return true;
}

Symbol* SymbolTable::retrieveSymbol(const std::string& id) {
	auto s = _table.find(id);
	if (s != _table.end() && !s->second.empty()) return &s->second.top();
	return nullptr;
}

void SymbolTable::dumpTable() {
	std::cout << "\n<< Symbol Table >> \n";
	std::cout << "Identifier | Type | Scope\n";
	for (const auto& [id, s] : _historicalTable) {
		std::cout << id << "\t" << s.type.typeString << "\t" << s.scopeLevel << "\n";
	}
}



