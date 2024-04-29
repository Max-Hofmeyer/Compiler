//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/19/2024

#include "jasminGenerator.h"

//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void GenerateJasminCode::generateCode() {
	if (_program == nullptr || !CliConfig::dumpCode) return;
	if (CliConfig::demo || CliConfig::verboseEnabled) debugStmts = true;
	os.open(CliConfig::jasminFileLocation);
	if (!os.is_open()) {
		Logger::error("Failed to open jasmin file at: " + CliConfig::jasminFileLocation.string());
		return;
	}
	Logger::codeGeneratorStream("Writing to jasmin file: " + CliConfig::jasminFileLocation.string());

	Logger::codeGenerator("Generating ToyCProgram");
	os << "; File:     \t" << CliConfig::jasminFileLocation.string() << "\n"
		<< "; Author(s):\tMax Hofmeyer, Ahmed Malik, 19 April 2024\n"
		<< "; -------------------------------------------------------------------------\n\n"
		<< ".class public " + CliConfig::className + "\n"
		<< ".super java/lang/Object\n\n"
		<< ".method public <init>()V\n\taload_0\n\tinvokenonvirtual java/lang/Object/<init>()V\n\treturn\n"
		<< ".end method\n\n";
	os << "\n";

	for (auto& definition : _program->definitions) {
		generateDefinition(*definition);
		rawOut(".end method");
		os << "\n\n";
	}
}

void GenerateJasminCode::generateDefinition(const NodeDefinition& definition) {
	Logger::codeGenerator("definition");
	if (definition.lhs->id.value == "main") {
		rawOut(".method public static main([Ljava/lang/String;)V\n");
		inMain = true;
	}
	else {
		auto s = _table.lookupSymbol(definition.lhs->id.value);
		rawOut(".method public static " + definition.lhs->id.value + "(");
		if (s->parameters.has_value()) {
			for (auto params : s->parameters.value()) {
				rawOut("I");
			}
		}
		rawOut(")I\n");
	}
	out(".limit locals " + std::to_string(_table._index * 5));
	out(".limit stack " + std::to_string(_table._index * 5));
	os << "\n";
	if (definition.rhs.has_value()) {
		generateFunctionDefinition(*definition.rhs.value());
	}
}

void GenerateJasminCode::generateFunctionDefinition(const NodeFunctionDefinition& fDefinition) {
	Logger::codeGenerator("function definition");
	generateFunctionHeader(*fDefinition.lhs);
	generateFunctionBody(*fDefinition.rhs);
	os << "\n";
	if (inMain) {
		out("return");
		inMain = false;
	}
	else out("ireturn");
	os << "\n";
}

void GenerateJasminCode::generateFunctionHeader(const NodeFunctionHeader& fHeader) {
	Logger::codeGenerator("function header");
	if (fHeader.lhs.has_value() && fHeader.lhs != nullptr) {
		auto& temp = fHeader.lhs.value();
		generateFormalParamList(*temp);
	}
}

void GenerateJasminCode::generateFunctionBody(const NodeFunctionBody& fBody) {
	Logger::codeGenerator("function body");
	generateCompoundStatement(*fBody.lhs);
}

void GenerateJasminCode::generateFormalParamList(const NodeFormalParamList& formalParamList) {
	auto* s = _table.lookupSymbol(formalParamList.lhs->id.value);
	if (s == nullptr) return;

	out("iload " + std::to_string(s->index) + " ; " + s->id);
	for (auto& var : formalParamList.rhs) {
		auto* sym = _table.lookupSymbol(var->id.value);
		if (sym == nullptr) return;
		out("iload " + std::to_string(sym->index) + " ; " + sym->id);
	}
	os << "\n";
}

void GenerateJasminCode::generateStatement(NodeStatement& statement) {
	std::visit(overloaded{
		           [this](const std::unique_ptr<NodeExpressionStatement>& arg) { generateExpressionStatement(*arg); },
		           [this](const std::unique_ptr<NodeBreakStatement>& arg) { generateBreakStatement(*arg); },
		           [this](const std::unique_ptr<NodeCompoundStatement>& arg) { generateCompoundStatement(*arg); },
		           [this](const std::unique_ptr<NodeIfStatement>& arg) { generateIfStatement(*arg); },
		           [this](const std::unique_ptr<NodeNullStatement>& arg) { generateNullStatement(*arg); },
		           [this](const std::unique_ptr<NodeReturnStatement>& arg) { generateReturnStatement(*arg); },
		           [this](const std::unique_ptr<NodeWhileStatement>& arg) { generateWhileStatement(*arg); },
		           [this](const std::unique_ptr<NodeReadStatement>& arg) { generateReadStatement(*arg); },
		           [this](const std::unique_ptr<NodeWriteStatement>& arg) { generateWriteStatement(*arg); },
		           [this](const std::unique_ptr<NodeNewLineStatement>& arg) { generateNewLineStatement(*arg); }
	           }, statement.val);
}

void GenerateJasminCode::generateExpressionStatement(NodeExpressionStatement& expressionStatement) {
	generateExpression(*expressionStatement.exp);
}

void GenerateJasminCode::generateBreakStatement(NodeBreakStatement& breakStatement) {
}

void GenerateJasminCode::generateCompoundStatement(const NodeCompoundStatement& compoundStatement) {
	Logger::codeGenerator("compound statement");
	for (auto& declaration : compoundStatement.lhs) {
		auto* s = _table.lookupSymbol(declaration->id.value);
		if (!s->inUse) {
			out("ldc 0 ; init " + s->id);
			out("istore " + std::to_string(s->index) + "\n");
			s->inUse = true;
		}
	}

	for (auto& statement : compoundStatement.rhs) {
		generateStatement(*statement);
	}
}

void GenerateJasminCode::generateIfStatement(const NodeIfStatement& ifStatement) {
	Logger::codeGenerator("if statement");

	//if an expression has more than one conditional, special handling is needed
	std::vector<token> toks;
	extractTypesFromExpression(*ifStatement.lhs, toks, false, true);
	const std::string label = std::to_string(_labelIndex++);
	if (toks.size() > 1) generateComplexExpression(*ifStatement.lhs);
	else {
		generateExpression(*ifStatement.lhs);
		os << " Ltrue" << label << "\n";
	}

	if (ifStatement.rhs.has_value() && ifStatement.rhs != nullptr) {
		out("goto Lfalse" + label);
	}
	else out("goto Lend" + label);
	os << "\n";

	out("Ltrue" + label + ":");
	generateStatement(*ifStatement.mhs);
	out("goto Lend" + label);
	if (ifStatement.rhs.has_value() && ifStatement.rhs != nullptr) {
		os << "\n";
		out("Lfalse" + label + ":");
		generateStatement(*ifStatement.rhs.value());
	}
	out("Lend" + label + ":");
}

void GenerateJasminCode::generateNullStatement(NodeNullStatement& nullStatement) {
}

void GenerateJasminCode::generateReturnStatement(const NodeReturnStatement& returnStatement) {
	Logger::codeGenerator("return statement");
	if (!returnStatement.lhs.has_value()) return;

	std::vector<token> returnT;

	generateExpression(*returnStatement.lhs.value());
}

void GenerateJasminCode::generateWhileStatement(NodeWhileStatement& whileStatement) {
	Logger::codeGenerator("while statement");
	const std::string label = std::to_string(_labelIndex++);
	out("goto loopCheck" + label);
	os << "\nloop" + label + ":\n";

	generateStatement(*whileStatement.rhs);
	os << "\tgoto loopCheck" + label + "\n";

	os << "\nloopCheck" + label + ":\n";
	generateExpression(*whileStatement.lhs);
	os << " loop" + label + "\n";
}

void GenerateJasminCode::generateReadStatement(NodeReadStatement& readStatement) {
	Logger::codeGenerator("read statement");
	out("\n\tnew java/util/Scanner");
	out("dup");
	out("getstatic java/lang/System/in Ljava/io/InputStream;");
	out("invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V");

	const int scannerIndex = _table._index++;
	const std::string scanner = "astore " + std::to_string(scannerIndex);
	const std::string load = "aload " + std::to_string(scannerIndex);
	out(scanner);

	out(load);
	const auto* s = _table.lookupSymbol(readStatement.lhs.value);
	out("invokevirtual java/util/Scanner.nextInt()I");
	out("istore " + std::to_string(s->index) + " ; " + s->id + "\n");

	for (const auto& id : readStatement.rhs) {
		out(load);
		auto* s = _table.lookupSymbol(id.value);
		out("invokevirtual java/util/Scanner.nextInt()I");
		out("istore " + std::to_string(s->index) + " ; " + s->id + "\n");
	}
}

void GenerateJasminCode::generateWriteStatement(NodeWriteStatement& writeStatement) {
	if (!writeStatement.lhs || writeStatement.lhs == nullptr) return;
	bool isNum = false;
	Logger::codeGenerator("write statement");
	os << "\n";
	out("getstatic java/lang/System.out Ljava/io/PrintStream;");
	std::vector<token> t;

	extractTypesFromExpression(*writeStatement.lhs->lhs, t);
	for (auto& exprRhs : writeStatement.lhs->rhs) {
		extractTypesFromExpression(*exprRhs, t);
	}

	for (auto tok : t) {
		if (tok.type != Tokens::string) isNum = true;
	}
	generateActualParameters(*writeStatement.lhs);
	if (isNum) out("invokestatic java/lang/String/valueOf(I)Ljava/lang/String;");
	out("invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V");
}

void GenerateJasminCode::generateNewLineStatement(NodeNewLineStatement& newLineStatement) {
	Logger::codeGenerator("new line statement");
	os << "\n";
	out("getstatic java/lang/System.out Ljava/io/PrintStream;");
	out("ldc \"\n\t\"");
	out("invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V");
}

void GenerateJasminCode::generateExpression(NodeExpression& expression) {
	Logger::codeGenerator("expression");
	std::vector<token> id;
	int pos = 0;
	extractTypesFromRelopExpression(*expression.lhs, id, true);
	for (auto& ids : expression.rhs) {
		extractTypesFromRelopExpression(*ids.second, id, true);
	}

	generateRelopExpression(*expression.lhs);
	for (const auto& rhsExpr : expression.rhs) {
		auto* s = _table.lookupSymbol(id[pos++].value);
		generateRelopExpression(*rhsExpr.second);
		out("istore " + std::to_string(s->index) + " ; " + s->id);
		os << "\n";
	}
}

void GenerateJasminCode::generateComplexExpression(NodeExpression& expression) {
	Logger::codeGenerator("if expression");
	const std::string label = std::to_string(_labelIndex - 1);
	generateSimpleExpression(*expression.lhs->lhs);

	for (auto& expr : expression.lhs->rhs) {
		token t = expr.first;
		generateSimpleExpression(*expr.second);
		if (t.value == "==") rawOut("\tif_icmpeq");
		if (t.value == "!=") rawOut("\tif_icmpne");
		if (t.value == "<") rawOut("\tif_icmplt");
		if (t.value == "<=") rawOut("\tif_icmple");
		if (t.value == ">") rawOut("\tif_icmpgt");
		if (t.value == ">=") rawOut("\tif_icmpge");
		os << " Ltrue" << label << "\n";
		if (!orCatch.empty()) {
			out(orCatch);
			orCatch.clear();
		}
	}
}

void GenerateJasminCode::generateRelopExpression(NodeRelopExpression& relopExpression) {
	Logger::codeGenerator("Relop Expression");
	generateSimpleExpression(*relopExpression.lhs);
	int test = 0;
	for (const auto& rhsExpr : relopExpression.rhs) {
		token t = rhsExpr.first;
		generateSimpleExpression(*rhsExpr.second);
		if (t.type != Tokens::relop) return;
		if (t.value == "==") rawOut("\tif_icmpeq");
		if (t.value == "!=") rawOut("\tif_icmpne");
		if (t.value == "<") rawOut("\tif_icmplt");
		if (t.value == "<=") rawOut("\tif_icmple");
		if (t.value == ">") rawOut("\tif_icmpgt");
		if (t.value == ">=") rawOut("\tif_icmpge");
	}
}

void GenerateJasminCode::generateSimpleExpression(NodeSimpleExpression& simpleExpression) {
	Logger::codeGenerator("simple expression");
	generateTerm(*simpleExpression.lhs);
	for (auto& rhsTerm : simpleExpression.rhs) {
		token t = rhsTerm.first;
		if (t.value == "||") isOR = true;
		generateTerm(*rhsTerm.second);
		isOR = false;
		if (t.type != Tokens::addop) return;
		if (t.value == "+") out("iadd");
		if (t.value == "-") out("isub");
	}
}

void GenerateJasminCode::generateTerm(NodeTerm& term) {
	Logger::codeGenerator("term");
	generatePrimary(*term.lhs);
	for (auto& rhsPrimary : term.rhs) {
		generatePrimary(*rhsPrimary.second);
		token t = rhsPrimary.first;
		if (t.type != Tokens::mulop) return;
		if (t.value == "*") out("imul");
		if (t.value == "%") out("irem");
		if (t.value == "/") out("idiv");
		if (t.value == "&") out("iand");
	}
}

void GenerateJasminCode::generatePrimary(NodePrimary& primary) {
	Logger::codeGenerator("primary");
	std::visit(overloaded{
		           [this](token& t) {
			           if (t.type == Tokens::ID) {
				           auto* s = _table.lookupSymbol(t.value);
				           //stops unnecessary iloads
				           if (s == nullptr) return;
				           if (s->id == _lastUsedId) return;
				           //since output needs to be deferred for || store it 
				           if (isOR) orCatch = "iload " + std::to_string(s->index);
				           if (s->inUse && !isOR) out("iload " + std::to_string(s->index) + " ; " + t.value);
				           else s->inUse = true;
				           isOR = false;
			           }
			           _lastUsedId = t.value;
			           if (t.type == Tokens::number || t.type == Tokens::string) {
				           out("ldc " + t.value);
			           }
		           },
		           [this](const std::unique_ptr<NodeExpression>& expr) {
			           generateExpression(*expr);
		           },
		           [this](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
			           generatePrimary(*p.second);
			           //since jvm doesnt have not, just xor -1
			           if (p.first.type == Tokens::_not) {
				           out("iconst_m1");
				           out("ixor");
			           }
			           else if (p.first.type == Tokens::addop && p.first.value == "-") {
				           out("ineg");
			           }
		           }
	           }, primary.val);

	if (primary.rhs) {
		generateFunctionCall(_lastUsedId, *primary.rhs.value());
	}
}

void GenerateJasminCode::generateFunctionCall(const std::string& call, const NodeFunctionCall& args) {
	Logger::codeGenerator("function call");
	std::string funcCall = "invokestatic " + CliConfig::className + "/" + call + "(";

	const auto s = _table.lookupSymbol(call);
	if (s->parameters.has_value()) {
		for (auto arg : s->parameters.value()) funcCall.append("I");
	}
	funcCall.append(")I");
	if (args.lhs.has_value()) generateActualParameters(*args.lhs.value());
	out(funcCall);
}

void GenerateJasminCode::generateActualParameters(NodeActualParameters& params) {
	Logger::codeGenerator("actual parameters");
	generateExpression(*params.lhs);
	for (auto& param : params.rhs) {
		generateExpression(*param);
	}
}

void GenerateJasminCode::extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types,
                                                      const bool getID, const bool getAll) {
	if (functionCall.lhs) {
		extractTypesFromActualParameters(*functionCall.lhs.value(), types, getID, getAll);
	}
}

void GenerateJasminCode::extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types,
                                                          const bool getID, const bool getAll) {
	extractTypesFromExpression(*params.lhs, types, getID, getAll);
	for (const auto& expr : params.rhs) {
		extractTypesFromExpression(*expr, types, getID, getAll);
	}
}

void GenerateJasminCode::extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types,
                                                    const bool getID, const bool getAll) {
	extractTypesFromRelopExpression(*expr.lhs, types, getID, getAll);

	for (const auto& subExpr : expr.rhs | std::views::values) {
		if (getAll) types.emplace_back(expr.rhs.front().first);
		extractTypesFromRelopExpression(*subExpr, types, getID, getAll);
	}
}

void GenerateJasminCode::extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr,
                                                         std::vector<token>& types,
                                                         const bool getID, const bool getAll) {
	extractTypesFromSimpleExpression(*relopExpr.lhs, types, getID, getAll);
	for (const auto& subExpr : relopExpr.rhs | std::views::values) {
		if (getAll) types.emplace_back(relopExpr.rhs.front().first);
		extractTypesFromSimpleExpression(*subExpr, types, getID, getAll);
	}
}

void GenerateJasminCode::extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr,
                                                          std::vector<token>& types,
                                                          const bool getID, const bool getAll) {
	extractTypesFromTerm(*simpleExpr.lhs, types, getID, getAll);
	for (const auto& term : simpleExpr.rhs | std::views::values) {
		if (getAll) types.emplace_back(simpleExpr.rhs.front().first);
		extractTypesFromTerm(*term, types, getID, getAll);
	}
}

void GenerateJasminCode::extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types, const bool getID,
                                              const bool getAll) {
	extractTypesFromPrimary(*term.lhs, types, getID, getAll);
	for (const auto& primary : term.rhs | std::views::values) {
		if (getAll) types.emplace_back(term.rhs.front().first);
		extractTypesFromPrimary(*primary, types, getID, getAll);
	}
}

void GenerateJasminCode::extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types,
                                                 const bool getID, const bool getAll) {
	std::visit(overloaded{
		           [&](const token& t) {
			           if (getAll) return;
			           if (t.type == Tokens::_int || t.type == Tokens::_char || t.type == Tokens::string) {
				           types.push_back(t);
			           }

			           if (t.type == Tokens::number || t.type == Tokens::charliteral) {
				           if (!getID) types.push_back(convertDataToType(t));
			           }
			           if (t.type == Tokens::ID) {
				           const auto s = _table.lookupSymbol(t.value);
				           if (s != nullptr) {
					           if (!getID) types.push_back(s->type);
					           else types.push_back(convertSymbolToToken(*s));
				           }
			           }
		           },
		           [&](const std::unique_ptr<NodeExpression>& expr) {
			           extractTypesFromExpression(*expr, types, getID, getAll);
		           },
		           [&](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
			           extractTypesFromPrimary(*p.second, types, getID, getAll);
		           }
	           }, primary.val);

	if (primary.rhs) {
	}
}

token GenerateJasminCode::convertDataToType(const token& t) {
	switch (t.type) {
	case Tokens::number:
		return token(Tokens::_int, t.lineLoc, "int_c", t.value, false);
	case Tokens::charliteral:
		return token(Tokens::_char, t.lineLoc, "char_c", t.value, false);
	default:
		return t;
	}
}

token GenerateJasminCode::convertSymbolToToken(const Symbol& s) {
	return token(s.type.type, s.type.lineLoc, s.type.typeString, s.id, false);
}
