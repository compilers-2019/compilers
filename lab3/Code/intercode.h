#ifndef __INTER_CODE__
#define __INTER_CODE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "syntaxtree.h"
#include "sym_table.h"

enum R_KIND{
		G, GE, L, LE, E,
};

enum O_KIND{
		VARIABLE, CONSTANT, ADDRESS, POINTER, 
		TEMP, O_LABEL, FUNCTION, 
};

enum I_KIND{
		LABEL, FUNC, ASSIGN, ADD, 
		SUB, MUL, DIV, /* CITE, 
		GETPOINTER, ASSIGNPOINTER, */ GOTO, IFGOTO, 
		RETURN, DEC, ARG, CALL, 
		PARAM, READ, WRITE, 
};

int temp_num = 0;
int label_num = 0;
#define MAX_TEMP 0x3fff

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
// typedef struct Temp_* Temp;

struct Operand_ {
	enum O_KIND kind;
	union {
		int var_no;
		int value;
		//char *name;
		struct {
			int temp_no;
			char name[5];
			Operand prev;
			Operand next;
		} temp;
		struct {
			int label_no;
			char name[5];
		} label;
	} u;
};

struct InterCode_ {
	enum I_KIND kind;
	union {
		struct { Operand op; } single; 									/* LABEL, FUNCTION, GOTO, RETURN, ARG, PARAM, READ, WRITE */
		struct { Operand result, op; } assign;							/* ASSIGN, GETPOINTER, ASSIGNPOINTER */
		struct { Operand result, op1, op2; } binop;						/* ADD, SUB, MUL, DIV, CITE */
		struct { Operand re1, re2, label; enum R_KIND kind; } ifgoto;	/* IFGOTO */
		struct { Operand dec; int size; } dec;							/* DEC */
		struct { Operand ret, func; } call;								/* CALL */
	} u;
	InterCode prev;
	InterCode next;
};

// struct Temp_ {
// 	int num;
// 	char name[5];
// };

InterCode codeRoot;
Operand temp_table[MAX_TEMP];
Operand label_table[MAX_TEMP];

Operand new_temp();
Operand new_label();

Operand new_op(enum O_KIND kind);
InterCode new_code(enum I_KIND kind, ...);
InterCode merge_code(int n, InterCode code1, InterCode code2, ...);

InterCode translate_Exp(TreeNode tr, Operand place);

#endif
