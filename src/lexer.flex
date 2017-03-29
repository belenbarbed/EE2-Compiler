%option noyywrap

%{

extern "C" int fileno(FILE *stream);
#include "parser.tab.hpp"
#include <iostream>

void debug();
bool DEBUG = false;

%}

Digit				[0-9]
DigitSequence		{Digit}+

NonZeroDigit		[1-9]
HexadecimalDigit	[0-9a-fA-F]
OctalDigit			[0-7]

Sign				"+"|"-"

DecimalNo			[-]?{DigitSequence}
OctalNo				0{OctalDigit}*
HexadecimalNo		0[X|x]{HexadecimalDigit}+

NonDigit			[A-Za-z_]

NewLine				"\n"
Whitespace			" "

HexEscapeSeq		\\[x]{HexadecimalDigit}+
OctEscapeSeq		\\{OctalDigit}{1,3}
SimpleEscapeSeq		\\[\'\"\?\\abfnrtv]
EscapeSequence		{HexEscapeSeq}|{OctEscapeSeq}|{SimpleEscapeSeq}
StringLiteral		L?\"([^\\\"\n]|{EscapeSequence}|{Whitespace})*\"

Identifier			{NonDigit}({NonDigit}|{Digit})*

Preprocess			"# "{Digit}+" "{StringLiteral}.*
notPreprocess 		#.*

%%

\(					{ debug(); return T_LPAR; }		
\)					{ debug(); return T_RPAR; }
\{					{ debug(); return T_LBRA; }
\}					{ debug(); return T_RBRA; }
\[					{ debug(); return T_LSBRA; }
\]					{ debug(); return T_RSBRA; }

\,					{ debug(); return T_COMM; }
\;					{ debug(); return T_SEMICOL; }
\.					{ debug(); return T_DOT; }

\-\>				{ debug(); return T_PTR; }

\<\=				{ debug(); yylval.my_string = new std::string("<="); return T_LE; }
\>\=				{ debug(); yylval.my_string = new std::string(">="); return T_GE; }
\=\=				{ debug(); yylval.my_string = new std::string("=="); return T_EQ2; }
\!\=				{ debug(); yylval.my_string = new std::string("!="); return T_NEQ; }

\&\&				{ debug(); yylval.my_string = new std::string("&&"); return T_AND2; }
\|\|				{ debug(); yylval.my_string = new std::string("||"); return T_OR2; }

\>\>				{ debug(); return T_RIGHT; }
\<\<				{ debug(); return T_LEFT; }

\+\+				{ debug(); return T_INCR; }
\-\-				{ debug(); return T_DECR; }

\+					{ debug(); yylval.my_string = new std::string("+"); return T_PLUS; }
\-					{ debug(); yylval.my_string = new std::string("-"); return T_MINUS; }
\*					{ debug(); yylval.my_string = new std::string("*"); return T_STAR; }
\/					{ debug(); yylval.my_string = new std::string("/"); return T_DIV; }

\~					{ debug(); return T_SQGL; }
\!					{ debug(); yylval.my_string = new std::string("!"); return T_EXCL; }
\&					{ debug(); yylval.my_string = new std::string("&"); return T_AND; }
\|					{ debug(); yylval.my_string = new std::string("|"); return T_OR; }
\%					{ debug(); return T_PERCENT; }

\=					{ debug(); return T_EQ; }

\<					{ debug(); yylval.my_string = new std::string("<"); return T_LT; }
\>					{ debug(); yylval.my_string = new std::string(">"); return T_GT; }


int           		{ debug(); return T_INT; }
void				{ debug(); return T_VOID; }
return           	{ debug(); yylval.my_string = new std::string("return"); return T_RET; }
if           		{ debug(); yylval.my_string = new std::string("if");     return T_IF; }
else           		{ debug(); yylval.my_string = new std::string("else");   return T_ELSE; }
while           	{ debug(); yylval.my_string = new std::string("while");  return T_WHILE; }
do           		{ debug(); yylval.my_string = new std::string("do");     return T_DO; }
for           		{ debug(); yylval.my_string = new std::string("for");    return T_FOR; }

{Identifier}  		{ debug(); yylval.my_string = new std::string(yytext);   return T_ID; }

{DecimalNo}			{ debug(); yylval.my_string = new std::string(yytext);   return T_NUM; }

{NewLine}	      	{ debug(); }
{Whitespace}		{ debug(); }
{Preprocess}		{ debug(); }
{notPreprocess} 	{ debug(); }
.                  	{ debug(); }

%%

// Error handler. This will get called if none of the rules match.
void yyerror (char const *s) {
	fprintf (stderr, "Parse error: %s\n", s);
	fprintf(stderr, "Could not parse \"%s\"\n", yytext);
	exit(1);
}

void debug() {
	if(DEBUG) {
		std::cout << yytext;
	}
	return;
}