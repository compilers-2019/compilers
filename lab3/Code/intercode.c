#include "intercode.h"

Temp new_temp() {
		temp_num++;
		Temp t = malloc(sizeof(struct Temp_));
		temp_table[temp_num] = t;
		t->num = temp_num;
		strcpy(t->name, "t");
		char str[4];
		//itoa(t->num, str, 10);
		sprintf(str, "%d", t->num);
		strcat(t->name, str);
		return t;
}
