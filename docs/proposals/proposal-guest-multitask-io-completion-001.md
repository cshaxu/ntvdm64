# Proposal — guest multitask I/O completion and acceptance

## Admission disposition

Owner admits this complete package into T481. The [active working plan](../etc/operations/m0-t481-multitask-completion-plan.md) owns execution; the proposal below is retained as the original obligation inventory, not an additional queued task.

Owner's S2 closure disposition supersedes the historical failure prerequisites
below: fill002/119 are retained as non-passing history, not active debt. Investigate
again only with a current reproducer; see the working plan's S2 closure record.

## Original admission and ownership

Owner-directed next T candidate, placed first in Queue after the bounded T479
S6 closure. No numeric T is allocated before admission. This package owns all
uncompleted T479 guest-I/O, nested and Win95 implementation/acceptance work;
T479 S7 only audits its already delivered scope. Transfer is not a passing test
or abandonment of the original functional goal.

## Baselines and preserved work

- Published host package: daa392bf-02e0-439f-9b1c-e21804398133, complete11 images;
  bounded Win3.1 addon publication, no accepted Win95 .vxd release.
- Source freeze: cedebcab55ec1da1b15557454985da78dca7ccf8.
- [Handoff closure](../etc/evidence/m0-t479-s6-closure-handoff.md) distinguishes
  published, candidate, failed and unverified results.
- [Replayable candidate](../../artifacts/t479-s6-handoff/candidate.patch) and
  [exact manifest](../../artifacts/t479-s6-handoff/manifest.json) preserve all10
  candidate source/test files. They are not selected build inputs. Verify the
  manifest and review against the new base before applying; do not use stale
  cached candidate binaries as a release package.
- [Full audit](../etc/operations/m0-t479-s6-io-audit.md) and its operation inventory
  retain source paths, experiments and run identities. Full171 passed16/16 on
  an earlier candidate, not the final188/190 handback implementation.

## Finite required work and exit evidence

| Group | Remaining obligation | Completion evidence |
| --- | --- | --- |
| Input return | Review/apply the candidate; prove ordered input exceeding15 BIOS words, repeat counts, standard/extended function keys, two-VM isolation/reuse and encoding boundaries. Retain unsupported records, no false success. | Real guest before/after receipts, exact read/peek/repeat results and lifecycle checks;189 unit2959/width and190 Z return/exit194 are partial evidence only. |
| Input delivery | Complete actual keyboard/modifier/special-key, mouse/focus, input modes, later INT15/INT16 hook interactions and unmapped Alt/numpad character cases. | Actual producers and real guest/native child records, no duplication, correct focus/generation; distinguish synthetic from physical input. |
| Text operations | Reconcile47 protocol definitions against reached production callers; implement required write/fill/scroll semantics, not unreachable experiments. | Caller-to-provider matrix and real text behavior; unused SCROLL108 stays withdrawn unless a real required caller is proved. |
| Geometry/fonts/code pages | Resolve grid/mode/viewport changes, font transfers/queries and code-page ownership; validate hidden split-cursor181 and visible split-cursor limits. | Real state changes, rollback and matching guest/host snapshots, including negative cases. |
| Graphics/display/metadata | Complete required graphics transport, mode transitions, VIDEO_TEXT, title/layout/pointer/clip/keyboard-layout semantics. | Actual VM rendering/application acknowledgements, not protocol dispatch coverage or outer-window guesses. |
| Lifecycle/failure | Resolve or source-dispose fill002 and119 residual shutdown stall; complete final-frame acknowledgement, four cancellation owners, blocked-I/O retirement, reuse and failure rollback. | Retained failure/control attribution and real normal/negative lifecycle proofs. A later passing run alone does not close an unexplained failure. |
| Nested chains | Full independent text chains and nested payloads without ancestor frontend takeover or global command/environment state. | Two real VMs, both completion orders, input/output/redirection, cancellation and resource retirement. |
| Win95 | After Win3.1 works, complete deferred S4 wait/wake, identity/lifetime/init and input/presentation acceptance. Desktop mouse064 and shutdown1 control060 do not establish it. | Independent Win95 tests and explicit disposition of differences; preserve immutable guest. |
| Integrated delivery | Complete supported operation/behavior matrix, DOS/WOW/Win3.x regression, exceptional ICW1/VMM allocation paths as applicable, coherent product/addon publication. | Cached complete builds, applicable Full gates, all11 host hashes and addon manifest, review, commit/push and owner acceptance. |

## Approved boundaries and exclusions

Unread input may return to COMMAND through BIOS/INT16 semantics, preserving
characters, function keys and ordering without replaying IRQ1 or TSR raw
callbacks. Native child input retains INPUT_RECORD. No extra retry/recovery,
polling, arbitrary timeout or second keyboard controller is authorized.
NTSRV remains lifecycle authority; use existing RPC control and I/O channels.
Keep original guest bytes unchanged. Independent addons remain in their existing
owners and publish to assets/release, not system32. No hardware V86 monitor or
unrelated CPU work. Windowed-DOS garbled text remains a separate queued debug
package and is not silently absorbed. Existing source provenance and minimum
mirror-diff rules remain binding.

## Suggested execution order

First audit/replay and qualify the preserved input candidate; next complete the
remaining reached Win3.1 I/O and lifecycle/nested groups; then Win95 and combined
acceptance. Allocate bounded S packets at this T's admission. Do not reopen T479
S6 or claim its previously unverified rows passed merely to simplify planning.

## Remaining S sequence

S13 and S14 are closed. S15 is active. S16 follows the Win95 matrix.

| Packet | Scope and evidence required for closure |
| --- | --- |
| S14 | Win3.1 Standard and 386 Enhanced, each with the released INSIGNIA.386 and NTVDM64.386 pair configured or absent, each with DOSONLY=0 and 1. Record all eight cases independently. Reproduce the no-driver DOSONLY=1 prompt error and mouse loss on an isolated copy of the installed configuration, identify the failing transition and verify its repair with the same actions. |
| S15 | Win95, same driver pair configured or absent crossed with DOSONLY=0 and 1. Verify the generated profile, guest DOS prompt, mouse and keyboard interaction, return to Windows and normal shutdown in each case. Verify guest-only execution for DOSONLY=1 even with the drivers present. Exercise the supported two-VM host payload route with DOSONLY=0 and both drivers loaded. Record actual routing without the pair separately. |
| S16 | Reconcile outstanding mirror audit dispositions against the existing inventory, close remaining integrated acceptance and delivery obligations, and record an owner and evidence for any residual item. Carry forward earlier package and shutdown evidence with its limitations. |

For S14 the driver axis records the installed files and active configuration.
Standard mode does not load enhanced-mode VxDs. Launch a real fullscreen DOS
prompt, execute a command, exit to Windows, use the mouse to open an ordinary
Windows application and verify normal Windows exit. In enhanced mode open two
independent DOS windows, distinguish their command output, switch focus and
verify mouse and keyboard operation. Switch one DOS VM to fullscreen, exercise
both NTCON Console and Window text presentations, return to the guest window
and verify both DOS sessions still accept input before normal exit.

For guest DOS work, DOSONLY=1 keeps execution in the guest with either driver
configuration. Enhanced Windows still schedules its own DOS VMs. Test their
input, output and retirement without requiring a host payload. For an actual
host shell-out with DOSONLY=0 and no enhanced-mode bridge, the legacy wait may
pause guest Windows. Require the child to finish, its result to reach the
caller, and Windows input and execution to resume. Concurrent guest progress
during a host payload is required only for enhanced DOSONLY=0 with the pair
loaded. The NIDDB companion also serves per-VM mouse/device state independently
of host payload selection.

Capture configuration and behavior per row. Keep the two original setup
selections as package controls; vary the other driver combinations only in
isolated copies. Startup, synthetic input delivery alone, or forced teardown
cannot establish these interaction and normal-exit results.
