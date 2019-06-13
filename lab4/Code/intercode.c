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
	op->if_pointer = false;
	op->if_address = false;
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
	//printf("translate_Exp.\n");
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
				//printf("Why NULL? Exp -> ID: %s\n", first->name);
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
				//printf("Why NULL? Exp -> ID LP (Args) RP: %s\n", first->name);
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
		if(strcmp(second->unit, "LB") == 0) {
			// Array, exit
			printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
			// codeSignal = false;
			// return NULL;
			exit(-1);
		}
		else if(strcmp(second->unit, "DOT") == 0) {
			// Exp DOT ID
			TreeNode third = second->next;
			SymNode variable = check_sym_table(first->child->name);
			if(variable == NULL) {
				//printf("Why NULL? Exp ->Exp DOT ID: %s\n", first->child->name);
				return NULL;
			}
			else {
				// printf("%s: ", variable->name);
				// variable = variable->type->u.structure;
				// printf("STRUCT %s\n", variable->name);
				// variable = variable->struct_next;
				// while(variable != NULL) {
				// 	printf("%s\n", variable->name);
				// 	variable = variable->struct_next;
				// }
				// return NULL;
				int num = 0;
				SymNode structure = variable->type->u.structure->struct_next;
				while(strcmp(structure->name, third->name) != 0) {
					num += 4;
					structure = structure->struct_next;
				}

				if(num == 0) {
					Operand op = new_op(VARIABLE, variable->no);
					op->if_pointer = true;
					return new_code(ASSIGN, place, op);
				}
				else {
					Operand n = new_op(CONSTANT, num);
					Operand op = new_op(VARIABLE, variable->no);
					Operand t1 = new_temp();
					InterCode code1 = new_code(ADD, t1, op, n);
					InterCode code2 = new_code(GETPOINTER, place, t1);
					return merge_code(2, code1, code2);
				}
			}
		}
		else if((strcmp(second->unit, "RELOP") == 0) || (strcmp(second->unit, "AND") == 0) || (strcmp(second->unit, "OR") == 0)) {
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
			res = NULL;
			if(strcmp(first->child->unit, "Exp") == 0) {
				// Next level: Exp -> Exp DOT ID
				TreeNode next_level = first->child->next;
				if(strcmp(next_level->unit, "DOT") == 0) {
					SymNode variable = check_sym_table(first->child->child->name);
					int num = 0;
					SymNode structure = variable->type->u.structure->struct_next;
					while(strcmp(structure->name, next_level->next->name) != 0) {
						num += 4;
						structure = structure->struct_next;
					}
					Operand t1 = new_temp();
					Operand op = new_op(VARIABLE, variable->no);
					op->if_address = true;
					InterCode code1;
					Operand n;
					if(num == 0) {
						code1 = new_code(ASSIGN, t1, op);
					}
					else {
						n = new_op(CONSTANT, num);
						code1 = new_code(ADD, t1, op, n);
					}
					//printf("OK\n");
					Operand t2 = new_temp();
					InterCode code2 = translate_Exp(second->next, t2);
					InterCode code3 = new_code(ASSIGNPOINTER, t1, t2);
					return merge_code(3, code1, code2, code3);
				}
				else {
					// Array, exit
					printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
					// codeSignal = false;
					// return NULL;
					exit(-1);
				}
			}
			else {
				SymNode variable = check_sym_table(first->child->name);
				if(variable == NULL) {
					//printf("Why NULL? Exp -> ID: %s\n", first->child->name);
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
					res = merge_code(3, res, code1, code2);
					return res;
				}
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
	//printf("translate_Stmt.\n");
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
	//printf("translate_Cond.\n");
	TreeNode first=tr->child;
	TreeNode second=first->next;
	TreeNode third = second->next;
	if(strcmp(first->unit,"Exp")==0)
	{
		if(strcmp(second->unit,"RELOP")==0)
		{
			Operand t1, t2;
			InterCode code1 = NULL, code2 = NULL;
			if(strcmp(first->child->unit, "ID") == 0) {
				SymNode variable = check_sym_table(first->child->name);
				if(variable == NULL) {
					//printf("Why NULL? Cond: %s\n", first->child->name);
					return NULL;
				}
				else {
					t1 = new_op(VARIABLE, variable->no);
				}
			}
			else {
				t1 = new_temp();
				//code1
				code1=translate_Exp(first,t1);
			}
			if(strcmp(third->child->unit, "ID") == 0) {
				SymNode variable = check_sym_table(third->child->name);
				if(variable == NULL) {
					//printf("Why NULL? Cond: %s\n", first->child->name);
					return NULL;
				}
				else {
					t2 = new_op(VARIABLE, variable->no);
				}
			}
			else {
				t2 = new_temp();
				//code2
				code2=translate_Exp(second->next,t2);
			}
			
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
	//printf("translate_Args.\n");
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
	//printf("translate_Program.\n");
	codeRoot = translate_ExtDefList(tr->child);
}

InterCode translate_ExtDefList(TreeNode tr) {
	//printf("translate_ExtDefList.\n");
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
	//printf("translate_ExtDef.\n");
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
	//printf("translate_FunDec.\n");
	TreeNode first = tr->child;
	FuncNode func = check_func_table(first->name);
	if(func == NULL) {
		//printf("Why NULL? In translate_FunDec: %s\n", first->name);
		return NULL;
	}
	else {
		InterCode res = new_code(FUNC, func->name);
		TreeNode third = first->next->next;
		if(strcmp(third->unit, "VarList") == 0) {
			// FunDec -> ID LP VarList RP
			res = merge_code(2, res, translate_VarList(third));
		}
		return res;
	}
}

InterCode translate_VarList(TreeNode tr) {
	//printf("translate_VarList.\n");
	TreeNode first = tr->child;
	InterCode res = translate_ParamDec(first);
	if(first->next != NULL) {
		// VarList -> ParamDec COMMA ParamDec
		res = merge_code(2, res, translate_VarList(first->next->next));
	}
	return res;
}

InterCode translate_ParamDec(TreeNode tr) {
	//printf("translate_ParamDec.\n");
	TreeNode second = tr->child->next;
	SymNode variable = check_sym_table(second->child->name);
	if(variable == NULL) {
		//printf("Why NULL? In translate_Dec: %s\n", second->child->name);
		return NULL;
	}
	else {
		Operand v1 = new_op(VARIABLE, variable->no);
		return new_code(PARAM, v1);
	}
}

InterCode translate_Compst(TreeNode tr) {
	//printf("translate_CompSt.\n");
	TreeNode second = tr->child->next;
	TreeNode third = second->next;
	InterCode code1 = translate_DefList(second);
	InterCode code2 = translate_StmtList(third);
	return merge_code(2, code1, code2);
}

InterCode translate_StmtList(TreeNode tr) {
	//printf("translate_StmtList.\n");
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
	//printf("translate_DefList.\n");
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
	//printf("translate_Def.\n");
	return translate_DecList(tr->child->next);
}

InterCode translate_DecList(TreeNode tr) {
	//printf("translate_DecList.\n");
	TreeNode first = tr->child;
	InterCode code = translate_Dec(first);
	if(first->next != NULL) {
		code = merge_code(2, code, translate_DecList(first->next->next));
	}
	return code;
}

InterCode translate_Dec(TreeNode tr) {
	//printf("translate_Dec.\n");
	TreeNode first = tr->child;
	if(first->next == NULL) {
		return NULL;
	}
	else {
		// Dec -> VarDec ASSIGNOP Exp
		// For the time being, assume VarDec -> ID
		SymNode variable = check_sym_table(first->child->name);
		if(variable == NULL) {
			////printf("Why NULL? In translate_Dec: %s\n", first->child->name);
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
	//FILE *fp=NULL;
	switch(op->kind) {
		case VARIABLE:
			if(op->if_pointer == true) {
				printf("*");
				//fprintf(fp, "*");
			}
			else if(op->if_address == true) {
				printf("&");
				//fprintf(fp, "&");
			}
			printf("%s", sym_table[op->u.var_no]->name);
			//fputs(sym_table[op->u.var_no]->name,fp);
			
			//fprintf(fp,"%s", sym_table[op->u.var_no]->name);
			break;
		case CONSTANT:
			 
			printf("#%d", op->u.value);
			//fputs("#",fp);
			//fputs(op->u.value,fp);
			
			//fprintf(fp,"#%d", op->u.value);
			break;
		case TEMP:
			 
			printf("%s", op->u.temp.name);
			//fputs(op->u.temp.name,fp);
			
			//fprintf(fp,"%s", op->u.temp.name);
			break;
		case O_LABEL:
			 
			printf("%s", op->u.label.name);
			
			//fprintf(fp,"%s", op->u.label.name);
			//fputs(op->u.label.name,fp);
			break;
		default: break;
	}
	//fputs("NO",fp);
	//
	////printf("OK!");
}

// void print_code(InterCode code) {
// 	//printf("Start!");
// 	//printf("code type: %d: ", code->kind);
// 	//FILE *fp=NULL;
// 	switch(code->kind) {
// 		case LABEL:
// 			printf("LABEL ");
// 			fputs("LABEL ",fp);

// 			print_op(code->u.single.op);
// 			printf(" :\n");
// 			fputs(" :\n",fp);
// 			//printf(":\n");
// 			//fputs(":\n",fp);
// 			break;
// 		case FUNC:
// 			printf("FUNCTION %s :\n", code->u.func.name);
// 			//printf("\t%s:\n", code->u.func.name);
// 			fprintf(fp,"FUNCTION %s :\n",code->u.func.name);
// 			//fprintf(fp,"\t%s:\n",code->u.func.name);
// 			break;
// 		case ASSIGN:			 
// 			print_op(code->u.assign.result);
// 			printf(" := ");
// 			fputs(" := ",fp);
// 			print_op(code->u.assign.op);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case ADD:
// 			print_op(code->u.binop.result);
// 			printf(" := ");
// 			fputs(" := ",fp);
// 			print_op(code->u.binop.op1);
// 			printf(" + ");
// 			fputs(" + ",fp);
// 			print_op(code->u.binop.op2);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case SUB:
			 
// 			print_op(code->u.binop.result);
// 			printf(" := ");	
// 			fputs(" := ",fp);
// 			print_op(code->u.binop.op1);
// 			printf(" - ");
// 			fputs(" - ",fp);
// 			print_op(code->u.binop.op2);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case MUL:
			 
// 			print_op(code->u.binop.result);
// 			printf(" := ");
// 			fputs(" := ",fp);

// 			print_op(code->u.binop.op1);
// 			printf(" * ");
// 			fputs(" * ",fp);

// 			print_op(code->u.binop.op2);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case DIV:
			 
// 			print_op(code->u.binop.result);
// 			printf(" := ");
// 			fputs(" := ",fp);

// 			print_op(code->u.binop.op1);
// 			printf(" / ");
// 			fputs(" / ",fp);

// 			print_op(code->u.binop.op2);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case CITE:
			 
// 			print_op(code->u.assign.result);
// 			printf(" := &");
// 			fputs(" := &",fp);
// 			print_op(code->u.assign.op);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case GETPOINTER:
			 
// 			print_op(code->u.assign.result);
// 			printf(" := *");
// 			fputs(" := *",fp);
// 			print_op(code->u.assign.op);
// 			printf("\n");
// 			fputs("\n",fp);
// 			break;
// 		case ASSIGNPOINTER:
			 
// 			printf("*");
// 			fputs("*",fp);

// 			print_op(code->u.assign.result);
// 			printf(" := ");
// 			fputs(" := ",fp);
			
// 			print_op(code->u.assign.op);
// 			printf("\n");
			
// 			fputs("\n",fp);
// 			break;
// 		case GOTO:
			 
// 			printf("GOTO ");
			
// 			fputs("GOTO ",fp);
			
// 			print_op(code->u.single.op);
// 			printf("\n");
			
// 			fputs("\n",fp);
// 			break;
// 		case IFGOTO:
			 
// 			printf("IF ");
			
// 			fputs("IF ",fp);
			
// 			print_op(code->u.ifgoto.re1);
// 			printf(" ");
			
// 			fputs(" ",fp);
// 			switch(code->u.ifgoto.kind) {
// 				case G: printf(">"); fputs(">",fp);break;
// 				case GE: printf(">="); fputs(">=",fp);break;
// 				case L: printf("<");fputs("<",fp);break;
// 				case LE: printf("<="); fputs("<=",fp);break;
// 				case E: printf("=="); fputs("==",fp);break;
// 				case NE: printf("!="); fputs("!=",fp);break;
// 				default: break;
// 			}
// 			printf(" ");
// 			fputs(" ",fp);
			
// 			print_op(code->u.ifgoto.re2);
// 			printf(" GOTO ");
			
// 			fputs(" GOTO ",fp);
			
// 			print_op(code->u.ifgoto.label);
// 			printf("\n");
			
// 			fputs("\n",fp);
// 			break;
// 		case RETURN:
			 
// 			printf("RETURN ");
			
// 			fputs("RETURN ",fp);
			
// 			print_op(code->u.single.op);
// 			printf("\n");
			
// 			fputs("\n",fp);
// 			break;
// 		case DEC:
			 
// 			printf("DEC ");
			
// 			fputs("DEC ",fp);
			
// 			print_op(code->u.dec.dec);
			
			
			
			
// 			fprintf(fp," %d\n", code->u.dec.size);
// 			printf(" %d\n", code->u.dec.size);
// 			break;
// 		case ARG:
			 
// 			printf("ARG ");
			
// 			fputs("ARG ",fp);
			
// 			print_op(code->u.single.op);
			
// 			fputs("\n",fp);
// 			printf("\n");
// 			break;
// 		case CALL:
			 
// 			print_op(code->u.call.ret);		
// 			printf(" := CALL %s\n", code->u.call.name);
			
// 			fprintf(fp," := CALL %s\n", code->u.call.name);
// 			break;
// 		case PARAM:
			 
// 			printf("PARAM ");
			
// 			fputs("PARAM ",fp);
			
// 			print_op(code->u.single.op);
			
// 			fputs("\n",fp);
// 			printf("\n");
// 			break;
// 		case READ:
// 			printf("READ ");
			
// 			fputs("READ ",fp);
			
// 			print_op(code->u.single.op);
// 			printf("\n");
			
// 			fputs("\n",fp);
// 			break;
// 		case WRITE:
			 
// 			printf("WRITE ");
			
// 			fputs("WRITE ",fp);
			
// 			print_op(code->u.single.op);
// 			printf("\n");
			
// 			fputs("\n",fp);
// 			break;
// 		default: break;
// 	}
	
// }

// void print_codeTree() {
// 	InterCode code = codeRoot;
// 	fp = fopen(file_name, "w");
// 	while(code != NULL) {
// 		print_code(code);
// 		code = code->next;
// 	}
// 	fclose(fp);
// }


bool op_equaled(Operand op1, Operand op2) {
	if (op1 == NULL && op2 == NULL) {
		return true;
	}
	else if (op1 == NULL || op2 == NULL) {
		return false;
	}
	else if (op1->kind != op2->kind) {
		return false;
	}
	else {
		switch (op1->kind) {
			case VARIABLE: return (op1->u.var_no == op1->u.var_no);
			case TEMP: return (op1->u.temp.temp_no == op1->u.temp.temp_no);
			default: return false;
		}
	}
}

Register new_reg(char* c, int i) {
    Register reg = malloc(sizeof(struct Register_));
    reg->if_used = false;
    reg->op = NULL;
    strcpy(reg->name, c);
    char str[2];
    sprintf(str, "%d", i);
    strcat(reg->name, str);
    return reg;
}

Register search_reg(Operand op) {
    for (int i = 0; i < REG_A; i++) {
        if (op_equaled(op, a_list[i]->op)) {
            return a_list[i];
        }
    }
    for (int i = 0; i < REG_T; i++) {
        if (op_equaled(op, t_list[i]->op)) {
            return t_list[i];
        }
    }
    return NULL;
}

void init_reg() {
    for (int i = 0; i < REG_T; i++) {
        t_list[i] = new_reg("t", i);

        if (i < REG_S) {
            s_list[i] = new_reg("s", i);
        }

        if (i < REG_A) {
            a_list[i] = new_reg("a", i);
        }

        if (i < REG_V) {
            v_list[i] = new_reg("v", i);
        }
    }
}

bool op_existed(Operand op, InterCode code) {
    switch(code->kind) {
		case FUNC:
			break;
		case LABEL:
		case GOTO:
		case RETURN:
		case ARG:
		case PARAM:
		case READ:
		case WRITE: 
            return op_equaled(op, code->u.single.op);
		case ASSIGN:
		case CITE:
		case GETPOINTER:
		case ASSIGNPOINTER:
            return op_equaled(op, code->u.assign.op);
		case ADD:
		case SUB:
		case MUL:
		case DIV:
            return (op_equaled(op, code->u.binop.op1) || op_equaled(op, code->u.binop.op2));
		case IFGOTO: 
            return (op_equaled(op, code->u.ifgoto.re1) || op_equaled(op, code->u.ifgoto.re2));
		case DEC:
		case CALL:
		default: break;
    }
    return false;
}

bool op_used(Operand op, InterCode code) {
    if (op == NULL) {
        return false;
    }
    InterCode cur = codeRoot;
    while (cur != code) {
        cur = cur->next;
    }
    while ((cur != NULL) && (cur->kind != FUNC)) {
        if (op_existed(op, cur)) {
            return true;
        }
    }
    return false;
}

char* regT(Operand op, InterCode code) {
    // search for an empty regT
    for (int i = 0; i < REG_T; i++) {
        if (!t_list[i]->if_used) {
            // find an empty regT
            t_list[i]->if_used = true;
            t_list[i]->op = op;
			printf("Used reg: %s\n", t_list[i]->name);
            return t_list[i]->name;
        }
    }

	printf("Not found.\n");
	for (int i = 0; i < REG_T; i++) {
		printf("??\n");
		print_op(t_list[i]->op);
	}

    // clear some regT
    for (int i = 0; i < REG_T; i++) {
        if (!op_used(t_list[i]->op, code)) {
            // this op is no longer used
            Operand temp = t_list[i]->op;
            switch (temp->kind) {
                case O_LABEL:
                case CONSTANT:
                    return "WTF???";
                case VARIABLE: // save variable
                case TEMP: 
                    t_list[i]->op = op;
                    return t_list[i]->name;
            }
        }
    }

    return "FULL";
}

char* reg(Operand op, InterCode code) {
    Register reg = search_reg(op);
    if (reg != NULL) {
        return reg->name;
    }
    else {
        return regT(op, code);
    }
}

char* regS(Operand op, InterCode code) {
    // search for an empty regS
    for (int i = 0; i < REG_S; i++) {
        if (!s_list[i]->if_used) {
            // find an empty regS
            s_list[i]->if_used = true;
            s_list[i]->op = op;
            return s_list[i]->name;
        }
    }

    // clear some regS
    for (int i = 0; i < REG_S; i++) {
        if (!op_used(s_list[i]->op, code)) {
            // this op is no longer used
            Operand temp = s_list[i]->op;
            switch (temp->kind) {
                case O_LABEL:
                case CONSTANT:
                    return "WTF???";
                case VARIABLE: // save variable
                case TEMP: 
                    s_list[i]->op = op;
                    return s_list[i]->name;
            }
        }
    }

    return "FULL";
}

void sw_live_T(InterCode code) {
    int live_num = 0;
    for (int i = 0; i < REG_T; i++) {
        if (!op_used(t_list[i]->op, code)) {
            // this op is no longer used
            t_list[i]->op = NULL;
            t_list[i]->if_used = false;
        }
        else {
            // save live
            fprintf(fp, "\tsw $%s, %d($sp)\n", t_list[i]->name, live_num * 4);
            live_num++;
        }
    }
}

void lw_live_T(InterCode code) {
    int live_num = 0;
    for (int i = 0; i < REG_T; i++) {
        if (!op_used(t_list[i]->op, code)) {
            // this op is no longer used
            t_list[i]->op = NULL;
            t_list[i]->if_used = false;
        }
        else {
            // save live
            fprintf(fp, "\tlw $%s, %d($sp)\n", t_list[i]->name, live_num * 4);
            live_num++;
        }
    }
}

void arg_move(Operand op, InterCode code) {
    Register reg = search_reg(op);
    if (reg == NULL) {
        return;
    }
    if (arg_num == 0) {
        InterCode cur = code;
        int num = 0;
        while (cur->kind == ARG) {
            num++;
            cur = cur->next;
        }
        if (func) {
            fprintf(fp, "\tsubu $sp, $sp, %d\n", 4 * (num + 1));
        }
        else if (num > 4) {
            fprintf(fp, "\tsubu $sp, $sp, %d\n", 4 * (num - 5));
        }
    }
    if ((arg_num < 4) && !func) {
        fprintf(fp, "\tmove $%s, $%s\n", a_list[arg_num]->name, reg->name);
    }
    else if(!func) {
        fprintf(fp, "\tsw $%s, %d($sp)\n", reg->name, 4 * (arg_num - 5));
    }
    else {
        fprintf(fp, "\tsw $%s, %d($sp)\n", reg->name, 4 * arg_num);
    }
    arg_num++;
}

void param_define(Operand op) {
    func = true;
    if (param_num > 4) {

    }
    else {
        a_list[param_num]->op = op;
    }
}

void print_code(InterCode code) {
	//printf("Start!");
	//printf("code type: %d: ", code->kind);
	//FILE *fp=NULL;
    char result[5];
	switch(code->kind) {

		case LABEL:
			printf("Run: LABEL\n");
			//printf("LABEL ");
			//fputs("LABEL ",fp);
            fprintf(fp, "%s", code->u.single.op->u.label.name);
			//printf(" :\n");
			//fputs(" :\n",fp);
			fputs(":\n",fp);
			break;
		case FUNC:
			//printf("FUNCTION %s :\n", code->u.func.name);
			// printf("\t%s:\n", code->u.func.name);
			//fprintf(fp,"FUNCTION %s :\n",code->u.func.name);
			printf("Run: FUNC\n");
			fprintf(fp,"%s:\n",code->u.func.name);
            init_reg();
            func = false;
			break;
		case ASSIGN:			 
			//print_op(code->u.assign.result);
			printf("Run: ASSIGN\n");
            if(code->u.assign.result->kind==TEMP||
            (code->u.assign.result->kind==VARIABLE&&code->u.assign.result->if_pointer==false&&code->u.assign.result->if_address==false))
            {
                
                strcpy(result, reg(code->u.assign.result,code));
				printf("%s\n", result);
                switch (code->u.assign.op->kind)
                {
                case CONSTANT:
                    fprintf(fp,"\tli %s , %d\n",result,code->u.assign.op->u.value);
                    break;
                case VARIABLE:
				case TEMP:
                    {char op[5];
                    strcpy(op, reg(code->u.assign.op,code));
                    if(code->u.assign.op->if_pointer==false&&code->u.assign.op->if_address==false)
                    {
                        fprintf(fp,"\tmove %s, %s\n",result,op);
                    }
                    else if(code->u.assign.op->if_pointer==true||code->u.assign.result->if_address==true) 
                    {
                        fprintf(fp,"\tlw %s, 0(%s)\n",result,op);
                    }        
                    break;
                    }
                default:
                    break;
                }
            }
            else if((code->u.assign.result->kind==VARIABLE)&&(code->u.assign.result->if_pointer==true||code->u.assign.result->if_address==true))
            {
                
                strcpy(result, reg(code->u.assign.result,code));
                switch (code->u.assign.op->kind)
                {
                case CONSTANT:
                    fprintf(fp,"\tsw %d , 0(%s)\n",code->u.assign.op->u.value,result);
                    break;
                case VARIABLE:
				case TEMP:
                    {char op[5];
                    strcpy(op, reg(code->u.assign.op,code));
                        fprintf(fp,"\tsw %s, 0(%s)\n",op,result);    
                    break;}
                default:
                    break;
                }
            }
			break;
		case ADD:
            {
			printf("Run: ADD\n");
            strcpy(result, reg(code->u.binop.result,code));
            //char *result=reg(code->u.binop.result,code);
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\taddi %s, %d, %d\n",result,code->u.binop.op1->u.value,code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\taddi %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                fprintf(fp,"\taddi %s, %s, %d\n",result,op1,code->u.binop.op2->u.value);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tadd %s, %s, %s\n",result,op1,op2);
            }
			break;}
		case SUB:
            {
			printf("Run: SUB\n");
            strcpy(result, reg(code->u.binop.result,code));
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\taddi %s, %d, %d\n",result,code->u.binop.op1->u.value,-code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tsub %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                fprintf(fp,"\taddi %s, %s, %d\n",result,op1,-code->u.binop.op2->u.value);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tsub %s, %s, %s\n",result,op1,op2);
            }
			break;
            }
		case MUL:
			{
			printf("Run: MUL\n");
            strcpy(result, reg(code->u.binop.result,code));
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\tmul %s, %d, %d\n",result,code->u.binop.op1->u.value,code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tmul %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                fprintf(fp,"\tmul %s, %s, %d\n",result,op1,-code->u.binop.op2->u.value);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tmul %s, %s, %s\n",result,op1,op2);
            }
			break;
            }
		case DIV:
            printf("Run: DIV\n");
            strcpy(result, reg(code->u.binop.result,code));
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\tdiv %d, %d\n",code->u.binop.op1->u.value,code->u.binop.op2->u.value);
                fprintf(fp,"\tmflo %s\n",result);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tdiv %d, %s\n",code->u.binop.op1->u.value,op2);
                fprintf(fp,"\tmflo %s\n",result);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                fprintf(fp,"\tdiv %s, %d\n",op1,-code->u.binop.op2->u.value);
                fprintf(fp,"\tmflo %s\n",result);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.binop.op1,code));
                char op2[5];
                strcpy(op2,reg(code->u.binop.op2,code));
                fprintf(fp,"\tdiv %s, %s\n",op1,op2);
                fprintf(fp,"\tmflo %s\n",result);
            }
			break;
		case CITE:
			printf("Run: CITE\n");
			 fputs("\n",fp);
			/* print_op(code->u.assign.result);
			printf(" := &");
			fputs(" := &",fp);
			print_op(code->u.assign.op);
			printf("\n");
			fputs("\n",fp);*/
        
			break;
		case GETPOINTER:
			 /* 
			print_op(code->u.assign.result);
			printf(" := *");
			fputs(" := *",fp);
			print_op(code->u.assign.op);
			printf("\n");
			fputs("\n",fp);*/
			printf("Run: GETPOINTER\n");
            fputs("\n",fp);
			break;
		case ASSIGNPOINTER:
			 
		/* 	printf("*");
			fputs("*",fp);

			print_op(code->u.assign.result);
			printf(" := ");
			fputs(" := ",fp);
			
			print_op(code->u.assign.op);
			printf("\n");
			
			fputs("\n",fp);*/
			printf("Run: ASSIGNPOINTER\n");
            fputs("\n",fp);
			break;
		case GOTO:	 
			/*printf("GOTO ");
			fputs("GOTO ",fp);	*/
			printf("Run: GOTO\n");
            fprintf(fp, "\tj %s\n",code->u.single.op->u.label.name);
			//print_op(code->u.single.op);
			break;
		case IFGOTO:			 
			/* printf("IF ");			
			fputs("IF ",fp);			
			print_op(code->u.ifgoto.re1);
			printf(" ");			
			fputs(" ",fp);*/
			printf("Run: IFGOTO\n");
			switch(code->u.ifgoto.kind) {
				case G: fputs("\tbgt ",fp);break;
				case GE: fputs("\tbge ",fp);break;
				case L: fputs("\tblt ",fp);break;
				case LE:  fputs("\tble ",fp);break;
				case E: fputs("\tbeq ",fp);break;
				case NE:fputs("\tbne ",fp);break;
				default: break;
			}		
			/* print_op(code->u.ifgoto.re2);
			printf(" GOTO ");
			
			fputs(" GOTO ",fp);
			
			print_op(code->u.ifgoto.label);
			printf("\n");
			
			fputs("\n",fp);*/
            
            if(code->u.ifgoto.re1->kind==CONSTANT&&code->u.ifgoto.re2->kind==CONSTANT)
            {
                fprintf(fp,"%d, %d, %s\n",code->u.ifgoto.re1->u.value,code->u.ifgoto.re2->u.value,code->u.ifgoto.label->u.label.name);
            }
            else if(code->u.ifgoto.re1->kind==CONSTANT&&(code->u.ifgoto.re2->kind==VARIABLE||code->u.ifgoto.re2->kind==TEMP))
            {
                char op2[5];
                strcpy(op2,reg(code->u.ifgoto.re2,code));
                fprintf(fp,"%d, %s, %s\n",code->u.ifgoto.re1->u.value,op2,code->u.ifgoto.label->u.label.name);
                //fprintf(fp,"\tmul %s, %d, %s\n",result,code->u.ifgoto.re1->u.value,op2);
            }
            else if(code->u.ifgoto.re2->kind==CONSTANT&&(code->u.ifgoto.re1->kind==VARIABLE||code->u.ifgoto.re1->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.ifgoto.re1,code));
                //fprintf(fp,"\tmul %s, %s, %d\n",result,op1,-code->u.ifgoto.re2->u.value);
                fprintf(fp,"%s, %d, %s\n",op1,code->u.ifgoto.re2->u.value,code->u.ifgoto.label->u.label.name);
            }
            else if((code->u.ifgoto.re1->kind==VARIABLE||code->u.ifgoto.re1->kind==TEMP)&&(code->u.ifgoto.re2->kind==VARIABLE||code->u.ifgoto.re2->kind==TEMP))
            {
                char op1[5];
                strcpy(op1,reg(code->u.ifgoto.re1,code));
                char op2[5];
                strcpy(op2,reg(code->u.ifgoto.re2,code));
                //fprintf(fp,"\tmul %s, %s, %s\n",result,op1,op2);
                fprintf(fp,"%s, %s, %s\n",op1,op2,code->u.ifgoto.label->u.label.name);
            }
			break;
		case RETURN:
			printf("Run: RETURN\n");
            if(code->u.single.op->kind==VARIABLE||code->u.single.op->kind==TEMP)
            {
                char op[5];
                strcpy(op,reg(code->u.single.op,code));
                //char *op=reg(code->u.single.op,code);
                fprintf(fp,"\tmove $v0, %s\n",op);
                fprintf(fp,"\tjr $ra\n");
            }
            else
            {
                fprintf(fp,"\tmove $0, %d\n",code->u.single.op->u.value);
                fprintf(fp,"\tjr $ra\n");
            }
			break;
		case DEC:
			printf("Run: DEC\n");
			break;
		case ARG:		 
			printf("Run: ARG\n");
			arg_move(code->u.single.op, code);
			break;
		case CALL:
			printf("Run: CALL\n");
            sw_live_T(code);		 
			if (func) {
                fprintf(fp, "\tsw $ra, %d($sp)\n", param_num * 4);
            }
            else {
                fprintf(fp, "\taddi $sp, $sp, -4\n");
                fprintf(fp, "\tsw $ra, 0($sp)\n");
            }
            fprintf(fp, "jal %s\n", code->u.call.name);
            if (func) {
                for (int i = 0; i < REG_A; i++) {
                    if (i >= param_num) {
                        break;
                    }
                    fprintf(fp, "\tlw $%s, %d($sp)\n", a_list[i]->name, (i - 1) * 4);
                }
                fprintf(fp, "\taddi $sp, $sp, %d\n", (param_num + 1) * 4);
            }
            else {
                fprintf(fp, "\taddi $sp, $sp, 4\n");
            }
            fprintf(fp, "\tmove %s, $v0\n", reg(code->u.call.ret, code));
            lw_live_T(code);
			break;
		case PARAM:		 
			printf("Run: PARAM\n");
			param_define(code->u.single.op);
			break;
		case READ:
			printf("Run: READ\n");
            if (func) {
                fprintf(fp, "\tsw $ra, %d($sp)\n", param_num * 4);
            }
            else {
                fprintf(fp, "\taddi $sp, $sp, -4\n");
                fprintf(fp, "\tsw $ra, 0($sp)\n");
            }
            fprintf(fp, "\tjal read\n");
            if (func) {
                for (int i = 0; i < REG_A; i++) {
                    if (i >= param_num) {
                        break;
                    }
                    fprintf(fp, "\tlw $%s, %d($sp)\n", a_list[i]->name, (i - 1) * 4);
                }
                fprintf(fp, "\taddi $sp, $sp, %d\n", (param_num + 1) * 4);
            }
            else {
                fprintf(fp, "\taddi $sp, $sp, 4\n");
            }
            fprintf(fp, "\tmove %s, $v0\n", reg(code->u.call.ret, code));
			break;
		case WRITE:
			printf("Run: WRITE\n");
			if (func) {
                fprintf(fp, "\tsw $ra, %d($sp)\n", param_num * 4);
            }
            else {
                fprintf(fp, "\taddi $sp, $sp, -4\n");
                fprintf(fp, "\tsw $ra, 0($sp)\n");
            }
            fprintf(fp, "\tjal write\n");
            if (func) {
                for (int i = 0; i < REG_A; i++) {
                    if (i >= param_num) {
                        break;
                    }
                    fprintf(fp, "\tlw $%s, %d($sp)\n", a_list[i]->name, (i - 1) * 4);
                }
                fprintf(fp, "\taddi $sp, $sp, %d\n", (param_num + 1) * 4);
            }
            else {
                fprintf(fp, "\taddi $sp, $sp, 4\n");
            }
			break;
		default: break;
	}
	
}

void print_codeTree() {
	InterCode code = codeRoot;
	fp = fopen(file_name, "w");
    fprintf(fp,".data\n_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp,"_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $s0, _prompt\n\tsyscall\n\tjr $ra\n\nwrite:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n\n");
	// fclose(fp);

	while(code != NULL) {
		print_code(code);
		code = code->next;
	}
	fclose(fp);
}