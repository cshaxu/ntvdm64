# T420 S21 guest Console witness

## Question

Can the CCPU capability runner distinguish an actual guest success from a
launcher exit that merely appears successful?

## Inputs and procedure

The admitted S21 generator creates DOS fixtures only under
`build/M0-T420/S21/`.  Each successful fixture writes its own
`CCPU-<case>-OK` marker through DOS `INT 21h/AH=09` before `INT 21h/4C00`.
`VerifyCcpuIpProfile.ps1` reads both the observer record and its captured
`.console.txt`; it requires exited/zero **and** every fixture's declared text.
The COMMAND regression verifier likewise requires the real `MEM` and `VER`
screen strings, rather than trusting the outer process code.

Formal x86 product build:

```text
ninja -C build/M0-T420/S21/formal-x86-001 -j4 product-programs
```

Observed package logs below `O:\winnt\logs`:

```text
t420-s21-final-fpu-text-r1: FPU64, FPUI32, FPUCW, FPUSW, TIMBUSY, TRAP, FAULT
t420-s21-final-fpu-text-r1: HLTWAIT expected CPL3 privilege boundary
t420-s21-command-text-r1: interactive MEM, nested MEM, direct MEM, COMMAND /c VER
```

## Observations

All five positive CCPU fixtures exited zero and emitted their guest markers.
`TIMBUSY` additionally installs a guest INT 1Ch handler and waits for three
real clock callbacks; its marker proves timer/PIC delivery from guest code.
The four COMMAND/MEM routes emitted their required real `MEM`/`VER` text.

`HLTWAIT` prints `CCPU-HLTWAIT-BEFORE` and then times out.  A temporary,
uncommitted diagnostic observed original CCPU `GET_CPL()==3` at `HLT`; the
original `c_main.c` therefore raises the HLT access GP before its wait loop.
The fixture is retained as a negative control: it must not emit `AFTER` or a
success marker.  Direct HLT wakeup requires the retired kernel V86-monitor
virtualization contract and is not a selected standalone CPU40 feature.

## Worker-thread lifecycle witness

The guest Console gate applies to DOS execution.  CCPU's per-thread TLS
allocation and release are host-side mechanics, so they have a separate,
explicit host witness rather than pretending to be guest text.  The formal
generator now includes `ccpu-thread-lifecycle-test.exe`.  Its test-only entry
uses the selected original `host_CreateThread`, enters original CCPU through a
two-byte direct-unsimulate BOP, returns through that thread's original CCPU
TLS simulation frame, and exits with original `host_ExitThread`.

The current formal library set was compiled x86 and the compatible direct
fixture link/run at `build/M0-T420/S21/ccpu-thread-lifecycle-direct-r2/`
printed `CCPU thread lifecycle OK` and exited zero.  This distinguishes a
real initialized-and-released CCPU thread frame from a thread that merely
starts and returns without ever entering the simulator.

## Current capability matrix

The following runs were repeated against the current S21 formal library set;
their scope is deliberately recorded rather than inflated.

| Family | Evidence | Result and scope |
| --- | --- | --- |
| Instruction/FPU | Guest `SKIP*`, `TAKEN*`, `CALL*`, `BACK*`, `PAGE`, `STRING`, `MUL32`, `DIV32`, `BSF32`, `RMW`, `FPU64`, `FPUI32`, `FPUCW`, `FPUSW` | Each positive DOS program emitted its `CCPU-*-OK` Console witness and exited zero. |
| Guest exception boundary | Guest `TRAP`, `FAULT` | Both emitted their guest witnesses and exited zero.  They prove the selected worker reaches these CCPU exception paths, not all protected-mode handler registration. |
| Exception algorithm | `exception-profile-r1/exception-profile.exe` | `PASS 27 actual-source exception cases; failures=0`.  This is a direct compilation of original `c_xcptn.c`, covering handled/unhandled GP, IDT-GP, DF, DIV, PF and invalid-opcode branches. |
| IRQ/reset/event | Guest `TIMBUSY`; `ccpu-halt-reset-test.exe`; event consumer scan | `TIMBUSY` waited for three guest INT 1Ch callbacks and emitted a witness.  The original HALT/reset fixture restarted at `AX=BEEF`; the consumer scan passed, while explicitly not claiming full CPU/PIC execution. |
| Descriptor state | `cpu40-descriptor-domain-fixture.exe` | Current x86 execution exit 0.  It is the S18 owner fixture for GDT/LDT/IDT domain publication; S21 does not duplicate DPMI descriptor policy. |
| FPU layout | `x87-layout-fixture.exe` | Current x86 execution exit 0; guest FPU markers above validate the execution side. |
| Worker TLS/lifecycle | `ccpu-thread-lifecycle-test.exe` | The current formal x86 run first ends a worker with nonzero `53` through original `host_ExitThread`, then starts a second CCPU worker which obtains a fresh TLS/jmp frame and exits `0`. It emitted `CCPU thread lifecycle normal-and-abnormal OK`; this is host-side lifecycle proof and is not counted as a guest witness. |

The remaining work is not to repeat those passing probes.  It is to complete
the source-caller matrix: prove which selected `c_intr` and `c_xcptn` DPMI
hooks are reachable through the current provider, and either obtain a true
guest proof or record the original owner/no-selected-caller disposition for
each.  The existing S18 descriptor provider proof and this table are inputs,
not a premature S21 closure.

## Protected-hook ownership disposition

Source review resolves the CCPU-side edge without guessing:

```text
c_intr.c / c_xcptn.c
  -> original host_swint_hook / host_exint_hook (nt_inthk.c)
     -> installed original function pointer, or FALSE
        -> DPMI EnableIntHooks()
           -> DpmiHwIntHandler / DpmiSwIntHandler / DpmiFaultHandler
```

`nt_inthk.c` owns only pointer installation and a Boolean dispatch result; it
does not construct DPMI frames.  `dpmiint.c::EnableIntHooks` conditionally
publishes all three DPMI providers, and `DpmiFaultHandler` owns the protected
stack/frame and callback return semantics.  Therefore S21's direct CCPU
exception evidence verifies the generic CPU behavior and hook call shape, but
the remaining real protected-handler workload is explicitly transferred to
the existing **S38 DPMI32 capability closure**.  It is neither an absent
caller nor a reason to add a CCPU-side substitute.  S21 must retain this
owner mapping when it closes; S38 must supply the guest workload evidence.

## Original `c_intr.c` recovery and compact Console evidence

The selected CCPU call graph has two original software-interrupt instruction
owners: `INTx()` and `INTO()`.  Each already calls `host_swint_hook()` before
calling `do_intrupt(..., TRUE, ...)`.  The CCPU opcode dispatcher reaches
those two bodies directly.  The only other selected `do_intrupt` callers are
exception and hardware-delivery paths, which pass `FALSE` and must not be
treated as a software interrupt.

Consequently, the former ten-line `c_intr.c` hook was duplicate policy, not a
required CPU40 carrier.  It has been removed; the current
`c_intr.c` SHA-256 is identical to the pinned OpenNT source:
`9e6d69b1e05309ca87db41c66b89eb9d0862f2078c08a16fe20b82669bc46a7a`.
The DPMI hook route remains owned by the original `INTx`/`INTO`, `nt_inthk`
and `dpmiint` chain above.

The fresh formal graph is `build/M0-T420/S21/formal-x86-004`; it linked
`run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe` and
`ccpu-thread-lifecycle-test.exe`.  The latter printed `CCPU thread lifecycle
OK` and exited zero.

The Console observer formerly wrote every cell in the host Console's full
scrollback plane.  A default `120 x 9001` Console turned the one-line
`TIMBUSY` witness into a 1,080,120-byte file of spaces.  This is an
observer-only evidence defect, not guest output.  The observer now records
only nonblank rows, retaining their original row numbers and buffer/viewport
metadata.  Its `TIMBUSY` snapshot is 60 bytes and contains
`CCPU-TIMBUSY-OK`.

Deployed formal artifacts were copied to `O:\winnt` by matching SHA-256.
`t420-s21-cintr-original-r7` then completed the entire 22-case guest matrix:
21 positive guest Console passes plus the one expected original CPL3 HLT
boundary.  All 22 Console evidence files were 56--68 bytes.  The established
product regressions also passed under the same observer:
`MEM`, nested `COMMAND -> COMMAND -> MEM`, direct `MEM`, `COMMAND /c VER`,
and `EDIT` return (`t420-s21-cintr-command-r3` and
`t420-s21-cintr-edit-r1`).

A follow-up selected-CCPU sweep also removed the one dead local
`requested_address` from `ccpusas4.c::c_GetPhyAdd`: it was initialized from
`addr` but had no read, and therefore could not affect the retained physical
mapping translation/resolve calls.  `formal-build-r5` rebuilt the complete
x86 product graph after its removal; the original CCPU lifecycle witness
again printed `CCPU thread lifecycle OK` with exit zero.

## Event producer and consumer disposition

`Verify-CcpuEventConsumption.mjs --out build/M0-T420/S21/event-profile-r2`
compiled the current extracted source and passed its HALT/RESET/timer/SAD/IRQ
consumer matrix, including the stale-PIC acknowledgement boundary.  The test
is intentionally host-side and does not replace the real guest `TIMBUSY`
proof above.

The selected producer search is closed: timer, reset and hardware delivery
reach `c_cpu_interrupt()` through the CCPU40 `cpu_interrupt` binding, so they
all use the atomic event raise path.  The only direct writer in `yoda.c` is
non-PROD; CPU30/monitor direct-map code is unselected.  `ica_async_hw_interrupt`
and `ica_sigio_event` remain original system bodies, but no selected source
calls the async entry.  `CPU_SIGIO_EVENT` is consequently a source-proven
no-selected-caller disposition for this profile, rather than an untested
claim that the CCPU currently consumes it.

The same current-source event fixture was rerun at
`build/M0-T420/S21/event-profile-r3/`. Its `ica_intack=-1` control observes
one acknowledgement and zero deliveries, proving that DIV-221 does not narrow
the signed stale-notification sentinel into an `IU16` vector. The normal
vector-9 and reasserted-event controls also pass. This is a focused
CPU/PIC-boundary proof, not a claim of full PIC hardware emulation.

## `c_xcptn` handled-hook scope disposition

The five `MVDM-HOST-DIV-268` brace pairs in `c_xcptn.c` are retained as one
semantic correction, not formatting or a standalone adapter policy. In the
pinned OpenNT body, a protected-mode `host_exint_hook(...)` condition governs
only the preceding fault-state clear. The following `c_cpu_continue()` is
unconditional. In the selected NTVDM build,
`c_main.c::c_cpu_continue()` calls `ccpu386GotoThrdExptnPt()`; it does not
return to the exception body. Consequently the original spelling abandons the
CPU exception path even when the hook returns `FALSE`.

The original `nt_inthk.c::host_exint_hook()` initializes its result to
`FALSE` and forwards the provider's Boolean result. The selected
`dpmiint.c::DpmiFaultHandler()` documents the same contract: `TRUE` means
dispatched and `FALSE` means not handled. The retained braces therefore make
the jump occur only for a handler that actually accepted the exception; the
`FALSE` path reaches the existing original `do_intrupt()` delivery. This is
the exact five-site correction in the archived NTVDMx64
`patches/common/ccpu.patch`; no behavior was newly authored here.

The current-source reconstruction at
`build/M0-T420/S21/exception-profile-r2/` independently confirms that
disposition. It checks the patch hash and exact five-block delta, compiles the
actual current translation unit against observable CPU/host seams, and passes
all 27 protected/real, handled/unhandled and escalation cases. Its paired
original-spelling control fails 101 assertions, including the expected
unhandled fault delivery, vector/error, mode-reset and stack-push checks.

## Complete selected CCPU386 residual-diff ledger

The final S21 source sweep compares the selected
`softpc.new/base/ccpu386` mirror with pinned OpenNT, once bytewise and once
with newline/whitespace normalization.  The bytewise report is deliberately
inflated for a few historical CRLF mirror files; the normalized ledger has
exactly **14 files, 227 added and 63 removed source lines**.  No other
selected CCPU386 file has a normalized difference.  Each retained row below
has an original owner and a finite reason; none is an autonomous replacement
algorithm.

| Mirror file | Normalized delta / owner | Disposition and evidence |
| --- | --- | --- |
| `c_bsic.h` | shared `c_getEFLAGS` declaration | Required by original `c_intr.c` and `pushf.c`; moving it to an adapter produces undeclared-call diagnostics. |
| `c_main.c` | CCPU event/IRQ and thread-frame boundary | Header declarations and `setjmp(*jmp_buf)` are current compiler ABI corrections. DIV-214 atomic event consumption is covered by the selected producer/consumer fixture; DIV-221 rejects the original PIC `-1` stale acknowledgement instead of fabricating INT FF. The original DPMI hardware-hook call shape is retained; real protected handler work belongs to S38. |
| `c_page.c` | PROD `check_D` selection | `yoda.h` supplies the original production no-op macro; removing it changes the historical macro into an unresolved external debug call. |
| `c_reg.c`, `popf.c` | standard varargs declarations | Their original diagnostic `printf` calls remain unchanged; the declarations prevent an implicit, incompatible native call contract. |
| `c_seg.c` | early selector `0040h` carrier | Bounded CPU40 DOSX `FSTI` bootstrap before original BIOS-data descriptor publication; T407/S18 trace proves the later original publication. It is not a general descriptor substitute. |
| `c_xcptn.c` | DIV-268 handled-hook braces | Current-source exception profile passes 27 cases; the original spelling fails 101. |
| `ccpusas4.c` | physical mapping translation/resolve | The original CCPU RAM path is unchanged after the checked EMS/DIB external-page binding. T406 guest EMS mapping/move/exchange evidence owns this necessary kernel-VDM replacement boundary. |
| `cpu4gen.h` | selected return carriers | Declarations now agree with original selected `c_reg.c` definitions (`ISM32`), preventing incompatible generated-call ABI declarations. |
| `fpu.c` | T60 representation/rounding fixes | `memcpy` prevents unsafe `double` aliasing; M64 integer store uses the same rounding route as M16/M32. The CCPU guest FPU rows exercise this selected body. |
| `localfm.c` | C-VID singleton ownership | Selected C-VID owns `Sas` and `Gdp`; this removes duplicate CCPU allocation while preserving the original shared-state interface. S2 C-VID evidence owns the composition proof. |
| `ntthread.c` | typed failed frame lookup | Both original bare pointer returns become typed null; current normal-and-abnormal real CCPU worker lifecycle witness proves the failure carrier does not poison a subsequent worker. |
| `sascdef.c` | generated SAS null slot type | Preserves the original unavailable vector slot and layout with the selected function-pointer type. |
| `zfrsrvd.c` | original declarations and SAS extent | Includes/prototypes expose reached original contracts; the retained physical extent is the same CCPU SAS boundary as `ccpusas4.c`, not a host pointer. |

The sweep also rechecked the recently restored byte-exact `c_intr.c` and the
dead `ccpusas4.c::requested_address` local removal.  Therefore the S21
closure review has no remaining candidate whose only rationale is
"modern compiler convenience" or "diagnostic tracing".  The ledger does not
claim S38's protected DPMI provider lifecycle is complete; that capability
remains explicitly with its already-admitted package owner.

## Interpretation and follow-up

The text gate prevents false green acceptance when a wrapper exits zero after
guest failure. Together with the complete capability matrix, real worker
lifecycle witness and residual-diff ledger above, it closes S21. The HLT
result is an explicit source-shaped CPL3 boundary, not a reason to alter
original CCPU privilege semantics or introduce a CPU30/V86 monitor. Real
protected DPMI callback-provider lifecycle remains S38's separate original
package obligation, rather than a missing CCPU or an S21-owned substitute.

## COMMAND acceptance requires Console text

`Verify-CommandExitStatus.ps1` now treats the observation record as a
two-part contract for every positive case: the outer `run16` result must match
the expected status **and** the observer must have captured guest Console
text.  It rejects an unexpected `Bad command or filename` / native
command-resolution diagnostic even if the wrapper exits successfully.

The deliberately negative `missing` case is the sole exception: it must show
the expected native command-resolution diagnostic and then the `ver` witness.
All other cases carry a specific Console witness.  In particular, the
test-only `G7.COM` prints `S10_GUEST_SEVEN` before its intentional DOS exit 7;
an exit code can no longer be mistaken for evidence that DOS opened and
executed that image.

The current formal-x86-004 deployment then passed the complete strengthened
matrix. The test creates `G7.COM` only in its admitted `build/M0-T420/S21`
fixture root and maps that root to an otherwise unused short drive for the
direct DOS launch. It does not copy test media into `O:\winnt`. Original DOS
`COMMAND` does not enumerate that host-only `subst` drive, so the nested
COMMAND row deliberately executes package-owned `MEM.EXE` instead; this tests
the supported product route rather than treating a temporary host mount as
guest media.

| Evidence prefix | Cases | Result |
| --- | --- | --- |
| `t420-s21-console-contract-r3` | interactive native zero/missing, native child, streams, EOF | passed |
| `t420-s21-console-contract-r4` | nested COMMAND, direct MEM, COMMAND `/c`, native exit 7, EDIT return | passed |
| `t420-s21-console-contract-r5` | interactive MEM, nested COMMAND → COMMAND → MEM, repeated MEM | passed |
| `t420-s21-console-contract-r6` | unmodified empty COMMAND → `exit` | passed with original DOS banner witness |
| `t420-s21-cintr-original-r8` | 22 CCPU guest instruction/FPU/fault/text witnesses | 21 passed; one expected CPL3 HLT boundary |
| `t420-s21-command-text-r6` | empty/native/streams/EOF/MEM/nested MEM/direct MEM/COMMAND `/c`/guest exit 7/EDIT | all 17 passed with required Console text |

The first attempted nested variant inserted `ver` between the original nested
`COMMAND` and its paired exits.  That altered the original keyboard timing and
left a test-owned session waiting; it was not treated as a product failure.
The accepted test restores the original input sequence and uses the two
original DOS startup banners as the nested-level witness instead.

The `t420-s21-command-text-r6` run also caught and rejected two invalid test
arrangements before the accepted result: a long build-root path produced
`Cannot execute ...G7.COM`, and attempting `COMMAND /c` through the temporary
host-only drive produced `Bad command or file name`. Neither was counted as
product success. The accepted matrix requires the direct `G7.COM` Console
witness before accepting exit 7 and uses package-owned `COMMAND /c MEM.EXE`
for the nested-DOS route. The retired default-off DEM-open trace is not an
acceptance dependency; the guest's own `S10_GUEST_SEVEN` row is direct
execution proof.
