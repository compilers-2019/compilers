#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "syntaxtree.h"

#define HASH_LENGTH 0x3fff

enum { BASIC, ARRAY, STRUCT } Type;

unsigned int hash_pjw(char* name) {
		unsigned int val = 0, i;
		for (; *name; ++name) {
				val = (val << 2) + *name;
				if (i = val & ~HASH_LENGTH)
						val = (val ^ (i >> 12) & HASH_LENGTH);
		}
		return val;
}

typedef struct Type_* Type;
typedef struct StructField_* StructField;
typedef struct SymNode_* SymNode;
typedef struct FuncNode_* FuncNode;

struct Type_ {
		enum { BASIC, ARRAY, STRUCT } kind;
		union {
				float basic;
				struct {
						Type element;
						int size;
				} array;
				StructField structure;
		} u;
};

struct StructField_ {
		char name[20];
		Type type;
		StructField next;
};

struct SymNode_ {
		Type type;
		int lineno;
		char name[20];
		float val;
		SymNode hash_next;
		SymNode param_next;
}'

struct FuncNode_ {
		Type returnType;
		SymNode paramType;
		char name[20];
		bool ifdefine;
		int lineno;
		FuncNode hash_next;
};

SymNode sym_table[HASH_LENGTH];
FuncNode func_table[HASH_LENGTH];


