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

void Program(TreeNode pro) {
		// Program -> ExtDefList
		ExtDefList(pro->child);
}

void ExtDefList(TreeNode extl) {
		if(extl->child != NULL) {
				// ExtDefList -> ExtDef ExtDefList
				ExtDef(extl->child);
				ExtDefList(extl->child->next);
		}
		// ExtDefList -> empty
}

void ExtDef(TreeNode ext) {
		//Specifier
		SymNode sn = Specifier(ext->child);
		if(sn) {
				TreeNode second = ext->child->next;
				if(strcmp(second->unit, "ExtDecList") == 0) 
						// ExtDef -> Specifier ExtDecList SEMI
						ExtDecList(second, sn);
				else {
						// ExtDef -> Specifier FunDec CompSt
						FuncNode fn = FunDec(second, sn);
						CompSt(second->next, sn);
						if(!check_func_table(fn->name))
								insert_func_table(fn);
						else
								printf("Error type 4 at line %d: Redefined function \"%s\".", fn->lineno, fn->name);
				}
		}
		// ExtDef -> Specifier SEMI
}

void ExtDecList(TreeNode extl, SymNode sn) {
		// ExtDecList -> VarDec
		VarDec(extl->child, sn, IN_BASIC);
		if(extl->child->next)
				// ExtDecList -> VarDec COMMA ExtDecList
				ExtDecList(extl->child->next->next, sn);
}

SymNode Specifier(TreeNode spec) {
		SymNode res;
		TreeNode first = spec->child;
		if(strcmp(first->unit, "StructSpecifier")) 
				res = StructSpecifier(first);
		else {
				res = malloc(sizeof(struct SymNode_));
				Type t = malloc(sizeof(struct Type_));
				t->kind = BASIC;
				if(strcmp(first->name, "int"))
						t->u.basic = 0;
				else
						t->u.basic = 1;
				res->type = t;
		}
		return res;
}
