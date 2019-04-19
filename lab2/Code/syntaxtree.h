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

struct TreeNode {
		enum Type type;
		char unit[20];
		int lineno;
		float val;
		char name[20];
		struct TreeNode* child;
		struct TreeNode* next;
		struct TreeNode* parent;
};

struct TreeNode* root;
struct TreeNode* newNode(int no, int tp, int childno, ...);
void printNode(struct TreeNode* t, int n);
void printTree();


/*void init() {
		//printf("start...\n");
		struct TreeNode *t = (struct TreeNode*)malloc(sizeof(struct TreeNode));
		t->type = 0;
		strcpy(t->unit, "Program");
		t->lineno = 1;
		t->child = NULL;
		t->next = NULL;
		t->parent = NULL;
		root = t;
		current = t;
		printf("init...\n");
}

void next(int no) {
		printf("addChild...\n");
		struct TreeNode *c = (struct TreeNode*)malloc(sizeof(struct TreeNode));
		c->child = NULL;
		c->next = NULL;
		c->parent = current;
		current->child = c;
		for(int i = 1; i < no; i++) {
				struct TreeNode *p = (struct TreeNode*)malloc(sizeof(struct TreeNode));
				p->child = NULL;
				p->next = NULL;
				p->parent = current;
				c->next = p;
				c = c->next;
		}
		current = current->child;
		printf("addChild finish...\n");
}

void nextNode() {
		printf("nextNode...\n");
		while(current->next == NULL)
				current = current->parent;
		current = current->next;
		printf("nextNode finish...\n");
}

void next(int no) {
		addChild(no);
		//nextNode();		
}

void setN(char* u, int l) {
		printf("setN...\n");
		current->type = 0;
		strcpy(current->unit, u);
		current->lineno = l;
}

void setID(char* n, int l) {
		printf("setID...\n");
		current->type = 1;
		strcpy(current->unit, "ID");
		strcpy(current->name, n);
		current->lineno = l;
		nextNode();
}

void setINT(int v, int l) {
		printf("setINT...\n");
		current->type = 1;
		strcpy(current->unit, "INT");
		current->val = v;
		current->lineno = l;
		nextNode();
}

void setFLOAT(int v, int l) {
		printf("setFLOAT...\n");
		current->type = 1;
		strcpy(current->unit, "FLOAT");
		current->val = v;
		current->lineno = l;
		nextNode();
}

void setT(char* u, int l) {
		printf("setT...\n");
		current->type = 1;
		strcpy(current->unit, u);
		current->lineno = l;
		nextNode();
}*/

#endif
