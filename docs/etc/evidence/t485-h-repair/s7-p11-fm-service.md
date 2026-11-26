# S7 P11: FM service admission and original device lifecycle

## File and complete call chain

nt_sbfm.c starts with four non-error points/four lines: two added CCPU guards
around original OpenFMDevice and CloseFMDevice code. XP source blob is
9b312c2e9a06def7d5de4b5142c1a57137a1f3a0; the frozen IDs are
HXP-R-03958 through HXP-R-03961.

InitDevices calls OpenFMDevice, which enumerates MOD_FMSYNTH, opens its MIDI
handle, sets volume and requests direct Adlib I/O; otherwise it requests kernel
emulation. The actual compat/vdm_control.c provider returns
STATUS_NOT_IMPLEMENTED. Unlike the CLI requests in P10, the original code then
unconditionally sets FMActive=TRUE. FMDataWrite calls FMPortWrite; that active
state makes it call WriteFile with HFM, including NULL when no device exists.
Failure returns before Position is cleared and before software timer status
is updated. This is a concrete consumer of the wrongly asserted capability,
not a speculative need to exclude sound code.

The final adaptation restores both original device lifecycle bodies, assigns
FMActive from NT_SUCCESS of each existing service call, and removes the
unconditional TRUE assignment. No retry, new provider, callback, queue or
parallel state exists. CloseFMDevice retains original handle reset/close and
only requests USER_EMULATION when the service actually became active.
Original service absence remains absence: this does not implement FM audio.
S20 retains the separate audio-capability obligation.

## Mirror accounting and exclusions

Against XP, non-error differences change from 4 points/4 lines to 3 points/5
lines: one fewer point, one additional line. The owner standing approval for
small mirror changes covers this delta. The three residuals are two changed
service-call statements and removal of unconditional activation; all share the
actual unavailable-service/failure-state evidence above.

Original inactive bodies at XP lines 68-100, 154, 165 and 200-207 are replaced
with error-only bodies under their original conditions. Nested conditions
inside removed bodies disappear; selected timer semantics remain original.
Diagnostic anchors retain original selected line numbers. The added CCPU
guards are removed, not relabelled as free original exclusions. Whole-file
review and candidate identity are in build/t485-s7/p11/review.json.

## Verification

The new bounded test at test/mvdm-host/softpc.new/host/src/nt_sbfm_test.c links
the actual cached product nt_sbfm.obj and actual unavailable-service provider.
Only native MIDI function pointers are supplied by the fixture. Four cases
cover no device, failed capability query, failed open, and successful handle
open while the service is unavailable. Each repeats open/write/close twice,
checks timer-reset and timer-expiry status, drained port batches, inactive
service state, handle reset/close and idempotent second close. This is a
provider/lifecycle test, not evidence of audible output or hardware FM support.

Build first with tools/build.cmd; then tools/build-fixtures.cmd x86
system-fixtures and run build/.ninja/fixtures-x86/fm_service_fixture.exe.
The fixture link depends explicitly on the product object so later product
changes force relinking. No product algorithm is copied into the test.

Runtime14 cached build, dual publication and targeted verification are in
progress. Nothing is counted delivered before the final package passes.
Current delivered remains 15/20; cumulative reduction 859 points/7637 lines.

## Final package result

Runtime14 cached full build and dual thirteen-image publication passed. The
real-product FM fixture and existing VDM ABI fixture passed. WINMINE produced
its actual window. Win3.1 enhanced reached desktop, completed both fullscreen
entries with the same KVM window and working returned input, then exited
normally. DOS4GW/NESticle produced its real menu and passed Escape hide/restore
pixel checks. Delivery-result and targeted-result are PASS; Full was not run.

S7 is final-verified at 16/20 primary files, four remaining; next is sim32.c.
Cumulative non-error reduction is 860 points/7636 lines including the P10
required header (primary files alone: 857/7591). The one-line P11 increase is
explicitly subtracted from the cumulative line reduction, not hidden by the
excluded-body cleanup. All four old guard IDs reconcile to the three pending
residuals S7-P11-F01 through F03. Commit/push completes P11 only.
