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
int main(void) {
    void **a = NIDDB_Allocate_Instance_Data(16, NULL, NULL);
    void **b = NIDDB_Allocate_Instance_Data(16, NULL, NULL);
    int ordinal;
    if (!a || !b) return 2;
    armed = 1;
    for (ordinal = 0; ordinal <= 2; ++ordinal) {
        calls = 0; fail_at = ordinal ? ordinal : -1; eax_value = 3; ebx_value = 100;
        virtual_device_trap();
        if (calls != ordinal || live != 0 || errors != ordinal + 1 || carries != ordinal + 1) {
            printf("OBJECT_FAIL ordinal=%d calls=%d live=%d errors=%d carries=%d\n",
                   ordinal, calls, live, errors, carries); return 1;
        }
    }
    fail_at = 0; calls = 0; eax_value = 3; ebx_value = 100;
    virtual_device_trap();
    if (calls != 2 || live != 3 || errors != 3 || carries != 3) return 3;
    armed = 0; eax_value = 5; virtual_device_trap();
    if (live != 0) return 4;
    NIDDB_Deallocate_Instance_Data(a); NIDDB_Deallocate_Instance_Data(b);
    puts("OBJECT_PASS ordinals=3 failure_status=3 recreate=1 retirement=1");
    return 0;
}
