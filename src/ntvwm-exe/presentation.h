#ifndef NTVWM_PRESENTATION_H
#define NTVWM_PRESENTATION_H
#include <windows.h>
#include "common/protocol/console_io.h"
typedef struct ntvwm_presentation ntvwm_presentation;
/* Borrow authenticated pipe/frontend/stop references for this endpoint's
 * lifetime. One serialized request stream; close only after users join. */
DWORD ntvwm_presentation_open(HANDLE pipe,HANDLE frontend,HANDLE stop,DWORD generation,
    ntvwm_presentation **);
void ntvwm_presentation_close(ntvwm_presentation *);
/* Same operations as NTVDM, including explicit activation. Publishing a frame
 * never activates or selects this backend. Transport failure is latched;
 * ordinary frontend errors (including inactive owner) are returned unchanged. */
DWORD ntvwm_presentation_call(ntvwm_presentation *,const console_io_request *,console_io_reply *);
DWORD ntvwm_presentation_text(ntvwm_presentation *,const console_video_description *,
    const void *,SIZE_T);
/* Capture the actual active Console buffer. The supplied backend font state
 * is unchanged; geometry changes return ERROR_RETRY, not a partial frame. */
DWORD ntvwm_presentation_capture(ntvwm_presentation *,const console_text_style *);
/* Transfer one bounded frontend batch to the real Console. Accepted is not
 * consumed; a failed/partial transfer is never replayed. No implicit activation. */
DWORD ntvwm_presentation_input(ntvwm_presentation *,HANDLE input,DWORD *accepted);
/* Caller holds the execution handoff: no target may write while importing.
 * Copy the current frontend cells/cursor into the real backend before launch.
 * This does not activate, start a target, or change Console input modes. */
DWORD ntvwm_presentation_seed(ntvwm_presentation *,HANDLE output);
/* Execution-edge operations, never invoked merely because a frame arrives.
 * Begin rolls back activation if screen import fails. End attempts release
 * even on final-frame failure and preserves the first failure for its caller. */
DWORD ntvwm_presentation_begin(ntvwm_presentation *,HANDLE output);
DWORD ntvwm_presentation_end(ntvwm_presentation *,const console_text_style *);
#endif
