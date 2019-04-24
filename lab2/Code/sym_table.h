#ifndef __SYM_TABLE__
#define __SYM_TABLE__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "syntaxtree.h"

#define HASH_LENGTH 0x3fff
#define IN_BASIC 0
#define IN_STRUCT 1
#define IN_PARAM 2
#define IN_ARRAY 3

enum { BASIC, ARRAY, STRUCT } Type;

typedef struct Type_* Type;
typedef struct SymNode_* SymNode;
typedef struct FuncNode_* FuncNode;

struct Type_ {
	enum { BASIC, ARRAY, STRUCT } kind;
	union {
		int basic;
		struct {
			SymNode arrlist;
			Type element;
			int size;
		} array;
		SymNode structure;
	} u;
};

struct SymNode_ {
	Type type;
	int lineno;
	char* name;
	SymNode hash_next;
	SymNode param_next;
	SymNode struct_next;
};

struct FuncNode_ {
	Type returnType;
	SymNode params;
	char* name;
	bool ifdefine;
	int lineno;
	FuncNode hash_next;
};

SymNode sym_table[HASH_LENGTH];
FuncNode func_table[HASH_LENGTH];

unsigned int hash_pjw(char* name);

void start(TreeNode r);
void init();

bool match_type(Type a, Type b);

void insert_sym_table(SymNode n);
void insert_func_table(FuncNode n);
SymNode check_sym_table(char* name);
FuncNode check_func_table(char* name);

void Program(TreeNode pro);
void ExtDefList(TreeNode extl);
void ExtDef(TreeNode ext);
void ExtDecList(TreeNode extl, SymNode sn);

SymNode Specifier(TreeNode spec);
SymNode StructSpecifier(TreeNode sspec);
char* OptTag(TreeNode opt);

SymNode VarDec(TreeNode vdec, SymNode sn, int flag);
FuncNode FunDec(TreeNode fdec, SymNode sn);
SymNode VarList(TreeNode varl);
SymNode ParamDec(TreeNode pdec);

void CompSt(TreeNode comp, SymNode sn);
void StmtList(TreeNode stmtl, SymNode sn);
void Stmt(TreeNode stmt, SymNode sn);

SymNode DefList(TreeNode defl, int flag);
SymNode Def(TreeNode def, int flag);
SymNode DecList(TreeNode decl, SymNode sn, int flag);
SymNode Dec(TreeNode dec, SymNode sn, int flag);

SymNode Exp(TreeNode exp);
SymNode Args(TreeNode args);

#endif