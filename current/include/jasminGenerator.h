//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/19/2024

#pragma once
#include <utility>
#include <ranges>
#include <fstream>

#include "ast.hpp"
#include "symbolTable.h"
#include "cliConfig.h"

class GenerateJasminCode {
public:
	explicit GenerateJasminCode(SymbolTable& table, std::unique_ptr<NodeToyCProgram>& program) :
		_table(table), _program(program) {
	}
	~GenerateJasminCode() { if (CliConfig::dumpCode) os.close(); }

	void generateCode();

private:
	/* AST */
	void generateDefinition(const NodeDefinition& definition);
	void generateFunctionDefinition(const NodeFunctionDefinition& fDefinition);
	void generateFunctionHeader(const NodeFunctionHeader& fHeader);
	void generateFunctionBody(const NodeFunctionBody& fBody);
	void generateFormalParamList(const NodeFormalParamList& formalParamList);
	void generateStatement(NodeStatement& statement);
	void generateExpressionStatement(NodeExpressionStatement& expressionStatement);
	void generateComplexExpression(NodeExpression& expression);
	void generateBreakStatement(NodeBreakStatement& breakStatement);
	void generateCompoundStatement(const NodeCompoundStatement& compoundStatement);
	void generateIfStatement(const NodeIfStatement& ifStatement);
	void generateNullStatement(NodeNullStatement& nullStatement);
	void generateReturnStatement(const NodeReturnStatement& returnStatement);
	void generateWhileStatement(NodeWhileStatement& whileStatement);
	void generateReadStatement(NodeReadStatement& readStatement);
	void generateWriteStatement(NodeWriteStatement& writeStatement);
	void generateNewLineStatement(NodeNewLineStatement& newLineStatement);
	void generateExpression(NodeExpression& expression);
	void generateRelopExpression(NodeRelopExpression& relopExpression);
	void generateSimpleExpression(NodeSimpleExpression& simpleExpression);
	void generateTerm(NodeTerm& term);
	void generatePrimary(NodePrimary& primary);
	void generateFunctionCall(const std::string& call, const NodeFunctionCall& args);
	void generateActualParameters(NodeActualParameters& params);

	/* Helpers that get all the primitive data types */
	void extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types, bool getID = false, bool getAll = false);
	void extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types, bool getID = false, bool getAll = false);
	void extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types, bool getID = false, bool getAll = false);
	void extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr, std::vector<token>& types, bool getID = false, bool getAll = false);
	void extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr, std::vector<token>& types, bool getID = false, bool getAll = false);
	void extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types, bool getID = false, bool getAll = false);
	void extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types, bool getID = false, bool getAll = false);
	token convertDataToType(const token& t);
	token convertSymbolToToken(const Symbol& s);
	
	/* File stream wrappers */
	void out(const std::string& message) {
		if (debugStmts) Logger::codeGeneratorStream(message);
		os << "\t" << message << "\n";
	}

	void rawOut(const std::string& message) {
		if (debugStmts) Logger::codeGeneratorStream(message);
		os << message;
	}

	/* Class variables */
	SymbolTable& _table;
	std::unique_ptr<NodeToyCProgram>& _program;
	std::ofstream os;
	std::string _outputName;
	std::string _lastUsedId;
	std::string orCatch;
	std::string funcCall;
	int _labelIndex = 0;
	bool debugStmts = false;
	bool inMain = false;
	bool isOR = false;
};