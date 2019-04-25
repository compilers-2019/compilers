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
		sym_table[i] = NULL;
		func_table[i] = NULL;
	}
}
void print_type(SymNode a)
{
	for(SymNode p = a; p != NULL;p = p->param_next)
	{
		if(p->type->kind == BASIC)
		{
			if(p->type->u.basic == 0)
				printf("int");
			else
				printf("float");
		}
		else if(p->type->kind == ARRAY)
		{
			print_type(p->type->u.array.arrlist);
			for(Type q = p->type->u.array.element; q->u.array.element !=NULL; q = q->u.array.element)
				printf("[]");
		}
		else if(p->type->kind == STRUCT)
		{
			printf("struct %s",p->name);
		}
		if(p->param_next != NULL)
			printf(",");
	}
}

int use_func(SymNode a, SymNode b)//调用函数
{
	if(a == NULL && b == NULL)
		return 1;
	int count_a = 0;
	int count_b = 0;
	for(SymNode p = a; p != NULL;p = p->param_next)
		count_a++;
	for(SymNode p = b; p != NULL;p = p->param_next)
		count_b++;
	if(count_a != count_b)
		return 0;
	SymNode p = a; SymNode q = b;
	for(; p != NULL&&q != NULL;p = p->param_next, q = q->param_next)
	{
		if(!match_type(p->type,q->type))
			return 0;
	}	 
	return 1;
}

bool match_param(SymNode a,SymNode b)//参数匹配
{
	if(a==NULL&&b==NULL)
		return true;
	if(a==NULL||b==NULL)
		return false;
	if(match_type(a->type,b->type))
		return match_param(a->struct_next,b->struct_next);
	else
		return false;
}

bool match_type(Type a,Type b)//检查类型（类型匹配）
{
	if(a->kind!=b->kind)
	{
		return false;
	}
	else if(a->kind == BASIC)//basic
	{
		if(a->u.basic != b->u.basic)//只有一项
			return false;
	}
	else if(a->kind==STRUCT)
	{
		if(a->u.structure->name==NULL||b->u.structure->name==NULL)
		{
			return match_param(a->u.structure,b->u.structure);								
		}
		if(strcmp(a->u.structure->name,b->u.structure->name)!=0)
			return false;
	}
	else//array
	{
		return match_type(a->u.array.element,b->u.array.element);	
	}
	return true;
}

void insert_sym_table(SymNode n) {
	unsigned int num = hash_pjw(n->name);
	if(sym_table[num] == NULL)
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
	if(func_table[num] == NULL)
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
	printf("Program -> ExtDefList\n");
	ExtDefList(pro->child);
}

void ExtDefList(TreeNode extl) {
	if(extl->child != NULL) {
		// ExtDefList -> ExtDef ExtDefList
		printf("Program -> ExtDefList\n");
		ExtDef(extl->child);
		ExtDefList(extl->child->next);
	}
	// ExtDefList -> empty
}

void ExtDef(TreeNode ext) {
	//Specifier
	SymNode sn = Specifier(ext->child);
	if(sn) 
	{
		TreeNode second = ext->child->next;
		if(strcmp(second->unit, "ExtDecList") == 0) 
		{	// ExtDef -> Specifier ExtDecList SEMI
			printf("ExtDef -> Specifier ExtDecList SEMI\n");
			ExtDecList(second, sn);
		}
		else {
			// ExtDef -> Specifier FunDec CompSt
			printf("ExtDef -> Specifier FunDec CompSt\n");
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
	printf("ExtDecList -> VarDec\n");
	VarDec(extl->child, sn, IN_BASIC);
	if(extl->child->next)
		// ExtDecList -> VarDec COMMA ExtDecList
		printf("ExtDecList -> VarDec COMMA ExtDecList\n");
		ExtDecList(extl->child->next->next, sn);
}

SymNode Specifier(TreeNode spec) {
	SymNode res;
	TreeNode first = spec->child;
	if(strcmp(first->unit, "StructSpecifier") == 0)
	{	// Specifier -> StructSpecifier
		printf("Specifier -> StructSpecifier\n");
		res = StructSpecifier(first);
	}
	else {
		// Specifier -> TYPE
		printf("Specifier -> TYPE\n");
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
		printf("StructSpecifier -> STRUCT OptTag LC DefList RC\n");
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
		printf("StructSpecifier -> STRUCT Tag\n");
		res = check_sym_table(second->child->name);
		if((res == NULL) || (res->type->kind != STRUCT))
			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", second->lineno, second->child->name);
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
	SymNode res = malloc(sizeof(struct SymNode_));
	if(strcmp(first->unit, "ID") == 0) {
		// VarDec -> ID
		printf("VarDec -> ID\n");
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
		printf("VarDec -> VarDec LB INT RB\n");
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
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", res->lineno, res->name);
		else {
			insert_sym_table(res);
		}
	}
	return res;
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
	printf("FunDec -> ID LP RP\n");
	if(strcmp(third->name, "VarList") == 0) 
		// FunDec -> ID LP VarList RP
		printf("FunDec -> ID LP VarList RP\n");
		res->params = VarList(third);
	return res;
}

SymNode VarList(TreeNode varl) {
	TreeNode first = varl->child;
	SymNode res = ParamDec(first);
	// VarList -> ParamDec
	printf("VarList -> ParamDec\n");
	if(first->next != NULL) {
		// VarList -> ParamDec COMMA VarList
		printf("VarList -> ParamDec COMMA VarList\n");
		res->param_next = VarList(first->next->next);
	}
	return res;
}

SymNode ParamDec(TreeNode pdec) {
	// ParamDec -> Specifier VarDec
	printf("ParamDec -> Specifier VarDec\n");
	TreeNode first = pdec->child;
	TreeNode second = first->next;
	
	SymNode spec = Specifier(first);
	SymNode res = VarDec(second, spec, IN_PARAM);
	res->param_next = NULL;
	if(check_sym_table(res->name) != NULL)
		printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", res->lineno, res->name);
	else {
		insert_sym_table(res);
	}
	return res;
}

void CompSt(TreeNode comp, SymNode sn) {
	// CompSt -> LC DefList StmtList RC
	printf("CompSt -> LC DefList StmtList RC\n");
	TreeNode second = comp->child->next;
	TreeNode third = second->next;
	DefList(second, IN_BASIC);
	StmtList(third, sn);
}

void StmtList(TreeNode stmtl, SymNode sn) {
	if(stmtl->child != NULL) {
		// StmtList -> Stmt StmtList
		printf("StmtList -> Stmt StmtList\n");
		Stmt(stmtl->child, sn);
		StmtList(stmtl->child->next, sn);
	}
	// StmtList -> empty
}

void Stmt(TreeNode stmt, SymNode sn) {
	TreeNode first = stmt->child;
	if(strcmp(first->unit, "Exp") == 0) {	
		// Stmt -> Exp SEMI
		printf("Stmt -> Exp SEMI\n");
		Exp(first);
	}
	else if(strcmp(first->unit, "CompSt")) {
		// Stmt -> CompSt
		printf("Stmt -> CompSt\n");
		CompSt(first, sn);
	}
	else if(strcmp(first->unit, "RETURN")) {
		// Stmt -> RETURN Exp SEMI
		printf("Stmt -> RETURN Exp SEMI\n");
		SymNode returnType = Exp(first->next);
		if(!match_type(returnType->type, sn->type))
			printf("Error type 8 at Line %d: Type mismatched for return.\n", stmt->lineno);
	}
	else if(strcmp(first->unit, "IF")) {
		// Stmt -> IF LP Exp RP Stmt
		printf("Stmt -> IF LP Exp RP Stmt\n");
		TreeNode third = first->next->next;
		TreeNode fifth = third->next->next;
		SymNode exp = Exp(third);
		Stmt(fifth,sn);
		if(fifth->next != NULL) {
			// Stmt -> IF LP Exp RP Stmt ELSE Stmt
			printf("Stmt -> IF LP Exp RP Stmt ELSE Stmt\n");
			Stmt(fifth->next->next, sn);
		}
	}
	else if(strcmp(first->unit, "WHILE")) {
		// Stmt -> WHILE LP Exp RP Stmt
		printf("Stmt -> WHILE LP Exp RP Stmt\n");
		TreeNode third = first->next->next;
		TreeNode fifth = third->next->next;
		SymNode exp = Exp(third);
		Stmt(fifth, sn);
	}
}

SymNode DefList(TreeNode defl, int flag) {
	if(defl->child == NULL)
	{	// DefList -> empty
		printf("DefList -> empty\n");
		return NULL;
	}
	else {
		// DefList -> Def DefList
		printf("DefList -> Def DefList\n");
		TreeNode first = defl->child;
		TreeNode second = first->next;
		SymNode def = Def(first, flag);
		if(def == NULL)
			return NULL;
		SymNode res = DefList(second, flag);
		SymNode p = def;
	
		while(p->struct_next != NULL)
			p = p->struct_next;
		p->struct_next = res;
		return def;
	}
}

SymNode Def(TreeNode def, int flag) {
	// Def -> Specifier DecList SEMI
	printf("Def -> Specifier DecList SEMI\n");
	TreeNode first = def->child;
	TreeNode second = first->next;
	SymNode spec = Specifier(first);
	return DecList(second, spec, flag);
}

SymNode DecList(TreeNode decl, SymNode sn, int flag) {
	// DecList -> Dec
	printf("DecList -> Dec\n");
	TreeNode first = decl->child;
	SymNode res = Dec(first, sn, flag);
	if(first->next != NULL) {
		// DecList -> Dec COMMA DecList
		printf("DecList -> Dec COMMA DecList\n");
		res->struct_next = DecList(first->next->next, sn, flag);
	}
	return res;
}

SymNode Dec(TreeNode dec, SymNode sn, int flag) {
	// Dec -> VarDec
	printf("Dec -> VarDec\n");
	TreeNode first = dec->child;
	SymNode res = VarDec(first, sn, flag);
	if(first->next != NULL && flag == IN_STRUCT) {
		// Dec -> VarDec ASSIGNOP Exp
		printf("Dec -> VarDec ASSIGNOP Exp\n");
		// But it seems to be illegal to initialize VarDec in one structure...
		SymNode exp = Exp(first->next->next);
		printf("Error type 15 at line %d: initialized variable in field \"%s\".\n", first->lineno, res->name);
	}
	return res;
}

SymNode Exp(TreeNode exp)
{
	TreeNode first = exp->child;
	TreeNode second = first->next;
	SymNode res = malloc(sizeof(struct SymNode_));
	
	
	
	if(strcmp(first->unit,"Exp")==0)
	{
		TreeNode third = second->next;

		SymNode t = Exp(first);
		SymNode type1 = Exp(third);
		if(t == NULL || type1 == NULL)//
			return NULL;
		//Exp->Exp ASSIGNOP Exp
		if(strcmp(second->unit, "ASSIGNOP") == 0)
		{
			printf("Exp->Exp ASSIGNOP Exp\n");
			//t在赋值号左边，type1在赋值号后边
			if(!t->CanBeAss )//赋值号左边出现一个只有右值的表达式
				printf( "Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", first->lineno );
			if(!match_type(t->type,type1->type))//赋值号两边的表达式类型不匹配
				printf( "Error type 5 at line %d: Type mismatched for assignment.\n", first->lineno );
		}
		//Exp->Exp PLUS Exp| Exp -> Exp MINUS Exp| Exp -> Exp STAR Exp|Exp -> Exp DIV Exp|Exp -> Exp RELOP Exp

		else if(strcmp(second->unit,"PLUS") == 0||strcmp(second->unit, "MINUS") == 0||strcmp(second->unit,"STAR")==0||strcmp(second->unit,"DIV")==0||strcmp(second->unit,"RELOP")==0)
		{
			printf("Exp->Exp PLUS Exp| Exp -> Exp MINUS Exp| Exp -> Exp STAR Exp|Exp -> Exp DIV Exp|Exp -> Exp RELOP Exp\n");
			if(t->type->kind != BASIC||type1->type->kind != BASIC)//操作数类型与操作符不匹配
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			if(t->type->u.basic != type1->type->u.basic)//操作数类型不匹配
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			res->type = t->type;
			if(strcmp(second->unit,"RELOP") == 0)
				res->type->u.basic = 0;
			res->CanBeAss = false;
			return res;
		}
		//Exp : Exp AND Exp
		//Exp : Exp OR Exp
		else if(strcmp(second->unit,"AND")==0||strcmp(second->unit,"OR") == 0)
		{
			printf("Exp AND Exp||Exp : Exp OR Exp\n");
			if(t->type->kind != BASIC||type1->type->kind != BASIC)
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			if(t->type->u.basic != 0 || type1->type->u.basic != 0)
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			res->type = t->type;
			res->CanBeAss = false;
			return res;
		}
		//Exp : Exp LB Exp RB
		else if(strcmp(second->unit,"LB") == 0)
		{
			printf("Exp : Exp LB Exp RB\n");
			if(t->type->kind != ARRAY)
			{
				printf( "Error type 10 at line %d: \"%s\" is not an array.\n", first->lineno, t->name );
				return NULL;
			}
			if(type1->type->kind != BASIC || type1->type->u.basic != 0)
			{
				printf( "Error type 12 at line %d: \"%s\" is not an integer.\n", first->lineno , third->child->name );
				return NULL;
			}
			Type p = t->type->u.array.element;
			res->type = p;
			if(p->u.array.element == NULL)
			{
				res = p->u.array.arrlist;
			}
			res->CanBeAss = true;
			return res;
		}
		else if(strcmp(second->unit,"DOT") == 0)
		{
			//Exp->Exp DOT ID
		printf("Exp->Exp DOT ID\n");
		SymNode t = Exp(first);
		if(t == NULL)
			return NULL;
		if(t->type->kind != STRUCT)//13.对非结构体型变量使用“.”操作符
		{
			printf( "Error type 13 at line %d: Illegal use of \".\".\n", first->lineno );
			return NULL;
		}
		//14.访问结构体中未定义过的域
		SymNode p=t->type->u.structure->struct_next;
		while(p->name!=NULL)
		{
			if(strcmp(p->name, second->next->name)==0)
			{
				res = p;
				res->CanBeAss = true;//CanBeAss是否能被赋值
				return res;
			}
			p=p->struct_next;
		}
		printf( "Error type 14 at line %d: Non-existent field \"%s\".\n", first->lineno, second->next->name );
		return NULL;
		}
	}
	else if(strcmp(first->name, "LP") == 0)
	{
		return Exp(first->next);
	}
	//Exp : MINUS Exp
	//Exp : NOT Exp
	else if(strcmp(first->name, "MINUS") == 0 || strcmp(first->name, "NOT") == 0)
	{ 
		printf("Exp : MINUS Exp||Exp : NOT Exp\n");
		SymNode type1 = Exp(first);
		if(type1 == NULL)
			return NULL;
		if(type1->type->kind != BASIC)
		{
			printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
			return NULL;
		}
		if(type1->type->u.basic != 0)
		{
			printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
			return NULL;
		}
		res->type = type1->type;
		res->CanBeAss = false;
		return res;
		
	}
	else if(strcmp(first->name, "ID") == 0)
	{
		//Exp : ID
		printf("Exp : ID\n");
		if(second == NULL)
		{
			res = check_sym_table(first->name);
			if(res == NULL)
			{
				printf( "Error type 1 at line %d: Undefined variable \"%s\".\n", first->lineno, first->name );
				return NULL;
			}
			res->CanBeAss = true;
			return res;	
		}

		TreeNode third = second->next;
		//Exp : ID LP Args RP
		//Exp : ID LP RP
		printf("Exp : ID LP Args RP||Exp : ID LP RP\n");
		FuncNode func = check_func_table(first->name);
		SymNode sym = check_sym_table(first->name);
		if(sym != NULL&&func == NULL)
		{
			printf( "Error type 11 at line %d: \"%s\" is not a function.\n", first->lineno, first->name );
			return NULL;
		}
		if(sym == NULL&&func == NULL)
		{
			printf( "Error type 2 at line %d: Undefined function \"%s\".\n", first->lineno, first->name );
			return NULL;
		}
		if(strcmp(third->name, "Args")==0)
		{
			SymNode params = Args(third);
			if(use_func(params,func->params) == 0)//调用函数
			{
				printf( "Error type 9 at line %d: Function \"%s(", first->lineno, func->name );
				print_type( func->params );
				printf( ")\" is not applicable for arguments \"(");
				print_type(params);
				printf( ")\".\n" );
			}
		}
		else
		{
			if(func->params != NULL)
			{
				printf( "Error type 9 at line %d: Function \"%s(", first->lineno, func->name );
				print_type( func->params );
				printf( ")\" is not applicable for arguments \"()\".\n" );
			}
		}
		res = func->returnType;
		res->CanBeAss = false;
		return res;
	}
	else if(strcmp(first->name, "INT") == 0)
	{	
		//Exp : INT
		printf("Exp : INT\n");
		Type type = malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = 0;
		res->type = type;
		res->CanBeAss = false;
		return res;
	}
	else if(strcmp(first->name, "FLOAT") == 0)
	{
		//Exp : FLOAT
		printf("Exp : FLOAT\n");
		Type type = malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = 1;
		res->type = type;
		res->CanBeAss = false;
		return res;
	}
	return NULL;
}

SymNode Args(TreeNode arg)
{
	SymNode res=Exp(arg->child);
	TreeNode second=arg->child->next;
	//Args->Exp COMMA Args
	printf("Args->Exp COMMA Args\n");
	if(second!= NULL)
	{
		res->param_next = Args(second->next);
	}
	//Args->Exp
	printf("Args->Exp\n");
	return res;
}
