# Diagnostic guest-access recovery

Original BOP 59, INT 15 and keyboard-BOP handlers do not read the guest stack
for reporting. The existing project additions evaluated those reads even when
the diagnostic output was disabled. The candidate removes those stack samples,
retaining scalar reports and original handler calls. The BOP 59 report's second
value is now CS:IP; the caller-stack report records are no longer produced.
The illegal-op report keeps its existing trace path but receives the already
decoded CS:IP and register values rather than reading four further bytes;
its former four-word trace field is removed. The DPMI target report likewise
retains its handler metadata but no longer maps or samples guest code.
These are deliberate changes to opt-in diagnostic output, not guest workarounds.

The cached x86 worker compiled after the final source changes. The source-body tests
compared the pinned original and candidate: BOP 59 had zero guest reads and
one handler call with the same AX; INT 15 had zero reads and the same modeled
register result; illegal-op had four original frame reads, two writes, one
panel and one dispatch in both bodies. A selected full keyboard translation
unit with controlled failure-on-read boundary dispatched successfully both
with and without injection, with zero reads and translations. The observer
last-error suite passed 53 checks including an enabled DPMI target record;
a second fresh process also verified the enabled illegal-op trace and the
absence of the removed stack-word field.
Those are controlled witnesses, not full real-device or guest acceptance.

An additional source-body DPMI witness exercises absent-handler rejection,
virtual-mode dispatch, and protected-mode stack-frame failure while counting
reporting and dispatch. It passes on the current source and does not claim
actual-object or guest execution. The current retention generator reports
204 differing mirror files, with 203 textual estimates; the five touched
mirror sources remain partial. The estimates are source-text comparisons,
not runtime coverage. Outputs and input identities are adjacent to this note.

The actual selected DPMI object disassembly contains the metadata-report and
original stack-frame calls but no direct Sim32 mapping call inside
`DpmiSwIntHandler`; the production map identifies its owning object. This is
not an injected object-level failure test and does not exclude indirect
mapping through normal original dispatch. The separate instruction-window
paths remain outside this check. The separate SOL 1400 failure is not
attributed to this change. No guest was modified.

The first Full attempt stopped before any gate on a sealed-image/control-cache
hash mismatch. A second attempt passed guest media, export ordinals, WOW
frontiers, Console-17 and Window-17, then stopped because the newly prepared
control cache lacked four test clients. These are test-preparation failures,
not passing Full results. The third attempt with those clients passed all 16
Full gates against the sealed candidate identified by the adjacent runtime
manifest. A separate visible-desktop Winmine probe passed flag/counter, reset,
cell opening, close and launcher completion. These tests are not a proof of
every DPMI failure or of the unlocalized SOL 1400. The separately recorded
publication verified all 20 component hashes at the two destinations.
