%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const ASTnode *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
	const ASTnode *ast;
	Scope *scope;
	ParamDec *par_dec;
	ParamSeq *par_seq;
	ArgSeq *arg_seq;
	VarDec *var_dec;
	VarSeq *var_seq;
	const Expression *expression;
	Statement *statement;
	StatementSequence *statement_sequence;
	const std::string *my_string;
	const int *my_int;
}

%token T_INT T_ID T_RET T_NUM T_VOID
%token T_IF T_ELSE T_WHILE T_DO T_FOR
%token T_LPAR T_RPAR T_LBRA T_RBRA T_LSBRA T_RSBRA
%token T_COMM T_SEMICOL T_DOT
%token T_PTR
%token T_LE T_GE T_EQ2 T_NEQ T_AND2 T_OR2
%token T_RIGHT T_LEFT
%token T_INCR T_DECR
%token T_PLUS T_MINUS T_STAR T_DIV
%token T_SQGL T_EXCL T_PERCENT T_AND T_OR
%token T_LT T_GT T_EQ

%type <ast> PROGRAM FUN_DEC
%type <scope> SCOPE SCOPE_STAT
%type <var_dec> VAR_DEC
%type <var_seq> VAR_SEQ
%type <par_dec> PAR_DEC
%type <par_seq> PAR_SEQ
%type <arg_seq> ARG_SEQ
%type <expression> EXPR BIN_EXPR COMP_EXPR UN_EXPR ID_EXPR FN_EXPR CONST_EXPR EXPR_STAT
%type <statement> STAT ASS_STAT IF_STAT IFELSE_STAT WHILE_STAT DOWHILE_STAT FOR_STAT RET_STAT
%type <statement_sequence> STAT_SEQ
%type <my_string> T_INT T_VOID T_RET T_ID T_IF T_ELSE T_WHILE T_DO T_FOR TYPE
%type <my_string> BIN_OP T_PLUS T_MINUS T_STAR T_DIV T_AND T_OR T_LEFT T_RIGHT
%type <my_string> COMP_OP T_LE T_GE T_EQ2 T_NEQ T_AND2 T_OR2 T_LT T_GT UN_OP T_INCR T_DECR T_EXCL
%type <my_string> T_NUM

%start ROOT

%%

ROOT : PROGRAM 									{ g_root = $1; }

PROGRAM : VAR_DEC T_SEMICOL						{ $$ = new Program($1); }
        | FUN_DEC								{ $$ = new Program($1); }
        | VAR_DEC T_SEMICOL PROGRAM				{ $$ = new Program($1, $3); }
        | FUN_DEC PROGRAM						{ $$ = new Program($1, $2); }

FUN_DEC : TYPE T_ID T_LPAR T_RPAR T_SEMICOL				{ $$ = new FunDec($1, $2, NULL, NULL); }
        | TYPE T_ID T_LPAR PAR_SEQ T_RPAR T_SEMICOL		{ $$ = new FunDec($1, $2, $4,   NULL); }
        | TYPE T_ID T_LPAR T_RPAR SCOPE					{ $$ = new FunDec($1, $2, NULL, $5  ); }
        | TYPE T_ID T_LPAR PAR_SEQ T_RPAR SCOPE			{ $$ = new FunDec($1, $2, $4,   $6  ); }

SCOPE : T_LBRA T_RBRA							{ $$ = new Scope(); }
      | T_LBRA VAR_SEQ T_RBRA					{ $$ = new Scope($2); }
      | T_LBRA STAT_SEQ T_RBRA					{ $$ = new Scope($2); }
      | T_LBRA VAR_SEQ STAT_SEQ T_RBRA			{ $$ = new Scope($2, $3); }

PAR_SEQ : PAR_DEC								{ $$ = new ParamSeq(); $$->addDeclaration($1); }
		| PAR_SEQ T_COMM PAR_DEC	    		{ $$ = $1; $$->addDeclaration($3); }

PAR_DEC : TYPE T_ID 							{ $$ = new ParamDec($1, $2); }

ARG_SEQ : EXPR									{ $$ = new ArgSeq(); $$->addDeclaration($1); }
		| ARG_SEQ T_COMM EXPR					{ $$ = $1; $$->addDeclaration($3); }

VAR_SEQ : VAR_DEC T_SEMICOL						{ $$ = new VarSeq(); $$->addDeclaration($1); }
		| VAR_SEQ VAR_DEC T_SEMICOL	    		{ $$ = $1; $$->addDeclaration($2); }

VAR_DEC : TYPE T_ID 							{ $$ = new VarDec($1, $2, NULL); }
		| TYPE T_ID T_EQ EXPR					{ $$ = new VarDec($1, $2, $4  ); }

STAT_SEQ : STAT									{ $$ = new StatementSequence(); $$->addStatement($1); }
		 | STAT_SEQ STAT			    		{ $$ = $1; $$->addStatement($2); }

STAT : EXPR_STAT								{ $$ = new ExpressionStatement($1); }
     | ASS_STAT									{ $$ = $1; }
	 | IF_STAT									{ $$ = $1; }
	 | IFELSE_STAT								{ $$ = $1; }
	 | WHILE_STAT								{ $$ = $1; }
	 | DOWHILE_STAT								{ $$ = $1; }
	 | FOR_STAT	 								{ $$ = $1; }
	 | RET_STAT									{ $$ = $1; }
	 | SCOPE_STAT								{ $$ = new ScopeStatement($1); }

EXPR_STAT : EXPR 								{ $$ = $1; }
		  | EXPR T_SEMICOL						{ $$ = $1; }

SCOPE_STAT : SCOPE								{ $$ = $1; }		  

ASS_STAT : T_ID T_EQ EXPR T_SEMICOL				{ $$ = new AssignmentStatement($1, $3); }

RET_STAT : T_RET EXPR T_SEMICOL					{ $$ = new ReturnStatement($2); }
		 | T_RET T_SEMICOL						{ $$ = new ReturnStatement(NULL); }

EXPR : BIN_EXPR									{ $$ = $1; }
	 | COMP_EXPR								{ $$ = $1; }
     | UN_EXPR	 								{ $$ = $1; }
	 | ID_EXPR	 								{ $$ = $1; }
	 | FN_EXPR	 								{ $$ = $1; }
	 | CONST_EXPR								{ $$ = $1; }
	 | T_LPAR EXPR T_RPAR						{ $$ = $2; }

BIN_EXPR : EXPR BIN_OP EXPR						{ $$ = new BinaryExpression($1, $2, $3); }

COMP_EXPR : EXPR COMP_OP EXPR					{ $$ = new BinaryExpression($1, $2, $3); }

BIN_OP : T_PLUS									{ $$ = new std::string("+"); }
       | T_MINUS								{ $$ = new std::string("-"); }
       | T_STAR									{ $$ = new std::string("*"); }
       | T_DIV									{ $$ = new std::string("/"); }
       | T_AND									{ $$ = new std::string("&"); }
       | T_OR									{ $$ = new std::string("|"); }
	   | T_LEFT									{ $$ = new std::string("<<"); }
	   | T_RIGHT								{ $$ = new std::string(">>"); }

UN_EXPR : T_ID UN_OP							{ $$ = new UnaryExpression($1, $2); }
        | UN_OP T_ID							{ $$ = new UnaryExpression($2, $1); }
       
UN_OP : T_INCR									{ $$ = new std::string("++"); }
      | T_DECR									{ $$ = new std::string("--"); }
      | T_EXCL									{ $$ = new std::string("!"); }

COMP_OP : T_LE									{ $$ = new std::string("<="); }
        | T_GE									{ $$ = new std::string(">="); }
        | T_EQ2									{ $$ = new std::string("=="); }
        | T_NEQ									{ $$ = new std::string("!="); }
		| T_AND2								{ $$ = new std::string("&&"); }
		| T_OR2									{ $$ = new std::string("||"); }
        | T_LT									{ $$ = new std::string("<"); }
        | T_GT									{ $$ = new std::string(">"); }

ID_EXPR : T_ID	 								{ $$ = new IdentifierExpression($1); }

FN_EXPR : T_ID T_LPAR T_RPAR	 				{ $$ = new FunctionExpression($1, NULL); }
		| T_ID T_LPAR ARG_SEQ T_RPAR			{ $$ = new FunctionExpression($1, $3  ); }

CONST_EXPR : T_NUM								{ $$ = new ConstantExpression($1); }

IF_STAT : T_IF T_LPAR EXPR T_RPAR STAT 			{ $$ = new IfStatement($3, $5); }

IFELSE_STAT : T_IF T_LPAR EXPR T_RPAR STAT T_ELSE STAT				{ $$ = new IfElseStatement($3, $5, $7); }

WHILE_STAT : T_WHILE T_LPAR EXPR T_RPAR SCOPE						{ $$ = new WhileStatement($3, $5); }

DOWHILE_STAT : T_DO SCOPE T_WHILE T_LPAR EXPR T_RPAR T_SEMICOL		{ $$ = new DoWhileStatement($2, $5); }

FOR_STAT : T_FOR T_LPAR STAT STAT STAT T_RPAR SCOPE					{ $$ = new ForStatement($3, $4, $5, $7);}

TYPE : T_INT									{ $$ = new std::string("int"); }
     | T_VOID									{ $$ = new std::string("void"); }

%%

const ASTnode *g_root; // Definition of variable (to match declaration earlier)

const ASTnode *parseAST() {
	g_root=0;
	yyparse();
	return g_root;
}

