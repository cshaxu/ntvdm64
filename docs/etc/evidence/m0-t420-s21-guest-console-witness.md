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
| Worker TLS | `ccpu-thread-lifecycle-test.exe` | Current direct formal-library link/run emitted `CCPU thread lifecycle OK`; this is host-side proof and is not counted as a guest witness. |

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

## Interpretation and follow-up

The text gate prevents false green acceptance when a wrapper exits zero after
guest failure.  It does not claim S21 closure: the remaining CCPU family and
worker-lifetime matrix must still be completed.  The HLT result is an explicit
source-shaped CPL3 boundary, not a reason to alter original CCPU privilege
semantics or introduce a CPU30/V86 monitor.
