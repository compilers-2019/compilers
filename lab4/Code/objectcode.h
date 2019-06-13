// #ifndef __OBJECT_CODE__
// #define __OBJECT_CODE__

// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include "intercode.h"

// #define REG_T 10
// #define REG_S 8
// #define REG_A 4
// #define REG_V 2

// typedef struct Register_* Register;
// struct Register_ {
//     char name[5];
//     bool if_used;
//     Operand op;
// };

// Register t_list[REG_T];
// Register s_list[REG_S];
// Register a_list[REG_A];
// Register v_list[REG_V];

// int arg_num = 0;
// int param_num = 0;
// bool func = false;

// void init_reg();
// char* reg(Operand op, InterCode code);
// char* regT(Operand op, InterCode code);
// char* regS(Operand op, InterCode code);
// bool reg_used(Operand op, InterCode code);
// bool op_existed(Operand op, InterCode code);
// void sw_live_T(InterCode code);
// void lw_live_T(InterCode code);

// void print_code(InterCode code);
// void print_codeTree();

// #endif