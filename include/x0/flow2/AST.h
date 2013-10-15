#pragma once

#include <x0/flow2/FlowType.h>
#include <x0/flow2/FlowToken.h>
#include <x0/flow2/FlowLocation.h>
#include <x0/flow2/ASTVisitor.h>
#include <x0/RegExp.h>
#include <x0/Api.h>
#include <utility>
#include <memory>
#include <vector>
#include <list>

namespace x0 {

class ASTVisitor;
class FlowBackend;

class X0_API ASTNode // {{{
{
protected:
	FlowLocation location_;

public:
	ASTNode() {}
	ASTNode(const FlowLocation& loc) : location_(loc) {}
	virtual ~ASTNode() {}

	FlowLocation& location() { return location_; }
	const FlowLocation& location() const { return location_; }
	void setLocation(const FlowLocation& loc) { location_ = loc; }

	virtual void accept(ASTVisitor& v) = 0;
};
// }}}
// {{{ Symbols
class X0_API Symbol : public ASTNode {
	std::string name_;

protected:
	Symbol(const std::string& name, const FlowLocation& loc) :
		ASTNode(loc),
		name_(name)
	{}

public:
	const std::string& name() const { return name_; }
	void setName(const std::string& value) { name_ = value; }
};

enum class Lookup {
	Self            = 1,
	Parents         = 2,
	Outer           = 4,
	SelfAndParents  = 3,
	SelfAndOuter    = 5,
	OuterAndParents = 6,
	All             = 7
};

inline bool operator&(Lookup a, Lookup b) {
	return static_cast<unsigned>(a) & static_cast<unsigned>(b);
}

class X0_API SymbolTable {
public:
	typedef std::vector<Symbol*> list_type;
	typedef list_type::iterator iterator;
	typedef list_type::const_iterator const_iterator;

public:
	explicit SymbolTable(SymbolTable* outer);
	~SymbolTable();

	void setOuterTable(SymbolTable* table) { outerTable_ = table; }
	SymbolTable* outerTable() const { return outerTable_; }

	SymbolTable* appendParent(SymbolTable* table);
	SymbolTable* parentAt(size_t i) const;
	void removeParent(SymbolTable* table);
	size_t parentCount() const;

	Symbol* appendSymbol(Symbol* symbol);
	void removeSymbol(Symbol* symbol);
	Symbol* symbolAt(size_t i) const;
	size_t symbolCount() const;

	Symbol* lookup(const std::string& name, Lookup lookupMethod) const;

	template<typename T>
	T* lookup(const std::string& name, Lookup lookupMethod) { return dynamic_cast<T*>(lookup(name, lookupMethod)); }

	iterator begin() { return symbols_.begin(); }
	iterator end() { return symbols_.end(); }

private:
	list_type symbols_;
	std::vector<SymbolTable*> parents_;
	SymbolTable* outerTable_;
};

class X0_API ScopedSymbol : public Symbol {
protected:
	std::unique_ptr<SymbolTable> scope_;

protected:
	ScopedSymbol(SymbolTable* outer, const std::string& name, const FlowLocation& loc) :
		Symbol(name, loc),
		scope_(new SymbolTable(outer))
	{}

	ScopedSymbol(std::unique_ptr<SymbolTable>&& scope, const std::string& name, const FlowLocation& loc) :
		Symbol(name, loc),
		scope_(std::move(scope))
	{}

public:
	SymbolTable* scope() { return scope_.get(); }
	const SymbolTable* scope() const { return scope_.get(); }
	void setScope(std::unique_ptr<SymbolTable>&& table) { scope_ = std::move(table); }
};

class X0_API Variable : public Symbol {
private:
	std::unique_ptr<Expr> initializer_;

public:
	Variable(const std::string& name,
			std::unique_ptr<Expr>&& initializer,
			const FlowLocation& loc) :
		Symbol(name, loc), initializer_(std::move(initializer)) {}

	Expr* initializer() const { return initializer_.get(); }
	void setInitializer(std::unique_ptr<Expr>&& value) { initializer_ = std::move(value); }

	virtual void accept(ASTVisitor& v);
};

class X0_API Handler : public ScopedSymbol {
private:
	std::unique_ptr<Stmt> body_;

public:
	Handler(const std::string& name, const FlowLocation& loc) :
		ScopedSymbol(nullptr, name, loc),
		body_(nullptr /*forward declared*/)
	{
	}

	Handler(const std::string& name,
		std::unique_ptr<SymbolTable>&& scope,
		std::unique_ptr<Stmt>&& body,
		const FlowLocation& loc) :
		ScopedSymbol(std::move(scope), name, loc),
		body_(std::move(body))
	{
	}

	bool isForwardDeclared() const { return body_.get() == nullptr; }

	Stmt* body() const { return body_.get(); }
	void setBody(std::unique_ptr<Stmt>&& body) { body_ = std::move(body); }

	virtual void accept(ASTVisitor& v);
};

class X0_API BuiltinFunction : public Symbol {
private:
	FlowBackend* owner_;
	FlowType returnType_;
	std::vector<FlowType> signature_;

public:
	BuiltinFunction(FlowBackend* owner, FlowType rt);

	FlowBackend* owner() const { return owner_; }

	FlowType returnType() const { return returnType_; }
	void setReturnType(FlowType type) { returnType_ = type; }

	const std::vector<FlowType>& signature() const { return signature_; }
	std::vector<FlowType>& signature() { return signature_; }

	virtual void accept(ASTVisitor& v);
};

class X0_API BuiltinHandler : public Symbol {
private:
	FlowBackend* owner_;

public:
	BuiltinHandler(FlowBackend* owner, const std::string& name, const FlowLocation& loc) :
		Symbol(name, loc),
		owner_(owner)
	{
	}

	virtual void accept(ASTVisitor& v);
};

class X0_API Unit : public ScopedSymbol {
private:
	std::vector<std::pair<std::string, std::string> > imports_;

public:
	Unit() :
		ScopedSymbol(nullptr, "#unit", FlowLocation()),
		imports_()
	{}

	// global scope
	Symbol* insert(Symbol* symbol) {
		scope()->appendSymbol(symbol);
		return symbol;
	}

	// plugins
	void import(const std::string& moduleName, const std::string& path) {
		imports_.push_back(std::make_pair(moduleName, path));
	}

	virtual void accept(ASTVisitor& v);
};
// }}}
// {{{ Expr
class X0_API Expr : public ASTNode {
protected:
	explicit Expr(const FlowLocation& loc) : ASTNode(loc) {}
};

class X0_API UnaryExpr : public Expr {
	FlowToken operator_;
	std::unique_ptr<Expr> subExpr_;

public:
	UnaryExpr(FlowToken op, std::unique_ptr<Expr>&& subExpr, const FlowLocation& loc) :
		Expr(loc), operator_(op), subExpr_(std::move(subExpr))
	{}

	FlowToken op() const { return operator_; }
	Expr* subExpr() const { return subExpr_.get(); }

	virtual void accept(ASTVisitor& v);
};

class X0_API BinaryExpr : public Expr {
private:
	FlowToken operator_;
	std::unique_ptr<Expr> lhs_;
	std::unique_ptr<Expr> rhs_;

public:
	BinaryExpr(FlowToken op, std::unique_ptr<Expr>&& lhs, std::unique_ptr<Expr>&& rhs);

	FlowToken op() const { return operator_; }
	Expr* leftExpr() const { return lhs_.get(); }
	Expr* rightExpr() const { return rhs_.get(); }

	virtual void accept(ASTVisitor& v);
};

template<typename T>
class X0_API LiteralExpr : public Expr {
private:
	T value_;

public:
	explicit LiteralExpr(const T& value, const FlowLocation& loc) :
		Expr(loc), value_(value) {}

	const T& value() const { return value_; }
	void setValue(const T& value) { value_ = value; }

	virtual void accept(ASTVisitor& v) { v.visit(*this); }
};

class X0_API FunctionCallExpr : public Expr {
	BuiltinFunction* callee_;
	std::unique_ptr<ListExpr> args_;

public:
	BuiltinFunction* callee() const { return callee_; }
	ListExpr* args() const { return args_.get(); }

	virtual void accept(ASTVisitor& v);
};

class X0_API VariableExpr : public Expr
{
private:
	Variable* variable_;

public:
	VariableExpr(Variable* var, const FlowLocation& loc);
	~VariableExpr();

	Variable* variable() const;
	void setVariable(Variable* var);

	virtual void accept(ASTVisitor& v);
};

class X0_API HandlerRefExpr : public Expr
{
private:
	Handler* handler_;

public:
	HandlerRefExpr(Handler* ref, const FlowLocation& loc) :
		Expr(loc),
		handler_(ref)
	{}

	Handler* handler() const { return handler_; }
	void setHandler(Handler* handler) { handler_ = handler; }

	virtual void accept(ASTVisitor& v);
};

class X0_API ListExpr : public Expr
{
private:
	std::vector<std::unique_ptr<Expr>> list_;

public:
	explicit ListExpr(const FlowLocation& loc) : Expr(loc), list_() {}

	void push_back(std::unique_ptr<Expr> expr);
	Expr* back() { return !list_.empty() ? list_.back().get() : nullptr; }
	size_t size() const { return list_.size(); }
	Expr* at(size_t i) { return list_[i].get(); }
	bool empty() const { return list_.empty(); }
	void clear() { return list_.clear(); }

	void replaceAt(size_t i, Expr* expr);
	void replaceAll(Expr* expr);

	std::vector<Expr*>::iterator begin();
	std::vector<Expr*>::iterator end();

	std::vector<Expr*>::const_iterator begin() const;
	std::vector<Expr*>::const_iterator end() const;

	virtual void accept(ASTVisitor& v);
};
// }}}
// {{{ Stmt
class X0_API Stmt : public ASTNode
{
protected:
	explicit Stmt(const FlowLocation& loc) : ASTNode(loc) {}
};

class X0_API ExprStmt : public Stmt
{
private:
	std::unique_ptr<Expr> expression_;

public:
	ExprStmt(std::unique_ptr<Expr> expr, const FlowLocation& loc);
	~ExprStmt();

	Expr *expression() const;
	void setExpression(std::unique_ptr<Expr> value);

	virtual void accept(ASTVisitor&);
};

class X0_API CompoundStmt : public Stmt
{
private:
	std::list<std::unique_ptr<Stmt>> statements_;

public:
	explicit CompoundStmt(const FlowLocation& loc) : Stmt(loc) {}

	void push_back(std::unique_ptr<Stmt>&& stmt);

	bool empty() const { return statements_.empty(); }
	size_t count() const { return statements_.size(); }

	std::list<std::unique_ptr<Stmt>>::iterator begin() { return statements_.begin(); }
	std::list<std::unique_ptr<Stmt>>::iterator end() { return statements_.end(); }

	virtual void accept(ASTVisitor&);
};

class X0_API HandlerCallStmt : public Stmt {
	Handler* handler_;

public:
	virtual void accept(ASTVisitor&);
};

class X0_API CondStmt : public Stmt
{
private:
	std::unique_ptr<Expr> cond_;
	std::unique_ptr<Stmt> thenStmt_;
	std::unique_ptr<Stmt> elseStmt_;

public:
	CondStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> thenStmt,
			std::unique_ptr<Stmt> elseStmt, const FlowLocation& loc);
	~CondStmt();

	Expr* condition() const;
	Stmt* thenStmt() const;
	Stmt* elseStmt() const;

	virtual void accept(ASTVisitor&);
};
// }}}

} // namespace x0
