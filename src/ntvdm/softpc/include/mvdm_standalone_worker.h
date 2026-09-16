#ifndef MVDM_STANDALONE_WORKER_H
#define MVDM_STANDALONE_WORKER_H

#include <setjmp.h>
#include <windows.h>

/* The standalone process binding is deliberately finite: it owns only data
 * that NT4 process creation/CSRSS supplied to this one worker address space.
 * It does not classify images, own command records or schedule VDM work. */
DWORD mvdm_standalone_worker_begin(void);
jmp_buf *mvdm_standalone_worker_termination_escape(void);
DWORD mvdm_standalone_worker_completion_code(void);
int mvdm_standalone_worker_finish(int result);
void mvdm_standalone_worker_record_phase(const char *phase);

#endif
