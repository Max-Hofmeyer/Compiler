//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/13/2024

#include "traverseAST.h"


//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void TraverseAST::analyzeSemantics() {
	if (_program == nullptr) return;
	bool mainInProg = false;

	for (auto& definition : _program->definitions) {
		analyzeDefinition(*definition);
		if (definition->lhs->id.value == "main") mainInProg = true;
	}
	if (!mainInProg && !hasError) reportError("No main declared");
}


void TraverseAST::analyzeDefinition(const NodeDefinition& definition) {
	if (hasError) return;
	Logger::semanticAnalyzer("definition");
	if (!_table.insertSymbol(definition.lhs->id.value, definition.lhs->type)) {
		reportError(definition.lhs->id.value + " already declared within the scope");
	}
	if (definition.rhs.has_value()) {
		analyzeFunctionDefinition(*definition.rhs.value());
	}
}

void TraverseAST::analyzeFunctionDefinition(const NodeFunctionDefinition& fDefinition) {
	if (hasError) return;
	Logger::semanticAnalyzer("function definition");
	_table.enterScope();
	analyzeFunctionHeader(*fDefinition.lhs);
	analyzeFunctionBody(*fDefinition.rhs);
	_table.exitScope();
}

void TraverseAST::analyzeFunctionHeader(const NodeFunctionHeader& fHeader) {
	if (hasError) return;
	Logger::semanticAnalyzer("function header");
	if (fHeader.lhs.has_value() && fHeader.lhs != nullptr) {
		auto& temp = fHeader.lhs.value();
		analyzeFormalParamList(*temp);
	}
}

void TraverseAST::analyzeFunctionBody(const NodeFunctionBody& fBody) {
	if (hasError) return;
	Logger::semanticAnalyzer("function body");
	analyzeCompoundStatement(*fBody.lhs);
}

void TraverseAST::analyzeFormalParamList(const NodeFormalParamList& formalParamList) {
	if (hasError) return;
	Logger::semanticAnalyzer("formal parameters ");
	const std::string functionID = _table.getLastSymbol();
	const auto actualSymbol = _table.lookupSymbol(functionID);
	std::vector<token> parameters;
	parameters.push_back(formalParamList.lhs->type);

	if (!_table.insertSymbol(formalParamList.lhs->id.value, formalParamList.lhs->type)) {
		reportError(std::to_string(formalParamList.lhs->id.lineLoc) + ": '" + formalParamList.lhs->id.value +
			"' already declared within the scope");
	}
	for (auto& param : formalParamList.rhs) {
		if (param && param != nullptr) {
			parameters.push_back(param->type);
			if (!_table.insertSymbol(param->id.value, param->type)) {
				reportError(param->id.value + "already declared within the scope");
			}
		}
		else Logger::semanticAnalyzer("NULLPTR in formalparamlist");
	}
	if (actualSymbol != nullptr) actualSymbol->parameters = parameters;
}

void TraverseAST::analyzeStatement(NodeStatement& statement) {
	if (hasError) return;
	std::visit(overloaded{
		           [this](const std::unique_ptr<NodeExpressionStatement>& arg) { analyzeExpressionStatement(*arg); },
		           [this](const std::unique_ptr<NodeBreakStatement>& arg) { analyzeBreakStatement(*arg); },
		           [this](const std::unique_ptr<NodeCompoundStatement>& arg) { analyzeCompoundStatement(*arg); },
		           [this](const std::unique_ptr<NodeIfStatement>& arg) { analyzeIfStatement(*arg); },
		           [this](const std::unique_ptr<NodeNullStatement>& arg) { analyzeNullStatement(*arg); },
		           [this](const std::unique_ptr<NodeReturnStatement>& arg) { analyzeReturnStatement(*arg); },
		           [this](const std::unique_ptr<NodeWhileStatement>& arg) { analyzeWhileStatement(*arg); },
		           [this](const std::unique_ptr<NodeReadStatement>& arg) { analyzeReadStatement(*arg); },
		           [this](const std::unique_ptr<NodeWriteStatement>& arg) { analyzeWriteStatement(*arg); },
		           [this](const std::unique_ptr<NodeNewLineStatement>& arg) { analyzeNewLineStatement(*arg); }
	           }, statement.val);
}

void TraverseAST::analyzeExpressionStatement(NodeExpressionStatement& expressionStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("expression statement");
	analyzeExpression(*expressionStatement.exp);
}

void TraverseAST::analyzeBreakStatement(NodeBreakStatement& breakStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("break statement");
}

void TraverseAST::analyzeCompoundStatement(const NodeCompoundStatement& compoundStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("compound statement");
	for (auto& declaration : compoundStatement.lhs) {
		if (!_table.insertSymbol(declaration->id.value, declaration->type)) {
			std::cout << "\n\t";
			std::cout << declaration->type.value << " " << declaration->id.value;
			std::cout << "\n";
			reportError(
				" ^ " + std::to_string(declaration->id.lineLoc) + ": '" + declaration->id.value +
				"' already declared within the scope");
		}
	}
	for (auto& statement : compoundStatement.rhs) analyzeStatement(*statement);
}

void TraverseAST::analyzeIfStatement(NodeIfStatement& ifStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("if statement");
	_table.enterScope();
	analyzeExpression(*ifStatement.lhs);
	analyzeStatement(*ifStatement.mhs);
	if (ifStatement.rhs.has_value()) {
		analyzeStatement(*ifStatement.rhs.value());
	}
	_table.exitScope();
}

void TraverseAST::analyzeNullStatement(NodeNullStatement& nullStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("null statement");
}

void TraverseAST::analyzeReturnStatement(const NodeReturnStatement& returnStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("return statement");
	if (!returnStatement.lhs.has_value()) return;

	std::vector<token> returnT;
	const auto func = _table.lookupSymbol(_table.getFirstSymbol());
	if (func == nullptr) return;

	extractTypesFromExpression(*returnStatement.lhs.value(), returnT, false);
	if (returnT.empty()) return;
	if (returnT[0].type != func->type.type) {
		reportError("'" + returnT[0].value +
			"' incorrect return type for function '"
			+ func->id + "', expected type '" + func->type.typeString + "'");
	}

	analyzeExpression(*returnStatement.lhs.value());
}

void TraverseAST::analyzeWhileStatement(NodeWhileStatement& whileStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("while statement");
	_table.enterScope();
	analyzeExpression(*whileStatement.lhs);
	analyzeStatement(*whileStatement.rhs);
	_table.exitScope();
}

void TraverseAST::analyzeReadStatement(NodeReadStatement& readStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("read statement");
	for (const auto& id : readStatement.rhs) {
		Logger::semanticAnalyzer("reading " + id.value);
	}
}

void TraverseAST::analyzeWriteStatement(NodeWriteStatement& writeStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("write statement");
	analyzeActualParameters(*writeStatement.lhs);
}

void TraverseAST::analyzeNewLineStatement(NodeNewLineStatement& newLineStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("new line statement");
}

void TraverseAST::analyzeExpression(NodeExpression& expression) {
	if (hasError) return;
	Logger::semanticAnalyzer("expression");
	std::cout << "\n";
	expression.print(std::cout);
	std::cout << "\n";
	//std::vector<token> lhsTypes, rhsTypes, lhsID, rhsID;
	std::vector<token> types;
	extractTypesFromRelopExpression(*expression.lhs, types);
	extractTypesFromRelopExpression(*expression.lhs, types, true);

	if (!expression.rhs.empty()) {
		for (auto& rhsExpr : expression.rhs | std::views::values) {
			extractTypesFromRelopExpression(*rhsExpr, types, false);
			extractTypesFromRelopExpression(*rhsExpr, types, true);
		}
	}
	if (!types.empty()) {
		const auto ref = types[0];
		for (const auto& t : types) {
			if (ref.type != t.type) {
				std::cout << "\n\t";
				expression.print(std::cout);
				std::cout << "\n";
				reportError(" ^ " + std::to_string(t.lineLoc) + ": Type mismatch for expression");
			}
		}
	}

	analyzeRelopExpression(*expression.lhs);
	for (auto& rhsExpr : expression.rhs | std::views::values) {
		analyzeRelopExpression(*rhsExpr);
	}
}

void TraverseAST::analyzeRelopExpression(NodeRelopExpression& relopExpression) {
	if (hasError) return;
	analyzeSimpleExpression(*relopExpression.lhs);
	for (auto& [relop, rhsExpr] : relopExpression.rhs) {
		analyzeSimpleExpression(*rhsExpr);
	}
}

void TraverseAST::analyzeSimpleExpression(NodeSimpleExpression& simpleExpression) {
	if (hasError) return;
	Logger::semanticAnalyzer("simple expression");
	analyzeTerm(*simpleExpression.lhs);
	for (auto& [addop, rhsTerm] : simpleExpression.rhs) {
		analyzeTerm(*rhsTerm);
	}
}

void TraverseAST::analyzeTerm(NodeTerm& term) {
	if (hasError) return;
	Logger::semanticAnalyzer("term");
	analyzePrimary(*term.lhs);
	for (auto& [op, rhsPrimary] : term.rhs) {
		if (op.value == "/") {
			const auto& temp = rhsPrimary->val;
			if (std::holds_alternative<token>(temp) && std::get<token>(temp).value == "0") {
				reportError(std::to_string(op.lineLoc) + ": Invalid operation, cannot divide by 0");
			}
		}
		analyzePrimary(*rhsPrimary);
	}
}

void TraverseAST::analyzePrimary(NodePrimary& primary) {
	if (hasError) return;
	std::string lastUsedID;
	std::visit(overloaded{
		           [this, &lastUsedID](token& t) {
			           if (t.type == Tokens::ID) {
				           if (!_table.checkForSymbol(t.value)) {
					           reportError(
						           std::to_string(t.lineLoc) + ": Identifier '" + t.value +
						           "' was referenced before being declared");
				           }
				           lastUsedID = t.value;
			           }

		           },
		           [this](const std::unique_ptr<NodeExpression>& expr) {
			           analyzeExpression(*expr);
		           },
		           [this](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
			           analyzePrimary(*p.second);
		           }
	           }, primary.val);

	if (primary.rhs) {
		analyzeFunctionCall(lastUsedID, *primary.rhs.value());
	}
}

void TraverseAST::analyzeFunctionCall(const std::string& call, const NodeFunctionCall& args) {
	if (hasError || call.empty()) return;
	Logger::semanticAnalyzer("function call");
	std::vector<token> arguments;
	std::vector<token> declared;
	const auto functionDef = _table.lookupSymbol(call);
	if (functionDef->parameters.has_value()) declared = functionDef->parameters.value();

	extractTypesFromFunctionCall(args, arguments, false);
	const int argumentSize = arguments.size();
	const int declareSize = declared.size();
	int argumentLoc = 0;
	if (argumentSize != 0) argumentLoc = arguments[0].lineLoc;

	if (arguments.empty() && declared.empty()) return;
	if (declared.empty() && !arguments.empty()) {
		std::cout << "\n\t";
		args.print(std::cout);
		std::cout << "\n";
		reportError(
			" ^ " + std::to_string(argumentLoc) + ": Function " + "'" + call + "'" +
			" expects no arguments, but received " + std::to_string(argumentSize));
		return;
	}

	//so the error output doesn't look weird
	if (functionDef->parameters.has_value() && arguments.empty()) {
		reportError(
			std::to_string(declared[0].lineLoc) + ": Function " + "'" + call + "'" +
			" expects arguments, but received 0");
		return;
	}

	if (declareSize != argumentSize) {
		std::cout << "\n\t";
		args.print(std::cout);
		std::cout << "\n";
		reportError(
			" ^ " + std::to_string(argumentLoc) + ": Expected " + std::to_string(declareSize) + " arguments, got "
			+ std::to_string(argumentSize) + " in function call " + "'" + call + "'");
		return;
	}

	for (int i = 0; i < declareSize; ++i) {
		if (declared[i].type != arguments[i].type) {
			std::cout << "\n\t";
			args.print(std::cout);
			std::cout << "\n";
			reportError(
				" ^ : Type mismatch for argument " + std::to_string(i + 1)
				+ " in function call '" + call + "': Expected type '" + declared[i].typeString + "', got type '" +
				arguments[i].typeString + "'");
			return;
		}
	}

	if (args.lhs.has_value()) {
		analyzeActualParameters(*args.lhs.value());
	}
}

void TraverseAST::analyzeActualParameters(NodeActualParameters& params) {
	if (hasError) return;
	Logger::semanticAnalyzer("actual parameters");
	analyzeExpression(*params.lhs);
	for (auto& param : params.rhs) {
		analyzeExpression(*param);
	}
}

void TraverseAST::extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types,
                                               const bool getID) {
	if (functionCall.lhs) {
		extractTypesFromActualParameters(*functionCall.lhs.value(), types, getID);
	}
}

void TraverseAST::extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types,
                                                   const bool getID) {
	extractTypesFromExpression(*params.lhs, types, getID);
	for (const auto& expr : params.rhs) {
		extractTypesFromExpression(*expr, types, getID);
	}
}

void TraverseAST::extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types, const bool getID) {
	std::vector<token> id;
	extractTypesFromRelopExpression(*expr.lhs, types, getID);
	extractTypesFromRelopExpression(*expr.lhs, id, getID);

	for (const auto& [op, subExpr] : expr.rhs) {
		extractTypesFromRelopExpression(*subExpr, types, getID);
		extractTypesFromRelopExpression(*subExpr, id, getID);
	}
}

void TraverseAST::extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr, std::vector<token>& types,
                                                  const bool getID) {
	extractTypesFromSimpleExpression(*relopExpr.lhs, types, getID);
	for (const auto& [relop, subExpr] : relopExpr.rhs) {
		extractTypesFromSimpleExpression(*subExpr, types, getID);
	}
}

void TraverseAST::extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr, std::vector<token>& types,
                                                   const bool getID) {
	extractTypesFromTerm(*simpleExpr.lhs, types, getID);
	for (const auto& [addop, term] : simpleExpr.rhs) {
		extractTypesFromTerm(*term, types, getID);
	}
}

void TraverseAST::extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types, const bool getID) {
	extractTypesFromPrimary(*term.lhs, types, getID);
	for (const auto& [mulop, primary] : term.rhs) {
		extractTypesFromPrimary(*primary, types, getID);
	}
}

void TraverseAST::extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types, const bool getID) {
	std::visit(overloaded{
		           [&](const token& t) {
			           if (t.type == Tokens::_int || t.type == Tokens::_char) {
				           types.push_back(t);
			           }

			           if (t.type == Tokens::string || t.type == Tokens::number || t.type == Tokens::charliteral) {
				           if (!getID) types.push_back(convertDataToType(t));
			           }
			           if (t.type == Tokens::ID) {
				           const auto s = _table.lookupSymbol(t.value);
				           if (s != nullptr) {
					           types.push_back(s->type);
				           }
				           else reportError(std::to_string(t.lineLoc) + ": Identifier '" + t.value + "' was referenced before being declared");
			           }
		           },
		           [&](const std::unique_ptr<NodeExpression>& expr) {
			           extractTypesFromExpression(*expr, types, getID);
		           },
		           [&](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
			           extractTypesFromPrimary(*p.second, types, getID);
		           }
	           }, primary.val);

	if (primary.rhs) {
	}
}


void TraverseAST::reportError(const std::string& message) {
	hasError = true;
	Logger::error(message);
}

token TraverseAST::convertDataToType(const token& t) {
	switch (t.type) {
	case Tokens::number:
		return token(Tokens::_int, t.lineLoc, "int_c", t.value, false);
	case Tokens::string:
		return token(Tokens::_char, t.lineLoc, "string_c", t.value, false);
	case Tokens::charliteral:
		return token(Tokens::_char, t.lineLoc, "char_c", t.value, false);
	default:
		return t;
	}
}
