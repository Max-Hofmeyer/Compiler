//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/19/2024

#include "jasminGenerator.h"


//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

//todo list
	// Finish read
	// Finish || in term
	// Function call

void GenerateJasminCode::generateCode() {
	if (_program == nullptr) return;
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
		<< ".end method\n\n"
		<< ".method public static main([Ljava/lang/String;)V\n";
	out(".limit locals " + std::to_string(_table._index + 1));
	out(".limit stack " + std::to_string(_table._index + 2));
	os << "\n";

	for (auto& definition : _program->definitions) {
		generateDefinition(*definition);
	}
	os << ".end method\n";
}


void GenerateJasminCode::generateDefinition(const NodeDefinition& definition) {
	Logger::codeGenerator("definition");
	if (definition.rhs.has_value()) {
		generateFunctionDefinition(*definition.rhs.value());
	}
}

//todo: EC 
void GenerateJasminCode::generateFunctionDefinition(const NodeFunctionDefinition& fDefinition) {
	Logger::codeGenerator("function definition");
	generateFunctionHeader(*fDefinition.lhs);
	generateFunctionBody(*fDefinition.rhs);
	os << "\n";
	out("return");
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
	Logger::codeGenerator("expression statement");
	generateExpression(*expressionStatement.exp);
}

void GenerateJasminCode::generateBreakStatement(NodeBreakStatement& breakStatement) {
}

void GenerateJasminCode::generateCompoundStatement(const NodeCompoundStatement& compoundStatement) {
	Logger::codeGenerator("compound statement");
	for (auto& declaration : compoundStatement.lhs) {
		auto* s = _table.lookupSymbol(declaration->id.value);
		if(!s->inUse) {
			out("ldc 0 ; init " + s->id);
			out("istore " + std::to_string(s->index) + "\n");
			s->inUse = true;
		}
	}

	for (auto& statement : compoundStatement.rhs) generateStatement(*statement);
}

void GenerateJasminCode::generateIfStatement(const NodeIfStatement& ifStatement) {
	Logger::codeGenerator("if statement");
	const std::string label = std::to_string(_labelIndex++);
	generateExpression(*ifStatement.lhs);
	os << " Ltrue" << label << "\n";
	out("goto Lfalse" + label);
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
	Logger::codeGenerator("null statement");
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
	out("getstatic java/lang/System.in : Ljava/io/InputStream;");
	out("invokespecial java/util/Scanner/<init> (Ljava/io/InputStream;)V");

	int scannerIndex = _table._index++;
	std::string scanner = "astore " + std::to_string(scannerIndex);
	std::string load = "aload " + std::to_string(scannerIndex);
	out(scanner);

	out(load);
	auto* s = _table.lookupSymbol(readStatement.lhs.value);
	out("invokevirtual java/util/Scanner.nextInt()I");
	out("istore " + std::to_string(s->index) + " ; " + s->id + "\n");

	for (const auto& id : readStatement.rhs) {
		out(load);
		auto* s = _table.lookupSymbol(id.value);
		out("invokevirtual java/util/Scanner.nextInt()I");
		out("istore " + std::to_string(s->index) + " ; " + s->id + "\n");
	}
}

//todo
void GenerateJasminCode::generateWriteStatement(NodeWriteStatement& writeStatement) {
	if (!writeStatement.lhs || writeStatement.lhs == nullptr) return;
	Logger::codeGenerator("write statement");
	out("getstatic java/lang/System.out Ljava/io/PrintStream;");
	std::vector<token> t;
	extractTypesFromActualParameters(*writeStatement.lhs, t, true);

	if (t[0].type == Tokens::_int) {
		auto* s = _table.lookupSymbol(t[0].value);
		out("iload " + std::to_string(s->index));
		out("invokevirtual java/io/PrintStream.println (I)V\n");
	}
	else if (t[0].type == Tokens::string) {
		out("ldc " + t[0].value);
		out("invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n");
	}

}

void GenerateJasminCode::generateNewLineStatement(NodeNewLineStatement& newLineStatement) {
	Logger::codeGenerator("new line statement");
	out("getstatic java/lang/System.out Ljava/io/PrintStream;");
	out("ldc \"\n\"");
	out("invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V");
}

void GenerateJasminCode::generateExpression(NodeExpression& expression) {
	Logger::codeGenerator("expression");
	std::vector<token> id;
	extractTypesFromRelopExpression(*expression.lhs, id, true);
	generateRelopExpression(*expression.lhs);

	for (const auto& rhsExpr : expression.rhs | std::views::values) {
		generateRelopExpression(*rhsExpr);
		auto* s = _table.lookupSymbol(id[0].value);
		out("istore " + std::to_string(s->index) + " ; " + s->id);
		if (CliConfig::verboseEnabled) {
			os << "\t\t; ^ ";
			expression.print(os);
			os << "\n";
		}
		os << "\n";
	}
}

void GenerateJasminCode::generateRelopExpression(NodeRelopExpression& relopExpression) {
	Logger::codeGenerator("Relop Expression");
	generateSimpleExpression(*relopExpression.lhs);
	for (auto& rhsExpr : relopExpression.rhs | std::views::values) {
		generateSimpleExpression(*rhsExpr);
		token t = relopExpression.rhs.front().first;
		if (t.type != Tokens::relop) return;
		if (t.value == "==") os <<"\tif_icmpeq";
		if (t.value == "!=") os << "\tif_icmpne";
		if (t.value == "<") os << "\tif_icmplt";
		if (t.value == "<=") os << "\tif_icmple";
		if (t.value == ">") os << "\tif_icmpgt";
		if (t.value == ">=") os << "\tif_icmpge";
	}
}

//todo
void GenerateJasminCode::generateSimpleExpression(NodeSimpleExpression& simpleExpression) {
	Logger::codeGenerator("simple expression");
	generateTerm(*simpleExpression.lhs);
	for (auto& rhsTerm : simpleExpression.rhs | std::views::values) {
		generateTerm(*rhsTerm);
		token t = simpleExpression.rhs.front().first;
		if (t.type != Tokens::addop) return;
		if (t.value == "+") out("iadd");
		if (t.value == "-") out("isub");
		if (t.value == "||"){
			//const std::string label = std::to_string(_labelIndex++);
			//os << "\tifne ";
			//generateTerm(*rhsTerm);
			//os << "\tifne ";
			//out("dup");
			//out("ifne logic" + label);
			//out("pop");
			//generateTerm(*rhsTerm);
			//out("ifne logic" + label);
			//out("iconst_0");
			//out("goto logic" + label + "end");
			//out("logic" + label + ":");
			//out("iconst_1");
			//out("logic" + label + "end");
		}
	}
}

void GenerateJasminCode::generateTerm(NodeTerm& term) {
	Logger::codeGenerator("term");
	generatePrimary(*term.lhs);
	for (auto& rhsPrimary : term.rhs | std::views::values) {
		generatePrimary(*rhsPrimary);
		token t = term.rhs.front().first;
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
			   if (s->inUse) out("iload " + std::to_string(s->index) + " ; " + t.value);
			   else s->inUse = true;
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
				else if(p.first.type == Tokens::addop && p.first.value == "-") {
					out("ineg");
				}
		   }
		}, primary.val);

	if (primary.rhs) {
		generateFunctionCall(_lastUsedId, *primary.rhs.value());
	}
}

//todo
void GenerateJasminCode::generateFunctionCall(const std::string& call, const NodeFunctionCall& args) {
	Logger::codeGenerator("function call");

	if (args.lhs.has_value()) {
		generateActualParameters(*args.lhs.value());
	}
}

//todo
void GenerateJasminCode::generateActualParameters(NodeActualParameters& params) {
	Logger::codeGenerator("actual parameters");
	generateExpression(*params.lhs);
	for (auto& param : params.rhs) {
		generateExpression(*param);
	}
}


void GenerateJasminCode::extractTypesFromFunctionCall(const NodeFunctionCall& functionCall, std::vector<token>& types,
                                                      const bool getID) {
	if (functionCall.lhs) {
		extractTypesFromActualParameters(*functionCall.lhs.value(), types, getID);
	}
}

void GenerateJasminCode::extractTypesFromActualParameters(const NodeActualParameters& params, std::vector<token>& types,
                                                          const bool getID) {
	extractTypesFromExpression(*params.lhs, types, getID);
	for (const auto& expr : params.rhs) {
		extractTypesFromExpression(*expr, types, getID);
	}
}

void GenerateJasminCode::extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types,
                                                    const bool getID) {
	extractTypesFromRelopExpression(*expr.lhs, types, getID);

	for (const auto& subExpr : expr.rhs | std::views::values) {
		extractTypesFromRelopExpression(*subExpr, types, getID);
	}
}

void GenerateJasminCode::extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr,
                                                         std::vector<token>& types,
                                                         const bool getID) {
	extractTypesFromSimpleExpression(*relopExpr.lhs, types, getID);
	for (const auto& [relop, subExpr] : relopExpr.rhs) {
		extractTypesFromSimpleExpression(*subExpr, types, getID);
	}
}

void GenerateJasminCode::extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr,
                                                          std::vector<token>& types,
                                                          const bool getID) {
	extractTypesFromTerm(*simpleExpr.lhs, types, getID);
	for (const auto& term : simpleExpr.rhs | std::views::values) {
		extractTypesFromTerm(*term, types, getID);
	}
}

void GenerateJasminCode::extractTypesFromTerm(const NodeTerm& term, std::vector<token>& types, const bool getID) {
	extractTypesFromPrimary(*term.lhs, types, getID);
	for (const auto& primary : term.rhs | std::views::values) {
		extractTypesFromPrimary(*primary, types, getID);
	}
}

void GenerateJasminCode::extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types,
                                                 const bool getID) {
	std::visit(overloaded{
		           [&](const token& t) {
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
			           extractTypesFromExpression(*expr, types, getID);
		           },
		           [&](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
			           extractTypesFromPrimary(*p.second, types, getID);
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
