#include <stdio.h>
#include <stdbool.h>
#include "syntaxtree.h"
#include "sym_table.h"

//#ifdef YY_DEBUG
//extern int yydebug;
//#endif

//extern int yylineno;
extern TreeNode root;
extern void yyparse();
extern void yyrestart(FILE* f);
extern void printTree();
extern void Program(TreeNode r);
extern bool signal;
//extern struct TreeNode* current;

int main(int argc, char** argv) {
		root = NULL;
		signal = true;
		//yylineno = 1;
		if(argc <= 1) return 1;
		FILE* f = fopen(argv[1], "r");
		if(!f) {
				perror(argv[1]);
				return 1;
		}
		yyrestart(f);
//#ifdef YY_DEBUG
//		yydebug = 1;
//#endif
		yyparse();
		if(signal) {
				printTree();
				start(root);
		}
		return 0;
}


