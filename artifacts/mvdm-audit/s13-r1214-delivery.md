# S13 per-file checkpoint r1214

Ten source files changed: `illegalp.c`, `nt_cpu.c`, `nt_event.c`, `probe.c`,
`apisvc.c`, `clres.c`, `rtlinit.c`, `wow.c`, `chartran.c`, and `drawtext.c`.
Original conditions and selected local bindings were audited per file; the
[per-file inventory](s13-r489-per-file-observed-inventory.json) links each
record. Guest sources and media were not changed.

An earlier candidate incorrectly paired repeated `X86GFX` guards in
`nt_fulsc.c`. That batch-local edit was withdrawn and the file remains pending;
the [reclassification](s13-r1212-ntfulsc-reclassification.json) explains why.
The r1211 comparison records that rejected candidate and is not the delivered
package. The final package is r1214.

Cached x86, x86-extra, and x64 builds [passed](s13-r1214-full-build-results.json).
The [ten-image comparison](s13-r1214-build-image-compare.json) found 27
non-timestamp bytes in NTVDM; the [byte analysis](s13-r1214-ntvdm-difference.json)
identifies all of them as 18 diagnostic line immediates in `nt_event.c`. The
other nine images are exact or differ only in PE/debug timestamps. The new
[Full verification](s13-r1214-full-evidence.json) passed, including Console
17/17, Window 17/17 and all selected Full gates; its
[process](s13-r1214-full-process.json) exited 0. DOS4GW verifies an active
visible window, not gameplay or pixels.

Both [structure and rejection test suites](s13-r1214-guard-test-evidence.json)
passed; the latter covered 181 guards in each of x86 and x64. Their old
assertions were updated to understand retained `#line` directives and the
already-adopted exclusion of hardware i386 monitor paths. The 73 stale ledger
positions were [reconciled](s13-r1224-ledger-reconciliation.json) against
historical source snapshots without withdrawing profile checks. Two previously
completed files were reopened after diagnosing incorrect diagnostic continuations:
[timer.c](s13-r1219-timer-line-reopen.json) and
[base_def.h](s13-r1220-base-def-line-reopen.json).

The complete ten-image candidate was published and hash-verified at
`assets/release` and `O:/winnt/system32`. The [census](s13-r1214-counts.json)
records 1,058 present C/H files: 181 reviewed identical, 837 reviewed with
differences and 40 pending. Rename-aware net 578 deleted C/H paths against
`c97d4ee2c` are separate. Full enumeration of supported contexts, including
non-Ninja addon consumers and guest-shared headers, remains open.
