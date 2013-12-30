/* <flow/Flow.cpp>
 *
 * This file is part of the x0 web server project and is released under LGPL-3.
 * http://redmine.xzero.io/projects/flow
 *
 * (c) 2010-2013 Christian Parpart <trapni@gmail.com>
 */

#include <x0/flow/FlowCallVisitor.h>
#include <x0/flow/AST.h>
#include <algorithm>

#include <cassert>

namespace x0 {

FlowCallVisitor::FlowCallVisitor(ASTNode* root) :
    functionCalls_(),
    handlerCalls_()
{
    visit(root);
}

FlowCallVisitor::~FlowCallVisitor()
{
}

void FlowCallVisitor::visit(ASTNode* node)
{
    if (node) {
        node->visit(*this);
    }
}

// {{{ symbols
void FlowCallVisitor::accept(Variable& variable)
{
    visit(variable.initializer());
}

void FlowCallVisitor::accept(Handler& handler)
{
    for (auto sym: *handler.scope()) {
        visit(sym);
    }

    visit(handler.body());
}

void FlowCallVisitor::accept(BuiltinFunction& v)
{
}

void FlowCallVisitor::accept(BuiltinHandler& v)
{
}

void FlowCallVisitor::accept(Unit& unit)
{
    for (auto s: *unit.scope()) {
        visit(s);
    }
}
// }}}
// {{{ expressions
void FlowCallVisitor::accept(UnaryExpr& expr)
{
    visit(expr.subExpr());
}

void FlowCallVisitor::accept(BinaryExpr& expr)
{
    visit(expr.leftExpr());
    visit(expr.rightExpr());
}

void FlowCallVisitor::accept(FunctionCall& call)
{
    for (const auto& arg: call.args().values()) {
        visit(arg);
    }

    functionCalls_.push_back(&call);
}

void FlowCallVisitor::accept(VariableExpr& expr)
{
}

void FlowCallVisitor::accept(HandlerRefExpr& expr)
{
}

void FlowCallVisitor::accept(StringExpr& expr)
{
}

void FlowCallVisitor::accept(NumberExpr& expr)
{
}

void FlowCallVisitor::accept(BoolExpr& expr)
{
}

void FlowCallVisitor::accept(RegExpExpr& expr)
{
}

void FlowCallVisitor::accept(IPAddressExpr& expr)
{
}

void FlowCallVisitor::accept(CidrExpr& expr)
{
}

void FlowCallVisitor::accept(ExprStmt& stmt)
{
}
// }}}
// {{{ stmt
void FlowCallVisitor::accept(CompoundStmt& compound)
{
    for (const auto& stmt: compound) {
        visit(stmt.get());
    }
}

void FlowCallVisitor::accept(CondStmt& condStmt)
{
    visit(condStmt.condition());
    visit(condStmt.thenStmt());
    visit(condStmt.elseStmt());
}

void FlowCallVisitor::accept(MatchStmt& stmt)
{
    visit(stmt.condition());
    for (auto& one: stmt.cases()) {
        visit(one.first.get());
        visit(one.second.get());
    }
    visit(stmt.elseStmt());
}

void FlowCallVisitor::accept(AssignStmt& assignStmt)
{
    visit(assignStmt.expression());
}

void FlowCallVisitor::accept(HandlerCall& call)
{
    for (const auto& arg: call.args().values()) {
        visit(arg);
    }

    handlerCalls_.push_back(&call);
}
// }}}

} // namespace x0