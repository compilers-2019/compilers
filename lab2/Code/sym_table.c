#include "sym_table.h"

unsigned int hash_pjw(char* name)
{
	unsigned int val = 0, i;
	for( ; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~0x3fff)
			val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val;
}

//检查类型
//检查参数
//检查重定义
//建立符号表
//检查符号
//建立函数表
//检查函数

/* non-terminal function*/
/*void Program(struct TreeNode *p)
{
	ExtDefList(p->child);
}
void ExtDefList(struct TreeNode *p)
{
	struct TreeNode *children=p->child;
	if(children!=NULL)
	{
		ExtDef(children);
		ExtDefList(children->next);
	}

}*/
/*definition of variables*/
/*void ExtDef(struct TreeNode *p)
{	

}*/
