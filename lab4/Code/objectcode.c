#include "intercode.h"

void print_op(Operand op) {
	//FILE *fp=NULL;
	switch(op->kind) {
		case VARIABLE:
			if(op->if_pointer == true) {
				printf("*");
				fprintf(fp, "*");
			}
			else if(op->if_address == true) {
				printf("&");
				fprintf(fp, "&");
			}
			printf("%s", sym_table[op->u.var_no]->name);
			fprintf(fp,"%s", sym_table[op->u.var_no]->name);
			break;
		case CONSTANT:		 
			printf("#%d", op->u.value);		
			fprintf(fp,"#%d", op->u.value);
			break;
		case TEMP:			 
			printf("%s", op->u.temp.name);			
			fprintf(fp,"%s", op->u.temp.name);
			break;
		case O_LABEL:			 
			printf("%s", op->u.label.name);		
			fprintf(fp,"%s", op->u.label.name);
			break;
		default: break;
	}
}

void print_code(InterCode code) {
	//printf("Start!");
	//printf("code type: %d: ", code->kind);
	//FILE *fp=NULL;
	switch(code->kind) {
		case LABEL:
			//printf("LABEL ");
			//fputs("LABEL ",fp);

			print_op(code->u.single.op);
			//printf(" :\n");
			//fputs(" :\n",fp);
			printf(":\n");
			fputs(":\n",fp);
			break;
		case FUNC:
			//printf("FUNCTION %s :\n", code->u.func.name);
			printf("\t%s:\n", code->u.func.name);
			//fprintf(fp,"FUNCTION %s :\n",code->u.func.name);
			fprintf(fp,"\t%s:\n",code->u.func.name);
			break;
		case ASSIGN:			 
			//print_op(code->u.assign.result);
            if(code->u.assign.result->kind==TEMP||
            (code->u.assign.result->kind==VARIABLE&&code->u.assign.result->if_pointer==false&&code->u.assign.result->if_address==false))
            {
                char *result=reg(code->u.assign.result,code);
                switch (code->u.assign.op->kind)
                {
                case CONSTANT:
                    fprintf(fp,"\tli %s , %d\n",result,code->u.assign.op->u.value);
                    break;
                case VARIABLE:
				case TEMP:
                    char *op=reg(code->u.assign.op,code);
                    if(code->u.assign.op->if_pointer==false&&code->u.assign.op->if_address==false)
                    {
                        fprintf(fp,"\tmove %s, %s\n",result,op);
                    }
                    else if(code->u.assign.op->if_pointer==true||code->u.assign.result->if_address==true) 
                    {
                        fprintf(fp,"\tlw %s, 0(%s)\n",result,op);
                    }        
                    break;
                default:
                    break;
                }
            }
            else if((code->u.assign.result->kind==VARIABLE)&&(code->u.assign.result->if_pointer==true||code->u.assign.result->if_address==true))
            {
                char *result=reg(code->u.assign.result,code);
                switch (code->u.assign.op->kind)
                {
                case CONSTANT:
                    fprintf(fp,"\tsw %d , 0(%s)\n",code->u.assign.op->u.value,result);
                    break;
                case VARIABLE:
				case TEMP:
                    char *op=reg(code->u.assign.op,code);
                        fprintf(fp,"\tsw %s, 0(%s)\n",op,result);    
                    break;
                default:
                    break;
                }
            }
			break;
		case ADD:
            char *result=reg(code->u.binop.result,code);
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\taddi %s, %d, %d\n",result,code->u.binop.op1->u.value,code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\taddi %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                fprintf(fp,"\taddi %s, %s, %d\n",result,op1,code->u.binop.op2->u.value);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tadd %s, %s, %s\n",result,op1,op2);
            }
			break;
		case SUB:
            char *result=reg(code->u.binop.result,code);
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\taddi %s, %d, %d\n",result,code->u.binop.op1->u.value,-code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tsub %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                fprintf(fp,"\taddi %s, %s, %d\n",result,op1,-code->u.binop.op2->u.value);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tsub %s, %s, %s\n",result,op1,op2);
            }
			break;
		case MUL:
			char *result=reg(code->u.binop.result,code);
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\tmul %s, %d, %d\n",result,code->u.binop.op1->u.value,code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tmul %s, %d, %s\n",result,code->u.binop.op1->u.value,op2);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                fprintf(fp,"\tmul %s, %s, %d\n",result,op1,-code->u.binop.op2->u.value);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tmul %s, %s, %s\n",result,op1,op2);
            }
			break;
		case DIV:
			/* print_op(code->u.binop.result);
			printf(" := ");
			fputs(" := ",fp);

			print_op(code->u.binop.op1);
			printf(" / ");
			fputs(" / ",fp);

			print_op(code->u.binop.op2);
			printf("\n");
			fputs("\n",fp);*/
            
            char *result=reg(code->u.binop.result,code);
            if(code->u.binop.op1->kind==CONSTANT&&code->u.binop.op2->kind==CONSTANT)
            {
                fprintf(fp,"\tdiv %d, %d\n",code->u.binop.op1->u.value,code->u.binop.op2->u.value);
                fprintf(fp,"\tmflo %s\n",result);
            }
            else if(code->u.binop.op1->kind==CONSTANT&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tdiv %d, %s\n",code->u.binop.op1->u.value,op2);
                fprintf(fp,"\tmflo %s\n",result);
            }
            else if(code->u.binop.op2->kind==CONSTANT&&(code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                fprintf(fp,"\tdiv %s, %d\n",op1,-code->u.binop.op2->u.value);
                fprintf(fp,"\tmflo %s\n",result);
            }
            else if((code->u.binop.op1->kind==VARIABLE||code->u.binop.op1->kind==TEMP)&&(code->u.binop.op2->kind==VARIABLE||code->u.binop.op2->kind==TEMP))
            {
                char *op1=reg(code->u.binop.op1,code);
                char *op2=reg(code->u.binop.op2,code);
                fprintf(fp,"\tdiv %s, %s\n",op1,op2);
                fprintf(fp,"\tmflo %s\n",result);
            }
			break;
		case CITE:
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
            fputs("\n",fp);
			break;
		case GOTO:	 
			/*printf("GOTO ");
			fputs("GOTO ",fp);	*/
            fprintf("\tj %s\n",code->u.single.op->u.label.name);
			//print_op(code->u.single.op);
			break;
		case IFGOTO:			 
			/* printf("IF ");			
			fputs("IF ",fp);			
			print_op(code->u.ifgoto.re1);
			printf(" ");			
			fputs(" ",fp);*/
			switch(code->u.ifgoto.kind) {
				case G: printf(">"); fputs("\tbgt ",fp);break;
				case GE: printf(">="); fputs("\tbge ",fp);break;
				case L: printf("<");fputs("\tblt ",fp);break;
				case LE: printf("<="); fputs("\tble ",fp);break;
				case E: printf("=="); fputs("\tbeq ",fp);break;
				case NE: printf("!="); fputs("\tbne ",fp);break;
				default: break;
			}		
			/* print_op(code->u.ifgoto.re2);
			printf(" GOTO ");
			
			fputs(" GOTO ",fp);
			
			print_op(code->u.ifgoto.label);
			printf("\n");
			
			fputs("\n",fp);*/
            
            if(code->u.ifgoto.re1==CONSTANT&&code->u.ifgoto.re2==CONSTANT)
            {
                fprintf(fp,"%d, %d, %s\n",code->u.ifgoto.re1->u.value,code->u.ifgoto.re2->u.value,code->u.ifgoto.label->u.label.name);
            }
            else if(code->u.ifgoto.re1->kind==CONSTANT&&(code->u.ifgoto.re2->kind==VARIABLE||code->u.ifgoto.re2->kind==TEMP))
            {
                char *op2=reg(code->u.ifgoto.re2,code);
                fprintf(fp,"%d, %s, %s\n",code->u.ifgoto.re1->u.value,op2,code->u.ifgoto.label->u.label.name);
                //fprintf(fp,"\tmul %s, %d, %s\n",result,code->u.ifgoto.re1->u.value,op2);
            }
            else if(code->u.ifgoto.re2->kind==CONSTANT&&(code->u.ifgoto.re1->kind==VARIABLE||code->u.ifgoto.re1->kind==TEMP))
            {
                char *op1=reg(code->u.ifgoto.re1,code);
                //fprintf(fp,"\tmul %s, %s, %d\n",result,op1,-code->u.ifgoto.re2->u.value);
                fprintf(fp,"%s, %d, %s\n",op1,code->u.ifgoto.re2->u.value,code->u.ifgoto.label->u.label.name);
            }
            else if((code->u.ifgoto.re1->kind==VARIABLE||code->u.ifgoto.re1->kind==TEMP)&&(code->u.ifgoto.re2->kind==VARIABLE||code->u.ifgoto.re2->kind==TEMP))
            {
                char *op1=reg(code->u.ifgoto.re1,code);
                char *op2=reg(code->u.ifgoto.re2,code);
                //fprintf(fp,"\tmul %s, %s, %s\n",result,op1,op2);
                fprintf(fp,"%s, %s, %s\n",op1,op2,code->u.ifgoto.label->u.label.name);
            }
			break;
		case RETURN:
            if(code->u.single.op->kind==VARIABLE||code->u.single.op->kind==TEMP)
            {
                char *op=reg(code->u.single.op,code);
                fprintf(fp,"\tmove $v0, $s\n",op);
                fprintf(fp,"\tjr $ra\n");
            }
            else
            {
                fprintf(fp,"\tmove $0, $d\n",code->u.single.op->u.value);
                fprintf(fp,"\tjr $ra\n");
            }
			break;
		case DEC:
			break;
		case ARG:		 
			
			break;
		case CALL:		 
			
			break;
		case PARAM:		 
			
			break;
		case READ:
			
			break;
		case WRITE:
			
			break;
		default: break;
	}
	
}

void print_codeTree() {
	InterCode code = codeRoot;
	fp = fopen(file_name, "w");
	while(code != NULL) {
		print_code(code);
		code = code->next;
	}
	fclose(fp);
}