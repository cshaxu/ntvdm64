/* Link selected original command object; deliberately replace worker termination. */
#include <stdio.h>
static unsigned calls;
void TerminateVDM(void){++calls;}
extern void cmdExitVDM(void);
int main(void){cmdExitVDM();printf("TerminateVDM calls=%u\n",calls);return calls==1?0:1;}
