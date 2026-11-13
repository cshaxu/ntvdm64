#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_reg.h>
#include <c_seg.h>
#include "ntvdm-exe/session/session.h"
extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void), sas_init(IU32), c_cpu_init(void);
extern void c_setPE(int), c_setCPL(int);
int main(void) {
 session owner; int ids[]={0,3,4,5}; unsigned i; DWORD written;
 session_initialize(&owner,483);
 if(!session_activate(&owner)||!session_thread_bind(&owner)||!setup_global_data_ptr()) return 2;
 sas_init(0x200000);setup_vga_globals();c_cpu_init();c_setPE(1);c_setCPL(3);
 for(i=0;i<4;i++) {
  int n=ids[i]; SET_SR_LIMIT(n,0);SET_SR_AR_W(n,0);SET_SR_AR_R(n,0);
  SET_SR_AR_E(n,1);SET_SR_AR_C(n,1);SET_SR_AR_X(n,1);SET_SR_AR_DPL(n,0);
  load_data_seg(n,0x40);
  if(GET_SR_SELECTOR(n)!=0x40||GET_SR_BASE(n)!=0x400||GET_SR_LIMIT(n)!=0xffff||
    GET_SR_AR_DPL(n)!=3||GET_SR_AR_W(n)!=1||GET_SR_AR_R(n)!=1||
    GET_SR_AR_E(n)||GET_SR_AR_C(n)||GET_SR_AR_X(n))return 3;
  load_data_seg(n,0);
  if(GET_SR_SELECTOR(n)||GET_SR_BASE(n)||GET_SR_LIMIT(n)||GET_SR_AR_W(n)||GET_SR_AR_R(n))return 4;
 }
 WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),"SEGMENT_STATE_4_REGISTERS_AND_NULL_OK\n",sizeof("SEGMENT_STATE_4_REGISTERS_AND_NULL_OK\n")-1,&written,NULL);
 return 0;
}
