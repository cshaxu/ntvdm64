# CPU40 control and exception guest evidence

This S10 checkpoint adds an independently authored COM workload, retaining all
original guest images and production files. The [guest source](../../tests/observation/ccpu_control_probe.asm)
runs through the existing [runner](../../tests/observation/verify-ccpu-sign-flags-guest.ps1)
with `-Workload Control`. Default SignFlags execution remains available and
was rerun, with its deliberately wrong oracle, after the runner change.

## Actual runs

The [observation record](ccpu-control/observations.json) and
[hashed raw records](ccpu-control/records.json) retain medium-integrity modern
pwsh parents, private desktops, original observer/logon helpers, exact inputs,
PIF producer, package images, process commands, completion and console output.
Cases ran serially against physical helper-owned stages, without Computer Use,
UI automation, original guest rebuilding or mapped drives.

| Case | Observed result |
| --- | --- |
| Accepted package, initial probe output | Non-pass: guest exited zero, but a long result line was clipped in the 53-column captured viewport. Not counted as final acceptance. |
| Accepted package, final probe | Guest and launcher exit zero; all three short result lines present. |
| Deliberately wrong control expectation | Guest exit one at phase A; no success marker; runner accepts this expected negative. |
| Held source-built candidate, same final probe | Guest and launcher exit zero; same result lines. This does not qualify SOL Options or the whole candidate package. |
| Existing default SignFlags | 524,288 sign cases, eight CDQ boundaries, 114,688 flag cases and reverse-copy marker pass. |
| Existing wrong SignFlags expectation | Guest exit one at phase A; no success marker. |

The final probe includes NOP and CLI under 32 flag patterns; CLTS and WAIT
under all four MP/TS combinations; 16 IDTR form/base/limit round trips; six
zero-divisor/overflow cases over byte, word and dword DIV; four CX/ECX branch
outcomes; real-mode STR rejection; and five selected original reserved routes.
These are logical cases, not a claimed exhaustive state-space or assertion count.

The original guest exception path enters locally installed INT 0/6/7 handlers
and returns with IRET. The probe checks fault CS:IP and register preservation
for DIV, expects exactly eight exceptions overall, and resumes at its own
known continuation. Both normal and assertion-failure exits restore CR0,
IDTR when changed, all three vectors and captured flags before returning to DOS.
IDTR variation occurs only in a short IF-clear interval without concurrent NMI
injection; it is not general asynchronous interrupt acceptance.

## Actual selected image correspondence

The [binary checker](../../tests/observation/verify_ccpu_control_image.py) uses the
existing COFF/PE parser. Its [result](ccpu-control/image/result.json) binds the
whole selected decoder and 14 original helper definitions to the actual
candidate executable. Function references are resolved from original COFF
relocations to their map-owned destinations. Leaf matching ignores only
declared relocations; it does not claim all global-data relocations were
independently validated. Identical empty functions can share a linker-folded
address without constituting separate behavioral evidence.

The first checker incorrectly required the map-producing cache executable
and sealed package to have identical whole-file hashes. Package identity
sealing may change nonexecutable resources. The corrected checker requires
identical complete executable sections, VAs and image base, then checks decoder
and helper bindings. The distinct whole-file hashes remain recorded. An
unrelated x86 test EXE is specifically rejected for a different executable layout.

STR's helper is bound in the image, but this real-mode probe proves its decoder
rejection only. It never enters the protected-mode STR helper. Do not turn that
negative into a positive-mode claim.

## Per-file result and remaining scope

The [ten dispositions](ccpu-control-role-dispositions.csv) record nine partial
source owners: CLI, CLTS, WAIT, RSRVD, DIV, LIDT, SIDT, JCXZ and STR. Their
privilege, operand, memory-fault, prefix, x87 and wider lifecycle obligations
remain explicit. The [earlier 22-file review](ccpu-remaining-c-source-review.md)
is the source-contract snapshot; this checkpoint resolves ten of those roles,
leaving twelve pending rather than rewriting its original baseline.

Only the standalone `nop.c` **file role** is excluded. Its original empty
helper is compiled but has no selected public-map entry or decoder reference.
The original `c_main.c` directly implements opcode 90 with `break`, and the
new guest witness executes that route. No NOP capability is removed, no new
success stub is introduced, and the decoder file remains partial under its
existing mirror differences and broader obligations.

All ten source files remain byte-identical to their selected originals.
Text retention is 100 percent. No behavioral code percentage is inferred from
sampled cases or emitted-byte matching; partial estimates retain their explicit
unmeasurable basis. The unused helper has no separate selected behavior
denominator. The exact-delta checker reports 1,678 pending, 408 partial,
52 full and 2,546 excluded physical file roles. No new full file is claimed.

The current delivery is test/audit work only. No production component, guest,
release archive or installed image changes, and the unrelated held SOL Options
candidate failure remains unresolved. S11 retains the CPU behavioral gaps;
S10 still owes the rest of the whole-MVDM reconciliation.
