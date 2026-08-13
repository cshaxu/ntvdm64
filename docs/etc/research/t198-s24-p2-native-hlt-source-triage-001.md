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

## Interpretation

This is evidence of a guest-side deliberate fatal wait, not evidence that
Bochs must map a new aperture, manufacture an interrupt, or that the BOP
plane should add a service. Increasing the instruction budget would repeat a
halted CPU and cannot identify the predecessor.

## Required Next Admission

The S24 stop condition applies: a new diagnostic would alter the mantle
fixture ABI/source.  Any successor must be separately admitted as a
default-off, selector-blind terminal CS:IP snapshot: copied fixed-width
result only, no guest-memory read, no CPU mutation, no device or BOP
semantics.  Its sole purpose is to classify this already observed HLT site.
