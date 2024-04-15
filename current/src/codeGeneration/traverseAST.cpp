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
    if(!_table.insertSymbol(formalParamList.lhs->id.value, formalParamList.lhs->type)) {
        reportError(formalParamList.lhs->id.value + " already declared within the scope");
    }
    for (auto& param : formalParamList.rhs) {
        if (param) {
            if (!_table.insertSymbol(param->id.value, param->type)) {
                reportError(param->id.value + "already declared within the scope");
            }
        }
        else Logger::semanticAnalyzer("NULLPTR in formalparamlist");
    }
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
    std::visit(overloaded{
        [this](const token& t) {
            if (t.type == Tokens::ID) {
                if (!_table.checkForSymbol(t.value)) {
                    reportError("Identifier (" + t.value + ") must be declared before being referenced");
                }
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
        analyzeFunctionCall(*primary.rhs.value());
    }
}

void TraverseAST::analyzeFunctionCall(NodeFunctionCall& functionCall) {
    if (hasError) return;
    Logger::semanticAnalyzer("function call");
    const std::vector<token> call = extractTokensFromFunctionCall(functionCall);
    
   // std::cout << "\n" <<call.value << "\n";
    if (functionCall.lhs.has_value()) {
        analyzeActualParameters(*functionCall.lhs.value());
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

std::vector<token> TraverseAST::extractTokensFromFunctionCall(const NodeFunctionCall& functionCall) {
    std::vector<token> t;
    bool loop = false;
    if (!functionCall.lhs.has_value()) return t;
    const auto& actualParameters = *functionCall.lhs.value();
    if (!actualParameters.rhs.empty()) loop = true;
    const NodeExpression& expr = *actualParameters.lhs;
    const NodeRelopExpression& relopExpr = *expr.lhs;
    const NodeSimpleExpression& simpleExpr = *relopExpr.lhs;
    const NodeTerm& term = *simpleExpr.lhs;
    const NodePrimary& primary = *term.lhs;

    if (std::holds_alternative<token>(primary.val)) t.emplace_back(std::get<token>(primary.val));

    return t;
}

//bool TraverseAST::isFunctionDeclared(NodeFunctionCall& functionCall) {
//    if (!functionCall.lhs.has_value()) return false;
//
//	//descending to the primary
//    const auto& actualParameters = *functionCall.lhs.value();
//    const NodeExpression& expr = *actualParameters.lhs;
//    const NodeRelopExpression& relopExpr = *expr.lhs;
//    const NodeSimpleExpression& simpleExpr = *relopExpr.lhs;
//    const NodeTerm& term = *simpleExpr.lhs;
//    const NodePrimary& primary = *term.lhs;
//
//    if(std::holds_alternative<token>(primary.val)) {
//        const token& t = std::get<token>(primary.val);
//        if (t.type == Tokens::ID) return _table.checkForSymbol(t.value);
//    }
//
//    //const NodePrimary primary = functionCall.lhs.value()->lhs->lhs->lhs->lhs->lhs->val;
//    //if(std::holds_alternative<token>(primary.val))
//    //get identifier info
//}



void TraverseAST::reportError(const std::string& message) {
    //hasError = true;
	Logger::error(message);
}
