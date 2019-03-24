%{
		#include <stdio.h>
%}

/* declared types */
%union {
		int type_int;
		float type_float;
		double type_double;
}

/* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token ID
%token SEMI
%token COMMA
%right ASSIGNOP
%left AND OR
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%token TYPE
%left LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%token WHILE

%%
/* High-level Definitions */
Program: ExtDefList
;
ExtDefList: ExtDef ExtDefList
|
;
ExtDef: Specifier ExtDecList SEMI
| Specifier SEMI
| Specifier FunDec CompSt
;
ExtDecList: VarDec
| VarDec COMMA ExtDecList
;

/* Specifiers */
Specifier: TYPE
| StructSpecifier
;
StructSpecifier: STRUCT OptTag LC DefList RC
| STRUCT Tag
;
OptTag: ID
|
;
Tag: ID
;

/* Declarators */
VarDec: ID
| VarDec LB INT RB
;
FunDec: ID LP VarList RP
| ID LP RP
;
VarList: ParamDec COMMA VarList
| ParamDec
;
ParamDec: Specifier VarDec
;

/* Statements */
CompSt: LC DefList StmtList RC
;
StmtList: Stmt StmtList
| 
;
Stmt: Exp SEMI
| CompSt
| RETURN Exp SEMI
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
| IF LP Exp RP Stmt ELSE Stmt
| WHILE LP Exp RP Stmt
;

/* Local Definitions */
DefList: Def DefList
|
;
Def: Specifier DecList SEMI
;
DecList: Dec
| Dec COMMA DecList
;
Dec: VarDec
| VarDec ASSIGNOP Exp
;

/* Expressions */
Exp: Exp ASSIGNOP Exp
| Exp AND Exp
| Exp OR Exp
| Exp RELOP Exp
| Exp PLUS Exp
| Exp MINUS Exp
| Exp STAR Exp
| Exp DIV Exp
| LP Exp RP
| MINUS Exp
| NOT Exp
| ID LP Args RP
| ID LP RP
| Exp LB Exp RB
| Exp DOT ID
| ID
| INT
| FLOAT
;
Args: Exp COMMA Args
| Exp
;

%%

#include "lex.yy.c"
int main() {
		yypause();
}
yyerror(char* msg) {
		fprintf(stderr, "error: %s\n", msg);
}
