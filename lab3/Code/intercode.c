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

Operand new_op(enum O_KIND kind) {
	Operand op = malloc(sizeof(struct Operand_));
	op->kind = kind;
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
			code->u.call.func = va_arg(argptr, Operand); 
			break;
		default: break;
	}
	return code;
}

InterCode merge_code(InterCode code1, InterCode code2) {
	InterCode cur = code1;
	while(cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = code2;
	code2->prev = cur;
	return code1;
}

InterCode translate_Exp(TreeNode tr, Operand place) {
	TreeNode first = tr->child;
	TreeNode second = first->next;
	InterCode res;

	if(strcmp(first->unit, "INT") == 0) {
		// Exp -> INT
		int value = (int)first->val;
		Operand op = new_op(CONSTANT);
		op->u.value = value;
		// res->kind = ASSIGN;
		// res->u.assign.result = place;
		// res->u.assign.op = op;
		res = new_code(ASSIGN, place, op);
		return res;
	}
	else if(strcmp(first->unit, "ID") == 0) {
		// Exp -> ID
		SymNode variable = check_sym_table(first->name);
		if(variable == NULL) {
			printf("Why NULL? Exp -> ID\n");
			return NULL;
		}
		else {
			Operand op = new_op(VARIABLE);
			op->u.var_no = variable->no;
			// res->kind = ASSIGN;
			// res->u.assign.result = place;
			// res->u.assign.op = op;
			res = new_code(ASSIGN, place, op);
			return res;
		}
	}
	else if(strcmp(first->unit, "MINUS") == 0) {
		// Exp -> MINUS Exp
		Operand t1 = new_temp(); 
		InterCode code1 = translate_Exp(second, t1);
		Operand zero = new_op(CONSTANT);
		zero->u.value = 0;
		// res = new_code(SUB);
		// res->u.binop.result = place;
		// res->u.binop.op1 = zero;
		// res->u.binop.op2 = t1;
		res = new_code(SUB, place, zero, t1);
		return merge_code(code1, res);
	}
	else if(strcmp(first->unit, "NOT") == 0) {
		// Exp -> NOT Exp
		Operand label1 = new_label();
		Operand label2 = new_label();

		// code0
		Operand op0 = new_op(CONSTANT);
		op0->u.value = 0;
		// res->kind = ASSIGN;
		// res->u.assign.result = place;
		// res->u.assign.op = op0;
		res = new_code(ASSIGN, place, op0);

		// code1
		InterCode code1 = translate_Cond(tr, label1, label2);

		// code2
		InterCode code2 = new_code(LABEL);
		code2->u.single.op = label1;

		//
	}
}