#include "intercode.h"

Operand new_temp() {
	temp_num++;
	Operand t = new_op(TEMP);
	temp_table[temp_num] = t;
	t->u.temp.temp_no = temp_num;
	strcpy(t->u.temp.name, "t");
	char str[4];
	// itoa(t->num, str, 10);
	sprintf(str, "%d", t->u.temp.temp_no);
	strcat(t->u.temp.name, str);
	t->u.temp.prev = NULL;
	t->u.temp.next = NULL;
	return t;
}

Operand new_label() {
	label_num++;
	Operand l = new_op(O_LABEL);
	label_table[label_num] = l;
	l->u.label.label_no = label_num;
	strcpy(l->u.label.name, "label");
	char str[4];
	// itoa(t->num, str, 10);
	sprintf(str, "%d", l->u.label.label_no);
	strcat(l->u.label.name, str);
	return l;
}

Operand new_op(enum O_KIND kind, ...) {
	Operand op = malloc(sizeof(struct Operand_));
	op->kind = kind;
	va_list argptr;
	va_start(argptr, kind);
	switch(kind) {
		case CONSTANT:
			op->u.value = va_arg(argptr, int);
			break;
		case VARIABLE:
			op->u.var_no = va_arg(argptr, int);
			break;
		default: break;
	}
	return op;
}

InterCode new_code(enum I_KIND kind, ...) {
	InterCode code = malloc(sizeof(struct InterCode_));
	code->kind = kind;
	code->prev = NULL;
	code->next = NULL;
	va_list argptr;
	va_start(argptr, kind);
	switch(kind) {
		case FUNC:
			strcpy(code->u.func.name, va_arg(argptr, char*));
			break;
		case LABEL:
		case GOTO:
		case RETURN:
		case ARG:
		case PARAM:
		case READ:
		case WRITE: 
			code->u.single.op = va_arg(argptr, Operand);
			break;
		case ASSIGN:
		case CITE:
		case GETPOINTER:
		case ASSIGNPOINTER:
			code->u.assign.result = va_arg(argptr, Operand);
			code->u.assign.op = va_arg(argptr, Operand); 
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV:
			code->u.binop.result = va_arg(argptr, Operand);
			code->u.binop.op1 = va_arg(argptr, Operand);
			code->u.binop.op2 = va_arg(argptr, Operand);
			break;
		case IFGOTO: 
			code->u.ifgoto.re1 = va_arg(argptr, Operand);
			code->u.ifgoto.re2 = va_arg(argptr, Operand);;
			code->u.ifgoto.label = va_arg(argptr, Operand);
			code->u.ifgoto.kind = va_arg(argptr, int);
			break;
		case DEC:
			code->u.dec.dec = va_arg(argptr, Operand);
			code->u.dec.size = va_arg(argptr, int); 
			break;
		case CALL:
			code->u.call.ret = va_arg(argptr, Operand);
			strcpy(code->u.call.name, va_arg(argptr, char*)); 
			break;
		default: break;
	}
	//print_code(code);
	return code;
}

// InterCode merge_code(int n,InterCode code1,InterCode code2, ...)
// {
// 	va_list parg;
// 	va_start(parg,code2);
// 	InterCode cur = code1;
// 	while(cur->next != NULL) {
// 		cur = cur->next;
// 	}
// 	cur->next = code2;
// 	code2->prev = cur;
// 	n=n-2;
// 	while(n>0)
// 	{
// 		InterCode code = va_arg(parg, InterCode);
// 		InterCode cur = code1;
// 		while(cur->next != NULL) 
// 		{
// 			cur = cur->next;
// 		}
// 		cur->next = code;
// 		code->prev = cur;
// 		n--;
// 	}
// 	va_end(parg);
// 	return code1;

// }
InterCode merge_code(int n,InterCode code1,InterCode code2, ...)
{
	if(n==2)
	{
		if(code1==NULL)
		{
			if(code2==NULL)
				return NULL;
			else
			{
				return code2;
			}
			
		}
		else
		{
			if(code2==NULL)
				return code1;
			else
			{
				InterCode cur = code1;
				while(cur->next != NULL) {
					cur = cur->next;
				}
				cur->next = code2;
				code2->prev = cur;
				return code1;
			}
			
		}	
	}
	else 
	{
		va_list parg;
		va_start(parg,code2);
		if(code1==NULL)
		{
			if(code2==NULL)
			{
				//code1=NULL;
			}
			else
			{
				code1=code2;
			}
		}
		else
		{
			if(code2==NULL)
			{

			}
			else
			{
				InterCode cur = code1;
				while(cur->next != NULL) 
				{
					cur = cur->next;
				}
				cur->next = code2;
				code2->prev = cur;
			}
		}
		
		n=n-2;
		while(n>0)
		{
			InterCode code = va_arg(parg, InterCode);
			if(code==NULL)
			{
				n--;
			}
			else
			{		
				if(code1==NULL)
				{	
					code1=code;
					n--;
				}
				else
				{
					InterCode cur = code1;
					while(cur->next != NULL) 
					{
						cur = cur->next;
					}
					cur->next = code;
					code->prev = cur;
					n--;
				}
			}	
		}
		
		va_end(parg);
		return code1;	
	}
}

InterCode translate_Exp(TreeNode tr, Operand place) {
	printf("translate_Exp.\n");
	TreeNode first = tr->child;
	TreeNode second = first->next;
	InterCode res;
/*
	Operand label1=new_label();
	InterCode code2=new_code(LABEL,label1);
	InterCode code1=new_code(LABEL,label1);
	InterCode code3=new_code(LABEL,label1);
	if(merge_code(3,code1,code2,code3)==code1)
		printf("merge_codeOK!\n");
*/
	if(strcmp(first->unit, "INT") == 0) {
		// Exp -> INT
		int value = (int)first->val;
		Operand op = new_op(CONSTANT, value);
		// res->kind = ASSIGN;
		// res->u.assign.result = place;
		// res->u.assign.op = op;
		res = new_code(ASSIGN, place, op);
		return res;
	}
	else if(strcmp(first->unit, "ID") == 0) {
		if(second == NULL) {
			// Exp -> ID
			SymNode variable = check_sym_table(first->name);
			if(variable == NULL) {
				printf("Why NULL? Exp -> ID\n");
				return NULL;
			}
			else {
				Operand op = new_op(VARIABLE, variable->no);
				// res->kind = ASSIGN;
				// res->u.assign.result = place;
				// res->u.assign.op = op;
				res = new_code(ASSIGN, place, op);
				return res;
			}
		}
		else {
			FuncNode func = check_func_table(first->name);
			if(func == NULL) {
				printf("Why NULL? Exp -> ID LP (Args) RP\n");
				return NULL;
			}
			else {
				TreeNode third = second->next;
				if(strcmp(third->unit, "Args") == 0) {
					// Exp -> ID LP Args RP
					Operand arg_list = new_temp();
					InterCode code1 = translate_Args(third, arg_list);
					if(strcmp(func->name, "write") == 0) {
						InterCode code2 = new_code(WRITE, arg_list->u.temp.next);
						// printf("Args identified successfully.\n");
						// printf("WRITE %s\n", code2->u.single.op->u.temp.name);
						res = merge_code(2, code1, code2);
					}
					else {
						Operand cur = arg_list->u.temp.next;
						code1 = merge_code(2, code1, new_code(ARG, cur));
						while(cur->u.temp.next != NULL) {
							cur = cur->u.temp.next;
							code1 = merge_code(2, code1, new_code(ARG, cur));
						}
						res = merge_code(2, code1, new_code(CALL, place, func->name));
					}
					return res;
				}
				else {
					// Exp -> ID LP RP
					if(strcmp(func->name, "read") == 0) {
						res = new_code(READ, place);
					}
					else {
						res = new_code(CALL, place, func->name);
					}
					return res;
				}
			}
		}
	}
	else if(strcmp(first->unit, "MINUS") == 0) {
		// Exp -> MINUS Exp
		Operand t1 = new_temp(); 
		InterCode code1 = translate_Exp(second, t1);
		Operand zero = new_op(CONSTANT, 0);
		// res = new_code(SUB);
		// res->u.binop.result = place;
		// res->u.binop.op1 = zero;
		// res->u.binop.op2 = t1;
		res = new_code(SUB, place, zero, t1);
		return merge_code(2, code1, res);
	}
	else if(strcmp(first->unit, "NOT") == 0) {
		// Exp -> NOT Exp
		Operand label1 = new_label();
		Operand label2 = new_label();

		// code0
		Operand op0 = new_op(CONSTANT, 0);
		// res->kind = ASSIGN;
		// res->u.assign.result = place;
		// res->u.assign.op = op0;
		res = new_code(ASSIGN, place, op0);

		// code1
		InterCode code1 = translate_Cond(tr, label1, label2);

		// code2
		InterCode code2 = new_code(LABEL, label1);
		code2 = merge_code(2, code2, new_code(ASSIGN, place, new_op(CONSTANT, 1)));

		// code3
		InterCode code3 = new_code(LABEL, label2);

		return merge_code(4, res, code1, code2, code3);
	}
	else if(strcmp(first->unit, "Exp") == 0) {
		if((strcmp(second->unit, "RELOP") == 0) || (strcmp(second->unit, "AND") == 0) || (strcmp(second->unit, "OR") == 0)) {
			// Exp -> Exp RELOP/AND/OR Exp
			Operand label1 = new_label();
			Operand label2 = new_label();
			
			Operand op0 = new_op(CONSTANT, 0);
			res = new_code(ASSIGN, place, op0);

			// code1
			InterCode code1 = translate_Cond(tr, label1, label2);

			// code2
			InterCode code2 = new_code(LABEL, label1);
			code2 = merge_code(2, code2, new_code(ASSIGN, place, new_op(CONSTANT, 1)));

			// code3
			InterCode code3 = new_code(LABEL, label2);

			return merge_code(4, res, code1, code2, code3);
		}
		else if(strcmp(second->unit, "ASSIGNOP") == 0) {
			// Exp -> Exp ASSIGNOP Exp
			SymNode variable = check_sym_table(first->child->name);
			if(variable == NULL) {
				printf("Why NULL? Exp -> ID\n");
				return NULL;
			}
			else {
				Operand op = new_op(VARIABLE, variable->no);
				Operand t1 = new_temp();
				InterCode code1 = translate_Exp(second->next, t1);
				//printf("Here?\n");
				InterCode code2 = new_code(ASSIGN, op, t1);
				if(place != NULL) {
					code2 = merge_code(2, code2, new_code(ASSIGN, place, op));
				}
				res = merge_code(2, code1, code2);
				return res;
			}
		}
		else if((strcmp(second->unit, "PLUS") == 0) || (strcmp(second->unit, "MINUS") == 0) || (strcmp(second->unit, "STAR") == 0) || (strcmp(second->unit, "DIV") == 0)) {
			Operand t1 = new_temp();
			Operand t2 = new_temp();
			InterCode code1 = translate_Exp(first, t1);
			InterCode code2 = translate_Exp(second->next, t2);
			InterCode code3;
			if((strcmp(second->unit, "PLUS") == 0)) {
				// Exp -> Exp PLUS Exp
				code3 = new_code(ADD, place, t1, t2);
			}
			else if((strcmp(second->unit, "MINUS") == 0)) {
				// Exp -> Exp MINUS Exp
				code3 = new_code(SUB, place, t1, t2);
			}
			else if((strcmp(second->unit, "STAR") == 0)) {
				// Exp -> Exp STAR Exp
				code3 = new_code(MUL, place, t1, t2);
			}
			else {
				// Exp -> Exp DIV Exp
				code3 = new_code(DIV, place, t1, t2);
			}
			res = merge_code(3, code1, code2, code3);
			return res;
		}
	}
	else if(strcmp(first->unit, "LP") == 0) {
		// Exp -> LP Exp RP
		Operand t1 = new_temp();
		res = translate_Exp(second, t1);
		InterCode code2 = new_code(ASSIGN, place, t1);
		return merge_code(2, res, code2);
	}
	return NULL;
}

InterCode translate_Stmt(TreeNode tr)
{
	printf("translate_Stmt.\n");
	TreeNode first=tr->child;
	TreeNode second=first->next;
	//InterCode res;
	if(strcmp(first->unit,"Exp")==0)
	{
		//Stmt -> Exp SEMI
		return translate_Exp(first,NULL);
	}
	else if(strcmp(first->unit,"CompSt")==0)
	{
		//Stmt -> CompSt
		return translate_Compst(first);
	}
	else if(strcmp(first->unit,"RETURN")==0)
	{
		//Stmt -> RETURN Exp SEMI
		Operand t1=new_temp();
		//code1
		InterCode code1=translate_Exp(second,t1);
		//code2
		InterCode code2=new_code(RETURN,t1);
		//code2->u.single.op = t1;
		//res = new_code(ASSIGN, place, op0);
		return merge_code(2,code1,code2);
	}
	else if(strcmp(first->unit,"WHILE")==0)
	{
		//Stmt->WHILE LP Exp RP Stmt1
		Operand label1=new_label();
		Operand label2=new_label();
		Operand label3=new_label();
		//code1
		InterCode code1=translate_Cond(second->next,label2,label3);
		//code2
		InterCode code2=translate_Stmt(second->next->next->next);
		//[LABEL label1]
		InterCode temp1=new_code(LABEL,label1);
		//temp1->u.single.op = label1;
		//[LABEL label2]
		InterCode temp2=new_code(LABEL,label1);
		//temp2->u.single.op = label2;
		//[GOTO label1]
		InterCode temp3=new_code(GOTO,label1);
		//temp1->u.single.op = label1;
		//[LABEL label3]
		InterCode temp4=new_code(LABEL,label3);
		//temp4->u.single.op = label3;
		return merge_code(6,temp1,code1,temp2,code2,temp3,temp4);
	}
	else if(strcmp(first->unit,"IF")==0)
	{
		TreeNode third=second->next->next->next;//实际为第5位
		if(strcmp(third->unit,"Stmt")&&third->next==NULL)
		{
			//Stmt->IF LP Exp RP Stmt
			Operand label1=new_label();
			Operand label2=new_label();
		
			//code1
			InterCode code1=translate_Cond(second->next,label1,label2);
			//code2
			InterCode code2=translate_Stmt(second->next->next->next);
			//[LABEL label1]
			InterCode temp1=new_code(LABEL,label1);
			//temp1->u.single.op = label1;
			//[LABEL label2]
			InterCode temp2=new_code(LABEL,label2);
			//temp2->u.single.op = label2;
			return merge_code(4,code1,temp1,code2,temp2);
		}
		else
		{
			//Stmt->IF LP Exp RP Stmt ELSE Stmt
			Operand label1=new_label();
			Operand label2=new_label();
			Operand label3=new_label();
			//code1
			InterCode code1=translate_Cond(second->next,label1,label2);
			//code2
			InterCode code2=translate_Stmt(third);
			//code3
			InterCode code3=translate_Stmt(third->next->next);
			//[LABEL label1]
			InterCode temp1=new_code(LABEL,label1);
			//temp1->u.single.op = label1;
			//[LABEL label2]
			InterCode temp2=new_code(LABEL,label2);
			//temp2->u.single.op = label2;
			//[GOTO label3]
			InterCode temp3=new_code(GOTO,label3);
			//temp1->u.single.op = label3;
			//[LABEL label3]
			InterCode temp4=new_code(LABEL,label3);
			//temp4->u.single.op = label3;
			return merge_code(7,code1,temp1,code2,temp3,temp2,code3,temp4);
		}

	}
}

InterCode translate_Cond(TreeNode tr,Operand label_true,Operand label_false)
{
	printf("translate_Cond.\n");
	TreeNode first=tr->child;
	TreeNode second=first->next;
	if(strcmp(first->unit,"Exp")==0)
	{
		if(strcmp(second->unit,"RELOP")==0)
		{
			Operand t1=new_temp();
			Operand t2=new_temp();
			//code1
			InterCode code1=translate_Exp(first,t1);
			//code2
			InterCode code2=translate_Exp(second->next,t2);
			//op = get_relop(RELOP)
			enum R_KIND op;
			if(strcmp(second->name,">=")==0)
			{
				op=GE;
			}
			else if(strcmp(second->name,">")==0)
			{
				op=G;
			}
			else if(strcmp(second->name,"<=")==0)
			{
				op=LE;
			}
			else if(strcmp(second->name,"<")==0)
			{
				op=L;
			}
			else if(strcmp(second->name,"==")==0)
			{
				op=E;
			}
			else
			{
				op=NE;
			}		
			//code3
			InterCode code3=new_code(IFGOTO,t1,t2,label_true,op);
			//[GOTO label_false]
			InterCode temp=new_code(GOTO,label_false);
			return merge_code(4,code1,code2,code3,temp);
		}
		else if(strcmp(second->unit,"AND")==0)
		{
			Operand label1=new_label();
			InterCode code1=translate_Cond(first,label1,label_false);
			InterCode code2=translate_Cond(second->next,label_true,label_false);
			//[LABEL label1]
			InterCode temp1=new_code(LABEL,label1);
			return merge_code(3,code1,temp1,code2);
		}
		else if(strcmp(second->unit,"OR")==0)
		{
			Operand label1=new_label();
			InterCode code1=translate_Cond(first,label_true,label1);
			InterCode code2=translate_Cond(second->next,label_true,label_false);
			//[LABEL label1]
			InterCode temp1=new_code(LABEL,label1);
			return merge_code(3,code1,temp1,code2);
		}	
	}
	else if(strcmp(first->unit,"NOT")==0)
	{
		return translate_Cond(second,label_false,label_true);
	}
	else
	{
		Operand t1=new_temp();
		InterCode code1=translate_Exp(tr,t1);
		Operand op = new_op(CONSTANT, 0);
		InterCode code2=new_code(IFGOTO,t1,op,label_true, NE);
		//[GOTO label_false]
		InterCode temp=new_code(GOTO,label_false);
		return merge_code(3, code1,code2,temp);
	}
	
	
}


InterCode translate_Args(TreeNode tr, Operand arg_list) {
	printf("translate_Args.\n");
	TreeNode first = tr->child;
	TreeNode second = first->next;
	Operand t1 = new_temp();
	InterCode code1 = translate_Exp(first, t1);
	Operand cur = arg_list;
	while(cur->u.temp.next != NULL) {
		cur = cur->u.temp.next;
	}
	cur->u.temp.next = t1;
	t1->u.temp.prev = cur;
	// printf("Args identified successfully.\n");
	// Args -> Exp
	if(second != NULL) {
		// Args -> Exp COMMA Args
		InterCode code2 = translate_Args(second->next, arg_list);
		code1 = merge_code(2, code1, code2);
	}
	return code1;
}



void translate_Program(TreeNode tr) {
	printf("translate_Program.\n");
	codeRoot = translate_ExtDefList(tr->child);
}

InterCode translate_ExtDefList(TreeNode tr) {
	printf("translate_ExtDefList.\n");
	TreeNode first = tr->child;
	if(first != NULL) {
		InterCode code1 = translate_ExtDef(first);
		InterCode code2 = translate_ExtDefList(first->next);
		return merge_code(2, code1, code2);
	}
	else {
		return NULL;
	}
}

InterCode translate_ExtDef(TreeNode tr) {
	printf("translate_ExtDef.\n");
	TreeNode second = tr->child->next;
	if(strcmp(second->unit, "ExtDecList") == 0) {
		// global variable
		return NULL;
	}
	else if(strcmp(second->unit, "SEMI") == 0) {
		return NULL;
	}
	else {
		// ExtDef -> Specifier FunDec CompSt
		InterCode code1 = translate_FunDec(second);
		InterCode code2 = translate_Compst(second->next);
		return merge_code(2, code1, code2);
	}
}

InterCode translate_FunDec(TreeNode tr) {
	printf("translate_FunDec.\n");
	TreeNode first = tr->child;
	FuncNode func = check_func_table(first->name);
	if(func == NULL) {
		printf("Why NULL? In translate_FunDec.\n");
		return NULL;
	}
	else {
		return new_code(FUNC, func->name);
	}
}

InterCode translate_Compst(TreeNode tr) {
	printf("translate_CompSt.\n");
	TreeNode second = tr->child->next;
	TreeNode third = second->next;
	InterCode code1 = translate_DefList(second);
	InterCode code2 = translate_StmtList(third);
	return merge_code(2, code1, code2);
}

InterCode translate_StmtList(TreeNode tr) {
	printf("translate_StmtList.\n");
	TreeNode first = tr->child;
	if(first != NULL) {
		InterCode code1 = translate_Stmt(first);
		InterCode code2 = translate_StmtList(first->next);
		return merge_code(2, code1, code2);
	}
	else {
		return NULL;
	}
}

InterCode translate_DefList(TreeNode tr) {
	printf("translate_DefList.\n");
	TreeNode first = tr->child;
	if(first != NULL) {
		InterCode code1 = translate_Def(first);
		InterCode code2 = translate_DefList(first->next);
		return merge_code(2, code1, code2);
	}
	else {
		return NULL;
	}
}

InterCode translate_Def(TreeNode tr) {
	printf("translate_Def.\n");
	return translate_DecList(tr->child->next);
}

InterCode translate_DecList(TreeNode tr) {
	printf("translate_DecList.\n");
	TreeNode first = tr->child;
	InterCode code = translate_Dec(first);
	if(first->next != NULL) {
		code = merge_code(2, code, translate_DecList(first->next->next));
	}
	return code;
}

InterCode translate_Dec(TreeNode tr) {
	printf("translate_Dec.\n");
	TreeNode first = tr->child;
	if(first->next == NULL) {
		return NULL;
	}
	else {
		// Dec -> VarDec ASSIGNOP Exp
		// For the time being, assume VarDec -> ID
		SymNode variable = check_sym_table(first->child->name);
		if(variable == NULL) {
			printf("Why NULL? In translate_Dec.\n");
			return NULL;
		}
		else {
			Operand v1 = new_op(VARIABLE, variable->no);
			Operand t1 = new_temp();
			InterCode code2 = translate_Exp(first->next->next, t1);
			InterCode code1 = new_code(ASSIGN, v1, t1);
			return merge_code(2, code1, code2);
		}
	}
}

void print_op(Operand op) {
	switch(op->kind) {
		case VARIABLE:
			printf("%s", sym_table[op->u.var_no]->name);
			break;
		case CONSTANT:
			printf("#%d", op->u.value);
			break;
		case TEMP:
			printf("%s", op->u.temp.name);
			break;
		case O_LABEL:
			printf("%s", op->u.label.name);
			break;
		default: break;
	}
	//printf("OK!");
}

void print_code(InterCode code) {
	//printf("Start!");
	//printf("code type: %d: ", code->kind);
	switch(code->kind) {
		case LABEL:
			printf("LABEL ");
			print_op(code->u.single.op);
			printf(" :\n");
			break;
		case FUNC:
			printf("FUNCTION %s :\n", code->u.func.name);
			break;
		case ASSIGN:
			print_op(code->u.assign.result);
			printf(" := ");
			print_op(code->u.assign.op);
			printf("\n");
			break;
		case ADD:
			print_op(code->u.binop.result);
			printf(" := ");
			print_op(code->u.binop.op1);
			printf(" + ");
			print_op(code->u.binop.op2);
			printf("\n");
			break;
		case SUB:
			print_op(code->u.binop.result);
			printf(" := ");
			print_op(code->u.binop.op1);
			printf(" - ");
			print_op(code->u.binop.op2);
			printf("\n");
			break;
		case MUL:
			print_op(code->u.binop.result);
			printf(" := ");
			print_op(code->u.binop.op1);
			printf(" * ");
			print_op(code->u.binop.op2);
			printf("\n");
			break;
		case DIV:
			print_op(code->u.binop.result);
			printf(" := ");
			print_op(code->u.binop.op1);
			printf(" / ");
			print_op(code->u.binop.op2);
			printf("\n");
			break;
		case CITE:
			print_op(code->u.assign.result);
			printf(" := &");
			print_op(code->u.assign.op);
			printf("\n");
			break;
		case GETPOINTER:
			print_op(code->u.assign.result);
			printf(" := *");
			print_op(code->u.assign.op);
			printf("\n");
			break;
		case ASSIGNPOINTER:
			printf("*");
			print_op(code->u.assign.result);
			printf(" := ");
			print_op(code->u.assign.op);
			printf("\n");
			break;
		case GOTO:
			printf("GOTO ");
			print_op(code->u.single.op);
			printf("\n");
			break;
		case IFGOTO:
			printf("IF ");
			print_op(code->u.ifgoto.re1);
			printf(" ");
			switch(code->u.ifgoto.kind) {
				case G: printf(">"); break;
				case GE: printf(">="); break;
				case L: printf("<"); break;
				case LE: printf("<="); break;
				case E: printf("=="); break;
				case NE: printf("!="); break;
				default: break;
			}
			printf(" ");
			print_op(code->u.ifgoto.re2);
			printf(" GOTO ");
			print_op(code->u.ifgoto.label);
			printf("\n");
			break;
		case RETURN:
			printf("RETURN ");
			print_op(code->u.single.op);
			printf("\n");
			break;
		case DEC:
			printf("DEC ");
			print_op(code->u.dec.dec);
			printf(" %d\n", code->u.dec.size);
			break;
		case ARG:
			printf("ARG ");
			print_op(code->u.single.op);
			printf("\n");
			break;
		case CALL:
			print_op(code->u.call.ret);
			printf(" := CALL %s\n", code->u.call.name);
			break;
		case PARAM:
			printf("PARAM ");
			print_op(code->u.single.op);
			printf("\n");
			break;
		case READ:
			printf("READ ");
			print_op(code->u.single.op);
			printf("\n");
			break;
		case WRITE:
			printf("WRITE ");
			print_op(code->u.single.op);
			printf("\n");
			break;
		default: break;
	}
	//printf("OK!\n");
}

void print_codeTree() {
	InterCode code = codeRoot;
	while(code != NULL) {
		print_code(code);
		code = code->next;
		//printf("OK!\n");
	}
}
