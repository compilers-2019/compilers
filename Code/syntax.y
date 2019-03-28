%{
		#include "lex.yy.c"
		#include "syntaxtree.h"
		//#define YYERROR_VERBOSE 1
		extern int yylineno;
		extern struct TreeNode* root;
		extern bool signal;
		//void yyerror(char *msg, int no);
		extern void errorInfo(const char* msg);
		extern void printError(const char* msg);
		extern void yyerror(const char *msg);
		//void yyerror(const char*, int);
		//extern struct TreeNode* current;
		//#define YYSTYPE struct TreeNode*
%}

/* declared types */
%union {
		struct TreeNode* type_tree;
}

/* declared tokens */
%token <type_tree> INT
%token <type_tree> FLOAT
%token <type_tree> ID
%token <type_tree> SEMI
%token <type_tree> COMMA
%right <type_tree> ASSIGNOP
%left <type_tree> AND OR
%left <type_tree> RELOP
%left <type_tree> PLUS MINUS
%left <type_tree> STAR DIV
%right <type_tree> NOT
%left <type_tree> DOT
%token <type_tree> TYPE
%left <type_tree> LP RP LB RB LC RC
%token <type_tree> STRUCT
%token <type_tree> RETURN
%token <type_tree> IF
%nonassoc <type_tree> LOWER_THAN_ELSE
%nonassoc <type_tree> ELSE
%token <type_tree> WHILE

%token <type_tree> error

%type <type_tree> Program ExtDefList ExtDef ExtDecList
%type <type_tree> Specifier StructSpecifier OptTag Tag
%type <type_tree> VarDec FunDec VarList ParamDec
%type <type_tree> CompSt StmtList Stmt 
%type <type_tree> DefList Def DecList Dec
%type <type_tree> Exp Args

%%
/* High-level Definitions */
Program: ExtDefList { $$ = newNode(yylineno, 27, 1, $1); root = $$; }
;
ExtDefList: ExtDef ExtDefList { $$ = newNode(yylineno, 28, 2, $1, $2); }
| { $$ = newNode(yylineno, 28, 0); }
;
ExtDef: Specifier ExtDecList SEMI { $$ = newNode(yylineno, 29, 3, $1, $2, $3); }
| Specifier SEMI { $$ = newNode(yylineno, 29, 2, $1, $2); }
| Specifier FunDec CompSt { $$ = newNode(yylineno, 29, 3, $1, $2, $3); }
| Specifier error { $$ = newNode(yylineno, 29, 2, $1, $2); errorInfo(yytext); }
/*| error SEMI { $$ = newNode(yylineno, 29, 2, $1, $2); errorInfo("aaa"); }*/
;
ExtDecList: VarDec { $$ = newNode(yylineno, 30, 1, $1); }
| VarDec COMMA ExtDecList { $$ = newNode(yylineno, 30, 3, $1, $2, $3); }
;

/* Specifiers */
Specifier: TYPE { $$ = newNode(yylineno, 31, 1, $1); }
| StructSpecifier { $$ = newNode(yylineno, 31, 1, $1); }
;
StructSpecifier: STRUCT OptTag LC DefList RC { $$ = newNode(yylineno, 32, 5, $1, $2, $3, $4, $5); }
| STRUCT Tag { $$ = newNode(yylineno, 32, 2, $1, $2); }
| STRUCT error { $$ = newNode(yylineno, 32, 2, $1, $2); errorInfo(yytext); }
;
OptTag: ID { $$ = newNode(yylineno, 33, 1, $1); }
| { $$ = newNode(yylineno, 33, 0); }
;
Tag: ID { $$ = newNode(yylineno, 34, 1, $1); }
;

/* Declarators */
VarDec: ID { $$ = newNode(yylineno, 35, 1, $1); }
| VarDec LB INT RB { $$ = newNode(yylineno, 35, 4, $1, $2, $3, $4); }
| VarDec LB error RB { $$ = newNode(yylineno, 35, 4, $1, $2, $3, $4); errorInfo(yytext); }
;
FunDec: ID LP VarList RP { $$ = newNode(yylineno, 36, 4, $1, $2, $3, $4); }
| ID LP RP { $$ = newNode(yylineno, 36, 3, $1, $2, $3); }
;
VarList: ParamDec COMMA VarList { $$ = newNode(yylineno, 37, 3, $1, $2, $3); }
| ParamDec { $$ = newNode(yylineno, 37, 1, $1); }
;
ParamDec: Specifier VarDec { $$ = newNode(yylineno, 38, 2, $1, $2); }
;

/* Statements */
CompSt: LC DefList StmtList RC { $$ = newNode(yylineno, 39, 4, $1, $2, $3, $4); }
;
StmtList: Stmt StmtList { $$ = newNode(yylineno, 40, 2, $1, $2); }
| { $$ = newNode(yylineno, 40, 0); }
;
Stmt: Exp SEMI { $$ = newNode(yylineno, 41, 2, $1, $2); }
| CompSt { $$ = newNode(yylineno, 41, 1, $1); }
| RETURN Exp SEMI { $$ = newNode(yylineno, 41, 3, $1, $2, $3); }
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = newNode(yylineno, 41, 5, $1, $2, $3, $4, $5); }
| IF LP Exp RP Stmt ELSE Stmt { $$ = newNode(yylineno, 41, 7, $1, $2, $3, $4, $5, $6, $7); }
| WHILE LP Exp RP Stmt { $$ = newNode(yylineno, 41, 5, $1, $2, $3, $4, $5); }
| RETURN error SEMI { $$ = newNode(yylineno, 41, 3, $1, $2, $3); errorInfo(yytext); }
| RETURN Exp error { $$ = newNode(yylineno, 41, 3, $1, $2, $3); errorInfo(yytext); }
| Exp error { $$ = newNode(yylineno, 41, 2, $1, $2); errorInfo(yytext); }
;

/* Local Definitions */
DefList: Def DefList { $$ = newNode(yylineno, 42, 2, $1, $2); }
| { $$ = newNode(yylineno, 42, 0); }
;
Def: Specifier DecList SEMI { $$ = newNode(yylineno, 43, 3, $1, $2, $3); }
| Specifier error { $$ = newNode(yylineno, 43, 2, $1, $2); errorInfo(yytext); }
;
DecList: Dec { $$ = newNode(yylineno, 44, 1, $1); }
| Dec COMMA DecList { $$ = newNode(yylineno, 44, 3, $1, $2, $3); }
;
Dec: VarDec { $$ = newNode(yylineno, 45, 1, $1); }
| VarDec ASSIGNOP Exp { $$ = newNode(yylineno, 45, 3, $1, $2, $3); }
;

/* Expressions */
Exp: Exp ASSIGNOP Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp AND Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp OR Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp RELOP Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp PLUS Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp MINUS Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp STAR Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp DIV Exp { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| LP Exp RP { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| MINUS Exp { $$ = newNode(yylineno, 46, 2, $1, $2); }
| NOT Exp { $$ = newNode(yylineno, 46, 2, $1, $2); }
| ID LP Args RP { $$ = newNode(yylineno, 46, 4, $1, $2, $3, $4); }
| ID LP RP { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| Exp LB Exp RB { $$ = newNode(yylineno, 46, 4, $1, $2, $3, $4); }
| Exp DOT ID { $$ = newNode(yylineno, 46, 3, $1, $2, $3); }
| ID { $$ = newNode(yylineno, 46, 1, $1); }
| INT { $$ = newNode(yylineno, 46, 1, $1); }
| FLOAT { $$ = newNode(yylineno, 46, 1, $1); }
| Exp LB error RB { $$ = newNode(yylineno, 46, 4, $1, $2, $3, $4); errorInfo(yytext);  }
| error RP { $$ = newNode(yylineno, 46, 2, $1, $2); errorInfo(yytext); }
| error RB { $$ = newNode(yylineno, 46, 2, $1, $2); errorInfo(yytext); }
;
Args: Exp COMMA Args { $$ = newNode(yylineno, 47, 3, $1, $2, $3); }
| Exp { $$ = newNode(yylineno, 47, 1, $1); }
;

%%

/*#include "lex.yy.c"
int main() {
		yypause();
}
*/

void errorInfo(const char* info) {
		char error[30] = "syntax error, before \"";
		strcat(error, info);
		strcat(error, "\"");
		signal = false;
		printf("Error type B at Line %d: %s\n", yylineno, error);

}

void yyerror(const char *msg) {
		//printf("%s\n", msg);
		//signal = false;
		//printf("Error type B at Line %d: %s\n", yylineno, msg);
}

/*void yyerror(char *msg) {
		signal = false;
		printf("Error type B at Line %d: %s\n", yylineno, msg);
}*/

