#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

bool signal;

enum Type {
		_ERROR = -1, 
		_INT = 0, _FLOAT, _ID, _SEMI, _COMMA, _ASSIGNOP, _RELOP, _PLUS, _MINUS,
	   	_STAR, 	_DIV, _AND, _OR, _DOT, _NOT, _TYPE, _LP, _RP, _LB, _RB, _LC, _RC,
	   	_STRUCT, _RETURN, _IF, _ELSE, _WHILE, _PROGRAM, _EXTDEFLIST, _EXTDEF, 
		_EXTDECLIST, _SPECIFIER, _STRUCTSPECIFIER, _OPTTAG, _TAG, _VARDEC, _FUNDEC, 
		_VARLIST, _PARAMDEC, _COMPST, _STMTLIST, _STMT, _DEFLIST, _DEF, _DECLIST, 
		_DEC, _EXP, _ARGS 
};

typedef struct TreeNode_* TreeNode;

struct TreeNode_ {
		enum Type type;
		char unit[20];
		int lineno;
		float val;
		char name[20];
		TreeNode child;
		TreeNode next;
		//struct TreeNode* parent;
};

TreeNode root;
TreeNode newNode(int no, int tp, int childno, ...);
void printNode(TreeNode t, int n);
void printTree();

#endif
