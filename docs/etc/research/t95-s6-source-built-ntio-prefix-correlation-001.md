# T95 S6 Source-Built NTIO First-Prefix Correlation 001

## Purpose and limits

This is a read-only correlation of the one accepted first-prefix execution
record.  It classifies the observed FPU diagnostic; it neither changes the
guest nor proposes a repair.  In particular, it does not infer a missing
host service, BIOS implementation, BOP/DEM path, or CPU configuration from
one diagnostic string.

## Inputs and procedure

| Input | Identity / read-only procedure |
| --- | --- |
| First-prefix evidence | `artifacts/analysis/t95-s6-source-built-ntio-prefix-001-20260811-001/observation.json`, its `stderr.log`, and `guest/artifact-manifest.json`. The placed NTIO is 33,792 bytes, SHA-256 `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937`. |
| NTIO layout | `artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/ntio.map` and the matching source-built `NTIO.SYS`. Read the first entry bytes and the mapped `init` symbol only. |
| OpenNT initial prefix | `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm`, `msinit.asm`, and `src/opennt/base/mvdm/inc/intmac.inc`. |
| Bochs diagnostic | `src/bochs/fpu/fpu.cc`, `BX_CPU_C::FPU_check_pending_exceptions`. |
| CPU-profile authority | `docs/etc/research/t95-s6-cpu-profile-authority-reconciliation.md`. |

No executable, compiler, linker, configuration generator, or source file was
run or changed for this record.

## Established facts

| Question | Evidence | Classification |
| --- | --- | --- |
| Was the requested plan accepted? | `observation.json` records `planApplied: true`; `stderr.log` logs `requested execution plan applied`. | Established: this is not endpoint rejection. |
| Was NTIO really the placed payload? | The artifact manifest calls NTIO the only placed payload and records the hash above. NTDOS and COMMAND are identity-only and not placed. | Established: no DOS load may be inferred. |
| Is offset zero accidental data execution? | `msbio1.asm` begins `BData_start` with `jmp init`. The built NTIO begins `E9 6D 03`; `ntio.map` places `init` at `0000:0370`, matching the near jump from offset `0000` to `0370`. | Rejected explanation: the recorded entry is intentionally a jump into initialization, not a direct fall-through into data. |
| What exactly produces the FPU log? | In `fpu.cc`, `FPU_check_pending_exceptions` first requires `the_i387.get_partial_status() & FPU_SW_Summary`. With CPU level at least 4 and `CR0.NE == 0`, it logs `math_abort: MSDOS compatibility FPU exception` and raises PIC IRQ13; with `NE != 0` it raises `#MF`. | Established diagnostic meaning: an x87 pending-summary condition was observed on the compatibility IRQ13 branch. The log does **not** identify the instruction, original exception, or an interrupt-vector owner. |
| What is the observed terminal state? | The log emits repeated FPU diagnostics, then `HLT instruction with IF=0`; at the native benchmark stop it reports real mode, `CS:0000`, `EIP=00000001`, and `CR0=0x60000010`. | Established terminal snapshot only. It is not a trace of the first failing instruction. |

## Initial-state correlation

`msbio1.asm` reserves `ntvdmstate dd 0` and asserts its offset from
`BData_start` equals `FIXED_NTVDMSTATE_OFFSET`.  The existing source evidence
locates this fixed word at physical `0x714`.  `msinit.asm:init` begins with
`FCLI`, sets `DS=0`, saves and replaces selected interrupt vectors, installs a
local stack, then executes `FSTI`.  It later executes `SVC SVC_DEMLOADDOS`;
that later service is outside the accepted first-prefix mechanics and is not
an authorized next implementation.

For the non-`WOW_x86` branch selected in `intmac.inc`, `FCLI` and `FSTI` are
ordinary `cli` and `sti`.  They contain no x87 operation and therefore cannot
by themselves explain the FPU diagnostic.  The fixed VDM-state word is still
a source-owned startup input used by other virtual-interrupt macro paths, but
this record has no instruction-level evidence that it caused the observed x87
pending condition.

## Candidates deliberately left unresolved

| Candidate | Why it remains unresolved | Disposition |
| --- | --- | --- |
| Reached NTIO instruction or data sequence that creates the x87 pending state | The retained log has diagnostics and terminal snapshot, not an instruction trace or x87 status/control snapshot. | Diagnostic candidate only. |
| Reset CPU/FPU configuration, including `CR0.NE` and FPU state | The log proves the compatibility branch used `NE=0`, but does not identify why the x87 summary became set. | Diagnostic candidate only; no profile change. |
| Fixed VDM state / IVT / BDA provenance | OpenNT source proves the values participate in initialization, but no current evidence connects one to the FPU summary. | Candidate input only; no synthetic state. |
| Historical host service such as `SVC_DEMLOADDOS` | It is source-reached later than the currently correlated prefix; the log does not prove reachability. | Explicitly out of scope. |

The separate CPU-profile reconciliation is binding here: CPU3 is an
analysis-only projection and CPU5 is historical stock-firmware evidence.
Neither is selected as a unified OpenNT runtime profile, so this record cannot
use the FPU diagnostic to choose one.

## Result and bounded next question

The evidence excludes wrong-entry fall-through and adapter rejection.  It
establishes only an early native x87 compatibility-IRQ13 condition after NTIO
handoff.  The next admissible question, if separately approved, is how to
obtain a **passive, bounded instruction/x87-state diagnostic** for this same
immutable input and profile.  Its design must name the exact observation
boundary and copied fields before any run; it must not introduce a state
override, interrupt/BIOS shim, host service, guest patch, or retry.
