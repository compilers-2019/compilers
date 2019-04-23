#include "sym_table.h"

unsigned int hash_pjw(char* name)
{
	unsigned int val = 0, i;
	for( ; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~HASH_LENGTH)
			val = (val ^ (i >> 12)) & HASH_LENGTH;
	}
	return val;
}

void start(TreeNode r) {
		init();
		Program(r);
}

void init() {
		for(int i = 0; i < HASH_LENGTH; i++) {
				symtable[i] = NULL;
				func_table[i] = NULL;
		}
}

void insert_sym_table(SymNode n) {
		unsigned int num = hash_pjw(n->name);
		if(sym_table[num] == null)
				sym_table[num] = n;
		else {
				SymNode s = sym_table[num];
				while(s->hash_next != NULL)
						s = s->hash_next;
				s->hash_next = n;
		}
}

void insert_func_table(FuncNode n) {
		unsigned int num = hash_pjw(n->name);
		if(func_table[num] == null)
				func_table[num] = n;
		else {
				FuncNode f = func_table[num];
				while(f->hash_next != NULL)
						f = f->hash_next;
				f->hash_next = n;
		}
}

SymNode check_sym_table(char* name) {
		unsigned int num = hash_pjw(name);
		SymNode s = sym_table[num];
		while(s != NULL) {
				if(strcmp(s->name, name) == 0)
						return s;
				s = s->hash_next;
		}
		return NULL;
}

FuncNode check_func_table(char* name) {
		unsigned int num = hash_pjw(name);
		FuncNode f = func_table[num];
		while(f != NULL) {
				if(strcmp(f->name, name) == 0)
						return f;
				f = f->hash_next;
		}
		return NULL;
}


