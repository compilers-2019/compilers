#include "syntaxtree.h"

extern struct TreeNode* root;

char* str[] = {
		"INT", "FLOAT", "ID", "SEMI", "COMMA", "ASSIGNOP", "RELOP", "PLUS", 
		"MINUS", "STAR", "DIV", "AND", "OR", "DOT", "NOT", "TYPE", "LP", 
		"RP", "LB", "RB", "LC", "RC", "STRUCT", "RETURN", "IF", "ELSE", 
		"WHILE", "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier", 
		"StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec", "VarList", 
		"ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList", 
		"Dec", "Exp", "Args" };

struct TreeNode* newNode(int no, int tp, int childno, ...) {
		//printf("New Node: %s, no = %d\n", str[tp], childno);
		struct TreeNode* t = (struct TreeNode*)malloc(sizeof(struct TreeNode));
		t->type = tp;
		t->lineno = no;
		t->child = NULL;
		t->next = NULL;
		strcpy(t->unit, str[tp]);
		va_list argptr;
		va_start(argptr, childno);
		if(childno == 0) {
				switch(tp) {
						case _INT: 
								t->val = va_arg(argptr, int);
								break;
						case _FLOAT: t->val = va_arg(argptr, double); break;
						case _TYPE:
						case _RELOP:
						case _ID: 
								strcpy(t->name, va_arg(argptr, char*)); 
								//printf("%s: %s\n", str[t->type], t->name);
								break;
						default: break;
				}
		}
		else {
				struct TreeNode* p;
				while(childno > 0) {
						struct TreeNode* q = va_arg(argptr, struct TreeNode*);
						//printf("q: %s\n", str[q->type]);
						if(t->child == NULL)
								t->child = q;
						else
								p->next = q;
						p = q;
						childno--;
				}
				t->lineno = t->child->lineno;
		}
		//printf("OK\n");
		return t;
}

void printNode(struct TreeNode* t, int n) {
		if(t == NULL)
				return;
		if((t->type > _WHILE) && (t->child == NULL)) {
				printNode(t->next, n);
				return;
		}
		for(int i = 0; i < n; i++)
				printf("  ");
		switch(t->type) {
				case _ERROR: printf("Error!\n"); break;
				case _TYPE: 
				case _ID: printf("%s: %s\n", t->unit, t->name); break;
				case _INT: printf("INT: %d\n", (int)t->val); break;
				case _FLOAT: printf("FLOAT: %.6f\n", t->val); break;
				default: printf("%s(%d)\n", t->unit, t->lineno); break;
		}
		printNode(t->child, n + 1);
		printNode(t->next, n);
}

void printTree() {
		printNode(root, 0);
}
