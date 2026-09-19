# T420 S38 DPMI32 Capability Review

## Admission And Baseline

The owner authorizes sequential automatic S admission after S37 closure
`2e56df9eb`. S38 verifies the selected DPMI32 host package; S39 retains the
guest DOSX package. Guest media remain immutable. Product baseline is
`6b80471d9`; S18 source recovery and S36 real DPMI allocation/cleanup evidence
are reusable inputs, not proof of every interrupt, fault or stack path.

## Source And Dispatch Inventory

The selected fifteen original bodies are `debug.c`, `dpmi32.c`, `dpmiint.c`,
`dpmiselr.c`, `buffer.c`, `data.c`, `int21map.c`, `modesw.c`, `register.c`,
`savestat.c`, `stack.c`, `vxd.c`, `xmem.c`, `dpmimemr.c` and `dpmimscr.c`.
The associated headers are `dpmi32p.h`, `dpmidata.h` and `dpmiint.h`.
The portable CPU40 composition uses the original non-monitor memory bodies;
it must not silently substitute kernel-monitor assumptions.

Source inspection of `src/mvdm/dpmi32/dpmi32.c` identifies 25 BOP slots:

| Slots (hex) | Contract requiring disposition |
| --- | --- |
| 00, 0F | Descriptor updates and table publication: domains, base, limit, rights and invalid requests. |
| 01, 04, 05, 0C, 0D, 11 | Mode switch, DOSX/application initialization, in-use state and protected-mode stack lifecycle. |
| 02, 0A, 14-18 | Interrupt/fault registration, 16/32-bit returns and unhandled exceptions. |
| 06 | INT21 translation, real DOS results and failure propagation. |
| 07-09, 0B, 10, 13 | Allocation, reallocation, memory information, explicit free and task cleanup. |
| 03, 0E, 12 | Fast-BOP, debug-register and VCD entries: selected-profile reachability or original unavailable behavior. |

## Verification Obligations

Every selected body and slot needs a source/compiled/reached disposition.
Real guest probes must check data, registers, FLAGS, invalid requests,
interrupt/fault return and repeated-task cleanup. Boundary mocks supplement
but do not replace real guest evidence. S36 allocation/stress/lifecycle
probes are baseline coverage only. Required final checks include formal x86
build and all 17 established transcript-gated product routes.

All new build/probe products belong under `build/M0-T420/S38`; deployed test
artifacts and observations belong under `O:/winnt/tests` and `O:/winnt/logs`.
Any repair must follow the four-rung original-source recovery audit and
report mirror and non-mirror changes separately.

## Current Result

S38 remains open; no production repair or complete dispatch coverage is claimed.

## Initial Runtime And Source Checks

`Verify-T420S36DpmiGuest.ps1` ran against the deployed x86 CPU40 product with
the S34 Console observer and retained S36 independently authored probes:

| Run prefix under O:/winnt/logs | Result |
| --- | --- |
| s38-baseline-lifecycle-r1 | Direct and twice-nested COMMAND routes pass; four live-allocation children, restored task capacity and worker exit after broker loss are asserted. |
| s38-baseline-stress-r1 | Harness failure: the 39-column Console wrapped the success marker across rows 3 and 4. Guest exit was zero and the complete marker is present in captured text. |
| s38-baseline-stress-r2 | Both routes pass after joining observer-prefixed physical rows for marker matching; forced relocation, failed growth, data preservation and free are asserted. |

The harness preserves captured characters and spaces, removes only observer
row prefixes and row separators, and retains exit/failure-marker checks.
This is a test parsing correction, not a product or guest change.

Source inspection distinguishes the special dispatch slots: original
`dpmimscr.c::DpmiGetFastBopEntry` returns BX/DX/ES zero; non-i386
`dpmidata.h` maps debug-register BOP to `DpmiIllegalFunction`; original
`vxd.c` implements VCD version and host serial-port enumeration and sets CF
for unsupported operations. They must not be classified together as absent.
The non-monitor interrupt returns retain actual guest IRET execution, whereas
fault returns restore saved frames directly. Both still require focused real
guest coverage, including 16/32-bit cases, before closure.

## Real 16-bit Interrupt And Fault Return

The independent `tests/observation/dpmi_interrupt_return.asm` probe follows
original `dpmi/486/dxint31.asm` vector contracts and the 16-bit fault frame in
`dpmi32/dpmiint.c`. It uses INT 31h to install and restore software interrupt
60h and divide-error handlers; it never modifies original guest code. The
divide handler advances the saved IP past the probe's own DIV instruction.
Assertions check one handler invocation, restored SP, preserved AX and carry,
and rejection of exception-vector number 11h by original DOSX.

Build: NASM `-f bin tests/observation/dpmi_interrupt_return.asm -o
build/M0-T420/S38/interrupt-guest-r1/D38I.COM`. The retained runner is
`tools/audit/Verify-T420S38DpmiInterrupt.ps1`, using the S34 observer,
that probe and `-LogPrefix s38-interrupt16-matrix-r1`. Both direct and
twice-nested COMMAND routes exit zero and emit `S38_INT16_RETURN_OK` plus
`S38_FAULT16_RETURN_NEGATIVE_OK`; hashes and reports are in
`O:/winnt/logs/s38-interrupt16-matrix-r1-summary.json`.

This proves selected 16-bit guest-observable return behavior, not all BOP
slots or 32-bit frames. Hardware interrupt nesting, 32-bit clients, complete
descriptor coverage and the remaining package audit are still open. No
product diff or immutable guest-media change was made in this delivery.

## 32-bit Client Frame Verification

The same independent probe now has `CLIENT32`: AX=1 at DPMI entry selects
32-bit client frames, vector offsets retain EDX, and the handlers use IRETD
and a 32-bit far return with the original DWORD exception frame. Its code
segment remains 16-bit. This distinguishes frame-width coverage from a
claim of complete 32-bit instruction/code-segment coverage.

NASM builds under `build/M0-T420/S38/interrupt-guest-r2`: `D38I.COM` without
defines and `D38J.COM` with `-DCLIENT32`. The retained verifier accepts
`-ClientBits 16` or `32` and requires the corresponding guest markers.
`s38-interrupt32-matrix-r1` and `s38-interrupt16-matrix-r2` both pass direct
and twice-nested routes with zero exits and matching markers. Each summary
under `O:/winnt/logs` retains the exact probe hash.

The unchanged deployed product also passes all 17 established routes using
`Verify-CommandExitStatus.ps1`, the S34 observer and `G7.COM`, with prefix
`s38-product-baseline-r1`. This includes direct/nested/repeated MEM, COMMAND,
native streams/EOF, exit codes and EDIT return. Expected historical nonzero
codes are asserted by that matrix rather than misclassified as failures.

## Current Mirror Comparison

Byte/hash and CRLF-normalized comparison of the fifteen bodies and three
headers against `O:/repos.external/OpenNT/base/mvdm/dpmi32` found five byte-exact
files: debug, register, savestat, vxd and dpmimscr. Buffer, int21map and stack
are normalized-exact but not byte-exact: their format restoration remains a
closure obligation. The other ten files have retained non-newline changes.
In particular, xmem's BOOL failure check is the S36 verified repair, not an
unnecessary type-cleanup difference; removing it would reintroduce failure
misclassification. No mirror edit has been made during this initial review.

## True 32-bit Code And Exact Formatting Recovery

The probe's `CODE32` variant uses original INT31 descriptor get/allocate/set
services to clone its code descriptor, set the default-size bit and far-jump
to independently assembled 32-bit code. Its exception handler consumes the
original 32-bit frame. Bootstrap failures retain a separate 16-bit exit path.
All variants now verify the complete EAX value rather than AX alone.

NASM outputs in `build/M0-T420/S38/interrupt-guest-r3` are D38I (16-bit
client), D38J (`CLIENT32`, 16-bit code) and D38K (`CLIENT32` plus `CODE32`).
Prefixes `s38-frame16-r3`, `s38-frame32-r3`, `s38-code32-r3` record six passing
direct/nested routes on the preceding deployed product. This closes these
specific code/frame combinations, not hardware interrupt nesting or all APIs.

The three normalized-exact mirror files buffer.c, int21map.c and stack.c
were mechanically restored to their upstream bytes after checking normalized
equality. Git attributes already preserve mirror bytes (`-text`). Byte hashes
now match for eight of eighteen selected files. Raw diff is +3731/-3731
solely from line endings; ignoring end-of-line differences produces no diff.
Semantic diff reduction and autonomous production-code reduction are zero.

The retained formal x86 S36 graph was intentionally reused for unchanged
inputs. Its three affected objects and DPMI library rebuilt, followed by the
explicit run16.exe/basesrv.exe/ntvdm.exe targets and successful VdmTib storage
verification. New worker SHA256 is
`9afae9d7903b39b607247c646f36d9ac8867919e739201fc2e81491130a5dc4c`.
It was deployed after checking that the package was unused. On that worker,
`s38-code32-formatted-r1` passes both routes. The full product regression
prefix is `s38-format-product-r1`: all 17 routes completed and passed,
including EDIT return. Original guest media remain unchanged.

## Nested Locked-stack Exception Verification

The fault probe now raises a second divide exception inside the first
handler, while retaining the outer frame. It preserves EAX/EDX/EBX around
the inner trigger, advances each saved IP past its own DIV and requires
exactly two handler invocations before checking the outer client state.
This exercises nested locked-stack return rather than inferring it from
a single exception. No original guest or product source changes are involved.

NASM outputs under `build/M0-T420/S38/nested-fault-r1` retain the three
variants. Prefixes `s38-nested-fault16-r1`, `s38-nested-frame32-r1` and
`s38-nested-code32-r1` each pass direct and nested COMMAND on the deployed
worker recorded above; verifier summaries retain probe hashes. These are
six passing real-guest routes, not hardware-IRQ proof.

The formal x86 `cpu40-descriptor-domain-fixture.exe` target was also rebuilt
and executed successfully. Its inspected assertions cover GDT versus LDT
selection, rejection before GDT publication, final DOSX IDT descriptor,
replacement publication rather than first-address latching, and invalid IDT
limit. This is explicitly host fixture evidence; the true CODE32 guest probe
provides separate end-to-end descriptor publication and execution evidence.

Source review distinguishes `DpmiSwIntHandler` (direct client IRET frame)
from `DpmiHwIntHandler` (locked stack plus DOSX return-hook frame). Consequently
software INT success alone does not verify BOP 14/15 hardware-return hooks.
That hardware path remains a required follow-up before S38 closure.

## Real Timer IRQ Return

The probe now saves protected-mode vector 08h, installs its own small
counter-and-chain handler and spins with interrupts enabled until two actual
timer interrupts occur. It never executes software INT 08h. The handler
far-jumps to the original vector, preserving timer/PIC handling. The probe
checks the returned SP, restores vector 08h and requires the additional
`S38_HARDWARE_IRQ_RETURN_OK` marker before successful exit.

Builds under `build/M0-T420/S38/hardware-irq-r1` use the same three variants.
Prefixes `s38-hardware16-r1`, `s38-hardware-frame32-r1` and
`s38-hardware-code32-r1` each pass direct and twice-nested COMMAND: six real
routes. The verifier now requires the IRQ marker as well as interrupt and
nested-fault results. Product and guest-media bytes remain unchanged.

Source attribution: `ccpu386/c_main.c` invokes `host_hwint_hook` for accepted
hardware interrupts; the selected DPMI hardware path constructs its locked
stack and DOSX return-hook frames in `dpmi32/dpmiint.c`. These runs establish
observable hardware return behavior. They are not an independently captured
per-BOP trace, nor proof that all possible IRQ devices or nesting cases pass.
The earlier hardware-coverage follow-up is superseded for this timer case.

Remaining audit includes the unhandled-fault-to-interrupt-chain behavior,
special fast/debug/VCD service dispositions and complete selected-body/slot
coverage. A registered exception handler passing does not prove unhandled
exception behavior; `DpmiUnhandledExceptionHandler` has distinct reflection
and fatal branches that must not be silently marked passed.

## Unhandled Exception Reflection And Original Frame Correction

An independent `REFLECT` variant restores the default divide fault handler,
installs a protected-mode interrupt 0 handler and triggers DIV-by-zero. The
default DOSX fault entry must transfer through the original unhandled-fault
reflection into that interrupt handler. The handler skips only this probe's
DIV instruction and returns; EAX, carry, SP and one reflection are asserted.
Reflection and timer cases are selected separately, avoiding ambiguous
attribution of a failure in the preceding timer wait.

Both pinned `OpenNT/base/mvdm/dpmi32/dpmiint.c` and
`OpenNT-4.5/nt/private/mvdm/dpmi32/dpmiint.c` write FrameCS to DWORD-frame
offset 4 and then overwrite it with FrameFlags at offset 4. The required
EIP/CS/EFLAGS layout is offsets 0/4/8, as the same file's other IRETD frame
construction demonstrates. DIV-280 changes only that flags destination to 8.

Recovery ladder: the original translation unit already composes and remains
the implementation owner. A facade cannot fix a wrong store offset inside
that owner without changing its algorithm or replacing it; no external-code
intrusion or autonomous provider is needed. The minimal registered mirror
correction preserves original ordering, reflected-handler selection and
failure flow. Product source delta is +3/-1 including two comment lines;
adapter/overlay delta is zero. This is an upstream layout defect, not a
newly invented exception policy.

Evidence chronology: `s38-reflect16-r1` and the two r2 16-bit cases emit their
success markers. The first 32-bit combined attempt times out before an IRQ
marker, so it cannot identify reflection as the failing operation. The
isolated `s38-reflect32-r2` also times out with no captured success text;
that absence is not a per-instruction fault trace. With the single offset
correction and the same r2 binary, `s38-reflect32-fixed-r1` passes direct and
nested routes. `s38-reflect16-fixed-r1` and `s38-hardware-fixed-r1` also pass
both routes. This source/layout plus same-probe contrast supports the fix;
it does not claim that every earlier timeout has been conclusively attributed.

One 16-bit run additionally exposed test cleanup racing normal broker-loss
exit after success. The runner now tolerates a process already gone while
still requiring any looked-up process to finish within five seconds.

Formal x86 compile/link and VdmTib verification passed using the retained
S36 graph. Deployed worker hash is
`55214ce4a67d08716432b83b54e061eef37fc238eb2bc8c9ccc9bfe0307f11ec`.
The full product gate `s38-reflection-product-r1` completed with all 17 routes
passing, including EDIT return. S38 remains open, including the distinct
fatal branch and remaining service/coverage dispositions.

## VCD Guest Service And Special-slot Source Attribution

The independent `tests/observation/dpmi_vcd_service.asm` enters DPMI,
discovers VCD via INT2F/1684 and BX=0Eh, calls the returned original DOSX
entry and asserts version 030Ah, a nine-bit port array and CF refusal for
FFFFh. Original `486/dxfunc.asm::VCD_PM_Svc_Call` performs BOP 53:12 into
the existing `vxd.c` body; no test-specific BOP or provider is introduced.

NASM output `build/M0-T420/S38/vcd-guest-r1/D38V.COM` passes direct and
twice-nested routes with the S38 runner's `-VcdService` switch and prefix
`s38-vcd-r1`. Both transcripts contain `S38_VCD_VERSION_PORTS_REFUSAL_OK`.
This proves discovery, version, bounded response and refusal, not physical
serial I/O or equality to the exact host port bitmap. Product and guest
media remain unchanged.

Source attribution: Fast BOP discovery in `486/dxboot.asm` is WOW_x86-only;
portable `dpmimscr.c` returns a zero fast-entry address. The non-WOW_x86
debug path in `486/dxint31.asm` uses guest MOV DRx in Store/Load_DBG_Regs,
not BOP 0E. Zero hits at that slot cannot prove missing debug capability.
Actual watchpoint behavior remains unverified; these source routes are not
runtime success claims.
