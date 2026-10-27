#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
static unsigned reads,translations,logs,dispatches; static jmp_buf checkpoint; static int inject_fault;
static unsigned char return_ah(void){return 1;}
static unsigned char return_al(void){return 0;}
static unsigned short return_ax(void){return 0x100;}
static unsigned short return_ss(void){return 0x200;}
static unsigned short return_sp(void){return 0x300;}
unsigned char (*getAH_func)(void)=return_ah; unsigned char (*getAL_func)(void)=return_al;
unsigned short (*getAX_func)(void)=return_ax,*(*unused_dummy)(void);
unsigned short (*getSS_func)(void)=return_ss; unsigned short (*getSP_func)(void)=return_sp;
uint32_t effective_addr(uint16_t selector,uint32_t offset){translations++;return ((uint32_t)selector<<4)+offset;}
uint16_t c_sas_w_at(uint32_t address){(void)address;reads++;if(inject_fault)longjmp(checkpoint,1);return 0x1234;}
void mvdm_softpc_report_setup_hardware_probe(const char *name,unsigned long a,unsigned long b){(void)name;(void)a;(void)b;logs++;}
void idetect(int n){(void)n;dispatches++;}
void *working_keybd_funcs;
void keyboard_io(void);
int main(int argc,char **argv){int fault=0;(void)argv;inject_fault=argc>1;if(!setjmp(checkpoint))keyboard_io();else fault=1;fprintf(stderr,"reads=%u translations=%u logs=%u dispatches=%u fault=%d\n",reads,translations,logs,dispatches,fault);return 0;}
