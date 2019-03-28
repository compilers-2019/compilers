%{
		//#include <stdio.h>
		//#include <stdlib.h>
		//#include <string.h>
		#include "syntax.tab.h"
		#include <stdbool.h>
		#include "syntaxtree.h"
		extern bool signal;
		extern struct TreeNode* newNode(int no, int tp, int childno, ...);
		//#include "syntaxtree.h"
		//extern struct TreeNode* root;
		//extern struct TreeNode* current;
		/*int yycolumn = 1;
		#define YY_USER_ACTION
				yylloc.first_line = yylloc.last_line = yylineno; \
				yylloc.first_column = yycolumn; \
				yylloc.last_column = yycolumn + yyleng - 1; \
				yycolumn += yyleng;*/
%}
%x COMMENT
%option yylineno
digit [0-9]
letter [_a-zA-Z]
efloat (({digit}+\.{digit}*)|({digit}*\.{digit}+))[Ee][+-]?{digit}+
float {efloat}|({digit}+\.{digit}+)
int ([1-9][0-9]*)|0
id [_a-zA-Z][_a-zA-Z0-9]*
bool true|false|TRUE|FALSE
delim [ \t]
space {delim}+
%%
{space} {;}
{int} {
		//printf("int: %d\n", atoi(yytext));
		yylval.type_tree = newNode(yylineno, 0, 0, atoi(yytext));
		//setINT(yylval, yylineno);
		return INT;
}
{float} {
		//printf("float: %s\n", yytext);
		yylval.type_tree = newNode(yylineno, 1, 0, atof(yytext));
		//setFLOAT(yylval, yylineno);
		return FLOAT;
}
";" { yylval.type_tree = newNode(yylineno, 3, 0); return SEMI; }
"," { yylval.type_tree = newNode(yylineno, 4, 0); return COMMA; }
">" { yylval.type_tree = newNode(yylineno, 6, 0, ">"); return RELOP; }
"<" { yylval.type_tree = newNode(yylineno, 6, 0, "<"); return RELOP; }
">=" { yylval.type_tree = newNode(yylineno, 6, 0, ">="); return RELOP; }
"<=" { yylval.type_tree = newNode(yylineno, 6, 0, "<="); return RELOP; }
"==" { yylval.type_tree = newNode(yylineno, 6, 0, "=="); return RELOP; }
"!=" { yylval.type_tree = newNode(yylineno, 6, 0, "!="); return RELOP; }
"=" { yylval.type_tree = newNode(yylineno, 5, 0); return ASSIGNOP; } 
"+" { yylval.type_tree = newNode(yylineno, 7, 0); return PLUS; }
"-" { yylval.type_tree = newNode(yylineno, 8, 0); return MINUS; }
"*" { yylval.type_tree = newNode(yylineno, 9, 0); return STAR; }
"/" { yylval.type_tree = newNode(yylineno, 10, 0); return DIV; }
"&&" { yylval.type_tree = newNode(yylineno, 11, 0); return AND; }
"||" { yylval.type_tree = newNode(yylineno, 12, 0); return OR; }
"." { yylval.type_tree = newNode(yylineno, 13, 0); return DOT; }
"!" { yylval.type_tree = newNode(yylineno, 14, 0); return NOT; }
int|float { yylval.type_tree = newNode(yylineno, 15, 0, yytext); return TYPE; }
"(" { yylval.type_tree = newNode(yylineno, 16, 0); return LP; }
")" { yylval.type_tree = newNode(yylineno, 17, 0); return RP; }
"[" { yylval.type_tree = newNode(yylineno, 18, 0); return LB; }
"]" { yylval.type_tree = newNode(yylineno, 19, 0); return RB; }
"{" { yylval.type_tree = newNode(yylineno, 20, 0); return LC; }
"}" { yylval.type_tree = newNode(yylineno, 21, 0); return RC; }
struct { yylval.type_tree = newNode(yylineno, 22, 0); return STRUCT; }
return { yylval.type_tree = newNode(yylineno, 23, 0); return RETURN; }
if { yylval.type_tree = newNode(yylineno, 24, 0); return IF; }
else { yylval.type_tree = newNode(yylineno, 25, 0); return ELSE; }
while { yylval.type_tree = newNode(yylineno, 26, 0); return WHILE; }
\n { /*yycolumn = 1;*/ }
{id} { yylval.type_tree = newNode(yylineno, 2, 0, yytext); return ID; }
. {
		printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);
		signal = false;
}

%%
/*
int main(int argc, char** argv) 
{
	if (argc > 1) 
	{
		if (!(yyin = fopen(argv[1], "r"))) 
		{
			perror(argv[1]);
			return 1;
		}
	}
	yylex();
	printf("%8d%8d%8d%8d\n", lines, words, num,chars);
	return 0;
}*/
/*
int main(int argc, char** argv) 
{
	int i, totchars = 0, totwords = 0, totlines = 0;
	if (argc < 2) { // just read stdin 
		yylex();
		printf("%8d%8d%8d\n", lines, words, chars);
		return 0;
	}

	for (i = 1; i < argc; i++) 
	{
		FILE *f = fopen(argv[i], "r");
		if (!f) 
		{
			perror(argv[i]);
			return 1;
		}
		yyrestart(f);
		yylex();
		fclose(f);
		printf("%8d%8d%8d %s\n", lines, words, chars, argv[i]);
		totchars += chars; chars = 0;
		totwords += words; words = 0;
		totlines += lines; lines = 0;
	}
	if (argc > 1)
	printf("%8d%8d%8d total\n", totlines, totwords, totchars);
	return 0;
}*/
int yywrap()
{
	return 1;
}
