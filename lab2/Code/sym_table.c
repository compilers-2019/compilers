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

int check_type(SymNode a, SymNode b)//检查类型（类型匹配）
{
	if(a->type->kind == BASIC && b->type->kind == BASIC)
	{
		if(a->type->u.basic == b->type->u.basic)
			return 1;
	}
	else if(a->type->kind == ARRAY && b->type->kind == ARRAY)
	{
		int count_a = 0;
		int count_b = 0;
		for(Type p = a->type->u.array.element; p != NULL; p = p->u.array.element);
			count_a++;
		for(Type p = b->type->u.array.element; p != NULL; p = p->u.array.element);
			count_b++;
		if(a != b)
			return 0;
		if(check_type(a->type->u.array.arrlist,b->type->u.array.arrlist)==0)
			return 0;
	}
	else if(a->type->kind == STRUCTURE && b->type->kind == STRUCTURE)
	{
		int flag = 1;
		int a_count = 0;
		int b_count = 0;
		//printf("dename: %s \n",b->name);
		for(SymNode p = a->type->u.structure; p != NULL; p = p->struct_next)
			a_count++;
		for(SymNode q = b->type->u.structure; q != NULL; q = q->struct_next)
			b_count++;
		//printf("dename: %d \n",a_count);
		//printf("dde:%d \n", b_count);
		if(a_count != b_count)
			return 0;
		//printf("dename: %s \n",a->name);
		SymNode p = a->type->u.structure;
		SymNode q = b->type->u.structure;
		for(; p != NULL&&p->name!=NULL&&q->name!=NULL&&q!=NULL; p = p->struct_next,q = q->struct_next)
		{
			//printf("cename: %d \n",check_type(p,q));
			if(check_type(p,q)==0)
				flag = 0;
		}
		return flag;
	}
	return 1;
}

int check_params(SymNode a,SymNode b)//检查参数（）
{
	//printf("dename: %s \n",a->name);
	if(a == NULL && b == NULL)
		return 1;
	int count_a = 0;
	int count_b = 0;
	//printf("pename: %s \n",a->name);
	for(SymNode p = a; p != NULL;p = p->param_next)
	{
		//printf("penamddce:\n");
		count_a++;
	}
	//printf("penaxsme: \n");
	for(SymNode p = b; p != NULL;p = p->param_next)
		count_b++;
	if(count_a != count_b)
		return 0;
	//printf("pename: %s \n",a->name);
	SymNode p = a; SymNode q = b;
	for(; p != NULL&&q != NULL;p = p->param_next, q = q->param_next)
	{
		if(check_type(p,q)==0||strcmp(p->name,q->name)==1)
			return 0;
	}	 
	return 1;
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
		if(check_type(p,q)==0)
			return 0;
	}	 
	return 1;
}

void print_type(SymNode a)
{
	for(SymNode p = a; p != NULL;p = p->param_next)
	{
		if(p->type->kind == BASIC)
		{
			if(p->type->u.basic == INT_TYPE)//////////INT_TYPE未定义
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
		else if(p->type->kind == STRUCTURE)
		{
			printf("struct %s",p->name);
		}
		if(p->param_next != NULL)
			printf(",");
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

SymNode check_stru_table(char* name)//检查结构体定义
{
	unsigned int num = hash_pjw(name);
	SymNode s=sym_table[num];
	while(s!=NULL){
		if(strcmp( s->name, name)==0&&(s->struct_next != NULL))
			return s;
		s=s->hash_next;
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

void ExtDef(TreeNode ext) {//不完整
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
		SymNode res;//SymNode res = malloc(sizeof(struct SymNode_));?
		TreeNode first = spec->child;
		if(strcmp(first->unit, "StructSpecifier"))//返回0?取反
				res = StructSpecifier(first);
		else {
				res = malloc(sizeof(struct SymNode_));
				Type t = malloc(sizeof(struct Type_));
				t->kind = BASIC;
				if(strcmp(first->name, "int"))//==0
						t->u.basic = 0;
				else//float
						t->u.basic = 1;
				res->type = t;
		}
		return res;
}
SymNode StructSpecifier(TreeNode stru){
	TreeNode second = stru->child->next; //OptTag or Tag
	TreeNode third = second->child;
	SymNode res = malloc(sizeof(SymNode));
	if(strcmp(stru->child->next->unit,"Tag")==0)//Tag
	{
		res = check_stru_table(third->name);
		if(res == NULL)
			printf( "Error type 17 at line %d: Undefined structure \"%s\".\n", second->lineno,second->child->name );
	}		
	else//OptTag LC DefList RC
	{
		Type t = malloc(sizeof(struct Type_));
		t->kind = STRUCTURE;
		res->type = t;
		res->struct_next = DefList(second->next->next, IN_STRUCT);//IN_STRUCT=1
		SymNode q = res->struct_next;
		SymNode p = q->struct_next;
		while(q != NULL)
		{
			while(p !=NULL && p->name!=NULL)
			{
				if(strcmp(q->name,p->name)==0)
				{
					printf( "Error type 15 at line %d: Redefined field \"%s\".\n", p->lineno,p->name );
					break;
				}
				p=p->struct_next;
			}
			q = q->struct_next;
		}
		if( strcmp(second->unit,"OptTag")==0)
		{	
			char* id = malloc(strlen(third->name));
			strcpy(id,third->name);
			res->name = id;
			if(check_stru_table(third->name) != NULL)
			{
				printf( "Error type 16 at line %d: Duplicated name \"%s\".\n", second->lineno,second->child->name );
				return res;
			}
			insert_sym_table(res);
		}
	}	
	return res;
}

SymNode VarDec(TreeNode var, SymNode spec, int flag)
{
	TreeNode first = var->child;
	SymNode res = malloc(sizeof(struct SymNode_));
	memset(res, 0, sizeof(struct SymNode_));
	res->name = first->name;
	res->lineno = first->lineno;
	//VarDec : ID
	if(strcmp(first->unit,"ID")==0)
	{
		Type type = malloc(sizeof(struct Type_));
		if(spec->type->kind == BASIC){
			type->kind = BASIC;
			type->u.basic = spec->type->u.basic;
		}
		else{
			type->kind = STRUCTURE;
			type->u.structure = spec;
		}
		res->type = type;
	}
	//VarDec : VarDec LB INT RB
	else
	{
		TreeNode third = first->next->next;
		res = VarDec(first, spec,flag);
		res->type->kind = ARRAY;
		res->type->u.array.size = atoi(third->name);	
		Type arr = malloc(sizeof(struct Type_));
		arr->kind = ARRAY;
		arr->u.array.arrlist = spec;
		arr->u.array.size = atoi(third->name);	
		if(res->type->u.array.element == NULL){
			res->type->u.array.element = arr;
			return res;
		}		
		Type p = res->type->u.array.element;
		for(; p->u.array.element !=NULL; p = p->u.array.element);
		p->u.array.element = arr;	
	}
	if(flag == IN_STRUCT || flag == IN_PARAMS)//IN_STRUCT=1,IN_PARAM=2
		return res;
	else
	{
		if(insert_sym_table(res) == SYM_REDEFINE)//////////SYM_REDEFINE未定义
			printf( "Error type 3 at line %d: Redefined variable \"%s\".\n", first->lineno, first->name );
	}
	return res;		
}

FuncNode FunDec(TreeNode func,SymNode spec)
{
	//FunDec : ID LP VarList RP
	//FunDec : ID LP RP
	TreeNode first = func->child; //ID
	TreeNode third = first->next->next; //VarList or RP
	
	FuncNode res = malloc(sizeof(struct FuncNode_));
	//memset(res, 0, sizeof(struct FuncNode_));
	res->name = first->name;
	res->lineno = first->lineno;
	res->returnType = spec;
	if(strcmp(third->unit,"VarList")==0)
	{

		//printf("name_var: %s \n",third->name);
		res->paramType = VarList(third);
	}
	return res;
}

SymNode VarList(TreeNode varl)
{
	//printf("name_var: %s \n",varl->name);
	//VarList : ParamDec COMMA VarList
	//VarList : ParamDec
	TreeNode first = varl->child;//ParamDec
	TreeNode second = first->next;//COMMA or NULL

	SymNode res = ParamDec(first);
	SymNode var;
	if(second != NULL)
	{
		var = VarList(second->next);
		res->param_next = var;
	}
	//printf("name_var: %s \n",res->name);
	return res;
}

SymNode ParamDec(TreeNode par)
{
	//ParamDec : Specifier VarDec
	TreeNode first = par->child;
	TreeNode second = first->next;

	SymNode spec = Specifier(first);
	SymNode res = VarDec(second, spec, IN_PARAMS);//IN_PARAMs 2
	res->param_next = NULL;
	insert_sym_table(res) ;
	return res;
}

void CompSt(TreeNode comp, SymNode spec)
{
	//CompSt : LC DefList StmtList RC
	TreeNode second = comp->child->next;
	TreeNode third = second->next;
	if(strcmp(second->unit,"DefList")==0)
	{
		DefList(second, 0);
		if(strcmp(third->unit,"StmtList") == 0)
		{	printf("ne: %s \n", third->unit);	
			StmtList(third,spec);
		}
	}
	if(strcmp(second->unit,"StmtList") == 0)
		StmtList(second, spec);
}

void StmtList(TreeNode stmtl, SymNode spec)
{
	//StmtList : Stmt StmtList
	TreeNode first = stmtl->child;
	TreeNode second = first->next;
	Stmt(first, spec);
	if(second != NULL)
		StmtList(second, spec);
}

void Stmt(TreeNode stm,SymNode spec)
{
	TreeNode first = stm->child;
	//Stmt : Exp SEMI
	if(strcmp(first->unit,"Exp") == 0)
	{
		Exp(first);
	}
	//Stmt : CompSt
	else if(strcmp(first->unit,"CompSt") == 0)
	{
		CompSt(first, spec);
	}
	//Stmt : RETURN Exp SEMI
	else if(strcmp(first->unit,"RETURN") == 0)
	{
		SymNode return_type = Exp(first->next);
		if(check_type(return_type, spec) == 0)
			printf( "Error type 8 at line %d: Type mismatched for return.\n", first->lineno );
	}
	//Stmt : IF LP Exp RP Stmt
	//Stmt : IF LP Exp RP Stmt ELSE Stmt
	else if(strcmp(first->unit,"IF") == 0)
	{
		TreeNode third = first->next->next;
		SymNode exp = Exp(third);
		TreeNode fifth = third->next->next;
		//printf("name: %s \n", fifth->name);	
		Stmt(fifth,spec);
		if(fifth->next != NULL)
			Stmt(fifth->next->next,spec);
	}
	//Stmt : WHILE LP Exp RP Stmt
	else if(strcmp(first->unit, "WHILE")==0)
	{
		TreeNode third = first->next->next;
		SymNode exp = Exp(third);
		if(exp->type->kind != BASIC||exp->type->u.basic != INT_TYPE)////////INT_TYPE未定义
			printf( "Error type 12 at line %d: \"%s\" is not an integer.\n", first->lineno , third->child->name );
		TreeNode fifth = third->next->next;
		Stmt(fifth,spec);
	}
		
}

SymNode DefList(TreeNode defl, int flag)
{	
	//printf("name: %s \n", defl->name);	
	//DefList : Def DefList
	TreeNode first = defl->child;//Def
	TreeNode second = first->next;//DefList

	SymNode res = malloc(sizeof(struct SymNode_));
	//memset(res, 0, sizeof(struct SymNode_));
	SymNode def = malloc(sizeof(struct SymNode_));
	//memset(def, 0, sizeof(struct SymNode_));

	def = Def(first, flag);
	if(def == NULL)
		return NULL;
	if(second != NULL)
	{
		//printf("name:  \n");
		res = DefList(second, flag);
	}
	//printf("name:\n");

	SymNode p = def;
	for(; p->struct_next != NULL; p = p->struct_next);
	p->struct_next = res;
	return def;
	
}
 
SymNode Def(TreeNode def, int flag)
{
	//printf("name: %s \n", def->name);
	SymNode res = malloc(sizeof(struct SymNode_));
	//memset(res, 0, sizeof(struct SymNode_));

	//Def : Specifier DecList SEMI
	TreeNode first = def->child;	//Specifier
	TreeNode second = first->next;	//DecList

	SymNode spec = Specifier(first);
	if(spec == NULL)
		return NULL;
	res = DecList(second, spec, flag);
	return res;
}

SymNode DecList(TreeNode decl, SymNode spec, int flag)
{
	//printf("name: %s \n", decl->name);
	SymNode res = malloc(sizeof(struct SymNode_));
	memset(res, 0, sizeof(struct SymNode_));

	//DecList : Dec
	TreeNode first = decl->child;
	TreeNode second = first->next;

	res = Dec(first, spec, flag);	

	//DecList : Dec COMMA DecList
	if(second != NULL) {
		TreeNode third = first->next->next;
		res->struct_next = DecList(third, spec, flag);
	}
	return res;	
}

SymNode Dec( TreeNode dec, SymNode spec, int flag)
{
	//printf("name: %s \n", dec->name);
	SymNode res = malloc(sizeof(struct SymNode_));
	//memset(res, 0, sizeof(struct SymNode_));

	TreeNode first = dec->child;
	TreeNode second = first->next;

	//Dec : VarDec
	res = VarDec(first, spec, flag);
	
	//Dec : VarDec ASSIGNOP Exp
	if(second != NULL &&flag == IN_STRUCT)//IN_SRTUCT=1
		printf("Error type 15 at line %d: initialized variable in field \"%s\".\n", first->lineno, res->name);
	return res;
}

SymNode Exp(TreeNode exp)
{
	TreeNode first = exp->child;
	TreeNode second = first->next;
	
	SymNode res = malloc(sizeof(struct SymNode_));
	//Exp : Exp DOT ID
	if(strcmp(first->unit,"Exp")==0 && strcmp(second->unit,"DOT") == 0)
	{
		SymNode typeL = Exp(first);
		if(typeL == NULL)
			return NULL;
		if(typeL->type->kind != STRUCTURE)
		{
			printf( "Error type 13 at line %d: Illegal use of \".\".\n", first->lineno );
			return NULL;
		}
		//printf("name: %s \n", first->name);
		for(SymNode p = typeL->type->u.structure->struct_next; p->name != NULL; p=p->struct_next)
		{
			//printf("name: %s \n", p->name);
			if(strcmp(p->name, second->next->name)==0)
			{
				res = p;
				res->CanBeAss = CAN_BE_ASSIGN;
				return res;
			}
		}
		printf( "Error type 14 at line %d: Non-existent field \"%s\".\n", first->lineno, second->next->name );
		return NULL;
	}
	else if(strcmp(first->unit,"Exp")==0)
	{
		TreeNode third = second->next;

		SymNode typeL = Exp(first);
		SymNode typeR = Exp(third);
		if(typeL == NULL || typeR == NULL)
			return NULL;
		//Exp : Exp ASSIGNOP Exp
		if(strcmp(second->unit, "ASSIGNOP") == 0)
		{
			if(typeL->CanBeAss != CAN_BE_ASSIGN)
				printf( "Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", first->lineno );
			if(check_type(typeL,typeR) == 0)
				printf( "Error type 5 at line %d: Type mismatched for assignment.\n", first->lineno );
		}
		//Exp : Exp PLUS Exp
		//Exp : Exp MINUS Exp
		//Exp : Exp STAR Exp
		//Exp : Exp DIV Exp
		//Exp : Exp RELOP Exp
		else if(strcmp(second->unit,"PLUS") == 0||strcmp(second->unit, "MINUS") == 0||strcmp(second->unit,"STAR")==0||strcmp(second->unit,"DIV")==0||strcmp(second->unit,"RELOP")==0)
		{
			if(typeL->type->kind != BASIC||typeR->type->kind != BASIC)
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			if(typeL->type->u.basic != typeR->type->u.basic)
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			res->type = typeL->type;
			if(strcmp(second->unit,"RELOP") == 0)
				res->type->u.basic = INT_TYPE;//////////INT_TYPE未定义
			res->CanBeAss = 0;
			return res;
		}
		//Exp : Exp AND Exp
		//Exp : Exp OR Exp
		else if(strcmp(second->unit,"AND")==0||strcmp(second->unit,"OR") == 0)
		{
			if(typeL->type->kind != BASIC||typeR->type->kind != BASIC)
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			if(typeL->type->u.basic != INT_TYPE || typeR->type->u.basic != INT_TYPE)
			{
				printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
				return NULL;
			}
			res->type = typeL->type;
			res->CanBeAss = 0;
			return res;
		}
		//Exp : Exp LB Exp RB
		else if(strcmp(second->unit,"LB") == 0)
		{
			//printf("name: %s \n", first->name);
			if(typeL->type->kind != ARRAY)
			{
				printf( "Error type 10 at line %d: \"%s\" is not an array.\n", first->lineno, typeL->name );
				return NULL;
			}
			if(typeR->type->kind != BASIC || typeR->type->u.basic != INT_TYPE)
			{
				printf( "Error type 12 at line %d: \"%s\" is not an integer.\n", first->lineno , third->child->name );
				return NULL;
			}
			Type p = typeL->type->u.array.element;
			res->type = p;
			if(p->u.array.element == NULL)
			{
				res = p->u.array.arrlist;
			}
			res->CanBeAss = CAN_BE_ASSIGN;
			return res;
		}
	}
	else if(strcmp(first->unit, "LP") == 0)
	{
		return Exp(first->next);
	}
	//Exp : MINUS Exp
	//Exp : NOT Exp
	else if(strcmp(first->unit, "MINUS") == 0 || strcmp(first->unit, "NOT") == 0)
	{ 
		SymNode typeR = Exp(first);
		if(typeR == NULL)
			return NULL;
		if(typeR->type->kind != BASIC)
		{
			printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
			return NULL;
		}
		if(typeR->type->u.basic != INT_TYPE)
		{
			printf( "Error type 7 at line %d: Type mismatched for operands.\n", first->lineno );
			return NULL;
		}
		res->type = typeR->type;
		res->CanBeAss = 0;
		return res;
		
	}
	else if(strcmp(first->unit, "ID") == 0)
	{
		//printf("unit: %s \n", first->unit);
		//Exp : ID
		if(second == NULL)
		{
			res = check_sym_table(first->name);
			if(res == NULL)
			{
				printf( "Error type 1 at line %d: Undefined variable \"%s\".\n", first->lineno, first->name );
				return NULL;
			}
			res->CanBeAss = CAN_BE_ASSIGN;
			return res;	
		}

		TreeNode third = second->next;
		//Exp : ID LP Args RP
		//Exp : ID LP RP
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
		if(strcmp(third->unit, "Args")==0)
		{
			SymNode params = Args(third);
			//printf("unit: %s \n", first->unit);
			//printf("unit: %d \n", use_func(params,func->paramType));
			if(use_func(params,func->paramType) == 0)
			{
				printf( "Error type 9 at line %d: Function \"%s(", first->lineno, func->name );
				print_type( func->paramType );
				printf( ")\" is not applicable for arguments \"(");
				print_type(params);
				printf( ")\".\n" );
			}
		}
		else
		{
			if(func->paramType != NULL)
			{
				printf( "Error type 9 at line %d: Function \"%s(", first->lineno, func->name );
				print_type( func->paramType );
				printf( ")\" is not applicable for arguments \"()\".\n" );
			}
		}
		res = func->returnType;
		res->CanBeAss = 0;
		return res;
	}
	else if(strcmp(first->unit, "INT") == 0)
	{	
		//Exp : INT
		Type type = malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = INT_TYPE;//////////INT_TYPE未定义
		res->type = type;
		res->CanBeAss = 0;
		return res;
	}
	else if(strcmp(first->unit, "FLOAT") == 0)
	{
		//Exp : FLOAT
		Type type = malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = FLOAT_TYPE;
		res->type = type;
		res->CanBeAss = 0;
		return res;
	}
	return NULL;
}

SymNode Args(TreeNode arg)
{

	TreeNode first = arg->child;
	TreeNode second = first->next;
	//printf("unit: %s \n", first->unit);
	SymNode res = Exp(first);
	//printf("unit: %s \n", first->unit);
	if(second != NULL)
	{
		res->param_next = Args(second->next);
		//printf("name_arg: %s \n", first->unit);
	}
	//Args : Exp COMMA Args
	//Args : Exp
	return res;
}