# T413 S5 — Exception-hook conditional scope

## Question and source

Repair the five source-proven exception-hook scope defects without replacing
the CCPU exception engine. Baseline eb0c4fa33, admission 03847f34b. Selected
OpenNT and OpenNT-4.5 Int0 have the same original missing braces. NTVDMx64
commit 84a13d2e7bb1a55d11148971e5b9c8ec99f670bf supplies the five corrections
in common/ccpu.patch, SHA-256
D3EA35D3F7C3BD4617F5F6EDC90725D3A70F503E353677C251720D9E5C9929FC.
PATCH-007 retains only those hunks; DIV-268 marks all five application sites.

## Recovery and boundaries

1. Original c_xcptn.c is already compiled, but its NTVDM branch unconditionally
   continues before fault delivery when the hook is unhandled or real mode.
2. A same-shaped host adapter cannot change that lexical control flow. Changing
   c_cpu_continue would damage its valid nonreturning restart contract.
3. Apply the existing upstream scope correction in place. No executable
   statement or original function body is added or replaced; flags, saved EIP,
   vector/error formation and do_intrupt stack ownership remain original.
4. No new runtime behavior provider or autonomous exception engine is authored.

The complete profile is all five affected host_exint_hook blocks plus the
already-correct benign_exception control. Scalar/RMW/string execution is
unchanged; the changed shared exception exit is checked independently of opcode.
Fetch restarts preserve CCPU_save_EIP; stack construction remains in do_intrupt.

## Procedure and observations

Fresh build roots: build/M0-T413/S5/formal and integration. MSVC x86 /MT,
CCPU40 only. Generator runtime is the same installed node executable used by
the S4 formal graph (currently v24.19.0); the graph's NodeExecutable parameter
retains its historical Node22 naming, not a new CPU/build profile.

BuildCcpuExceptionProfile.mjs verifies the full upstream patch hash and that
removing exactly the five corrections yields original exception logic (apart
from the existing header/prototype build differences). It compiles the actual
complete exception source, substituting only test CPU state and observable
external boundaries; it does not test a rewritten exception algorithm.

27 cases pass: each of five repaired branches and benign_exception under
real/protected mode and handled/unhandled hook settings, then contributory
and page-fault escalation and triple-fault reset. Checks cover hook invocation,
delivery/continuation, saved EIP, flags, vector/error, operand/address/pop state,
error-code-push and privilege-check arguments. Original-code negative control
fails 101 assertions. The fixture observes do_intrupt arguments; it does NOT
prove actual protected-mode IDT/TSS stack delivery or a working WOW application.
Real guest stack/handler execution is a separate required integration gate.

The exact five excerpted upstream hunks are checked verbatim (LF-normalized).
Removing those braces and their five local provenance comments reproduces the
selected original exception source apart from pre-existing build declarations.

Fresh formal graph completes all 483 build steps, including the three EXEs and
the C-VID owner-library fixture. The initial PATH-selected WinLibs Ninja made
no progress; only this run's verified Ninja/cmd processes were stopped. The
installed Ninja-build.Ninja runner completes the same graph. No product or
test contract was changed to work around that tool launch issue.

Observed integration results:

- Original C-VID fixture: all 38 pairs at four values, 81-slot layout,
  latch/selector routes, repeated setup, old-provider negative control and
  concurrent scratch preservation pass.
- 168 original IP-expression cases pass; old-expression negative control fails.
- All 12 real DOS instruction fixtures pass, including FAULT. It installs a
  guest INT 0 handler, divides by zero at IP fffch, checks the fault IP actually
  pushed on its guest stack, restores the vector and returns exit code zero.
  This supersedes the S4 old/new timeout. Runtime log:
  O:\winnt\logs\m0-t413-s5-ip-FAULT.txt and the matching summary JSON.
- All 17 command-matrix cases pass, including nested COMMAND/MEM, repeated MEM,
  EDIT return, native streams/EOF and original guest/native exit-code behavior.
  O:\winnt\logs\m0-t413-s5-command-summary.json.
- Native resize crop/grow/cursor and invalid-input checks pass:
  O:\winnt\logs\m0-t413-s5-resize-native.txt.
- All six Terminal/ConPTY profiles pass: 80x25, 45x34, 60x50, 120x30,
  mouse and live resize. Output mode remains 7, EXIT echoes once and MEM
  reports retain exactly one following blank row. Captures use
  O:\winnt\logs\m0-t413-s5-video-*.raw.
- Five prefilled 80x5 short-window runs pass with buffer 80x300, viewport
  0,36..79,40 and cursor 0,40. All 20 post-EDIT MEM snapshots complete.
  Logs use O:\winnt\logs\m0-t413-s5-video-short-1 through -5.

No all-protected-mode fault delivery or WRITE-completion claim follows from
these results. The protected-mode cases prove the actual repaired control flow
and original delivery arguments, not a substitute kernel or full WOW lifecycle.

## Footprint and deployment

Product code changes only c_xcptn.c: +15/-5 lines, net +10, comprising five
opening braces on existing conditions, five closing braces and five provenance
comments. No new executable statement, function, adapter or overlay. Patch
excerpt and tests are separately accounted evidence, not runtime providers.
Mirror diff distance is 437 (CCPU +314/-62, C-VID +40/-21), versus 417 before
this correction and 506 before T413: whole-task reduction remains 69. The
four-overlay cohort remains 125 lines, unchanged.

The following formal products match O:\winnt byte-for-byte; prior EXEs are
recoverable in build/M0-T413/S5/integration/prior-products. Version remains
0.0.413, protocol 3.

| Product | SHA-256 |
| --- | --- |
| run16.exe | AC92CFDFF2FFF5405A1FAD3D870566CE75C8BAB01D346EA9E538CE9E25EAD060 |
| basesrv.exe | C81AAD843785E92B95DCB0827A38EADA96D2EB31474B2182119ECF66A5C64578 |
| ntvdm.exe | 790866A03B898420B313BB6B981012012C59285E517E7F51A45E6A4041A9367F |

All admitted functional gates pass. Documentation/link governance and
git diff --check are delivery gates; P2 includes the reviewed commit/push.
Owner visual acceptance remains separate and is not asserted as received.

## Follow-up

Other NTVDMx64 changes are isolated in the
[queued proposal](../../proposals/proposal-ntvdmx64-source-corrections-001.md).
In particular existing DIV-221 already handles negative PIC acknowledgement
upstream: absence of spurint.patch at c_intr.c is not proof of a missing fix.
