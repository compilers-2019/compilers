#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "syntaxtree.h"

#define HASH_LENGTH 0x3fff

enum { BASIC, ARRAY, STRUCT } Type;

typedef struct Type_* Type;
typedef struct SymNode_* SymNode;
typedef struct FuncNode_* FuncNode;

struct Type_ {
		enum { BASIC, ARRAY, STRUCT } kind;
		union {
				bool basic;
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
}'

struct FuncNode_ {
		Type returnType;
		SymNode paramType;
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

void insert_sym_table(SymNode n);
void insert_func_table(FuncNode n);
SymNode check_sym_table(char* name);
FuncNode check_func_table(char* name);

void Program(TreeNode pro);

