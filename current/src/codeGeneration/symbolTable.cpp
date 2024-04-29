//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/8/2024

#include "symbolTable.h"

#include <ranges>

//void SymbolTable::exitScope() {
//	for (auto& x : _table) {
//		while (!x.second.empty() && x.second.top().scopeLevel == _scope) x.second.pop();
//	}
//	_scope--;
//}

bool SymbolTable::insertSymbol(const std::string& id, const token& type, bool isFunc) {
	if (isFunc) _index = 0;
	if (id == "main") _index = -1;

	const auto it = _table.find(id);

	if (it == _table.end() || it->second.top().scopeLevel != _scope || it->second.top().scopeCount != _scopeCount) {
		Logger::symbolTable(type.value + " " + id + " in scope " + std::to_string(_scope) + " at index " + std::to_string(_index));
		Symbol s(id, type, _scope, _index++);
		_table[id].emplace(s);
		_historicalTable.emplace_back(id, s);
		_lastScope = _scope;
		return true;
	}
	Logger::symbolTableWarning(type.value + " " + id + " in scope " + std::to_string(_scope));
	return false;
}

bool SymbolTable::checkForSymbol(const std::string& id) {
	if (lookupSymbol(id) == nullptr) return false;
	return true;
}

Symbol* SymbolTable::lookupSymbol(const std::string& id) {
	auto s = _table.find(id);
	if (s != _table.end() && !s->second.empty()) return &s->second.top();
	return nullptr;
}

std::string SymbolTable::getLastSymbol() {
	if (_historicalTable.empty()) return {};
	return _historicalTable.back().second.id;
}

std::string SymbolTable::getFirstSymbol() {
	if (_historicalTable.empty()) return {};

	for (auto& it : std::ranges::reverse_view(_historicalTable)) {
		if (it.second.scopeLevel == 0) {
			return it.second.id;
		}
	}
	return {};
}

void SymbolTable::dumpTable() {
	std::cout << "\n<< Symbol Table >> \n";
	std::cout << "Identifier  |  Type  |  Scope \n";
	for (const auto& [id, s] : _historicalTable) {
		std::cout << id << "\t\t" << s.type.typeString << "\t" << s.scopeLevel << "\n";
	}
	std::cout << "\n";
}



