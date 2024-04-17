//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/13/2024

#include "traverseAST.h"

#include <ranges>

//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

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
    if(!_table.insertSymbol(definition.lhs->id.value, definition.lhs->type)) {
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
    const auto actualSymbol = _table.retrieveSymbol(functionID);
    std::vector<token> parameters;
    parameters.push_back(formalParamList.lhs->type);

    if(!_table.insertSymbol(formalParamList.lhs->id.value, formalParamList.lhs->type)) {
        reportError(formalParamList.lhs->id.value + " already declared within the scope");
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
    if (actualSymbol != nullptr ) actualSymbol->parameters = parameters;
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
    //_table.enterScope();
    for (auto& declaration : compoundStatement.lhs) {
        if(!_table.insertSymbol(declaration->id.value, declaration->type)) {
            reportError(declaration->id.value + " already declared within the scope");
        }
    }
    for (auto& statement : compoundStatement.rhs) analyzeStatement(*statement);
    //_table.exitScope();
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

void TraverseAST::analyzeReturnStatement(NodeReturnStatement& returnStatement) {
    if (hasError) return;
    Logger::semanticAnalyzer("return statement");
    if (returnStatement.lhs.has_value()) {
        analyzeExpression(*returnStatement.lhs.value());
    }
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
    analyzeRelopExpression(*expression.lhs);
    for (auto& rhsExpr : expression.rhs | std::views::values) {
        analyzeRelopExpression(*rhsExpr);
    }
}

void TraverseAST::analyzeRelopExpression(NodeRelopExpression& relopExpression) {
    if (hasError) return;
    Logger::semanticAnalyzer("relop expression");
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
            if (std::holds_alternative<token>(temp) && std::get<token>(temp).value == "0"){
                reportError("cannot divide by 0");
            }
        }
        analyzePrimary(*rhsPrimary);
    }
}

void TraverseAST::analyzePrimary(NodePrimary& primary) {
    if (hasError) return;
    std::string lastUsedID;
    std::visit(overloaded{
        [this, &lastUsedID](token &t) {
            if (t.type == Tokens::ID) {
                if (!_table.checkForSymbol(t.value)) {
                    reportError("Identifier (" + t.value + ") must be declared before being referenced");
                }
                lastUsedID = t.value;
            }
        },
        [this](const std::unique_ptr<NodeExpression>& expr) {
            Logger::semanticAnalyzer("nested expression:");
            analyzeExpression(*expr);
        },
        [this](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
            Logger::semanticAnalyzer("Prefix op: " + p.first.value);
            analyzePrimary(*p.second);
			}
        }, primary.val);

    if (primary.rhs) {
        Logger::semanticAnalyzer("function call:");
        
        analyzeFunctionCall(lastUsedID, *primary.rhs.value());
    }
}

void TraverseAST::analyzeFunctionCall(const std::string& call, const NodeFunctionCall& args) {
    if (hasError || call.empty()) return;
    Logger::semanticAnalyzer("function call");
    std::vector<token> arguments;
    std::vector<token> declared;
    const auto functionDef = _table.retrieveSymbol(call);
    if(functionDef->parameters.has_value()) declared = functionDef->parameters.value();

	extractTypesFromFunctionCall(args, arguments);
    const int argumentSize = arguments.size();
    const int declareSize = declared.size();

	if (arguments.empty() && declared.empty()) return;
    if (declared.empty() && !arguments.empty()) {
        reportError("Function " + call + " expects no arguments, but received " + std::to_string(argumentSize));
        return;
    }

    if (functionDef->parameters.has_value() && arguments.empty()) {
        reportError("Function " + call + " expects arguments, but received none");
        return;
    }

    if (declareSize != argumentSize) {
        reportError("Expected " + std::to_string(declareSize) + " arguments, got "
            + std::to_string(argumentSize) + " in function " + call);
        return;
    }
    
    for(int i = 0; i < declareSize; ++i) {
	    if (declared[i].type != arguments[i].type) {
            reportError("Type mismatch for argument " + std::to_string(i + 1) + " in function '" + call +
                "': Expected type " + declared[i].typeString +
                ", got type " + arguments[i].typeString);
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

void TraverseAST::extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types) {
    if (functionCall.lhs) {
        extractTypesFromActualParameters(*functionCall.lhs.value(), types);
    }
}

// Extract types from actual parameters recursively
void TraverseAST::extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types) {
    extractTypesFromExpression(*params.lhs, types);
    for (const auto& expr : params.rhs) {
        extractTypesFromExpression(*expr, types);
    }
}

// Continue with expressions
void TraverseAST::extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types) {
    extractTypesFromRelopExpression(*expr.lhs, types);
    for (const auto& [op, subExpr] : expr.rhs) {
        extractTypesFromRelopExpression(*subExpr, types);
    }
}

// Process relational operator expressions
void TraverseAST::extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr, std::vector<token>& types) {
    extractTypesFromSimpleExpression(*relopExpr.lhs, types);
    for (const auto& [relop, subExpr] : relopExpr.rhs) {
        extractTypesFromSimpleExpression(*subExpr, types);
    }
}

// Process simple expressions
void TraverseAST::extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr, std::vector<token>& types) {
    extractTypesFromTerm(*simpleExpr.lhs, types);
    for (const auto& [addop, term] : simpleExpr.rhs) {
        extractTypesFromTerm(*term, types);
    }
}

// Process terms
void TraverseAST::extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types) {
    extractTypesFromPrimary(*term.lhs, types);
    for (const auto& [mulop, primary] : term.rhs) {
        extractTypesFromPrimary(*primary, types);
    }
}

void TraverseAST::extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types) {
    std::visit(overloaded{
        [&](const token& t) {
           auto const s =  _table.retrieveSymbol(t.value);
            if (s != nullptr) types.push_back(s->type);
            else reportError("Identifier (" + t.value + ") must be declared before being referenced");
        },
        [&](const std::unique_ptr<NodeExpression>& expr) {
            extractTypesFromExpression(*expr, types);
        },
        [&](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
            extractTypesFromPrimary(*p.second, types);
        }
        }, primary.val);

    if (primary.rhs) {
        // Pseudocode: You'd need to do a symbol table lookup here to get the function return type.
        // Example: types.push_back(_table.getFunctionReturnType(primary.rhs.value()->...));
    }
}


void TraverseAST::reportError(const std::string& message) {
    //hasError = true;
	Logger::error(message);
}
