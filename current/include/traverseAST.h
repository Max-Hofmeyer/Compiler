//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/13/2024
#pragma once

#include "ast.hpp"
#include "symbolTable.h"

class TraverseAST {
public:
	explicit TraverseAST(SymbolTable& table, std::unique_ptr<NodeToyCProgram>& program) :
	_table(table), _program(program) {}

	void analyzeSemantics();
	//void generateCode();

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
	void analyzeReturnStatement(NodeReturnStatement& returnStatement);
	void analyzeWhileStatement(NodeWhileStatement& whileStatement);
	void analyzeReadStatement(NodeReadStatement& readStatement);
	void analyzeWriteStatement(NodeWriteStatement& writeStatement);
	void analyzeNewLineStatement(NodeNewLineStatement& newLineStatement);
	void analyzeExpression(NodeExpression& expression);
	void analyzeRelopExpression(NodeRelopExpression& relopExpression);
	void analyzeSimpleExpression(NodeSimpleExpression& simpleExpression);
	void analyzeTerm(NodeTerm& term);
	void analyzePrimary(NodePrimary& primary);
	void analyzeFunctionCall(NodeFunctionCall& functionCall);
	void analyzeActualParameters(NodeActualParameters& params);

	//token getPrimaryType(NodePrimary& primary);
	void reportError(const std::string& message);

	SymbolTable& _table;
	std::unique_ptr<NodeToyCProgram>& _program;
};