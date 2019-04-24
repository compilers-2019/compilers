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
				printf("Error type 4 at line %d: Redefined function \"%s\".\n", fn->lineno, fn->name);
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
	if(strcmp(first->unit, "StructSpecifier") == 0)
		// Specifier -> StructSpecifier
		res = StructSpecifier(first);
	else {
		// Specifier -> TYPE
		res = malloc(sizeof(struct SymNode_));
		Type t = malloc(sizeof(struct Type_));
		t->kind = BASIC;
		if(strcmp(first->name, "int") == 0)
			t->u.basic = 0;
		else
			// float
			t->u.basic = 1;
			res->type = t;
	}
	return res;
}

SymNode StructSpecifier(TreeNode sspec) {
	TreeNode second = sspec->child->next;
	SymNode res;
	if(strcmp(second->unit, "OptTag") == 0) {
		// StructSpecifier -> STRUCT OptTag LC DefList RC
		res = malloc(sizeof(struct SymNode_));
		Type type = malloc(sizeof(struct Type_));
		type->kind = STRUCT;
		res->type = type;
		res->lineno = second->lineno;
		res->struct_next = DefList(second->next->next, IN_STRUCT);
		for(SymNode sn = res->struct_next; sn != NULL; sn = sn->struct_next) {
			for(SymNode s = sn->struct_next; s != NULL; s = s->struct_next) {
				if(strcmp(sn->name, s->name)) {
					printf("Error type 15 at Line %d: Redefined field \"%s\".\n", s->lineno, s->name);
					break;
				}
			}
		}
		if(second->child != NULL) {
			char* id = OptTag(second->child);
			res->name = id;
			if(check_sym_table(res->name) != NULL)
				printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", res->lineno, res->name);
			else
				insert_sym_table(res);
		}
	}
	else {
		// StructSpecifier -> STRUCT Tag
		res = check_sym_table(second->child->name);
		if((res == NULL) || (res->type->kind != STRUCT))
			printf("Error type 17 at Lint %d: Undefined structure \"%s\".\n", second->lineno, second->child->name);
	}
	return res;
}

char* OptTag(TreeNode opt) {
	char* id = malloc(strlen(opt->name));
	strcpy(id, opt->name);
	return id;
}

SymNode VarDec(TreeNode vdec, SymNode sn, int flag) {
	TreeNode first = vdec->child;
	SymNode res = malloc(sizeof(SymNode_));
	if(strcmp(first->unit, "ID") == 0) {
		// VarDec -> ID
		res->name = malloc(strlen(first->name));
		strcpy(res->name, first->name);
		res->lineno = first->lineno;
		Type type = malloc(sizeof(struct Type_));
		if(sn->type->kind == BASIC) {
			type->kind = BASIC;
			type->u.basic = sn->type->u.basic;
		}
		else if(sn->type->kind == STRUCT) {
			type->kind = STRUCT;
			type->u.structure = sn;
		}
		res->type = type;
	}
	else {
		// VarDec -> VarDec LB INT RB
		SymNode s = VarDec(first, sn, IN_ARRAY);
		res->name = malloc(strlen(s->name));
		strcpy(res->name, s->name);
		res->lineno = s->lineno;
		res->type->kind = ARRAY;
		res->type->u.array.element = s->type;
		res->type->u.array.size = (int)(first->next->next->val);
	}
	if(flag == IN_BASIC) {
		if(check_sym_table(res->name) != NULL)
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", res->name, res->lineno);
		else {
			insert_sym_table(res);
		}
	}
}

FuncNode FunDec(TreeNode fdec, SymNode sn) {
	TreeNode first = fdec->child;
	TreeNode third = fdec->child->next->next;

	FuncNode res = malloc(sizeof(struct FuncNode_));
	res->name = malloc(strlen(first->name));
	strcpy(res->name, first->name);
	res->lineno = first->lineno;
	res->returnType = sn;
	// FunDec -> ID LP RP
	if(strcmp(third->name, "VarList") == 0) 
		// FunDec -> ID LP VarList RP
		res->params = VarList(third);
	return res;
}

SymNode VarList(TreeNode varl) {
	TreeNode first = varl->child;
	SymNode res = ParamDec(first);
	// VarList -> ParamDec
	if(first->next != NULL) {
		// VarList -> ParamDec COMMA VarList
		res->param_next = VarList(first->next->next);
	}
	return res;
}

SymNode ParamDec(TreeNode pdec) {
	// ParamDec -> Specifier VarDec
	TreeNode first = pdec->child;
	TreeNode second = first->next;
	
	SymNode spec = Specifier(first);
	SymNode res = VarDec(second, spec, IN_PARAM);
	res->param_next = NULL;
	if(check_sym_table(res->name) != NULL)
		printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", res->name, res->lineno);
	else {
		insert_sym_table(res);
	}
	return res;
}

void CompSt(TreeNode comp, SymNode sn) {
	// CompSt -> LC DefList StmtList RC
	TreeNode second = comp->child->next;
	TreeNode third = second->next;
	DefList(second, IN_BASIC);
	StmtList(third, sn);
}

void StmtList(TreeNode stmtl, SymNode sn) {
	if(stmtl->child != NULL) {
		// StmtList -> Stmt StmtList
		Stmt(stmtl->child, sn);
		StmtList(stmtl->child->next, sn);
	}
	// StmtList -> empty
}

void Stmt(TreeNode stmt, SymNode sn) {
	TreeNode first = stmt->child;
	if(strcmp(first->unit, "Exp") == 0) {
		// Stmt -> Exp SEMI
		Exp(first);
	}
	else if(strcmp(first->unit, "CompSt")) {
		// Stmt -> CompSt
		CompSt(first, sn);
	}
	else if(strcmp(first->unit, "RETURN")) {
		// Stmt -> RETURN Exp SEMI
		SymNode returnType = Exp(first->next);
		if(!match_type(returnType->type, sn->type))
			printf("Error type 8 at Line %d: Type mismatched for return.\n", stmt->lineno);
	}
	else if(strcmp(first->unit, "IF")) {
		// Stmt -> IF LP Exp RP Stmt
		TreeNode third = first->next->next;
		TreeNode fifth = third->next->next;
		SymNode exp = Exp(third);
		Stmt(fifth,sn);
		if(fifth->next != NULL) {
			// Stmt -> IF LP Exp RP Stmt ELSE Stmt
			Stmt(fifth->next->next, sn);
		}
	}
	else if(strcmp(first->unit, "WHILE")) {
		// Stmt -> WHILE LP Exp RP Stmt
		TreeNode third = first->next->next;
		TreeNode fifth = third->next->next;
		SymNode exp = Exp(third);
		Stmt(fifth, sn);
	}
}

SymNode DefList(TreeNode defl, int flag) {
	if(defl->child == NULL)
		// DefList -> empty
		return NULL;
	else {
		// DefList -> Def DefList
		TreeNode first = defl->child;
		TreeNode second = first->next;
		SymNode def = Def(first, flag);
		if(def == NULL)
			return NULL;
		SymNode res = DefList(second, flag);
		SymNode s = def;
		while(p->struct_next != NULL)
			p = p->struct_next;
		p->struct_next = res;
		return def;
	}
}

SymNode Def(TreeNode def, int flag) {
	// Def -> Specifier DecList SEMI
	TreeNode first = def->child;
	TreeNode second = first->next;
	SymNode spec = Specifier(first);
	return DecList(second, spec, flag);
}

SymNode DecList(TreeNode decl, SymNode sn, int flag) {
	// DecList -> Dec
	TreeNode first = decl->child;
	SymNode res = Dec(first, sn, flag);
	if(first->next != NULL) {
		// DecList -> Dec COMMA DecList
		res->struct_next = DecList(first->next->next, sn, flag);
	}
	return res;
}

SymNode Dec(TreeNode dec, SymNode sn, int flag) {
	// Dec -> VarDec
	TreeNode first = dec->child;
	SymNode res = VarDec(first, sn, flag);
	if(second != NULL && flag == IN_STRUCT) {
		// Dec -> VarDec ASSIGNOP Exp
		// But it seems to be illegal to initialize VarDec in one structure...
		SymNode exp = Exp(first->next->next);
		printf("Error type 15 at line %d: initialized variable in field \"%s\".\n", first->lineno, res->name);
	}
	return res;
}