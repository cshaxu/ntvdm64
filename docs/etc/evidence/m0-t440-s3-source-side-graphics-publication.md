# M0 T440 S3 — source-side graphics publication

## Decision

This repair removes a project-authored cost before the existing publication
coalescer.  Each `InvalidateConsoleDIBits` rectangle previously caused
`console_graphics.c` to copy the entire logical DIB into a persistent snapshot
and then hand that snapshot to worker-base, where a later dirty update could
replace it.  A busy VGA painter could therefore pay several complete-frame
copies for one eventual visible frame.

`ntvdm_console_graphics` now records source dirtiness only.  The existing
worker-base publisher accepts that notification at its existing 50 Hz delivery
boundary, then invokes a private source-capture callback exactly once to build
the immutable full-frame payload.  The callback owns the DIB/palette read;
worker-base owns and frees the resulting process-heap allocation.  The
existing sender remains the sole transport owner, and NTCON receives every
frame that NTVDM elects to publish.

The final route drain is deliberately preserved.  On return to the canonical
text buffer or graphics-handle close, the graphics lock is released before
the publisher waits/captures.  This avoids a capture-vs-retirement lock cycle,
then forces the last dirty VGA frame through before the graphics surface is
marked inactive or destroyed.

## Historical recovery ledger

1. **Original source reuse.**  The reached original owner is
   `src/mvdm/softpc.new/host/src/nt_vga.c`: it writes the simulated DIB while
   holding its painter mutex, releases that mutex, and calls the output
   invalidation hook.  It remains unchanged.  SoftPC's comparison display
   path rejects an unchanged dirty generation before taking a capture.
2. **Smallest compatible seam.**  The project-owned
   `src/ntvdm-exe/win32/console_graphics.c` is the existing compatibility
   adapter for the original Console-graphics hook.  Its new dirty/capture
   split retains painter ordering and moves only the adapter's own DIB copy to
   the pre-existing delivery boundary.  The private worker-base capture seam
   retains the existing cadence, sender, shutdown, final-drain and copied
   payload ownership contracts.
3. **External intrusion.**  Not used; no OpenNT/MVDM mirror source or guest
   media changed.
4. **New behavior.**  Not selected.  There is no new protocol, process,
   timer policy, renderer policy, or guest display rule.

## Verification

| Check | Result |
| --- | --- |
| `build/M0-T440/S1/r001-current-x86/run-ninja-parallel.cmd product-programs` | Pass; changed x86 `ntvdm.exe` relinked and VdmTib ownership check passed. |
| `verify-software-video-publisher.ps1` (`r021-publisher`) | Pass: normal coalescing, no capture while idle, deferred source capture, final forced drain, resume/error and 50 destroy/join cycles. |
| `console-graphics-publication-test.exe` (`r022-graphics-source`) | Pass: 200 changed-DIB invalidations produce one deferred full-frame capture/send with the final pixel; retirement produces its final capture/send. |
| `verify-dos-video-source-contract.ps1` | Pass: original DIB write-before-invalidate, format and original final mode-settle contracts remain present. |
| `window-render.exe` (`r023-window-render`) | Pass: unchanged Window renderer 640x480 p50 581 us, p95 704 us, max 1252 us. This is a renderer microbenchmark, not Win3.1 end-to-end latency. |
| `console-client-test.exe` | Compiles against the asynchronous fixture update, but its initial `ntvdm_console_client_begin` currently fails with error 6 before any graphics test. This is retained as unavailable environment evidence, not counted as a pass. |
| Published package | Pass: the ten-image manifest at `build/M0-T440/S3/r026-publish/published-manifest.json` matches `O:/winnt/system32`; `ntvdm.exe` is `085AC4266E99472A7586B00C1235BA5E94C33E7FA71E10963FA968B263E9CB24`. |

## Retained boundary

The ordinary 50 Hz publisher policy is unchanged, as requested.  Source-side
capture coalescing proves that redundant DIB construction is no longer paid
before that policy, but does not prove an end-to-end Win3.1/RDP mouse or frame
latency improvement.

The existing Console-startup observer's post-publication COMMAND smoke still
returns `ERROR_INVALID_PARAMETER (87)` before a frontend/graphics handshake.
S1 previously recorded the same non-console/private-observer limitation.  It
is retained as unavailable evidence, not reported as a S3 regression or a
runtime pass.  Owner manual Window/Win3.1 acceptance remains required.
