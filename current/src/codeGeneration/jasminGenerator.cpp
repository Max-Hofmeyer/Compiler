//Max Hofmeyer & Ahmed Malik | EGRE 591 | 04/19/2024

#include "jasminGenerator.h"


//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

void GenerateJasminCode::generateCode() {
	if (_program == nullptr) return;
	os.open(CliConfig::jasminFileLocation);
	if(!os.is_open()) {
		Logger::error("Failed to open jasmin output file");
		return;
	}
	Logger::codeGenerator("Opening jasmin file at: " + CliConfig::jasminFileLocation.string());

	Logger::codeGenerator("Generating ToyCProgram");
	os  << "; File:     \t" << CliConfig::jasminFileLocation.string() << "\n"
		<< "; Author(s):\tMax Hofmeyer, Ahmed Malik, 19 April 2024\n"
		<< "; -------------------------------------------------------------------------\n\n"
		<< ".class public " + CliConfig::className + "\n"
		<< ".super java/lang/Object\n\n"
		<< ".method public <init>()V\n\taload_0\n\tinvokenonvirtual java/lang/Object/<init>()V\n\treturn\n"
		<< ".end method\n\n"
		<< ".method public static main([Ljava/lang/String;)V\n";

	for (auto& definition : _program->definitions) {
		generateDefinition(*definition);
	}
	os << "\n.end method\n";
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

	for (auto& statement : compoundStatement.rhs) generateStatement(*statement);
}

void GenerateJasminCode::generateIfStatement(NodeIfStatement& ifStatement) {
	
	Logger::codeGenerator("if statement");
	if (ifStatement.lhs != nullptr) generateExpression(*ifStatement.lhs);
	if (ifStatement.mhs != nullptr) generateStatement(*ifStatement.mhs);
	if (ifStatement.rhs.has_value() && ifStatement.rhs != nullptr) {
		generateStatement(*ifStatement.rhs.value());
	}
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
	generateExpression(*whileStatement.lhs);
	generateStatement(*whileStatement.rhs);
}

void GenerateJasminCode::generateReadStatement(NodeReadStatement& readStatement) {
	Logger::codeGenerator("read statement");
	for (const auto& id : readStatement.rhs) {
	}
}

void GenerateJasminCode::generateWriteStatement(NodeWriteStatement& writeStatement) {
	if (!writeStatement.lhs || writeStatement.lhs != nullptr) return;
	Logger::codeGenerator("write statement");
	out("getstatic java/lang/System.out Ljava/io/PrintStream;");
	std::vector<token> t;
	extractTypesFromActualParameters(*writeStatement.lhs, t);
	if(t.size())
	
	//ldc "generateActualParameters"
	   // if (writeStatement.lhs) {
    //    std::visit(overloaded{
    //        [this](const std::unique_ptr<NodeExpression>& expr) {
    //            // Assuming generateExpression handles loading expressions onto the stack
    //            generateExpression(*expr);
    //            // Determine type of expression and choose correct println method
    //            if (false) {
    //                out("invokevirtual java/io/PrintStream.println(I)V");
    //            } else {
				//	expr->print(os);
    //                out("\ninvokevirtual java/io/PrintStream.println(Ljava/lang/String;)V");
    //            }
    //        },
    //        [this](const std::string& str) {
    //            // Load a constant string directly
    //            out("ldc \"" + str + "\"");
    //            out("invokevirtual java/io/PrintStream.println(Ljava/lang/String;)V");
    //        }
    //    }, //writeStatement.lhs->);
    //}
	


	out("invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V");
}

void GenerateJasminCode::generateNewLineStatement(NodeNewLineStatement& newLineStatement) {
	
	Logger::codeGenerator("new line statement");
}

void GenerateJasminCode::generateExpression(NodeExpression& expression) {
	Logger::codeGenerator("expression");

	generateRelopExpression(*expression.lhs);
	for (const auto& rhsExpr : expression.rhs | std::views::values) {
		generateRelopExpression(*rhsExpr);
	}
}

void GenerateJasminCode::generateRelopExpression(NodeRelopExpression& relopExpression) {
	Logger::codeGenerator("Relop Expression");
	
	generateSimpleExpression(*relopExpression.lhs);
	for (auto& rhsExpr : relopExpression.rhs | std::views::values) {
		generateSimpleExpression(*rhsExpr);
	}
}

void GenerateJasminCode::generateSimpleExpression(NodeSimpleExpression& simpleExpression) {
	Logger::codeGenerator("simple expression");
	generateTerm(*simpleExpression.lhs);
	for (auto& rhsTerm : simpleExpression.rhs | std::views::values) {
		generateTerm(*rhsTerm);
	}
}

void GenerateJasminCode::generateTerm(NodeTerm& term) {
	Logger::codeGenerator("term");
	generatePrimary(*term.lhs);
	for (auto& rhsPrimary : term.rhs | std::views::values) {
		generatePrimary(*rhsPrimary);
	}
}

void GenerateJasminCode::generatePrimary(NodePrimary& primary) {
	
	//std::visit(overloaded{
	//	[&](const token& t) {
	//		//out << t.value;
	//	},
	//	[&](const std::unique_ptr<NodeExpression>& expr) {
	//		//expr->print(out);
	//	},
	//	[&](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
	//		//out << p.first.value << " ";
	//		//p.second->print(out);
	//	}
	//	}, 
	//	primary.val);

	//if (primary.rhs) {
	//	generateFunctionCall(lastUsedID, *primary.rhs.value());
	//}
}

void GenerateJasminCode::generateFunctionCall(const NodeFunctionCall& args) {
	Logger::codeGenerator("function call");

	if (args.lhs.has_value()) {
		generateActualParameters(*args.lhs.value());
	}
}

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

void GenerateJasminCode::extractTypesFromExpression(const NodeExpression& expr, std::vector<token>& types, const bool getID) {
	extractTypesFromRelopExpression(*expr.lhs, types, getID);

	for (const auto& subExpr : expr.rhs | std::views::values) {
		extractTypesFromRelopExpression(*subExpr, types, getID);
	}
}

void GenerateJasminCode::extractTypesFromRelopExpression(const NodeRelopExpression& relopExpr, std::vector<token>& types,
	const bool getID) {
	extractTypesFromSimpleExpression(*relopExpr.lhs, types, getID);
	for (const auto& [relop, subExpr] : relopExpr.rhs) {
		extractTypesFromSimpleExpression(*subExpr, types, getID);
	}

}

void GenerateJasminCode::extractTypesFromSimpleExpression(const NodeSimpleExpression& simpleExpr, std::vector<token>& types,
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

void GenerateJasminCode::extractTypesFromPrimary(const NodePrimary& primary, std::vector<token>& types, const bool getID) {
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
							   types.push_back(s->type);
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
