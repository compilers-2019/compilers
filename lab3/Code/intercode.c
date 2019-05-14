#include "intercode.h"

Operand new_temp() {
	temp_num++;
	Operand t = new_op(TEMP);
	temp_table[temp_num] = t;
	t->u.temp.temp_no = temp_num;
	strcpy(t->u.temp.name, "t");
	char str[4];
	//itoa(t->num, str, 10);
	sprintf(str, "%d", t->num);
	strcat(t->u.temp.name, str);
	return t;
}

Operand new_op(enum O_KIND kind) {
	Operand op = malloc(sizeof(struct Operand_));
	op->kind = kind;
	return op;
}

InterCode new_code(enum I_KIND kind) {
	InterCode code = malloc(sizeof(struct InterCode_));
	code->kind = kind;
	return code;
}

InterCode translate_Exp(TreeNode tr, Operand place) {
	TreeNode first = tr->child;
	TreeNode second = first->next;

	if(strcmp(first->unit, "INT") == 0) {
		// Exp -> INT
		int value = (int)first->val;
		Operand op = new_op(CONSTANT);
		op->u.value = value;
		InterCode res = new_code(ASSIGN);
		res->u.assign.result = place;
		res->u.assign.op = op;
		return res;
	}
}