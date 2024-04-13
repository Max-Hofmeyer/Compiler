//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
#pragma once
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include "tokens.h"
#include "logger.h"
#include "symbolTable.h"
#include <optional>
#include <unordered_map>

class NodeToyCProgram;
class NodeDefinition;
class NodeType;
class NodeFunctionDefinition;
class NodeFunctionHeader;
class NodeFunctionBody;
class NodeDeclaration;
class NodeFormalParamList;
class NodeStatement;
class NodeExpressionStatement;
class NodeBreakStatement;
class NodeCompoundStatement;
class NodeIfStatement;
class NodeNullStatement;
class NodeReturnStatement;
class NodeWhileStatement;
class NodeReadStatement;
class NodeWriteStatement;
class NodeNewLineStatement;
class NodeExpression;
class NodeRelopExpression;
class NodeSimpleExpression;
class NodeTerm;
class NodePrimary;
class NodeFunctionCall;
class NodeActualParameters;
class SymbolTable;

class PrettyPrint {
public:
	static PrettyPrint& Indentation() {
		static PrettyPrint indent;
		return indent;
	}

	std::string spaces() const {
		return std::string(pos, ' ');
	}

	void indent() { pos += indentSize; }
	void outdent() { pos -= indentSize;  }

private:
	int pos = 0;
	int indentSize = 2;
};

class Node {
public:
	virtual ~Node() {}
	virtual void print(std::ostream& out) const = 0;
};

class NodeToyCProgram : public Node {
public:
	std::vector<std::unique_ptr<NodeDefinition>> definitions;
	void addRHS(std::unique_ptr<NodeDefinition> def) {
		definitions.emplace_back(std::move(def));
	}

	void print(std::ostream& out) const override;
};

class NodeDefinition : public Node {
public:
	std::unique_ptr<NodeDeclaration> lhs;
	std::optional<std::unique_ptr<NodeFunctionDefinition>> rhs;
	explicit NodeDefinition(std::unique_ptr<NodeDeclaration> decl)
		: lhs(std::move(decl)) {}

	NodeDefinition(std::unique_ptr<NodeDeclaration> decl, std::unique_ptr<NodeFunctionDefinition> funcdef)
	: lhs(std::move(decl)), rhs(std::move(funcdef)) {}

	void print(std::ostream& out) const override;
};

class NodeFunctionDefinition : public Node {
	std::unique_ptr<NodeFunctionHeader> lhs;
	std::unique_ptr<NodeFunctionBody> rhs;

public:
	explicit NodeFunctionDefinition(std::unique_ptr<NodeFunctionHeader> header, std::unique_ptr<NodeFunctionBody> body)
		: lhs(std::move(header)), rhs(std::move(body)) {}

	void print(std::ostream& out) const override;
};

class NodeFunctionHeader : public Node {
	std::optional<std::unique_ptr<NodeFormalParamList>> lhs;

public:
	NodeFunctionHeader() : lhs(std::nullopt) {}

	explicit NodeFunctionHeader(std::unique_ptr<NodeFormalParamList> paramlist) :
		lhs(std::move(paramlist)) {}

	void print(std::ostream& out) const override;
};

class NodeFunctionBody : public Node {
	std::unique_ptr<NodeCompoundStatement> lhs;
public:
	explicit NodeFunctionBody(std::unique_ptr<NodeCompoundStatement> stmt)
		: lhs(std::move(stmt)) {}

	void print(std::ostream& out) const override;
};

class NodeDeclaration : public Node {
public:
	token type;
	token id;
	explicit NodeDeclaration(token t, token ID)
	: type(std::move(t)), id(std::move(ID)) {}

	void print(std::ostream& out) const override;
};

class NodeFormalParamList : public Node {
	std::unique_ptr<NodeDeclaration> lhs;
	std::vector<std::unique_ptr<NodeDeclaration>> rhs;

public:
	explicit NodeFormalParamList(std::unique_ptr<NodeDeclaration> lhsToks)
		: lhs(std::move(lhsToks)) {}

	void addRHS(std::unique_ptr<NodeDeclaration> n) {
		rhs.emplace_back(std::move(n));
	}

	void print(std::ostream& out) const override;
};

class NodeStatement : public Node {
	using StatementVal = std::variant <
		std::unique_ptr<NodeExpressionStatement>,
		std::unique_ptr<NodeBreakStatement>,
		std::unique_ptr<NodeCompoundStatement>,
		std::unique_ptr<NodeIfStatement>,
		std::unique_ptr<NodeNullStatement>,
		std::unique_ptr<NodeReturnStatement>,
		std::unique_ptr<NodeWhileStatement>,
		std::unique_ptr<NodeReadStatement>,
		std::unique_ptr<NodeWriteStatement>,
		std::unique_ptr<NodeNewLineStatement>>;
	StatementVal val;

public:
	explicit NodeStatement(StatementVal val) : val(std::move(val)) {}
	void print(std::ostream& out) const override;
};

class NodeExpressionStatement : public Node {
	std::unique_ptr<NodeExpression> exp;

public:
	explicit NodeExpressionStatement(std::unique_ptr<NodeExpression> expr) : exp(std::move(expr)) {}
	void print(std::ostream& out) const override;
};

class NodeBreakStatement : public Node {
public:
	NodeBreakStatement() = default;
	void print(std::ostream& out) const override;
};

class NodeCompoundStatement : public Node {
public:
	std::vector<std::unique_ptr<NodeDeclaration>> lhs;
	std::vector<std::unique_ptr<NodeStatement>> rhs;
	void addDeclaration(std::unique_ptr<NodeDeclaration> declaration) {
		lhs.emplace_back(std::move(declaration));
	}

	void addStatement(std::unique_ptr<NodeStatement> stmt) {
		rhs.emplace_back(std::move(stmt));
	}
	void print(std::ostream& out) const override;
};

class NodeIfStatement : public Node {
	std::unique_ptr<NodeExpression> lhs;
	std::unique_ptr<NodeStatement> mhs;
	std::optional<std::unique_ptr<NodeStatement>> rhs;

public:
	//constructor for if w/o an else
	explicit NodeIfStatement(std::unique_ptr<NodeExpression> expr,
		std::unique_ptr<NodeStatement> state) :
		lhs(std::move(expr)),
		rhs(std::move(state)) {}

	NodeIfStatement(std::unique_ptr<NodeExpression> expr,
		std::unique_ptr<NodeStatement> state,
		std::unique_ptr<NodeStatement> elseState) :
		lhs(std::move(expr)),
		mhs(std::move(state)),
		rhs(std::move(elseState)) {}

	void print(std::ostream& out) const override;
};

class NodeNullStatement : public Node {
public:
	NodeNullStatement() = default;
	void print(std::ostream& out) const override;
};

class NodeReturnStatement : public Node {
	std::optional<std::unique_ptr<NodeExpression>> lhs;

public:
	//when there is no statement to return
	NodeReturnStatement() : lhs(std::nullopt) {}

	explicit NodeReturnStatement(std::unique_ptr<NodeExpression> expr) :
		lhs(std::move(expr)) {}

	void print(std::ostream& out) const override;
};

class NodeWhileStatement : public Node {
	std::unique_ptr<NodeExpression> lhs;
	std::unique_ptr<NodeStatement> rhs;

public:
	explicit NodeWhileStatement(std::unique_ptr<NodeExpression> expr, 
								std::unique_ptr<NodeStatement> state) :
								lhs(std::move(expr)),
								rhs(std::move(state)) {}

	void print(std::ostream& out) const override;

};

class NodeReadStatement : public Node {
	token lhs;
	std::vector<token> rhs;

public:
	explicit NodeReadStatement(token lhsID) : lhs(std::move(lhsID)) {}

	void addRHS(token id) {
		rhs.emplace_back(std::move(id));
	}

	void print(std::ostream& out) const override;
};

class NodeWriteStatement : public Node {
	std::unique_ptr<NodeActualParameters> lhs;

public:
	explicit NodeWriteStatement(std::unique_ptr<NodeActualParameters> lhsParm) : lhs(std::move(lhsParm)) {}

	void print(std::ostream& out) const override;
};

class NodeNewLineStatement : public Node {
public:
	NodeNewLineStatement() = default;

	void print(std::ostream& out) const override;
};

class NodeExpression : public Node {
	std::unique_ptr<NodeRelopExpression> lhs;
	//C++ shenanigans.. its just a vector of [<token (assignop), Expression>, <token (assignop), Expression>]
	std::vector<std::pair<token, std::unique_ptr<NodeRelopExpression>>> rhs;

public:
	explicit NodeExpression(std::unique_ptr<NodeRelopExpression> lhsExpr)
		: lhs(std::move(lhsExpr)) {}

	void addRHS(token assignop, std::unique_ptr<NodeRelopExpression> expr) {
		rhs.emplace_back(std::move(assignop), std::move(expr));
	}
	void print(std::ostream& out) const override;
};

class NodeRelopExpression : public Node {
	std::unique_ptr<NodeSimpleExpression> lhs;
	std::vector<std::pair<token, std::unique_ptr<NodeSimpleExpression>>> rhs;

public:
	explicit NodeRelopExpression(std::unique_ptr<NodeSimpleExpression> lhsExpr)
	: lhs(std::move(lhsExpr)) {}

	void addRHS(token relop, std::unique_ptr<NodeSimpleExpression> expr) {
		rhs.emplace_back(std::move(relop), std::move(expr));
	}
	void print(std::ostream& out) const override;
};

class NodeSimpleExpression : public Node {
	std::unique_ptr<NodeTerm> lhs;
	std::vector<std::pair<token, std::unique_ptr<NodeTerm>>> rhs;

public:
	explicit NodeSimpleExpression(std::unique_ptr<NodeTerm> lhsExpr)
		: lhs(std::move(lhsExpr)) {}

	void addRHS(token addop, std::unique_ptr<NodeTerm> expr) {
		rhs.emplace_back(std::move(addop), std::move(expr));
	}

	void print(std::ostream& out) const override;
};

class NodeTerm : public Node {
	std::unique_ptr<NodePrimary> lhs;
	std::vector<std::pair<token, std::unique_ptr<NodePrimary>>> rhs;

public:
	explicit NodeTerm(std::unique_ptr<NodePrimary> lhsExpr)
		: lhs(std::move(lhsExpr)) {}

	void addRHS(token addop, std::unique_ptr<NodePrimary> expr) {
		rhs.emplace_back(std::move(addop), std::move(expr));
	}

	void print(std::ostream& out) const override;
};

class NodePrimary : public Node {
	using PrimaryVal = std::variant <
		token, //handles num,string,char,id, - and not
		std::unique_ptr<NodeExpression>,
		std::pair<token, std::unique_ptr<NodePrimary>>
	>;
	PrimaryVal val;
	std::optional<std::unique_ptr<NodeFunctionCall>> rhs;

	
public:
	explicit NodePrimary(token t) : val(std::move(t)) {}

	NodePrimary(token t, std::unique_ptr<NodeFunctionCall> func)
		: val(std::move(t)), rhs(std::move(func)) {}

	NodePrimary(std::unique_ptr<NodeExpression> expr) : val(std::move(expr)) {}
	NodePrimary(token t, std::unique_ptr<NodePrimary> p) : val(std::make_pair(std::move(t), std::move(p))) {}

	void print(std::ostream& out) const override;
};

class NodeFunctionCall : public Node {
	std::optional<std::unique_ptr<NodeActualParameters>> lhs;

public:
	NodeFunctionCall() : lhs(std::nullopt) {}
	explicit NodeFunctionCall(std::unique_ptr<NodeActualParameters> expr) :
		lhs(std::move(expr)) {}

	void print(std::ostream& out) const override;
};

class NodeActualParameters : public Node {
	std::unique_ptr<NodeExpression> lhs;
	std::vector<std::unique_ptr<NodeExpression>> rhs;

public:
	explicit NodeActualParameters(std::unique_ptr<NodeExpression> lhsExpr)
		: lhs(std::move(lhsExpr)) {}

	void addRHS(std::unique_ptr<NodeExpression> expr) {
		rhs.emplace_back( std::move(expr));
	}

	void print(std::ostream& out) const override;
};

//class SymbolTable {
//private:
//	std::unordered_map<std::string, token> table;
//
//public:
//	bool insert(const token& tok) {
//		if (table.find(tok.value) == table.end()) {
//			table[tok.value] = tok;
//			return true;
//		}
//		return false;
//	}
//
//	const token* lookup(const std::string& name) const {
//		auto it = table.find(name);
//		if (it != table.end()) return &it->second;
//		return nullptr;
//	}
//
//	void print(std::ostream& out);
//};