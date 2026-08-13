# T198 S24 P2 Native HLT Source Triage 001

## Question

What can the native CPU5 `HLT instruction with IF=0` observation after the
proven `DEM 50:3B` resume mean, without altering guest state or enabling
more BOP services?

## Source Evidence

The relevant original `sysinit1.asm` path calls `SVC_ISDEBUG`, consumes its
AL result, and continues into DOS relocation/initialization.  This source has
two explicit `cli; hlt` sites:

| Source site | Immediate predecessor | Meaning |
| --- | --- | --- |
| `FatalErr` at lines 1936--1943 | `AllocMemForDOS` has insufficient arena space | DOS low-memory allocation failure |
| `csisystemerror` at lines 3992--4000 | `AllocHMA`'s INT 16h country/system driver query or set operation failed | local system/driver initialization failure |

Bochs' retained `BX_CPU_C::HLT` log records IF but not guest CS:IP.  The
current finite-run ABI returns only a terminal status, so the observed
`HLT instruction with IF=0` cannot distinguish these two original paths.

## Superseded Interpretation

The two source sites are valid static candidates only.  P3 established that
the S24 fixture itself publishes a one-byte `HLT` mock NTDOS image through
`50:11`; the observed terminal HLT therefore cannot be attributed to either
NTIO source site.  This record must not be used to infer a missing machine,
memory, interrupt, or driver capability.

The snapshot diagnostic was admitted only to prove that limitation.  The
correct successor is exact source-built NTDOS composition, not a further HLT
or firmware change.
