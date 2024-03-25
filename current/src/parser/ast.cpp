//Max Hofmeyer & Ahmed Malik | EGRE 591 | 03/23/2024

#include "ast.hpp"

//needed for visit: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void NodeToyCProgram::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "ToyCProgram:\n";
    p.indent();
    for (const auto& def : definitions) {
        def->print(out);
        out << "\n";
    }
    p.outdent();
}

void NodeDefinition::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    lhs->print(out);
    if (rhs.has_value()) rhs.value()->print(out);
}

void NodeFunctionDefinition::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out <<" "<< "FunctionDefinition( \n";
    p.indent();
    lhs->print(out);
    out << ",\n";
    rhs->print(out);
    p.outdent();
    out << "\n" << p.spaces() << ")";
}

void NodeFunctionHeader::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "FunctionHeader(";
    if (lhs.has_value() && lhs != nullptr) lhs.value()->print(out);
    out << ")";
}

void NodeFunctionBody::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "functionBody(\n";
    p.indent();
    lhs->print(out);
    p.outdent();
    out << "\n" << p.spaces() << ")";
}

void NodeDeclaration::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << type.value << " " << id.value;
}

void NodeFormalParamList::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    //out << p.spaces() << "formalParamList(";
    lhs->print(out);
    for (const auto& param : rhs) {
        out << ", ";
        param->print(out);
    }
    out << ")";
}

void NodeStatement::print(std::ostream& out) const {
    std::visit(overloaded{
        [&](const std::unique_ptr<NodeExpressionStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeBreakStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeCompoundStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeIfStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeNullStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeReturnStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeWhileStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeReadStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeWriteStatement>& arg) { arg->print(out); },
        [&](const std::unique_ptr<NodeNewLineStatement>& arg) { arg->print(out); }
        }, val);
}

void NodeExpressionStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "ExpressionStatement(";
    exp->print(out);
    out << ")";
}

void NodeBreakStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "BreakStatement()";
}

void NodeCompoundStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "CompoundStatement\n" << p.spaces() << "[\n";
    p.indent();
    for (const auto& declaration : lhs) {
        declaration->print(out);
        out << ",\n";
    }
    for (const auto& statement : rhs) {
        statement->print(out);
        if (&statement != &rhs.back()) {
            out << ",\n";
        }
    }
    p.outdent();
    out << "\n" << p.spaces() << "]";
}

void NodeIfStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "IfStatement( ";
    lhs->print(out);
    out << ",\n";
    p.indent();
    mhs->print(out);
    if (rhs) {
        out << ",\n";
        rhs.value()->print(out);
    }
    p.outdent();
    out << "\n" << p.spaces() << ")";
}

void NodeNullStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "NullStatement()";
}

void NodeReturnStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "ReturnStatement(";
    if (lhs) {
        lhs.value()->print(out);
    }
    out << ")";
}

void NodeWhileStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "WhileStatement( ";
    lhs->print(out);
    out << ", \n";
    rhs->print(out);
    out << ")";
}

void NodeReadStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "readStatement(var(" << lhs.value << "))";
    for (const auto& id : rhs) {
        out << ", var(" << id.value << ")";
    }
}

void NodeWriteStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "writeStatement(";
    if (lhs) lhs->print(out);
    out << ")";
}

void NodeNewLineStatement::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    out << p.spaces() << "newLineStatement()";
}

void NodeExpression::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    lhs->print(out);
    for (const auto& [op, expr] : rhs) {
        out << " " << op.value << " ";
        expr->print(out);
    }
}

void NodeRelopExpression::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    lhs->print(out);
    for (const auto& [relop, expr] : rhs) {
        out << " " << relop.value << " ";
        expr->print(out);
    }
}

void NodeSimpleExpression::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    lhs->print(out);
    for (const auto& [addop, term] : rhs) {
        out << " " << addop.value << " ";
        term->print(out);
    }
}

void NodeTerm::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    lhs->print(out);
    for (const auto& [mulop, primary] : rhs) {
        out << " " << mulop.value << " ";
        primary->print(out);
    }
}

void NodePrimary::print(std::ostream& out) const {
    std::visit(overloaded{
        [&](const token& t) { out << t.value; },
        [&](const std::unique_ptr<NodeExpression>& expr) { expr->print(out); },
        [&](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
            out << p.first.value << " ";
            p.second->print(out);
        }
        }, val);

    if (rhs) {
        out << "(";
        rhs.value()->print(out);
        out << ")";
    }
}

void NodeFunctionCall::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    if (lhs) {
        lhs.value()->print(out);
    }
    else {
        out << "functionCall()"; // Adjust based on your structure if needed
    }
}

void NodeActualParameters::print(std::ostream& out) const {
    auto& p = PrettyPrint::Indentation();
    lhs->print(out);
    for (const auto& expr : rhs) {
        out << ", ";
        expr->print(out);
    }
    out << ")";
}
