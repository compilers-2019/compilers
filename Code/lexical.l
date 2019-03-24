%{
/* 此处省略#include部分 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntax.tab.h"
int chars = 0;
int words = 0;
int num=0;
int lines = 0;
%}
%x COMMENT
%option yylineno
digit [0-9]
letter [_a-zA-Z]
id [_a-zA-Z][_a-zA-Z0-9]*
int ([+-]?[1-9][0-9]*)|0
efloat [+-]?(({digit}+\.{digit}*)|({digit}*\.(digit}+))[Ee][+-]?{digit}+
float ({int}\.[0-9]+)|efloat
bool true|false|TRUE|FALSE
delim [ \t]
space {delim}+
%%
{id} { words++; chars+=yyleng;return ID;}
{int} { num++; chars+= yyleng; printf("num %d\n",atoi(yytext));return INT;}
{space} {chars++;}
{float} {return FLOAT;}
\n { chars++; lines++; }
";" {return SEMI;}
"," {return COMMA;}
"=" {return ASSIGNOP;} 
">" {return RELOP;}
"<" {return RELOP;}
">=" {return RELOP;}
"<=" {return RELOP;}
"==" {return RELOP;}
"!=" {return RELOP;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"." {return DOT;}
"!" {return NOT;}
int|float {return TYPE;}
"(" {return LP;}
")" {return RP;}
"{" {return LC;}
"}" {return RC;}
struct {return STRUCT;}
return {return RETURN;}
if {return IF;}
else {return ELSE;}
while {return WHILE;}
"[" {return LB;}
"]" {return RB;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}

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
