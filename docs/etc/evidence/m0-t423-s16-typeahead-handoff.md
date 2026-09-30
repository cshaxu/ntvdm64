# M0 T423 S16 — zero-delay input handoff

## Question and inputs

Why does the supplementary `dos-native-typeahead` case intermittently turn
the last `exit` into `eexit`? Inputs are the accepted S14 baseline, published
S15 eight-file package, `Verify-CommandExitStatus.ps1`, its unchanged observer
binary, original COMMAND/CMD/MEM media, and private-desktop Console/Window.

## First investigation P

The observer writes one paired key make/break per character, 100 ms apart,
with no *extra line* delay in this case. Window injection uses the public
Window key route; Console injection uses `WriteConsoleInputA`. Both route
through NTKVM before the selected worker consumes copied input.

- Published Window run `t423-s15-published-typeahead-r1` completed with two
  MEM outputs and exit 1. An identical `r2` timed out with real captured
  `WINNT>eexit`; this is not a marker-only test error.
- Console comparison `t423-s16-console-typeahead-r1` actually completed with
  exit 1 and two MEM outputs. Its line-02 snapshot contained the genuine
  native CMD version line, but the final 25-row DOS screen no longer did.
  The verifier checked only the final Console snapshot, falsely reporting
  that the native target never started.
- The existing contiguous-overlap merger reconstructs this run with one CMD
  version line and exactly two MEM outputs. Its test now includes the
  buffer-shrink case, and the verifier invokes it for this Console case as
  well as the established Window cases. Re-run
  `t423-s16-console-typeahead-r2` passed without relaxing the execution,
  output-count, bad-command, or exit-code assertions.

Thus one failure was definitely an observer/verifier false negative; the
Window `eexit` timeout remains a separate real input-order fault.

## Published-package counterchecks

The same unchanged ordinary eight-file package reproduced the defect after
P1, with actual final DOS text rather than only a timeout:

| Route / log prefix | Result | Final command |
| --- | --- | --- |
| Window, `t423-s16-window-repro-1` | timeout | `xexit` |
| Console, `t423-s16-console-repeat-1` | timeout | `eexit` |
| Window, short build-root alias `t423-s16-window-shortroot-control` | timeout | `xexit` |
| Pure DOS nested `t423-s16-dos-only-typeahead` | pass | intact `exit` |

Both Console and Window failures occurred after two real MEM outputs, so a
Window-only key mapping defect and a final-screen marker false negative are
insufficient explanations. The short-root control used the byte-identical
published NTKVM and still failed; the package path is not the cause. The
observer calls its scripted input writer once and submits one make/break pair
per character. The current evidence therefore establishes a real shared
handoff/consumption instability, but not yet its exact producer.

An isolated x86 `frontend-video-observer.exe` was built under the S15 formal
cache and run from a copied package in `build/M0-T423/S16` via a temporary
short drive alias. Its three Window runs passed. A reduced handoff-only trace
variant also passed twice; the experimental test-only change was removed
because logging changed timing and did not capture a failing run. Neither
observed pass is accepted as a product repair, and the production package
was never replaced. Static review narrows the next witness to NTKVM's copied
input queue, NTCON's hidden-Console `return_unused_input`, and the original
DOS history/reentry consumer. No code owner among those three has yet been
proved to duplicate or lose a particular key.

Next: obtain low-perturbation key-by-key ownership evidence across both
handoff directions, then patch only the proven boundary and repeat real
zero-delay routes plus the full production regression gate. Do not add sleeps
or weaken assertions.

## Device/IRQ owner localization and candidate repair

The published S15 executable was reproduced on both display routes, then a
test-only worker with a bounded memory-mapped keyboard observer sampled the
unchanged return/reentry sequence. The failing return had a full original
8042 output slot, `bKbdEoiPending=1`, PIC master ISR clear and keyboard IRR
set. `ReturnUnusedKeyEvents` returned the native key; the original BIOS
buffer was empty immediately after `ReturnBiosBufferKeys`, but held the same
character before the input thread resumed. It therefore was not merely
duplicated by the test writer or misread by the final-screen verifier. The
source-owned pending IRQ replayed it during the handoff gap. Evidence:
`O:/winnt/Logs2/t423-s16-irq-observed-r1.kbd` and the subsequent bounded
BIOS/insert observations under the same log root. The observer does not alter
guest media or issue input records.

An early candidate initially appeared to fail in the ordinary worker, while
the test-only worker passed 18 repeated routes. That apparent contradiction
was a build artifact: `keyba.obj` had been rebuilt but the linker consumed
the stale `original-softpc-keymouse.lib` from the S15 cache. That executable
did **not** contain the proposed repair; it is not a repair failure. The
library was rebuilt explicitly before relinking `ntvdm.exe` and comparing
hashes. That correctly linked, adapter-extracted candidate passed six
consecutive Console and six consecutive Window zero-delay
`dos-native-typeahead` runs, without sleeps, relaxed output checks or guest
changes (`t423-s16-adapter-{console,window}-r1` through `r6`). It also passed
the standard 17/17 Console and Window matrices and preserved the three WOW
frontiers; these are candidate observations, not a release.

The broader first adapter additionally cleared `bKbdEoiPending`,
`bDelayIntPending`, `KbdData` and `output_contents` on *every* reset. A separate
native→DOS→native control exposed actual missing keys after DOS returned:
one run produced `xit`/`em` and timed out, another completed but produced
`windove-return` instead of `window-native-return`. The untouched S15 package
passed the same reverse control three times. We therefore rejected that
broader candidate, regardless of its 12 primary-route passes. The later
candidate leaves those original keyboard variables alone and retires only a
delayed/PIC IRQ when the original 8042 output slot was full *and* its DIV-317
origin identifies a user key, not a device-internal ACK. The original-device
fixture verifies both no-output and internal-response exclusions. The final
candidate then passed six consecutive Console and six consecutive Window
zero-delay primary routes (`t423-s16-finalorigin-{console,window}-r1` through
`r6`), 17/17 in both full candidate matrices, and the retained three WOW
frontiers. No fixed delay or output-assertion relaxation was used.

The current candidate leaves one original-owner hook in
`softpc.new/base/keymouse/keyba.c::Reset6805and8042` (DIV-321). The bounded
`ntvdm-exe/softpc/mvdm_keyboard_reset.c` adapter retires the standalone
delayed IRQ/PIC request for a user-originated filled 8042 slot before the next
guest command;
original keyboard flags, shadow bytes, 6805 queue, scan translation, BIOS
buffer, task dispatch and frontend input remain where they were. The focused
original-device fixture passes against the hook and verifies the 8042-full
versus empty reset choice.

Reverse-case comparison also found a limitation of the supplementary scripted
observer: the Console final screen can lose the first MEM result, while two
separate snapshots prove both. A proposed generalization of the contiguous
overlap merger was rejected when the original Console switches buffers and
produces no contiguous overlap; the established strict merger stays limited
to its previously verified routes. The final candidate's Console reverse
control passed twice; a third completed with exit 1 but was rejected by that
supplemental verifier, and is **not** counted as a passing control. In Window,
the untouched S15 package and candidate both timed out in this extra reverse
script; this is a pre-existing, separately unresolved test/product boundary,
not acceptance and not evidence that S16 repaired the reverse Window path.
A pre-existing global NTSRV with idle NTCONs also contaminated an early
short-root run. The verifier now rejects a foreign broker before an isolated
test begins. After verified idle-process cleanup, both full candidate
matrices passed.

The coherent eight-file candidate was published with old/new SHA-256 and
rollback copies in `build/M0-T423/S16/publication-backup-r1/manifest.json`.
Only `ntvdm.exe` changed bytes; guest media and configuration matched the
pre-publication hashes. The published package passed 17/17 Console and 17/17
Window routes (`t423-s16-published-{console,window}-r1-summary.json`), and
the exact zero-delay handoff passed two further runs per route
(`t423-s16-published-typeahead-{console,window}-r1/r2`). The published
WINMINE main-window and original SOL/WRITE out-of-memory frontiers were also
preserved (`t423-s16-published-wow-r1-*`); those frontiers do not constitute
full SOL or WRITE acceptance.

The extra reverse nested script remains unresolved. Its untouched S15
Window baseline timed out after displaying both MEM outputs and the native
return marker, with the final DOS prompt still live. The candidate has the
same result. This is neither a regression introduced by DIV-321 nor a passing
test. It requires a separate key ownership/observer-timing determination
before the broad bidirectional S16 exit claim can be made.
