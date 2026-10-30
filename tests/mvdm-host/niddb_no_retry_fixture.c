/* Allocation-symbol interception of a compiled NIDDB object. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
extern void **NIDDB_Allocate_Instance_Data(int, void (*)(void **), void (*)(void));
extern void NIDDB_Deallocate_Instance_Data(void **);
extern void virtual_device_trap(void);
static uint32_t eax_value, ebx_value;
static int armed, fail_at, calls, live, errors, carries;
static void *tracked[8];
static void remember(void *p) {
    int i; if (!armed || !p) return;
    for (i = 0; i < 8; ++i) if (!tracked[i]) { tracked[i] = p; ++live; return; }
    abort();
}
void *niddb_test_malloc(size_t size) {
    void *p; if (armed && ++calls == fail_at) return NULL;
    p = malloc(size); remember(p); return p;
}
void *niddb_test_calloc(size_t n, size_t size) {
    void *p;
    if (armed && fail_at == -1) return NULL;
    p = calloc(n, size); remember(p); return p;
}
void niddb_test_free(void *p) {
    int i; if (!p) return;
    for (i = 0; i < 8; ++i) if (tracked[i] == p) { tracked[i] = NULL; --live; break; }
    free(p);
}
uint32_t c_getEAX(void) { return eax_value; }
uint32_t c_getEBX(void) { return ebx_value; }
uint16_t c_getBX(void) { return (uint16_t)ebx_value; }
uint16_t c_getCX(void) { return 0; }
uint16_t c_getDX(void) { return 0; }
void c_setEAX(uint32_t v) { eax_value = v; }
void c_setEDX(uint32_t v) { (void)v; }
void c_setCF(uint32_t v) { if (v) ++carries; }
uint8_t c_sas_hw_at(uint32_t a) { (void)a; return 0; }
int c_SasRegisterVirtualSelectors(uint16_t a, uint16_t b) { (void)a; (void)b; return 1; }
void mvdm_softpc_report_virtual_device_control(uint32_t a, uint16_t b, uint16_t c, uint16_t d)
{ (void)a; (void)b; (void)c; (void)d; }
void host_error(int a, int b, const char *c, ...) { (void)a; (void)b; (void)c; ++errors; }
void ClearInstanceDataMarking(void) { }
extern int NIDDB_is_active(void);
extern void NIDDB_System_Reboot(void);
int main(void) {
  int mode, ordinal;
  for(mode=0;mode<2;++mode) for(ordinal=0;ordinal<=4;++ordinal) {
    void **a=NIDDB_Allocate_Instance_Data(16,NULL,NULL);
    void **b=NIDDB_Allocate_Instance_Data(16,NULL,NULL);
    void **c=NIDDB_Allocate_Instance_Data(16,NULL,NULL);
    void **d=NIDDB_Allocate_Instance_Data(16,NULL,NULL);
    if(!a || !b || !c || !d) return 2;
    armed=1; calls=0; errors=0; carries=0; fail_at=ordinal ? ordinal : -1;
    eax_value=3; ebx_value=100; virtual_device_trap();
    if(calls!=ordinal || live!=0 || errors!=1 || carries!=1 || !NIDDB_is_active()) {
      printf("NIDDB_NO_RETRY_FAIL mode=%d ordinal=%d calls=%d live=%d errors=%d carries=%d\n",
          mode,ordinal,calls,live,errors,carries);
      return 3;
    }
    armed=0;
    if(NIDDB_Allocate_Instance_Data(16,NULL,NULL)!=NULL) return 5;
    NIDDB_Deallocate_Instance_Data(a);
    if(!*a || !*b || !*c || !*d) return 6;
    if(mode==0) { eax_value=5; virtual_device_trap(); } else NIDDB_System_Reboot();
    if(NIDDB_is_active() || live!=0) return 4;
    NIDDB_Deallocate_Instance_Data(d);
    {
      void **replacement=NIDDB_Allocate_Instance_Data(16,NULL,NULL);
      if(!replacement) return 7;
      NIDDB_Deallocate_Instance_Data(replacement);
    }
    NIDDB_Deallocate_Instance_Data(a); NIDDB_Deallocate_Instance_Data(b);
    NIDDB_Deallocate_Instance_Data(c);
    printf("NIDDB_NO_RETRY mode=%d ordinal=%d active_after_failure=1 active_after_exit=0 live=0\n",mode,ordinal);
  }
  return 0;
}
