#ifndef NTW32_PRESENTATION_H
#define NTW32_PRESENTATION_H
#include <windows.h>
#include "interface/console_io.h"
typedef struct ntw32_presentation ntw32_presentation;
/* Borrow authenticated pipe/frontend/stop references for this endpoint's
 * lifetime. One serialized request stream; close only after users join. */
DWORD ntw32_presentation_open(HANDLE pipe,HANDLE frontend,HANDLE stop,DWORD generation,
    ntw32_presentation **);
void ntw32_presentation_close(ntw32_presentation *);
/* Same operations as NTVDM, including explicit activation. Publishing a frame
 * never activates or selects this backend. Transport failure is latched;
 * ordinary frontend errors (including inactive owner) are returned unchanged. */
DWORD ntw32_presentation_call(ntw32_presentation *,const console_io_request *,console_io_reply *);
DWORD ntw32_presentation_text(ntw32_presentation *,const console_video_description *,
    const void *,SIZE_T);
/* Capture the actual active Console buffer. The supplied backend font state
 * is unchanged; geometry changes return ERROR_RETRY, not a partial frame. */
DWORD ntw32_presentation_capture(ntw32_presentation *,const console_text_style *);
/* Transfer one bounded frontend batch to the real Console. Accepted is not
 * consumed; a failed/partial transfer is never replayed. No implicit activation. */
DWORD ntw32_presentation_input(ntw32_presentation *,HANDLE input,DWORD *accepted);
/* Caller holds the execution handoff: no target may write while importing.
 * Copy the current frontend cells/cursor into the real backend before launch.
 * This does not activate, start a target, or change Console input modes. */
DWORD ntw32_presentation_seed(ntw32_presentation *,HANDLE output);
/* Execution-edge operations, never invoked merely because a frame arrives.
 * Begin rolls back activation if screen import fails. End attempts release
 * even on final-frame failure and preserves the first failure for its caller. */
DWORD ntw32_presentation_begin(ntw32_presentation *,HANDLE output);
DWORD ntw32_presentation_end(ntw32_presentation *,const console_text_style *);
#endif
