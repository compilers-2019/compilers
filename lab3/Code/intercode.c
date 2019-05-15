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
	Operand l = new_op(OLABLE);
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

InterCode new_code(enum I_KIND kind) {
	InterCode code = malloc(sizeof(struct InterCode_));
	code->kind = kind;
	code->prev = NULL;
	code->next = NULL;
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
	InterCode res = malloc(sizeof(struct InterCode_));
	res->prev = NULL;
	res->next = NULL;

	if(strcmp(first->unit, "INT") == 0) {
		// Exp -> INT
		int value = (int)first->val;
		Operand op = new_op(CONSTANT);
		op->u.value = value;
		res->kind = ASSIGN;
		res->u.assign.result = place;
		res->u.assign.op = op;
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
			res->kind = ASSIGN;
			res->u.assign.result = place;
			res->u.assign.op = op;
			return res;
		}
	}
	else if(strcmp(first->unit, "MINUS") == 0) {
		// Exp -> MINUS Exp
		Operand t1 = new_temp(); 
		InterCode code1 = translate_Exp(second, t1);
		res = new_code(SUB);
		res->u.binop.result = place;
		Operand zero = new_op(CONSTANT);
		zero->u.value = 0;
		res->u.binop.op1 = zero;
		res->u.binop.op2 = t1;
		return merge_code(code1, res);
	}
}