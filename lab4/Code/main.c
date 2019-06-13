#include <stdio.h>
#include <stdbool.h>
#include "syntaxtree.h"
#include "sym_table.h"
//#include "intercode.h"
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
extern bool codeSignal;
extern char file_name[20];
//extern struct TreeNode* current;

int main(int argc, char** argv) {
		root = NULL;
		signal = true;
		//yylineno = 1;
		if(argc <= 2) return 1;
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
			
			//printTree();
			
			start(root);
			//printTable();
			translate_Program(root);
			strcpy(file_name, argv[2]);
			print_codeTree();
			printf("These have been written into %s.\n", file_name);
	
		}
		return 0;

}


