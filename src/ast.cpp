#include "ast.hpp"
#include "context.hpp"

using namespace std;

int statementNo = 1;

vector<const std::string *> globalVars;

/////////////////////////////////////////////////////////////////////////////////////////////////// PROGRAM

Program::Program(const ASTnode* left_in)
	: left(left_in), right(NULL)
{}

Program::Program(const ASTnode* left_in, const ASTnode* right_in)
	: left(left_in), right(right_in)
{}

Program::~Program() {
	delete left;
	delete right;
}

void Program::print() const {
	if(left != NULL){
		left->print();
	}
	if(right != NULL){
		right->print();
	}
}

void Program::compile(Context & ctxt, unsigned int destLoc) const {
	if(left != NULL){
		left->compile(ctxt, destLoc);
	}
	if(right != NULL){
		right->compile(ctxt, destLoc);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// SCOPE

Scope::Scope()
	: decls(NULL), stats(NULL)
{}

Scope::Scope(VarSeq* decls_in)
	: decls(decls_in), stats(NULL)
{}

Scope::Scope(StatementSequence* stats_in)
	: decls(NULL), stats(stats_in)
{}

Scope::Scope(VarSeq* decls_in, StatementSequence* stats_in)
	: decls(decls_in), stats(stats_in)
{}

Scope::~Scope() {
	delete decls;
	delete stats;
}

void Scope::print() const {
	cout << "<Scope>" << endl;
	if(decls != NULL) {
		decls->print();
	}
	if(stats != NULL){
		stats->print();
	}
	cout << "</Scope>" << endl;
}

void Scope::compile(Context & ctxt, unsigned int destLoc) const {
	
	int declsNo = 0;
	if(decls != NULL) {
		// add these variables to stack too if they haven't been added already
		declsNo = decls->getCount();
		decls->compile(ctxt, destLoc);
	}
	
	if(stats != NULL){
		stats->compile(ctxt, destLoc);
	}
	
	// delete vars initialised here from stack
	for(int i = 0; i < declsNo; i++) {
		if(ctxt.isDynamic(decls->getDeclaration(i)->id)) {
			ctxt.deleteDynamic(decls->getDeclaration(i)->id);
			ctxt.SP = ctxt.SP - 4;
			ctxt.fsize = ctxt.fsize - 4;
			cout << "    addiu       $sp, $sp, 8" << endl;
		}
	}
	
	if((stats == NULL) && (decls == NULL)) {
		cout << "    nop" << endl;
	}		
}

/////////////////////////////////////////////////////////////////////////////////////////////////// EXPRESSION - BINARY

BinaryExpression::BinaryExpression(const Expression* left_in, const string* op_in, const Expression* right_in) 
	: left(left_in), right(right_in), op(op_in)
{}

BinaryExpression::~BinaryExpression() {
	delete left;
	delete op;
	delete right;
}

void BinaryExpression::print() const {
	if(left != NULL) {
		left->print();
	}
	cout << *op;
	if(right != NULL) {
		right->print();
	}

}

void BinaryExpression::compile(Context & ctxt, unsigned int destLoc) const {
	// TODO: implement operator hierarchy
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	// ARITHMETIC
	if(*op == "+") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    addu        $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
	
	} else if(*op == "-") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    sub         $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
	
	} else if(*op == "*") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    mult        $" << destLoc << ", $" << free[0] << endl;
		cout << "    mflo        $" << destLoc << endl;
	
	} else if(*op == "/") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    div         $" << destLoc << ", $" << free[0] << endl;
		cout << "    mflo        $" << destLoc << endl;
	
	} else if(*op == "&") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    and         $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
	
	} else if(*op == "|") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    or          $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;

	} else if(*op == ">>") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    srlv        $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
		
	} else if(*op == "<<") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    sllv        $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
	
	// COMPARISON	
	} else if(*op == "==") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    bne         $" << destLoc << ", $" << free[0] << ", $not" << statementNo << endl;
		cout << "    nop" << endl;
		// set to 1 if equal (true)
		cout << "    li          $" << destLoc << ", 1" << endl;
		cout << "    b           $end" << statementNo << endl;
		cout << "    nop" << endl;
		// set to 0 if not equal (false)
		cout << "$not" << statementNo << ":" << endl;
		cout << "    move        $" << destLoc << ", $0" << endl;
		// exit
		cout << "$end" << statementNo << ":" << endl;
		statementNo++;
		
	} else if(*op == "!=") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		cout << "    beq         $" << destLoc << ", $" << free[0] << ", $not" << statementNo << endl;
		cout << "    nop" << endl;
		// set to 1 if not equal (true)
		cout << "    li          $" << destLoc << ", 1" << endl;
		cout << "    b           $end" << statementNo << endl;
		cout << "    nop" << endl;
		// set to 0 if equal (false)
		cout << "$not" << statementNo << ":" << endl;
		cout << "    move        $" << destLoc << ", $0" << endl;
		// exit
		cout << "$end" << statementNo << ":" << endl;
		statementNo++;
		
	} else if(*op == ">") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		// set to 1 if greater than (true)
		cout << "    slt         $" << destLoc << ", $" << free[0] << ", $" << destLoc << endl;

	} else if(*op == "<") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		// set to 1 if less than (true)
		cout << "    slt         $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;

	// TODO: ">="
	} else if(*op == ">=") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
	
	// TODO: "<="
	} else if(*op == "<=") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
	
	} else if(*op == "&&") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		// left and right are 1 if true, return 1 if both are 1 ONLY
		cout << "    and         $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
	
	} else if(*op == "||") {
		left->compile(ctxt, destLoc);
		right->compile(ctxt, free[0]);
		// left and right are 1 if true, return 1 if one is 1
		cout << "    or          $" << destLoc << ", $" << destLoc << ", $" << free[0] << endl;
	
	}	else {
		cout << "    nop" << endl;
		//cout << "binary operator " << *op << " not implemented" << endl;
		//exit(1);
	}
	ctxt.setUnused(free[0]);
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////// EXPRESSION - UNARY

UnaryExpression::UnaryExpression(const std::string* id_in, const string* op_in) 
	: id(id_in), op(op_in)
{}

UnaryExpression::~UnaryExpression() {
	delete id;
	delete op;
}

void UnaryExpression::print() const {}

void UnaryExpression::compile(Context & ctxt, unsigned int destLoc) const {
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	std::string loc = ctxt.findOnStack(id);
	cout << "    lw          $" << free[0] << ", " << loc << endl;
	
	if(*op == "++") {
		cout << "    addiu       $" << free[0] << ", $" << free[0] << ", 1" << endl;
	} else if (*op == "--") {
		cout << "    addiu       $" << free[0] << ", $" << free[0] << ", -1" << endl;
	} else if (*op == "!") {
		ctxt.setUsed(free[1]);
		cout << "    addi        $" << free[1] << ", $0, -1" << endl;
		cout << "    xor         $" << free[0] << ", $" << free[0] << ", $" << free[1] << endl;
		ctxt.setUnused(free[1]);
	} else {
		cout << "    nop" << endl;
		//cout << "unary operator " << *op << " not implemented (" << *id << ")" << endl;
		//exit(1);
	}
	
	cout << "    sw          $" << free[0] << ", " << loc << endl;
	ctxt.setUnused(free[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// EXPRESSION - IDENTIFIER

IdentifierExpression::IdentifierExpression(const string* name_in)
	: name(name_in)
{}

IdentifierExpression::~IdentifierExpression() {
	delete name;
}

void IdentifierExpression::print() const {
	cout << *name;
}

void IdentifierExpression::compile(Context & ctxt, unsigned int destLoc) const {
	cout << "    lw          $" << destLoc << ", " << ctxt.findOnStack(name) << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// EXPRESSION - FUNCTION

FunctionExpression::FunctionExpression(const std::string* name_in, ArgSeq* args_in)
	: name(name_in), args(args_in)
{}

FunctionExpression::~FunctionExpression() {
	delete name;
}

void FunctionExpression::print() const {
	cout << *name;
}

void FunctionExpression::compile(Context & ctxt, unsigned int destLoc) const {
	
	int argsNo = 0;
	
	if(args != NULL) {
		argsNo = args->getCount();
	}
	
	for(int i = 0; i < argsNo; i++){
		// place arguments in arg registers
		args->getDeclaration(i)->compile(ctxt, i+4);
	}
	
	// jump and link
	cout << "    .option     pic0" << endl;
	cout << "    jal         " << *name << endl;
	cout << "    nop" << endl;
	
	// obtain returned value
	cout << "    move        $" << destLoc << ", $2" << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// EXPRESSION - CONSTANT

ConstantExpression::ConstantExpression(const string* value_in)
	: value(value_in)
{}
	
ConstantExpression::~ConstantExpression() {
	delete value;
}

void ConstantExpression::print() const {
	cout << *value;
}

void ConstantExpression::compile(Context & ctxt, unsigned int destLoc) const {
	cout << "    li          $" << destLoc << ", " << *value << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - SEQUENCE

StatementSequence::StatementSequence() { }

StatementSequence::~StatementSequence() { }

int StatementSequence::getCount() const {
	return list.size();
}

const Statement* StatementSequence::getStatement(unsigned int i) const {
	if(i < list.size()) {
		return list[i];
	} else {
		return NULL;
	}
}

void StatementSequence::addStatement(Statement* state) {
	list.push_back(state);
}

void StatementSequence::print() const {
	for(unsigned int i = 0; i < list.size(); i++) {
		list[i]->print();
	}
}

void StatementSequence::compile(Context & ctxt, unsigned int destLoc) const {
	for(unsigned int i = 0; i < list.size(); i++) {
		list[i]->compile(ctxt, destLoc);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - EXPRESSION

ExpressionStatement::ExpressionStatement(const Expression* expr_in)
	: expression(expr_in)
{}

ExpressionStatement::~ExpressionStatement() {
	delete expression;
}

void ExpressionStatement::print() const {
	expression->print();
}

void ExpressionStatement::compile(Context & ctxt, unsigned int destLoc) const {
	expression->compile(ctxt, destLoc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - SCOPE

ScopeStatement::ScopeStatement(const Scope* scope_in)
	: scope(scope_in)
{}

ScopeStatement::~ScopeStatement() {
	delete scope;
}

void ScopeStatement::print() const {
	scope->print();
}

void ScopeStatement::compile(Context & ctxt, unsigned int destLoc) const {
	scope->compile(ctxt, destLoc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - ASSIGNMENT

AssignmentStatement::AssignmentStatement(const string* id_in, const Expression* rhs_in)
	: id(id_in), rhs(rhs_in)
{}

AssignmentStatement::~AssignmentStatement() {
	delete id;
	delete rhs;
}

void AssignmentStatement::print() const {}

void AssignmentStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	std::string loc = ctxt.findOnStack(id);
	
	rhs->compile(ctxt, free[0]);
	
	cout << "    sw          $" << free[0] << ", " << loc << endl;
	
	ctxt.setUnused(free[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - IF

IfStatement::IfStatement(const Expression* cond_in, Statement* true_in)
	: condition(cond_in), trueclause(true_in)
{}

IfStatement::~IfStatement() {
	delete condition;
	delete trueclause;
}

void IfStatement::print() const {
	if(trueclause != NULL) {
		trueclause->print();
	}
}

void IfStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	if(trueclause != NULL) {
		
		vector<unsigned int> free = ctxt.freeSavedRegisters();
		ctxt.setUsed(free[0]);

		// evaluate expression
		condition->compile(ctxt, free[0]);
		// free[0] has 1 if true and 0 if false
		cout << "    beq         $0, $" << free[0] << ", $end" << statementNo << endl;
		cout << "    nop" << endl;
		ctxt.setUnused(free[0]);
		trueclause->compile(ctxt, destLoc);
		cout << "$end" << statementNo << ":" << endl;
		statementNo++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - IFELSE

IfElseStatement::IfElseStatement(const Expression* cond_in, Statement* true_in, Statement* false_in)
	: condition(cond_in), trueclause(true_in), falseclause(false_in)
{}

IfElseStatement::~IfElseStatement() {
	delete condition;
	delete trueclause;
	delete falseclause;
}

void IfElseStatement::print() const {
	if(trueclause != NULL) {
		trueclause->print();
	}
	if(falseclause != NULL) {
		falseclause->print();
	}
}

void IfElseStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	// evaluate expression
	condition->compile(ctxt, free[0]);
	// free[0] has 1 if true and 0 if false
	cout << "    beq         $0, $" << free[0] << ", $else" << statementNo << endl;
	cout << "    nop" << endl;
	ctxt.setUnused(free[0]);
	
	if(trueclause != NULL) {
		trueclause->compile(ctxt, destLoc);
	}
	cout << "    b           $end" << statementNo << endl;
	
	
	cout << "$else" << statementNo << ":" << endl;
	if(falseclause != NULL) {
		falseclause->compile(ctxt, destLoc);
	}
	
	cout << "$end" << statementNo << ":" << endl;
	statementNo++;
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - WHILE

WhileStatement::WhileStatement(const Expression* cond_in, Scope* body_in)
	: condition(cond_in), body(body_in)
{}

WhileStatement::~WhileStatement() {
	delete condition;
	delete body;
}

void WhileStatement::print() const {
	if(body != NULL) {
		body->print();
	}
}

void WhileStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	// evaluate expression
	cout << "$top" << statementNo << ":" << endl;
	condition->compile(ctxt, free[0]);
	// free[0] has 1 if true and 0 if false
	cout << "    beq         $0, $" << free[0] << ", $end" << statementNo << endl;
	cout << "    nop" << endl;
	
	if(body != NULL) {
		body->compile(ctxt, destLoc);
	}
	
	cout << "    b           $top" << statementNo << endl;
	cout << "    nop" << endl;
	cout << "$end" << statementNo << ":" << endl;
	statementNo++;
	ctxt.setUnused(free[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - DO WHILE

DoWhileStatement::DoWhileStatement(Scope* body_in, const Expression* cond_in)
	: body(body_in), condition(cond_in)
{}
	
DoWhileStatement::~DoWhileStatement() {
	delete body;
	delete condition;
}
	
void DoWhileStatement::print() const {
	if(body != NULL) {
		body->print();
	}
}

void DoWhileStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	// top
	cout << "$do" << statementNo << ":" << endl;
	
	// body
	body->compile(ctxt, destLoc);
	// evaluate expression
	condition->compile(ctxt, free[0]);
	// free[0] has 1 if true and 0 if false
	cout << "    beq         $0, $" << free[0] << ", $end" << statementNo << endl;
	cout << "    nop" << endl;
	cout << "    b           $top" << statementNo << endl;
	cout << "    nop" << endl;
	
	// end
	cout << "$end" << statementNo << ":" << endl;
	statementNo++;
	ctxt.setUnused(free[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - FOR

ForStatement::ForStatement(const Statement* init_in, const Statement* cond_in, const Statement* step_in, Scope* body_in)
	: init(init_in), condition(cond_in), step(step_in), body(body_in)
{}

ForStatement::~ForStatement() {
	delete init;
	delete condition;
	delete step;
	delete body;
}

void ForStatement::print() const {
	if(body != NULL) {
		body->print();
	}
}

void ForStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	vector<unsigned int> free = ctxt.freeSavedRegisters();
	ctxt.setUsed(free[0]);
	
	// initialisation
	init->compile(ctxt, destLoc);
	
	// top: check condition
	cout << "$top" << statementNo << ":" << endl;
	condition->compile(ctxt, free[0]);
	// free[0] has 1 if true and 0 if false
	cout << "    beq         $0, $" << free[0] << ", $end" << statementNo << endl;
	cout << "    nop" << endl;
	
	// body
	body->compile(ctxt, destLoc);
	
	// step
	step->compile(ctxt, destLoc);
	
	// end
	cout << "    b           $top" << statementNo << endl;
	cout << "    nop" << endl;
	cout << "$end" << statementNo << ":" << endl;
	statementNo++;
	ctxt.setUnused(free[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// STATEMENT - RETURN

ReturnStatement::ReturnStatement(const Expression* in)
	: thing(in)
{}

ReturnStatement::~ReturnStatement() {
	delete thing;
}

void ReturnStatement::print() const {}

void ReturnStatement::compile(Context & ctxt, unsigned int destLoc) const {
	
	if (thing != NULL) {
		thing->compile(ctxt, 2);
	} else {
		cout << "    nop" << endl;
	}
	
	// end of function
	
	// restore s0-$s7
	for(int i = 0; i < 8; i++) {
		cout << "    lw          $s" << i << ", " << 4*(ctxt.argsNo+i) << "($sp)" << endl;
	}
	
	// restore old $fp
	cout << "    lw          $fp, " << (ctxt.fsize - (4*(ctxt.varNo+ctxt.paramNo+1))) << "($sp)" << endl;
	
	// restore return address
	cout << "    lw          $ra, " << 4*(ctxt.argsNo+8) << "($sp)" << endl;
	
	// padding
	cout << "    nop" << endl;
	
	// restore stack frame of previous subroutine
	cout << "    addiu       $sp, $sp, " << ctxt.fsize << endl;
	
	// return from function
	cout << "    jr          $ra" << endl;
	cout << "    nop" << endl;

}

/////////////////////////////////////////////////////////////////////////////////////////////////// DECLARATION - VARIABLE

VarDec::VarDec(const string* _type = NULL, const string* _id = NULL, const Expression* _rhs = NULL)
    : type(_type), id(_id), rhs(_rhs)
{}

VarDec::VarDec(const VarDec* p)
	: type(p->type), id(p->id), rhs(p->rhs)
{}

VarDec::~VarDec() {
	delete type;
	delete id;
	delete rhs;
}

void VarDec::print() const {
	cout << "<Variable id=\"" << *id << "\" />" << endl;
}

void VarDec::compile(Context & ctxt, unsigned int destLoc) const {
	
	std::string loc;
	
	if(destLoc == 99) {
		// is a global variable
		globalVars.push_back(id);
		cout << "    .comm       " << *id << ", 4, 4" << endl;
		
	} else {
		// is not a global variable
		if(ctxt.isOnStack(id)) {
			// variable is already on stack
			loc = ctxt.findOnStack(id);
			
		} else {
			// variable hasn't been initialised yet, initialise as dynamic variable
			ctxt.addDynamic(id);
			ctxt.SP = ctxt.SP + 4;
			ctxt.fsize = ctxt.fsize + 4;
			cout << "    addiu       $sp, $sp, -8" << endl;
			loc = ctxt.findOnStack(id);
		}
		
		if(rhs != NULL) {
			vector<unsigned int> free = ctxt.freeSavedRegisters();
			ctxt.setUsed(free[0]);
			
			rhs->compile(ctxt, free[0]);
			cout << "    sw          $" << free[0] << ", " << loc << endl;
			
			ctxt.setUnused(free[0]);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// DECLARATION - SEQUENCE

VarSeq::VarSeq() { }

VarSeq::~VarSeq() { }

int VarSeq::getCount() const {
	return list.size();
}

const VarDec* VarSeq::getDeclaration(unsigned int i) const {
	if(i < list.size()) {
		return list[i];
	} else {
		return NULL;
	}
}

void VarSeq::addDeclaration(VarDec* state) {
	list.push_back(state);
}

void VarSeq::print() const {
	for(unsigned int i = 0; i < list.size(); i++) {
		list[i]->print();
	}
}

void VarSeq::compile(Context & ctxt, unsigned int destLoc) const {
	for(unsigned int i = 0; i < list.size(); i++) {
		list[i]->compile(ctxt, i);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// DECLARATION - PARAMETERS

ParamDec::ParamDec()
    : type(NULL), id(NULL)
{}

ParamDec::ParamDec(const string* _type, const string* _id)
    : type(_type), id(_id)
{}

ParamDec::ParamDec(const ParamDec* p)
	: type(p->type), id(p->id)
{}

ParamDec::~ParamDec() {
	delete type;
	delete id;
}

void ParamDec::print() const {
	cout << "<Parameter id=\"" << *id << "\" />";
}

void ParamDec::compile(Context & ctxt, unsigned int destLoc) const {}

/////////////////////////////////////////////////////////////////////////////////////////////////// DECLARATION - SEQUENCE

ParamSeq::ParamSeq() { }

ParamSeq::~ParamSeq() { }

int ParamSeq::getCount() const {
	return list.size();
}

const ParamDec* ParamSeq::getDeclaration(unsigned int i) const {
	if(i < list.size()) {
		return list[i];
	} else {
		return NULL;
	}
}

void ParamSeq::addDeclaration(ParamDec* state) {
	list.push_back(state);
}

void ParamSeq::print() const {
	for(unsigned int i = 0; i < list.size(); i++) {
		list[i]->print();
	}
}

void ParamSeq::compile(Context & ctxt, unsigned int destLoc) const {}

/////////////////////////////////////////////////////////////////////////////////////////////////// ARGUMENTS

ArgSeq::ArgSeq() {}

ArgSeq::~ArgSeq() {}

int ArgSeq::getCount() const {
	return list.size();
}

const Expression* ArgSeq::getDeclaration(unsigned int i) const {
	if(i < list.size()) {
		return list[i];
	} else {
		return NULL;
	}
}

void ArgSeq::addDeclaration(const Expression* state) {
	list.push_back(state);
}

void ArgSeq::print() const {}

void ArgSeq::compile(Context & ctxt, unsigned int destLoc) const {}

/////////////////////////////////////////////////////////////////////////////////////////////////// DECLARATION - FUNCTION

FunDec::FunDec(const string* _type, const string* _id, ParamSeq* param, Scope* body_in)
	: type(_type), id(_id), parameters(param), body(body_in)
{}

FunDec::~FunDec() {
	delete type;
	delete id;
	delete parameters;
	delete body;
}

void FunDec::print() const {
	
	if((parameters == NULL) && (body == NULL)) {
		cout << "<Function id=\"" << *id << "\" />" << endl;
		return;
	}
	
	cout << "<Function id=\"" << *id << "\">" << endl;

	if (parameters != NULL){
		parameters->print();
		cout << endl;
	}

	if (body != NULL){
		body->print();
	}

	cout << "</Function>" << endl;

}

void FunDec::compile(Context & ctxt, unsigned int destLoc) const {

	if(body == NULL) {
		// don't do anything for just declarations (not definitions)
		return;
	}
	
	// need fresh context
	ctxt = new Context();

	// .text stuff
	cout << "    .text       " << endl;
	cout << "    .align      2" << endl;
	cout << "    .globl      " << *id << endl;
	cout << "    .set        nomips16" << endl;
	cout << "    .set        nomicromips" << endl;
	cout << "    .ent        " << *id << endl;
	cout << "    .type       " << *id << ", @function" << endl;
	
	// function label
	cout << *id << ":"<< endl;
	
	if(parameters != NULL) {
		ctxt.paramNo = parameters->getCount();
	}
	if(body->decls != NULL) {
		ctxt.varNo = body->decls->getCount();
	}
	
	ctxt.globlVar = globalVars.size();
	
	// DETERMINING FRAME SIZE (in words)
	ctxt.fsize = 0;
	ctxt.fsize = ctxt.fsize + 1;				// save old $fp
	ctxt.fsize = ctxt.fsize + ctxt.globlVar;	// no of global variables in scope
	ctxt.fsize = ctxt.fsize + ctxt.varNo; 		// no of variables declared in this subroutine
	ctxt.fsize = ctxt.fsize + ctxt.paramNo; 	// parameters taken in by subroutine (saved as variables)
	ctxt.fsize = ctxt.fsize + 8;				// 8 registers to preserve accross subroutine calls
	ctxt.fsize = ctxt.fsize + 1;				// save old $ra
	ctxt.fsize = ctxt.fsize + ctxt.argsNo;		// pass up to 4 parameters on subroutine calls from here
	
	if(ctxt.fsize % 2 == 0) {
		// fsize is even, no need for padding
	} else {
		// fsize is odd, pad
		ctxt.fsize = ctxt.fsize + 1;
	}
	
	// from words to bytes
	ctxt.fsize = ctxt.fsize * 4;
	ctxt.fsize_org = ctxt.fsize;
	
	// more .text stuff
	cout << "    .frame      $fp, " << ctxt.fsize << ", $31" << endl;
	cout << "    .mask       0x40000000, -4" << endl;
	cout << "    .fmask      0x00000000, 0" << endl;
	cout << "    .set        noreorder" << endl;
	cout << "    .set        nomacro" << endl;
	
	// save current $sp-4 (aka new $fp)
	cout << "    addiu       $24, $sp, -4" << endl;
	
	// generate frame
	cout << "    addiu       $sp, $sp, -" << ctxt.fsize << endl;
	
	// set context SP
	ctxt.SP = 0;
	
	// save old $fp
	cout << "    sw          $fp, " << (ctxt.fsize - (4*(ctxt.varNo+ctxt.paramNo+1))) << "($sp)" << endl;
	
	// set new $sp
	cout << "    move        $fp, $24" << endl;
	
	// save return address
	cout << "    sw          $ra, " << 4*(ctxt.argsNo+8) << "($sp)" << endl;
	
	// save $s0-$s7 before modifying
	for(int i = 0; i < 8; i++) {
		cout << "    sw          $s" << i << ", " << 4*(ctxt.argsNo+i) << "($sp)" << endl;
	}
	
	// declare global vars
	for(unsigned int i = 0; i < globalVars.size(); i++) {
		ctxt.addVariable(globalVars[i]);
	}
	
	
	// declare parameters as avilable variables & save value on stack
	if(ctxt.paramNo < 4) {
		for(int i = 0; i < ctxt.paramNo; i++) {
			ctxt.addVariable(parameters->getDeclaration(i)->id);
			cout << "    sw          $" << i+4 << ", " << ctxt.findOnStack(parameters->getDeclaration(i)->id) << endl;
		}
	} else {
		// TODO:get arguments from previous stack frame
		cout << "I haven't implemented more than 4 arguments yet." << endl;
		exit(1);
	}
	
	// declare local variables on stack
	for(int i = 0; i < ctxt.varNo; i++){
		ctxt.addVariable(body->decls->getDeclaration(i)->id);
	}
	
	// assign variables
	if(body->decls != NULL) {
		body->decls->compile(ctxt, 2);
	}
	
	// compile statements
	if(body->stats != NULL) {
		body->stats->compile(ctxt, 2);
	}
	
	// delete parameters from context
	for(int i = 0; i < ctxt.paramNo; i++) {
		ctxt.deleteVariable(parameters->getDeclaration(i)->id);
	}
	
	// delete variables from context
	for(int i = 0; i < ctxt.varNo; i++) {
		ctxt.deleteVariable(body->decls->getDeclaration(i)->id);
	}
	
	// restore s0-$s7
	for(int i = 0; i < 8; i++) {
		cout << "    lw          $s" << i << ", " << 4*(ctxt.argsNo+i) << "($sp)" << endl;
	}
	
	// restore old $fp
	cout << "    lw          $fp, " << (ctxt.fsize - (4*(ctxt.varNo+ctxt.paramNo+1))) << "($sp)" << endl;
	
	// restore return address
	cout << "    lw          $ra, " << 4*(ctxt.argsNo+8) << "($sp)" << endl;
	
	// padding
	cout << "    nop" << endl;
	
	// restore stack frame of previous subroutine
	cout << "    addiu       $sp, $sp, " << ctxt.fsize << endl;
	
	// return from function
	cout << "    jr          $ra" << endl;
	cout << "    nop" << endl;
	
	// .text stuff
	cout << "    .set        macro" << endl;
	cout << "    .set        reorder" << endl;
	cout << "    .end        " << *id << endl;
	cout << "    .size       " << *id << ", .-" << *id << endl;
	cout << endl;
}

