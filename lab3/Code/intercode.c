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
		case LABEL:
		case FUNC:
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
	return code;
}

InterCode merge_code(int n,InterCode code1,InterCode code2, ...)
{
	va_list parg;
	va_start(parg,code2);
	InterCode cur = code1;
	while(cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = code2;
	code2->prev = cur;
	n=n-2;
	while(n>0)
	{
		InterCode code = va_arg(parg, InterCode);
		InterCode cur = code1;
		while(cur->next != NULL) 
		{
			cur = cur->next;
		}
		cur->next = code;
		code->prev = cur;
		n--;
	}
	va_end(parg);
	return code1;

}

InterCode translate_Exp(TreeNode tr, Operand place) {
	TreeNode first = tr->child;
	TreeNode second = first->next;
	InterCode res;

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
						res = merge_code(2, code1, new_code(WRITE, arg_list->u.temp.next));
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
				InterCode code2 = new_code(ASSIGN, op, t1);
				InterCode code3 = new_code(ASSIGN, place, op);
				res = merge_code(3, code1, code2, code3);
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

InterCode translate_Cond(TreeNode tr, Operand label_true, Operand label_false) {

}

InterCode translate_Args(TreeNode tr, Operand arg_list) {
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
	// Args -> Exp
	if(second != NULL) {
		// Args -> Exp COMMA Args
		InterCode code2 = translate_Args(second->next, arg_list);
		code1 = merge_code(2, code1, code2);
	}
	return code1;
}