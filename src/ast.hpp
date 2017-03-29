#ifndef ast_hpp
#define ast_hpp

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <stdlib.h> 

#include "context.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////// INITIALISATIONS

class ASTnode;
class Program;
class Scope;

class Expression;
class BinaryExpression;
class UnaryExpression;
class IdentifierExpression;
class FunctionExpression;
class ConstantExpression;

class Statement;
class StatementSequence;
class ExpressionStatement;
class ScopeStatement;
class AssignmentStatement;
class IfStatement;
class IfElseStatement;
class WhileStatement;
class DoWhileStatement;
class ForStatement;
class ReturnStatement;

class Declaration;
class VarDec;
class VarSeq;
class ParamDec;
class ParamSeq;
class ArgSeq;
class FunDec;

/////////////////////////////////////////////////////////////////////////////////////////////////// AST NODE

class ASTnode {
public:
	virtual ~ASTnode() {}
	
	virtual void print() const = 0;
	virtual void compile(Context & ctxt, unsigned int destLoc) const = 0;

};

/////////////////////////////////////////////////////////////////////////////////////////////////// PROGRAM

class Program : public ASTnode {
public:
    const ASTnode* left;
    const ASTnode* right;

    ~Program();

    Program(const ASTnode* left_in);

    Program(const ASTnode* left_in, const ASTnode* right_in);

    void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

/////////////////////////////////////////////////////////////////////////////////////////////////// SCOPE

class Scope : public ASTnode {
public:
	VarSeq *decls;
	StatementSequence *stats;
	
	Scope();
	
	Scope(VarSeq* decls_in);
	
	Scope(StatementSequence* stats_in);
	
	Scope(VarSeq* decls_in, StatementSequence* stats_in);
	
	~Scope();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

/////////////////////////////////////////////////////////////////////////////////////////////////// EXPRESSION

class Expression : public ASTnode {
public:
	virtual ~Expression() {}
	
	virtual void print() const = 0;
	virtual void compile(Context & ctxt, unsigned int destLoc) const = 0;

};

class BinaryExpression : public Expression {
public:
	const Expression  *left;
	const Expression  *right;
	const std::string *op;
	
	BinaryExpression(const Expression* left_in, const std::string* op_in, const Expression* right_in);

	~BinaryExpression();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class UnaryExpression : public Expression {
public:
	const std::string *id;
	const std::string *op;
	
	UnaryExpression(const std::string* id_in, const std::string* op_in);

	~UnaryExpression();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class IdentifierExpression : public Expression {
public:
	const std::string *name;
	
	IdentifierExpression(const std::string* name_in);
	
	~IdentifierExpression();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class FunctionExpression : public Expression {
public:
	const std::string *name;
	ArgSeq *args;
	
	FunctionExpression(const std::string* name_in, ArgSeq* args_in);
	
	~FunctionExpression();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ConstantExpression : public Expression {
public:
	const std::string *value;
	
	ConstantExpression(const std::string* value_in);
	
	~ConstantExpression();

	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};


/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT

class Statement : public ASTnode {
public:
	virtual ~Statement() {}
	
	virtual void print() const = 0;
	virtual void compile(Context & ctxt, unsigned int destLoc) const = 0;
	
};

class StatementSequence: public Statement {
private:
	std::vector <Statement *> list;

public:
	StatementSequence();

	~StatementSequence();
	
	int getCount() const;
	
	const Statement *getStatement(unsigned int i) const;
	
	void addStatement(Statement* state);
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ExpressionStatement : public Statement {
public:
	const Expression *expression;
	
	ExpressionStatement(const Expression* expr_in);
	
	~ExpressionStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ScopeStatement : public Statement {
public:
	const Scope *scope;
	
	ScopeStatement(const Scope* scope);
	
	~ScopeStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class AssignmentStatement : public Statement {
public:
	const std::string *id;
	const Expression  *rhs;
	
	AssignmentStatement(const std::string* id_in, const Expression* rhs_in);
	
	~AssignmentStatement() ;
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class IfStatement : public Statement {
public:
	const Expression *condition;
	Statement *trueclause;
	
	IfStatement(const Expression* cond_in, Statement* true_in);
	
	~IfStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class IfElseStatement : public Statement {
public:
	const Expression *condition;
	Statement *trueclause;
	Statement *falseclause;
	
	IfElseStatement(const Expression* cond_in, Statement* true_in, Statement* false_in);
	
	~IfElseStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class WhileStatement : public Statement {
public:
	const Expression *condition;
	Scope *body;
	
	WhileStatement(const Expression* cond_in, Scope* body_in);
	
	~WhileStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class DoWhileStatement : public Statement {
public:
	Scope *body;
	const Expression *condition;
	
	DoWhileStatement(Scope* body_in, const Expression* cond_in);
	
	~DoWhileStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ForStatement : public Statement {
public:
	const Statement *init;
	const Statement *condition;
	const Statement *step;
	Scope *body;
	
	ForStatement(const Statement* init_in, const Statement* cond_in, const Statement* step_in, Scope* body_in);
	
	~ForStatement();
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ReturnStatement : public Statement {
public:
	const Expression *thing;
	
	ReturnStatement(const Expression* in);
	
	~ReturnStatement();

	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};


/////////////////////////////////////////////////////////////////////////////////////////////////// DECLARATIONS

class Declaration : public ASTnode {
public:
	virtual ~Declaration() {}
	
	virtual void print() const = 0;
	virtual void compile(Context & ctxt, unsigned int destLoc) const = 0;

};

class VarDec : public Declaration {
public:
    const std::string *type;
    const std::string *id;
	const Expression *rhs;
	
    VarDec(const std::string* _type, const std::string* _id, const Expression* _rhs);
	
    VarDec(const VarDec* p);
    
    ~VarDec();

    void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class VarSeq: public VarDec {
private:
	std::vector <VarDec *> list;

public:
	VarSeq();

	~VarSeq();
	
	int getCount() const;
	
	const VarDec* getDeclaration(unsigned int i) const;
	
	void addDeclaration(VarDec* state);
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ParamDec : public Declaration {
public:
    const std::string *type;
    const std::string *id;
    
    ParamDec();
    
    ParamDec(const std::string* _type, const std::string* _id);
	
    ParamDec(const ParamDec* p);
    
    ~ParamDec();

    void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ParamSeq: public ParamDec {
private:
	std::vector <ParamDec *> list;

public:
	ParamSeq();

	~ParamSeq();
	
	int getCount() const;
	
	const ParamDec* getDeclaration(unsigned int i) const;
	
	void addDeclaration(ParamDec* state);
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class ArgSeq : public ParamDec {
private:
	std::vector <const Expression *> list;

public:
	ArgSeq();

	~ArgSeq();
	
	int getCount() const;
	
	const Expression* getDeclaration(unsigned int i) const;
	
	void addDeclaration(const Expression* state);
	
	void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};

class FunDec : public Declaration {
public:
    const std::string *type;
    const std::string *id;
    ParamSeq    *parameters;
    Scope *body;

    FunDec(const std::string* _type, const std::string* _id, ParamSeq* param, Scope* body);
    
    ~FunDec();

    void print() const override;
    void compile(Context & ctxt, unsigned int destLoc) const override;

};


#endif
