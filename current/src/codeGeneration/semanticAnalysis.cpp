//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/13/2024

#include "semanticAnalysis.h"

//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void AnalyseSemantics::traverseTree() {
	if (_program == nullptr) return;
	bool mainInProg = false;
	
	for (auto& definition : _program->definitions) {
		analyzeDefinition(*definition);
		if (definition->lhs->id.value == "main") mainInProg = true;
	}
	if (!mainInProg && !hasError) reportError("No main declared");
}


void AnalyseSemantics::analyzeDefinition(const NodeDefinition& definition) {
	if (hasError) return;
	Logger::semanticAnalyzer("definition");
	if (!_table.insertSymbol(definition.lhs->id.value, definition.lhs->type)) {
		reportError(definition.lhs->id.value + " already declared within the scope");
	}
	if (definition.rhs.has_value()) {
		analyzeFunctionDefinition(*definition.rhs.value());
	}
}

void AnalyseSemantics::analyzeFunctionDefinition(const NodeFunctionDefinition& fDefinition) {
	if (hasError) return;
	Logger::semanticAnalyzer("function definition");
	_table.enterScope();
	analyzeFunctionHeader(*fDefinition.lhs);
	analyzeFunctionBody(*fDefinition.rhs);
	_table.exitScope();
}

void AnalyseSemantics::analyzeFunctionHeader(const NodeFunctionHeader& fHeader) {
	if (hasError) return;
	Logger::semanticAnalyzer("function header");
	if (fHeader.lhs.has_value() && fHeader.lhs != nullptr) {
		auto& temp = fHeader.lhs.value();
		analyzeFormalParamList(*temp);
	}
}

void AnalyseSemantics::analyzeFunctionBody(const NodeFunctionBody& fBody) {
	if (hasError) return;
	Logger::semanticAnalyzer("function body");
	analyzeCompoundStatement(*fBody.lhs);
}

void AnalyseSemantics::analyzeFormalParamList(const NodeFormalParamList& formalParamList) {
	if (hasError) return;
	Logger::semanticAnalyzer("formal parameters ");
	const std::string functionID = _table.getLastSymbol();
	const auto actualSymbol = _table.lookupSymbol(functionID);
	std::vector<token> parameters;
	parameters.push_back(formalParamList.lhs->type);

	if (!_table.insertSymbol(formalParamList.lhs->id.value, formalParamList.lhs->type, 0, {} , true)) {
		reportError(std::to_string(formalParamList.lhs->id.lineLoc) + ": '" + formalParamList.lhs->id.value +
			"' already declared within the scope");
	}
	for (auto& param : formalParamList.rhs) {
		if (param && param != nullptr) {
			parameters.push_back(param->type);
			if (!_table.insertSymbol(param->id.value, param->type)) {
				reportError(param->id.value + "already declared within the scope");
				return;
			}
		}
		else Logger::semanticAnalyzer("NULLPTR in formalparamlist");
	}
	if (actualSymbol != nullptr) actualSymbol->parameters = parameters;
}

void AnalyseSemantics::analyzeStatement(NodeStatement& statement) {
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

void AnalyseSemantics::analyzeExpressionStatement(NodeExpressionStatement& expressionStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("expression statement");
	analyzeExpression(*expressionStatement.exp);
}

void AnalyseSemantics::analyzeBreakStatement(NodeBreakStatement& breakStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("break statement");
}

void AnalyseSemantics::analyzeCompoundStatement(const NodeCompoundStatement& compoundStatement) {
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
			return;
		}
	}
	for (auto& statement : compoundStatement.rhs) analyzeStatement(*statement);
}

void AnalyseSemantics::analyzeIfStatement(NodeIfStatement& ifStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("if statement");
	_table.enterScope();
	analyzeExpression(*ifStatement.lhs);
	analyzeStatement(*ifStatement.mhs);
	if (ifStatement.rhs.has_value() && ifStatement.rhs != nullptr) {
		analyzeStatement(*ifStatement.rhs.value());
	}
	_table.exitScope();
}

void AnalyseSemantics::analyzeNullStatement(NodeNullStatement& nullStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("null statement");
}

void AnalyseSemantics::analyzeReturnStatement(const NodeReturnStatement& returnStatement) {
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
		return;
	}

	analyzeExpression(*returnStatement.lhs.value());
}

void AnalyseSemantics::analyzeWhileStatement(NodeWhileStatement& whileStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("while statement");
	_table.enterScope();
	analyzeExpression(*whileStatement.lhs);
	analyzeStatement(*whileStatement.rhs);
	_table.exitScope();
}

void AnalyseSemantics::analyzeReadStatement(NodeReadStatement& readStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("read statement");
	if (!_table.checkForSymbol(readStatement.lhs.value)) {
		std::cout << "\n\t";
		readStatement.print(std::cout);
		std::cout << "\n";
		reportError( " ^ : Identifier '" + readStatement.lhs.value + "' was referenced before being declared");
		return;
	}
	for (const auto& id : readStatement.rhs) {
		if (!_table.checkForSymbol(id.value)) {
			std::cout << "\n\t";
			readStatement.print(std::cout);
			std::cout << "\n";
			reportError(" ^ : Identifier '" + id.value + "' was referenced before being declared");
			return;
		}
		Logger::semanticAnalyzer("reading " + id.value);
	}
}

void AnalyseSemantics::analyzeWriteStatement(NodeWriteStatement& writeStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("write statement");
	analyzeActualParameters(*writeStatement.lhs);
}

void AnalyseSemantics::analyzeNewLineStatement(NodeNewLineStatement& newLineStatement) {
	if (hasError) return;
	Logger::semanticAnalyzer("new line statement");
}

void AnalyseSemantics::analyzeExpression(NodeExpression& expression) {
	if (hasError) return;
	Logger::semanticAnalyzer("expression");
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
				reportError(" ^ : Type mismatch for expression");
				return;
			}
		}
	}

	analyzeRelopExpression(*expression.lhs);
	for (auto& rhsExpr : expression.rhs | std::views::values) {
		analyzeRelopExpression(*rhsExpr);
	}
}

void AnalyseSemantics::analyzeRelopExpression(NodeRelopExpression& relopExpression) {
	if (hasError) return;
	analyzeSimpleExpression(*relopExpression.lhs);
	for (auto& [relop, rhsExpr] : relopExpression.rhs) {
		analyzeSimpleExpression(*rhsExpr);
	}
}

void AnalyseSemantics::analyzeSimpleExpression(NodeSimpleExpression& simpleExpression) {
	if (hasError) return;
	Logger::semanticAnalyzer("simple expression");
	analyzeTerm(*simpleExpression.lhs);
	for (auto& [addop, rhsTerm] : simpleExpression.rhs) {
		analyzeTerm(*rhsTerm);
	}
}

void AnalyseSemantics::analyzeTerm(NodeTerm& term) {
	if (hasError) return;
	Logger::semanticAnalyzer("term");
	analyzePrimary(*term.lhs);
	for (auto& [op, rhsPrimary] : term.rhs) {
		if (op.value == "/") {
			const auto& temp = rhsPrimary->val;
			if (std::holds_alternative<token>(temp) && std::get<token>(temp).value == "0") {
				reportError(std::to_string(op.lineLoc) + ": Invalid operation, cannot divide by 0");
				return;
			}
		}
		analyzePrimary(*rhsPrimary);
	}
}

void AnalyseSemantics::analyzePrimary(NodePrimary& primary) {
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

void AnalyseSemantics::analyzeFunctionCall(const std::string& call, const NodeFunctionCall& args) {
	if (hasError || call.empty()) return;
	Logger::semanticAnalyzer("function call");
	std::vector<token> arguments;
	std::vector<token> declared;
	const auto functionDef = _table.lookupSymbol(call);
	if (functionDef->parameters.has_value()) declared = functionDef->parameters.value();

	extractTypesFromFunctionCall(args, arguments, false);
	const int argumentSize = arguments.size();
	const int declareSize = declared.size();

	if (arguments.empty() && declared.empty()) return;
	if (declared.empty() && !arguments.empty()) {
		std::cout << "\n\t";
		args.print(std::cout);
		std::cout << "\n";
		reportError(
			" ^ : Function '" + call + "'" +
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
			" ^ : Expected " + std::to_string(declareSize) + " arguments, got "
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

void AnalyseSemantics::analyzeActualParameters(NodeActualParameters& params) {
	if (hasError) return;
	Logger::semanticAnalyzer("actual parameters");
	analyzeExpression(*params.lhs);
	for (auto& param : params.rhs) {
		analyzeExpression(*param);
	}
}

void AnalyseSemantics::extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types,
                                               const bool getID) {
	if (functionCall.lhs) {
		extractTypesFromActualParameters(*functionCall.lhs.value(), types, getID);
	}
}

void AnalyseSemantics::extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types,
                                                   const bool getID) {
	extractTypesFromExpression(*params.lhs, types, getID);
	for (const auto& expr : params.rhs) {
		extractTypesFromExpression(*expr, types, getID);
	}
}

void AnalyseSemantics::extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types, const bool getID) {
	extractTypesFromRelopExpression(*expr.lhs, types, getID);

	for (const auto& subExpr : expr.rhs | std::views::values) {
		extractTypesFromRelopExpression(*subExpr, types, getID);
	}
}

void AnalyseSemantics::extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr, std::vector<token>& types,
                                                  const bool getID) {
	extractTypesFromSimpleExpression(*relopExpr.lhs, types, getID);
	for (const auto& subExpr : relopExpr.rhs | std::views::values) {
		extractTypesFromSimpleExpression(*subExpr, types, getID);
	}

}

void AnalyseSemantics::extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr, std::vector<token>& types,
                                                   const bool getID) {
	extractTypesFromTerm(*simpleExpr.lhs, types, getID);
	for (const auto& term : simpleExpr.rhs | std::views::values) {
		extractTypesFromTerm(*term, types, getID);
	}
}

void AnalyseSemantics::extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types, const bool getID) {
	extractTypesFromPrimary(*term.lhs, types, getID);
	for (const auto& primary : term.rhs | std::views::values) {
		extractTypesFromPrimary(*primary, types, getID);
	}
}

void AnalyseSemantics::extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types, const bool getID) {
	std::visit(overloaded{
		           [&](const token& t) {
		           	   if (t.type == Tokens::_int || t.type == Tokens::_char || t.type == Tokens::string) {
						   types.push_back(t);
					   }

			           if (t.type == Tokens::string || t.type == Tokens::number || t.type == Tokens::charliteral) {
				           if (!getID) types.push_back(convertDataToType(t));
			           }
			           if (t.type == Tokens::ID) {
				           if (const auto s = _table.lookupSymbol(t.value); s != nullptr) {
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

token AnalyseSemantics::convertDataToType(const token& t) {
	switch (t.type) {
	case Tokens::number:
		return token(Tokens::_int, t.lineLoc, "int_c", t.value, false);
	case Tokens::charliteral:
		return token(Tokens::_char, t.lineLoc, "char_c", t.value, false);
	default:
		return t;
	}
}

void AnalyseSemantics::reportError(const std::string& message) {
	if(!hasError) Logger::error(message);
	hasError = true;
}

