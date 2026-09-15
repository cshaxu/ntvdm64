# CCPU40 x87 host-layout restoration

## Status and objective

This is an unadmitted candidate at the head of the
[Queue](../states/QUEUE.md).  It does not allocate a T number, alter the
active M0 T412 S4 packet, or authorize a source change until admission.

Restore the selected CCPU40 x87 host-representation declarations in
`src/mvdm-host/softpc.new/host/inc/cfpu_def.h` where Microsoft x86 C bitfield
allocation and little-endian multi-word member order disagree with the
declaration currently mirrored by this repository.  The intended recovery is
narrow: retain the original FPU algorithms and instruction flow, correct only
the host-layout declarations proved necessary for the selected compiler/profile,
and prove the affected guest instruction paths.

This is not a guest x86 byte-order change.  x86 guest byte order can be
correct while a C bitfield declaration is still interpreted incorrectly by the
host compiler.  In particular, MSVC/x86 allocates bitfields from the low-order
end of the storage unit.  A declaration written in logical most-significant to
least-significant order can therefore expose the wrong sign, exponent or
mantissa fields despite a little-endian x86 EXE.

## Evidence to revalidate at admission

The proposed comparison candidate is the neighbouring SoftPC repair
`2c05238`, whose changed `cfpu_def.h` is reported to reverse the declarations
below under the little-endian condition.  Before any import or derivative
change, record the exact commit object, file hash, complete declaration diff,
licensing/provenance disposition, selected OpenNT counterpart and current
mirror hash.  The candidate is evidence of a narrow host-ABI repair, not by
itself authority to substitute a different FPU implementation.

Audit these declarations as one inseparable layout group:

- `FP32`, `FP64HI` and `FP80SE` bitfield declaration order;
- `FP64` and `FP80` high/low DWORD member order; and
- `FPU_I64` low/high word order, cross-checked against the little-endian BCD
  constants and consumers in `base/ccpu386/fpu.c`.

The current simple `FLD1; FSTP dword` observation is explicitly insufficient:
its final store can write an unchanged whole `IU32`, without demonstrating that
the `FP32` bitfields were parsed correctly.  Admission must also identify the
load/inspect paths in `fpu.c` (including the field-consuming path near the
current line 1541) and distinguish them from whole-word stores such as the
current path near line 873.

## Source and placement rules

The affected header is an existing MVDM/SoftPC mirror, so a proved correction
belongs there, with the smallest registered divergence and exact provenance.
No adapter, private overlay, CCPU algorithm, opcode semantic change, CPU30 or
MONITOR path is authorized.  Do not import a broad neighbouring-project patch;
the implementation must be the audited declaration-only subset.  If the
source audit proves a different original owner or an unavailable compiler-only
binding, record that result and place only that binding at the approved
boundary.

## Proposed sequential work

### S1 — provenance, layout and reachability audit

Freeze the current x86 CCPU40 build inputs and compare the complete header
against the selected OpenNT source and `2c05238`.  Produce a per-declaration
ledger with bit position/member offset, size, affected `fpu.c` consumers and
the precise reason the declaration is safe or wrong under MSVC/x86.  Search
same-pattern FPU unions, bitfields and multi-word numeric carriers in the
reached CCPU40 cohort; classify each rather than silently widening the patch.

Establish actual reachability from x87 load, conversion, extended-precision
and BCD instructions to each carrier.  Existing COMMAND/EDIT success is not
evidence for these paths.  Stop and report if the comparison change is not
reproducible, its provenance cannot be accepted, or the selected build does
not use the affected declarations.

### S2 — minimal mirror correction and structural fixtures

Apply only the audited little-endian declaration/member-order correction.
Retain names, packing and all FPU code flow.  Add focused structural fixtures
that construct known guest-format words and verify the intended sign,
exponent, mantissa and word positions under the actual x86 MSVC toolchain.
Include a `FPU_I64` fixture whose expected order agrees with the existing BCD
constant table; this prevents an apparently successful integer/BCD conversion
from hiding a reversed pair.

Record the before/after source diff and demonstrate that no autonomous
adapter/overlay implementation was added.  A failure to prove the exact
layout must leave the mirror unchanged rather than prompt a compensating FPU
algorithm patch.

### S3 — instruction-level CCPU40 regression and closure

Run focused guest instruction fixtures that require field parsing rather than
only whole-word stores:

- IEEE `m32real` load/arithmetic/store cases covering positive and negative
  values plus discriminating exponent/mantissa patterns;
- IEEE `m64real` equivalents;
- 80-bit extended load/store or conversion cases that exercise `FP80SE` and
  both mantissa DWORDs; and
- packed BCD `FBLD`/`FBSTP` plus the reached integer conversion path using
  `FPU_I64`.

Each fixture must assert result bytes and relevant exceptional/failure result,
not merely that CCPU completes.  Execute them in the formal x86 CCPU40 graph,
then run the existing focused FPU regression set and bounded DOS smoke
workload.  This candidate does not make an unsupported claim that a WOW16
application validates x87 field layout.

## Acceptance and exit criteria

Closure requires all of the following:

1. A reviewed, source-proven declaration ledger and provenance record for
   every changed field/member, including the exact `2c05238` comparison.
2. A declaration-only correction (or an evidence-backed no-change outcome),
   with no FPU algorithm rewrite and no new adapter/overlay provider.
3. Passing x86 structural fixtures and instruction-level single, double,
   80-bit and packed-BCD coverage that would detect the former layout error.
4. Passing affected formal build/link, focused regressions, bounded runtime
   smoke and documentation/diff governance checks.
5. A final residual same-pattern ledger: each non-changed carrier is either
   proved correct, unreachable in the selected profile, or transferred to a
   named owner.

Build products and intermediate results remain under `build/`; any staged EXE
follows the active product publication procedure, and real-run observations
remain under `O:\ntvdm64\logs\`.  Failure to establish a layout or source
contract is a reportable stop condition, not permission to invent replacement
floating-point behavior.
