# M0 T395 S3 — CPU40 DPMI dispatch and lifecycle audit

Date: 2026-09-06

## Question

Does the CPU40 formal DPMI32 cohort leave any unowned `53:00`–`53:18`
service, and can a complete original DPMI client lifecycle be proved without
admitting WOW32?

## Inputs and procedure

The audit inspected the original table in
`src/mvdm-host/dpmi32/dpmi32.c`, the selected `sources` manifest, every
referenced CPU40 body, and the original DOSX source in
`src/mvdm-host/dpmi/486/{dxboot,dxfunc,dxstrt}.asm`.  The formal x86 graph
contains the thirteen portable `sources` bodies plus the RISC/CCPU mechanics
`dpmimemr.c` and `dpmimscr.c`.  It excludes all three `i386/` kernel-VDM
bodies.

`Test-T289S7DpmiMirrorAudit.ps1` passes with 12 exact mirrors, 11
divergence-marked adaptations, two recorded non-semantic variants, and eight
registered divergence families.  The formal graph contains exactly those 15
translation units under `obj/dpmi/`.

## Complete dispatch ledger

| BOP | Source owner | CPU40 disposition | S2 runtime evidence |
| --- | --- | --- | --- |
| `00` | `dpmiselr.c::DpmiSetDescriptorEntry` | Composed; CCPU LDT synchronization is `DIV-225`. | reached |
| `01` | `modesw.c::DpmiCpu40SwitchToProtectedMode` | CPU40-shaped original frame transition, `DIV-222`; no kernel state. | reached |
| `02` | `dpmiint.c::DpmiSetProtectedmodeInterrupt` | Selected original CCPU body. | reached, 255 registrations |
| `03` | `dpmimscr.c::DpmiGetFastBopEntry` | Selected original CCPU/RISC failure form. | reached |
| `04` | `dpmi32.c::DpmiInitDosx` | Selected original body. | reached |
| `05` | `dpmi32.c::DpmiInitApp` | Selected original body. | not reached |
| `06` | `int21map.c::DpmiXlatInt21Call` | Selected original mapping owner. | not reached |
| `07` | `xmem.c::DpmiAllocateXmem` | Selected original body over CPU40 shared XMS, `DIV-224`. | reached |
| `08` | `xmem.c::DpmiFreeXmem` | Selected original body over CPU40 shared XMS, `DIV-224`. | not reached |
| `09` | `xmem.c::DpmiReallocateXmem` | Selected original body over CPU40 shared XMS, `DIV-224`. | not reached |
| `0A` | `dpmiint.c::DpmiSetFaultHandler` | Selected original CCPU body. | reached, 32 registrations |
| `0B` | `dpmimemr.c::DpmiGetMemoryInfo` | Selected CCPU/RISC memory owner; reports the actual shared XMS pool, `DIV-224`. | not reached |
| `0C` | `dpmimscr.c::DpmiDpmiInUse` | Selected original CCPU/RISC interrupt-hook owner. | not reached |
| `0D` | `dpmimscr.c::DpmiDpmiNoLongerInUse` | Selected original CCPU/RISC interrupt-hook teardown owner. | not reached |
| `0E` | `DpmiIllegalFunction` via `dpmidata.h` | Intentional CPU40 unsupported debug-register service; not a missing body. | not reached |
| `0F` | `dpmi32.c::DpmiPassTableAddress` | Selected original body plus CCPU LDT binding, `DIV-223`. | reached |
| `10` | `xmem.c::DpmiFreeAppXmem` | Selected original body over CPU40 shared XMS, `DIV-224`. | not reached |
| `11` | `dpmiint.c::DpmiPassPmStackInfo` | Selected original layout with guest-readable projection, `DIV-226`. | reached |
| `12` | `vxd.c::DpmiVcdPmSvcCall32` | Selected original VCD service; only its documented version/serial-port forms exist. | not reached |
| `13` | `xmem.c::DpmiFreeAllXmem` | Selected original teardown owner over CPU40 shared XMS, `DIV-224`. | not reached |
| `14` | `dpmiint.c::DpmiIntHandlerIret16` | Selected original CCPU IRET-hook body. | not reached |
| `15` | `dpmiint.c::DpmiIntHandlerIret32` | Selected original CCPU IRET-hook body. | not reached |
| `16` | `dpmiint.c::DpmiFaultHandlerIret16` | Selected original CCPU fault-return body. | not reached |
| `17` | `dpmiint.c::DpmiFaultHandlerIret32` | Selected original CCPU fault-return body. | not reached |
| `18` | `dpmiint.c::DpmiUnhandledExceptionHandler` | Selected original CCPU exception-chain body. | not reached |

Thus all 25 dispatch rows have an owner and a formal CPU40 disposition.  The
only deliberately unavailable row is `0E`, which has the original
CPU40-profile `DpmiIllegalFunction` mapping; it is not silently absent.

## Lifecycle result

The original DOSX exit source (`486/dxstrt.asm`) calls `FreeAllXmem` and then
`DpmiNoLongerInUse` only after the last DPMI client returns.  The S2 traces
never reach `53:13` or `53:0D`; they stop earlier at the BIOS `INT 19h`
termination path.  Therefore that trace proves bootstrap mechanics but cannot
prove normal client teardown.

The repository's available original DPMI clients are the Win16 path:
`KRNL386.EXE`, `WINHELP.EXE`, and the test-shell `WOWEXEC.EXE`.  Their normal
execution requires the not-yet-admitted WOW32 provider/callback/task-lifetime
owner.  DOSX is the extender/provider rather than an independent application
client.  No original non-WOW32 DPMI client product is present in the selected
media.  Creating one or synthesizing BOP traffic would violate this packet's
non-goals and would not be original workload evidence.

## Interpretation and follow-up

There is no remaining missing CPU40 DPMI32 host body to recover.  The next
runtime proof necessarily crosses the original WOW32 boundary: a Win16 client
must enter `51h → W32Init → W32Dispatch`, then return through DOSX's original
`53:13`/`53:0D` teardown.  That is a separate provider package, already next
in Queue, and cannot be folded into T395 without renewed owner approval.

S3 is high-confidence source/formal evidence, but not a claim that an
unadmitted WOW16 client is runnable.  The retained S2 observation remains the
only runtime evidence for CPU40 DPMI bootstrap.
