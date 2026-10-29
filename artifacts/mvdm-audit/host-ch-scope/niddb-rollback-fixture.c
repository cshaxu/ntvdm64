/* NIDDB allocation failure diagnostic. Allocation hooks are used only in
 * allocate_NIDDB; the remaining code is the current source. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void **NIDDB_Allocate_Instance_Data(int, void (*)(void **), void (*)(void));
extern void NIDDB_Deallocate_Instance_Data(void **);
extern void virtual_device_trap(void);
static uint32_t eax_value, ebx_value;
static int fail_at, calls, live, error_count, carry_count;
uint32_t c_getEAX(void) { return eax_value; }
uint32_t c_getEBX(void) { return ebx_value; }
uint16_t c_getBX(void) { return (uint16_t)ebx_value; }
uint16_t c_getCX(void) { return 0; }
uint16_t c_getDX(void) { return 0; }
void c_setEAX(uint32_t v) { eax_value = v; }
void c_setEDX(uint32_t v) { (void)v; }
void c_setCF(uint32_t v) { if (v) ++carry_count; }
uint8_t c_sas_hw_at(uint32_t a) { (void)a; return 0; }
int c_SasRegisterVirtualSelectors(uint16_t a, uint16_t b) { (void)a; (void)b; return 1; }
void mvdm_softpc_report_virtual_device_control(uint32_t a, uint16_t b, uint16_t c, uint16_t d)
{ (void)a; (void)b; (void)c; (void)d; }
void host_error(int a, int b, const char *c, ...) { (void)a; (void)b; (void)c; ++error_count; }
void ClearInstanceDataMarking(void) { }
void *niddb_test_calloc(size_t n, size_t size) {
    void *p = calloc(n, size); if (p) ++live; return p;
}
void *niddb_test_malloc(size_t size) {
    void *p; if (++calls == fail_at) return NULL;
    p = malloc(size); if (p) ++live; return p;
}
void niddb_test_free(void *p) { if (p) { --live; free(p); } }

int main(int argc, char **argv) {
    void **a = NIDDB_Allocate_Instance_Data(16, NULL, NULL);
    void **b = NIDDB_Allocate_Instance_Data(16, NULL, NULL);
    int ordinal;
    if (!a || !b) return 2;
    int first = argc == 2 ? atoi(argv[1]) : 1;
    int last = argc == 2 ? first : 2;
    if (first < 1 || last > 2) return 2;
    for (ordinal = first; ordinal <= last; ++ordinal) {
        fail_at = ordinal; calls = 0; eax_value = 3; ebx_value = 100;
        virtual_device_trap();
        if (calls != ordinal || live != 0) {
            printf("ROLLBACK_FAIL ordinal=%d calls=%d live=%d\n", ordinal, calls, live);
            return 1;
        }
    }
    fail_at = 0; calls = 0; eax_value = 3; ebx_value = 100;
    virtual_device_trap();
    if (calls != 2 || live != 3 || error_count != last - first + 1 ||
        carry_count != last - first + 1) return 3;
    /* Original normal retirement uses uninstrumented free. */
    eax_value = 5; virtual_device_trap();
    NIDDB_Deallocate_Instance_Data(a); NIDDB_Deallocate_Instance_Data(b);
    puts("ROLLBACK_PASS ordinals=2 recreate=1");
    return 0;
}
