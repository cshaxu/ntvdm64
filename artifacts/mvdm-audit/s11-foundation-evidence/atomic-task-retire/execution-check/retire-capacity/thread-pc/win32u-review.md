# System call stub observation

The prior three-round game-thread sample contains 73 instruction positions in
`win32u.dll`. The test identified the loaded 32-bit module's base and size;
the current `SysWOW64/win32u.dll` export table was dumped and each position
matched to the nearest export within 16 bytes. The system DLL and raw sample
hashes, dump, and [analysis](win32u-analysis.json) are retained. No loaded
module byte hash was captured, so the disk export table is an attribution
assumption rather than proof of the exact mapped image.

The most frequent inferred stubs were `NtUserGetWindowContextHelpId` (13 open,
four close), `NtUserGetAncestor` (nine open, four close), `NtUserGetProp2`
(seven open, six close), and `NtUserCreateWindowEx` (six open). Individual
painting and GDI stubs also appeared. `capture_window` calls the public
context-help API and the relationship sampler calls `GetAncestor`; another
page-domain path also calls the context-help API. Property checks have multiple
callers. No validated stack was captured, so the samples cannot identify
which caller produced any individual stub. They corroborate a native-window
observation lead, not a proven root cause.

Sampling periodically suspends the game thread, changes timings, and yields
instruction positions rather than call counts, duration, or exclusive CPU
percentages. The earlier QPC projection figures remain inclusive and do not
provide a complete latency budget. No API, lifecycle check or runtime code
was removed based on this attribution.

## Context-help ownership trace

`capture_window` samples `GetWindowContextHelpId` into a graph observation;
the page-domain commit copies it into the retained exact-generation borrow.
`wow_window_client_context_help` later pins that generation and reads its
stored value for original USER/control code. A second refresh path samples
the same API in `page_domain_window_info`. Original dialog/control code can
both set and read help IDs. Therefore replacing the sampled field with a
permanent zero or an independent cache would alter behavior. Deferring the
query until the original reader runs is not automatically equivalent either:
the retained exact generation can outlive native window destruction, or its
numeric HWND can be reused for another window. A deferred native query would
need an explicit authoritative value at retirement and a proven update rule
for external changes; neither is established by the current samples. No
context-help change is admitted solely from this performance observation.

This is also covered by an existing selected-object boundary witness, not
only a source inference. In `wow_page_domain_fixture.c`, 32 iterations set a
local help ID, publish the window, change the native ID, and assert the
retained view stays at the published value. A retained pin remains readable
after native destruction. The same fixture verifies a foreign window's old
snapshot remains stable until explicit publication or a mapped native setter
refreshes it. The integrated graph fixture records
`WOW_EXTERNAL_REFRESH_BOUNDARY_CCPU errors=0 retained=1 explicit_refresh=1
native_mutation=1 local_fresh=1` in `../integrated-graph/stderr.txt`.
These are controlled x86 lifecycle checks, not every destruction/reuse
interleaving or a proof of overall dialog behavior.
