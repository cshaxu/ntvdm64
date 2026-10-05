/* The original classifier's _X86_ branch identifies OS/2 NE status, not a
 * native calling convention. Enable that retained x86 guest rule only after
 * native Windows declarations have been selected for this translation unit.
 * Never applied to worker, MVDM or other Hook sources. */
#include <nt.h>
#if defined(_WIN64) && !defined(_X86_)
#define _X86_ 1
#endif
