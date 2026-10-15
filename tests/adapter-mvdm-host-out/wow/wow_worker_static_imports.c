#include <windows.h>

/* The production DLL imports these through ntvdm-wow32-provider.def:
 * call_ica_hw_interrupt = ica_hw_interrupt; ExpLdt = Ldt DATA.
 * This executable fixture statically links those same worker definitions.
 * Preserve the import address cell for original WOW32's dllimport data read;
 * do not allocate another LDT or replace an interrupt implementation. */
extern PVOID Ldt;
PVOID *_imp__ExpLdt = &Ldt;
#pragma comment(linker, "/alternatename:_call_ica_hw_interrupt=_ica_hw_interrupt")
