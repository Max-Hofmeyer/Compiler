//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/13/2024
#pragma once

#include "ast.hpp"
#include "symbolTable.h"
#include <algorithm>
#include <ranges>

class AnalyseSemantics {
public:
	explicit AnalyseSemantics(SymbolTable& table, std::unique_ptr<NodeToyCProgram>& program) :
	_table(table), _program(program) {}

	void traverseTree();
	bool hasError = false;
	
private:
	/* AST */
	void analyzeDefinition(const NodeDefinition& definition);
	void analyzeFunctionDefinition(const NodeFunctionDefinition& fDefinition);
	void analyzeFunctionHeader(const NodeFunctionHeader& fHeader);
	void analyzeFunctionBody(const NodeFunctionBody& fBody);
	void analyzeFormalParamList(const NodeFormalParamList& formalParamList);
	void analyzeStatement(NodeStatement& statement);
	void analyzeExpressionStatement(NodeExpressionStatement& expressionStatement);
	void analyzeBreakStatement(NodeBreakStatement& breakStatement);
	void analyzeCompoundStatement(const NodeCompoundStatement& compoundStatement);
	void analyzeIfStatement(NodeIfStatement& ifStatement);
	void analyzeNullStatement(NodeNullStatement& nullStatement);
	void analyzeReturnStatement(const NodeReturnStatement& returnStatement);
	void analyzeWhileStatement(NodeWhileStatement& whileStatement);
	void analyzeReadStatement(NodeReadStatement& readStatement);
	void analyzeWriteStatement(NodeWriteStatement& writeStatement);
	void analyzeNewLineStatement(NodeNewLineStatement& newLineStatement);
	void analyzeExpression(NodeExpression& expression);
	void analyzeRelopExpression(NodeRelopExpression& relopExpression);
	void analyzeSimpleExpression(NodeSimpleExpression& simpleExpression);
	void analyzeTerm(NodeTerm& term);
	void analyzePrimary(NodePrimary& primary);
	void analyzeFunctionCall(const std::string& call, const NodeFunctionCall& args);
	void analyzeActualParameters(NodeActualParameters& params);

	/* Helpers that get all the primitive data types */
	void extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types, bool getID = false);
	void extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types, bool getID = false);
	void extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types, bool getID = false);
	void extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr, std::vector<token>& types, bool getID = false);
	void extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr, std::vector<token>& types, bool getID = false);
	void extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types, bool getID = false);
	void extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types, bool getID = false);
	token convertDataToType(const token& t);

	void reportError(const std::string& message);

	/* Class variables */
	SymbolTable& _table;
	std::unique_ptr<NodeToyCProgram>& _program;
	bool isReturn = false;
};