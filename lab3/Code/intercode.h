#ifndef __INTER_CODE__
#define __INTER_CODE__

#include <stdio.h>
#include <stdlib.h>
#include "syntaxtree.h"
#include "sym_table.h"

enum R_KIND {
		G, GE, L, LE, E,
};

enum O_KIND {
		VARIABLE, CONSTANT, ADDRESS, LABEL, FUNCTION, 
};

enum I_KIND {
		LABEL, FUNC, ASSIGN, ADD, 
		SUB, MUL, DIV, CITE, 
		GETPOINTER, ASSIGNPOINTER, GOTO, IFGOTO, 
		RETURN, DEC, ARG, CALL, 
		PARAM, READ, WRITE, 
};

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

struct Operand_ {
		O_KIND kind;
		union {
				int var_no;
				int value;
				char *name;
		} u;
};

struct InterCode_ {
		I_KIND kind;
		union {
				struct { Operand op; } sinop; 									/* LABEL, FUNCTION, GOTO, RETURN, ARG, PARAM, READ, WRITE */
				struct { Operand right, left; } assign;							/* ASSIGN */
				struct { Operand result, op1, op2; } binop;						/* ADD, SUB, MUL, DIV, CITE, GETPOINTER, ASSIGNPOINTER */
				struct { Operand re1, re2, label; R_KIND kind; } ifgoto;		/* IFGOTO */
				struct { Operand dec; int size; } dec;							/* DEC */
				struct { Operand ret, func; } call;								/* CALL */
		} u;
		InterCode prev;
		InterCode next;
};

InterCode codeRoot;



#endif
