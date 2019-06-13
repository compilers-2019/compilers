// #include "objectcode.h"

// Register new_reg(char* c, int i) {
//     Register reg = malloc(sizeof(struct Register_));
//     reg->if_used = false;
//     reg->op = NULL;
//     strcpy(reg->name, c);
//     char str[2];
//     sprintf(str, "%d", i);
//     strcat(reg->name, str);
//     return reg;
// }

// Register search_reg(Operand op) {
//     for (int i = 0; i < REG_A; i++) {
//         if (op == a_list[i]->op) {
//             return a_list[i];
//         }
//     }
//     for (int i = 0; i < REG_T; i++) {
//         if (op == t_list[i]->op) {
//             return t_list[i];
//         }
//     }
//     return NULL;
// }

// void init_reg() {
//     for (int i = 0; i < REG_T; i++) {
//         t_list[i] = new_reg("t", i);

//         if (i < REG_S) {
//             s_list[i] = new_reg("s", i);
//         }

//         if (i < REG_A) {
//             s_list[i] = new_reg("a", i);
//         }

//         if (i < REG_V) {
//             s_list[i] = new_reg("v", i);
//         }
//     }
// }

// bool op_existed(Operand op, InterCode code) {
//     switch(code->kind) {
// 		case FUNC:
// 			break;
// 		case LABEL:
// 		case GOTO:
// 		case RETURN:
// 		case ARG:
// 		case PARAM:
// 		case READ:
// 		case WRITE: 
//             return (op == code->u.single.op);
// 		case ASSIGN:
// 		case CITE:
// 		case GETPOINTER:
// 		case ASSIGNPOINTER:
//             return (op == code->u.assign.op);
// 		case ADD:
// 		case SUB:
// 		case MUL:
// 		case DIV:
//             return ((op == code->u.binop.op1) || (op == code->u.binop.op2));
// 		case IFGOTO: 
//             return ((op == code->u.ifgoto.re1) || (op == code->u.ifgoto.re2));
// 		case DEC:
// 		case CALL:
// 		default: break;
//     }
//     return false;
// }

// bool op_used(Operand op, InterCode code) {
//     if (op == NULL) {
//         return false;
//     }
//     InterCode cur = codeRoot;
//     while (cur != code) {
//         cur = cur->next;
//     }
//     while ((cur != NULL) && (cur->kind != FUNC)) {
//         if (op_existed(op, cur)) {
//             return true;
//         }
//     }
//     return false;
// }

// char* regT(Operand op, InterCode code) {
//     // search for an empty regT
//     for (int i = 0; i < REG_T; i++) {
//         if (!t_list[i]->if_used) {
//             // find an empty regT
//             t_list[i]->if_used = true;
//             t_list[i]->op = op;
//             return t_list[i]->name;
//         }
//     }

//     // clear some regT
//     for (int i = 0; i < REG_T; i++) {
//         if (!op_used(t_list[i]->op, code)) {
//             // this op is no longer used
//             Operand temp = t_list[i]->op;
//             switch (temp->kind) {
//                 case O_LABEL:
//                 case CONSTANT:
//                     return "WTF???";
//                 case VARIABLE: // save variable
//                 case TEMP: 
//                     t_list[i]->op = op;
//                     return t_list[i]->name;
//             }
//         }
//     }

//     return "FULL";
// }

// char* reg(Operand op, InterCode code) {
//     Register reg = search_reg(op);
//     if (reg != NULL) {
//         return reg->name;
//     }
//     else {
//         return regT(op, code);
//     }
// }

// char* regS(Operand op, InterCode code) {
//     // search for an empty regS
//     for (int i = 0; i < REG_S; i++) {
//         if (!s_list[i]->if_used) {
//             // find an empty regS
//             s_list[i]->if_used = true;
//             s_list[i]->op = op;
//             return s_list[i]->name;
//         }
//     }

//     // clear some regS
//     for (int i = 0; i < REG_S; i++) {
//         if (!op_used(s_list[i]->op, code)) {
//             // this op is no longer used
//             Operand temp = s_list[i]->op;
//             switch (temp->kind) {
//                 case O_LABEL:
//                 case CONSTANT:
//                     return "WTF???";
//                 case VARIABLE: // save variable
//                 case TEMP: 
//                     s_list[i]->op = op;
//                     return s_list[i]->name;
//             }
//         }
//     }

//     return "FULL";
// }

// void sw_live_T(InterCode code) {
//     int live_num = 0;
//     for (int i = 0; i < REG_T; i++) {
//         if (!op_used(t_list[i]->op, code)) {
//             // this op is no longer used
//             t_list[i]->op = NULL;
//             t_list[i]->if_used = false;
//         }
//         else {
//             // save live
//             fprintf(fp, "sw $%s, %d($sp)\n", t_list[i]->name, live_num * 4);
//             live_num++;
//         }
//     }
// }

// void lw_live_T(InterCode code) {
//     int live_num = 0;
//     for (int i = 0; i < REG_T; i++) {
//         if (!op_used(t_list[i]->op, code)) {
//             // this op is no longer used
//             t_list[i]->op = NULL;
//             t_list[i]->if_used = false;
//         }
//         else {
//             // save live
//             fprintf(fp, "lw $%s, %d($sp)\n", t_list[i]->name, live_num * 4);
//             live_num++;
//         }
//     }
// }

// void arg_move(Operand op, InterCode code) {
//     Register reg = search_reg(op);
//     if (reg == NULL) {
//         return;
//     }
//     if (arg_num == 0) {
//         InterCode cur = code;
//         int num = 0;
//         while (cur->kind == ARG) {
//             num++;
//             cur = cur->next;
//         }
//         if (func) {
//             fprintf(fp, "subu $sp, $sp, %d\n", 4 * (num + 1));
//         }
//         else if (num > 4) {
//             fprintf(fp, "subu $sp, $sp, %d\n", 4 * (num - 5));
//         }
//     }
//     if ((arg_num < 4) && !func) {
//         fprintf(fp, "move $%s, $%s\n", a_list[arg_num]->name, reg->name);
//     }
//     else if(!func) {
//         fprintf(fp, "sw $%s, %d($sp)\n", reg->name, 4 * (arg_num - 5));
//     }
//     else {
//         fprintf(fp, "sw $%s, %d($sp)\n", reg->name, 4 * arg_num);
//     }
//     arg_num++;
// }

// void param_define(Operand op) {
//     func = true;
//     if (param_num > 4) {

//     }
//     else {
//         a_list[param_num]->op = op;
//     }
// }

// void print_code(InterCode code) {
// 	//printf("Start!");
// 	//printf("code type: %d: ", code->kind);
// 	//FILE *fp=NULL;
//     char result[5];
// 	switch(code->kind) {

// 		case LABEL:
// 			//printf("LABEL ");
// 			//fputs("LABEL ",fp);
//             fprintf(fp, "%s", code->u.single.op->u.label.name);
// 			//printf(" :\n");
// 			//fputs(" :\n",fp);
// 			printf(":\n");
// 			fputs(":\n",fp);
// 			break;
// 		case FUNC:
// 			//printf("FUNCTION %s :\n", code->u.func.name);
// 			printf("\t%s:\n", code->u.func.name);
// 			//fprintf(fp,"FUNCTION %s :\n",code->u.func.name);
// 			fprintf(fp,"\t%s:\n",code->u.func.name);
//             init_reg();
//             func = false;
// 			break;
// 		case ASSIGN:			 
// 			//print_op(code->u.assign.result);
//             if(code->u.assign.result->kind==TEMP||
//             (code->u.assign.result->kind==VARIABLE&&code->u.assign.result->if_pointer==false&&code->u.assign.result->if_address==false))
//             {
                
//                 strcpy(result, reg(code->u.assign.result,code));
//                 switch (code->u.assign.op->kind)
//                 {
//                 case CONSTANT:
//                     fprintf(fp,"\tli %s , %d\n",result,code->u.assign.op->u.value);
//                     break;
//                 case VARIABLE:
// 				case TEMP:
//                     {char op[5];
//                     strcpy(op, reg(code->u.assign.op,code));
//                     if(code->u.assign.op->if_pointer==false&&code->u.assign.op->if_address==false)
//                     {
//                         fprintf(fp,"\tmove %s, %s\n",result,op);
//                     }
//                     else if(code->u.assign.op->if_pointer==true||code->u.assign.result->if_address==true) 
//                     {
//                         fprintf(fp,"\tlw %s, 0(%s)\n",result,op);
//                     }        
//                     break;
//                     }
//                 default:
//                     break;
//                 }
//             }
//             else if((code->u.assign.result->kind==VARIABLE)&&(code->u.assign.result->if_pointer==true||code->u.assign.result->if_address==true))
//             {
                
//                 strcpy(result, reg(code->u.assign.result,code));
//                 switch (code->u.assign.op->kind)
//                 {
//                 case CONSTANT:
//                     fprintf(fp,"\tsw %d , 0(%s)\n",code->u.assign.op->u.value,result);
//                     break;
//                 case VARIABLE:
// 				case TEMP:
//                     {char op[5];
//                     strcpy(op, reg(code->u.assign.op,code));
//                         fprintf(fp,"\tsw %s, 0(%s)\n",op,result);    
//                     break;}
//                 default:
//                     break;
//                 }
//             }
// 			break;
// 		case ADD:
//             {
//             strcpy(result, reg(code->u.binop.result,code));
//             //char *result=reg(code->u.binop.result,code);
//             if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
//             {
//                 fprintf(fp,"\taddi %s, %d, %d\n",result,code->u.binop.op1->u.value,code->u.binop.op2->u.value);
//             }
//             else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\taddi %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
//             }
//             else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 fprintf(fp,"\taddi %s, %s, %d\n",result,op1,code->u.binop.op2->u.value);
//             }
//             else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tadd %s, %s, %s\n",result,op1,op2);
//             }
// 			break;}
// 		case SUB:
//             {
//             strcpy(result, reg(code->u.binop.result,code));
//             if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
//             {
//                 fprintf(fp,"\taddi %s, %d, %d\n",result,code->u.binop.op1->u.value,-code->u.binop.op2->u.value);
//             }
//             else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tsub %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
//             }
//             else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 fprintf(fp,"\taddi %s, %s, %d\n",result,op1,-code->u.binop.op2->u.value);
//             }
//             else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tsub %s, %s, %s\n",result,op1,op2);
//             }
// 			break;
//             }
// 		case MUL:
// 			{
//             strcpy(result, reg(code->u.binop.result,code));
//             if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
//             {
//                 fprintf(fp,"\tmul %s, %d, %d\n",result,code->u.binop.op1->u.value,code->u.binop.op2->u.value);
//             }
//             else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tmul %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
//             }
//             else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 fprintf(fp,"\tmul %s, %s, %d\n",result,op1,-code->u.binop.op2->u.value);
//             }
//             else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tmul %s, %s, %s\n",result,op1,op2);
//             }
// 			break;
//             }
// 		case DIV:
            
//             strcpy(result, reg(code->u.binop.result,code));
//             if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
//             {
//                 fprintf(fp,"\tdiv %d, %d\n",code->u.binop.op1->u.value,code->u.binop.op2->u.value);
//                 fprintf(fp,"\tmflo %s\n",result);
//             }
//             else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tdiv %d, %s\n",code->u.binop.op1->u.value,op2);
//                 fprintf(fp,"\tmflo %s\n",result);
//             }
//             else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 fprintf(fp,"\tdiv %s, %d\n",op1,-code->u.binop.op2->u.value);
//                 fprintf(fp,"\tmflo %s\n",result);
//             }
//             else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.binop.op1,code));
//                 char op2[5];
//                 strcpy(op2,reg(code->u.binop.op2,code));
//                 fprintf(fp,"\tdiv %s, %s\n",op1,op2);
//                 fprintf(fp,"\tmflo %s\n",result);
//             }
// 			break;
// 		case CITE:
// 			 fputs("\n",fp);
// 			/* print_op(code->u.assign.result);
// 			printf(" := &");
// 			fputs(" := &",fp);
// 			print_op(code->u.assign.op);
// 			printf("\n");
// 			fputs("\n",fp);*/
        
// 			break;
// 		case GETPOINTER:
// 			 /* 
// 			print_op(code->u.assign.result);
// 			printf(" := *");
// 			fputs(" := *",fp);
// 			print_op(code->u.assign.op);
// 			printf("\n");
// 			fputs("\n",fp);*/
//             fputs("\n",fp);
// 			break;
// 		case ASSIGNPOINTER:
			 
// 		/* 	printf("*");
// 			fputs("*",fp);

// 			print_op(code->u.assign.result);
// 			printf(" := ");
// 			fputs(" := ",fp);
			
// 			print_op(code->u.assign.op);
// 			printf("\n");
			
// 			fputs("\n",fp);*/
//             fputs("\n",fp);
// 			break;
// 		case GOTO:	 
// 			/*printf("GOTO ");
// 			fputs("GOTO ",fp);	*/
//             fprintf(fp, "\tj %s\n",code->u.single.op->u.label.name);
// 			//print_op(code->u.single.op);
// 			break;
// 		case IFGOTO:			 
// 			/* printf("IF ");			
// 			fputs("IF ",fp);			
// 			print_op(code->u.ifgoto.re1);
// 			printf(" ");			
// 			fputs(" ",fp);*/
// 			switch(code->u.ifgoto.kind) {
// 				case G: fputs("\tbgt ",fp);break;
// 				case GE: fputs("\tbge ",fp);break;
// 				case L: fputs("\tblt ",fp);break;
// 				case LE:  fputs("\tble ",fp);break;
// 				case E: fputs("\tbeq ",fp);break;
// 				case NE:fputs("\tbne ",fp);break;
// 				default: break;
// 			}		
// 			/* print_op(code->u.ifgoto.re2);
// 			printf(" GOTO ");
			
// 			fputs(" GOTO ",fp);
			
// 			print_op(code->u.ifgoto.label);
// 			printf("\n");
			
// 			fputs("\n",fp);*/
            
//             if(code->u.ifgoto.re1->kind==CONSTANT&&code->u.ifgoto.re2->kind==CONSTANT)
//             {
//                 fprintf(fp,"%d, %d, %s\n",code->u.ifgoto.re1->u.value,code->u.ifgoto.re2->u.value,code->u.ifgoto.label->u.label.name);
//             }
//             else if(code->u.ifgoto.re1->kind==CONSTANT&&(code->u.ifgoto.re2->kind==VARIABLE||code->u.ifgoto.re2->kind==TEMP))
//             {
//                 char op2[5];
//                 strcpy(op2,reg(code->u.ifgoto.re2,code));
//                 fprintf(fp,"%d, %s, %s\n",code->u.ifgoto.re1->u.value,op2,code->u.ifgoto.label->u.label.name);
//                 //fprintf(fp,"\tmul %s, %d, %s\n",result,code->u.ifgoto.re1->u.value,op2);
//             }
//             else if(code->u.ifgoto.re2->kind==CONSTANT&&(code->u.ifgoto.re1->kind==VARIABLE||code->u.ifgoto.re1->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.ifgoto.re1,code));
//                 //fprintf(fp,"\tmul %s, %s, %d\n",result,op1,-code->u.ifgoto.re2->u.value);
//                 fprintf(fp,"%s, %d, %s\n",op1,code->u.ifgoto.re2->u.value,code->u.ifgoto.label->u.label.name);
//             }
//             else if((code->u.ifgoto.re1->kind==VARIABLE||code->u.ifgoto.re1->kind==TEMP)&&(code->u.ifgoto.re2->kind==VARIABLE||code->u.ifgoto.re2->kind==TEMP))
//             {
//                 char op1[5];
//                 strcpy(op1,reg(code->u.ifgoto.re1,code));
//                 char op2[5];
//                 strcpy(op2,reg(code->u.ifgoto.re2,code));
//                 //fprintf(fp,"\tmul %s, %s, %s\n",result,op1,op2);
//                 fprintf(fp,"%s, %s, %s\n",op1,op2,code->u.ifgoto.label->u.label.name);
//             }
// 			break;
// 		case RETURN:
//             if(code->u.single.op->kind==VARIABLE||code->u.single.op->kind==TEMP)
//             {
//                 char op[5];
//                 strcpy(op,reg(code->u.single.op,code));
//                 //char *op=reg(code->u.single.op,code);
//                 fprintf(fp,"\tmove $v0, %s\n",op);
//                 fprintf(fp,"\tjr $ra\n");
//             }
//             else
//             {
//                 fprintf(fp,"\tmove $0, %d\n",code->u.single.op->u.value);
//                 fprintf(fp,"\tjr $ra\n");
//             }
// 			break;
// 		case DEC:
// 			break;
// 		case ARG:		 
// 			arg_move(code->u.single.op, code);
// 			break;
// 		case CALL:
//             sw_live_T(code);		 
// 			if (func) {
//                 fprintf(fp, "sw $ra, %d($sp)\n", param_num * 4);
//             }
//             else {
//                 fprintf(fp, "addi $sp, $sp, -4\n");
//                 fprintf(fp, "sw $ra, 0($sp)\n");
//             }
//             fprintf(fp, "jal %s\n", code->u.call.name);
//             if (func) {
//                 for (int i = 0; i < REG_A; i++) {
//                     if (i >= param_num) {
//                         break;
//                     }
//                     fprintf(fp, "lw $%s, %d($sp)\n", a_list[i]->name, (i - 1) * 4);
//                 }
//                 fprintf(fp, "addi $sp, $sp, %d\n", (param_num + 1) * 4);
//             }
//             else {
//                 fprintf(fp, "addi $sp, $sp, 4\n");
//             }
//             fprintf(fp, "move %s, $v0\n", reg(code->u.call.ret, code));
//             lw_live_T(code);
// 			break;
// 		case PARAM:		 
// 			param_define(code->u.single.op);
// 			break;
// 		case READ:
//             if (func) {
//                 fprintf(fp, "sw $ra, %d($sp)\n", param_num * 4);
//             }
//             else {
//                 fprintf(fp, "addi $sp, $sp, -4\n");
//                 fprintf(fp, "sw $ra, 0($sp)\n");
//             }
//             fprintf(fp, "jal read\n");
//             if (func) {
//                 for (int i = 0; i < REG_A; i++) {
//                     if (i >= param_num) {
//                         break;
//                     }
//                     fprintf(fp, "lw $%s, %d($sp)\n", a_list[i]->name, (i - 1) * 4);
//                 }
//                 fprintf(fp, "addi $sp, $sp, %d\n", (param_num + 1) * 4);
//             }
//             else {
//                 fprintf(fp, "addi $sp, $sp, 4\n");
//             }
//             fprintf(fp, "move %s, $v0\n", reg(code->u.call.ret, code));
// 			break;
// 		case WRITE:
// 			if (func) {
//                 fprintf(fp, "sw $ra, %d($sp)\n", param_num * 4);
//             }
//             else {
//                 fprintf(fp, "addi $sp, $sp, -4\n");
//                 fprintf(fp, "sw $ra, 0($sp)\n");
//             }
//             fprintf(fp, "jal write\n");
//             if (func) {
//                 for (int i = 0; i < REG_A; i++) {
//                     if (i >= param_num) {
//                         break;
//                     }
//                     fprintf(fp, "lw $%s, %d($sp)\n", a_list[i]->name, (i - 1) * 4);
//                 }
//                 fprintf(fp, "addi $sp, $sp, %d\n", (param_num + 1) * 4);
//             }
//             else {
//                 fprintf(fp, "addi $sp, $sp, 4\n");
//             }
// 			break;
// 		default: break;
// 	}
	
// }

// void print_codeTree() {
// 	InterCode code = codeRoot;
// 	fp = fopen(file_name, "w");
//     fprintf(fp,".data\n_prompt: .asciiz \"Enter an integer:\"\n");
//     fprintf(fp,"_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\t li $s0, _prompt\n\tsyscall\n\tjr $ra\n\nwrite:\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n\n");

// 	while(code != NULL) {
// 		print_code(code);
// 		code = code->next;
// 	}
// 	fclose(fp);
// }