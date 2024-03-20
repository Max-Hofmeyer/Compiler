////Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/27/2024
//#pragma once
//#include <vector>
//#include <string>
//#include <memory>
//#include <variant>
//#include "tokens.h"
//#include "logger.h"
//
//class NodeToyCProgram;
//class NodeDefinition;
//class NodeType;
//class NodeFunctionDefinition;
//class NodeFunctionHeader;
//class NodeFunctionBody;
//class NodeFormalParamList;
//class NodeStatement;
//class NodeExpressionStatement;
//class NodeBreakStatement;
//class NodeCompoundStatement;
//class NodeIfStatement;
//class NodeNullStatement;
//class NodeReturnStatement;
//class NodeWhileStatement;
//class NodeReadStatement;
//class NodeWriteStatement;
//class NodeNewLineStatement;
//class NodeExpression;
//class NodeRelopExpression;
//class NodeSimpleExpression;
//class NodeTerm;
//class NodePrimary;
//class NodeFunctionCall;
//class NodeActualParameters;
//
//class Node {
//public:
//	virtual ~Node() {}
//};
//
//class NodeToyCProgram : public Node {
//	std::unique_ptr<NodeDefinition> definition;
//	explicit NodeToyCProgram(std::unique_ptr<NodeDefinition> def) : definition(std::move(def)) {}
//};
//
//class NodeDefinition : public Node {
//	
//};
//
//class NodeFunctionDefinition : public Node {
//	
//};
//
//class NodeFunctionHeader : public Node {
//
//};
//
//class NodeFunctionBody : public Node {
//
//};
//
//class NodeDeclaration : public Node {
//	token type;
//	token id;
//public:
//	explicit NodeDeclaration(token t, token ID) : type(std::move(t)), id(std::move(ID)) {}
//};
//
//class NodeFormalParamList : public Node {
//	std::unique_ptr<NodeDeclaration> lhs;
//	std::vector<std::unique_ptr<NodeDeclaration>> rhs;
//
//public:
//	explicit NodeFormalParamList(std::unique_ptr<NodeDeclaration> lhsToks)
//		: lhs(std::move(lhsToks)) {}
//
//	void addRHS(token type, token id) {
//		std::pair<token, token> rhsToks;
//		rhsToks.first = type;
//		rhsToks.second = id;
//		rhs.emplace_back(std::move(rhs), std::move(rhsToks));
//	}
//};
//
//class NodeStatement : public Node {
//	using StatementVal = std::variant <
//		std::unique_ptr<NodeExpressionStatement>,
//		std::unique_ptr<NodeBreakStatement>,
//		std::unique_ptr<NodeCompoundStatement >,
//		std::unique_ptr<NodeIfStatement>,
//		std::unique_ptr<NodeNullStatement>,
//		std::unique_ptr<NodeReturnStatement>,
//		std::unique_ptr<NodeWhileStatement>,
//		std::unique_ptr<NodeReadStatement>,
//		std::unique_ptr<NodeWriteStatement>,
//		std::unique_ptr<NodeNewLineStatement>>;
//
//	StatementVal val;
//	explicit NodeStatement(StatementVal val) : val(std::move(val)) {}
//};
//
//class NodeExpressionStatement : public Node {
//	std::unique_ptr<NodeExpression> exp;
//	explicit NodeExpressionStatement(std::unique_ptr<NodeExpression> expr) : exp(std::move(expr)) {}
//};
//
//class NodeBreakStatement : public Node {
//	
//};
//
//class NodeCompoundStatement : public Node {
//	std::vector<std::pair<token, token>> lhs;
//	std::vector<std::unique_ptr<NodeStatement>> rhs;
//
//	void addDeclaration(const token& type, const token& id) {
//		lhs.emplace_back(type, id);
//	}
//
//	void addStatement(std::unique_ptr<NodeStatement> statement) {
//		rhs.emplace_back(std::move(statement));
//	}
//};
//
//class NodeIfStatement : public Node {
//
//};
//
//class NodeNullStatement : public Node {
//
//};
//
//class NodeReturnStatement : public Node {
//
//};
//
//class NodeWhileStatement : public Node {
//	std::unique_ptr<NodeExpression> lhs;
//	std::unique_ptr<NodeStatement> rhs;
//
//	explicit NodeWhileStatement(std::unique_ptr<NodeExpression> expr, 
//								std::unique_ptr<NodeStatement> state) :
//								lhs(std::move(expr)),
//								rhs(std::move(state)) {}
//
//};
//
//class NodeReadStatement : public Node {
//
//};
//
//class NodeWriteStatement : public Node {
//
//};
//
//class NodeNewlineStatement : public Node {
//
//};
//
//class NodeExpression : public Node {
//	std::unique_ptr<NodeRelopExpression> lhs;
//	//C++ shenanigans.. its just an array of [<token (assignop), Expression>, <token (assignop), Expression>]
//	std::vector<std::pair<token, std::unique_ptr<NodeRelopExpression>>> rhs;
//
//public:
//	explicit NodeExpression(std::unique_ptr<NodeRelopExpression> lhsExpr)
//		: lhs(std::move(lhsExpr)) {}
//
//	void addRHS(token assignop, std::unique_ptr<NodeRelopExpression> expr) {
//		rhs.emplace_back(std::move(assignop), std::move(expr));
//	}
//};
//
//class NodeRelopExpression : public Node {
//	std::unique_ptr<NodeSimpleExpression> lhs;
//	std::vector<std::pair<token, std::unique_ptr<NodeSimpleExpression>>> rhs;
//
//public:
//	explicit NodeRelopExpression(std::unique_ptr<NodeSimpleExpression> lhsExpr)
//	: lhs(std::move(lhsExpr)) {}
//
//	void addRHS(token relop, std::unique_ptr<NodeSimpleExpression> expr) {
//		rhs.emplace_back(std::move(relop), std::move(expr));
//	}
//};
//
//class NodeSimpleExpression : public Node {
//	std::unique_ptr<NodeTerm> lhs;
//	std::vector<std::pair<token, std::unique_ptr<NodeTerm>>> rhs;
//
//public:
//	explicit NodeSimpleExpression(std::unique_ptr<NodeTerm> lhsExpr)
//		: lhs(std::move(lhsExpr)) {}
//
//	void addRHS(token addop, std::unique_ptr<NodeTerm> expr) {
//		rhs.emplace_back(std::move(addop), std::move(expr));
//	}
//};
//
//class NodeTerm : public Node {
//	std::unique_ptr<NodePrimary> lhs;
//	std::vector<std::pair<token, std::unique_ptr<NodePrimary>>> rhs;
//
//public:
//	explicit NodeTerm(std::unique_ptr<NodePrimary> lhsExpr)
//		: lhs(std::move(lhsExpr)) {}
//
//	void addRHS(token addop, std::unique_ptr<NodePrimary> expr) {
//		rhs.emplace_back(std::move(addop), std::move(expr));
//	}
//};
//
//class NodePrimary : public Node {
//public:
//	using PrimaryVal = std::variant <
//		token,
//		std::unique_ptr<NodeExpression>
//		/*std::unique_ptr<NodePrimary>,*/
//
//	>;
//	PrimaryVal val;
//	explicit NodePrimary(PrimaryVal val) : val(std::move(val)) {}
//};
//
//class NodeFunctionCall : public Node {
//	
//};
//
//class NodeActualParameters : public Node {
//	std::unique_ptr<NodeExpression> lhs;
//	std::vector<std::unique_ptr<NodeExpression>> rhs;
//
//public:
//	explicit NodeActualParameters(std::unique_ptr<NodeExpression> lhsExpr)
//		: lhs(std::move(lhsExpr)) {}
//
//	void addRHS(std::unique_ptr<NodeExpression> expr) {
//		rhs.emplace_back( std::move(expr));
//	}
//};