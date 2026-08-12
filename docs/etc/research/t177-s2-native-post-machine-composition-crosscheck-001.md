# T177 S2 native POST and machine-composition cross-check 001

## Question

Does the currently observed machine-composition derivative satisfy the native
POST/INT10-vector prerequisite established by T158 and T177 S1, and is there
an already-declared composition that can satisfy both requirements without a
new emulator, device, adapter provider, or guest repair?

## Method and limits

This is a read-only reconciliation of the retained T176 S23 manifest, the
T159 static closure audit, the T160 native-POST observation, the derivative
generator, and the existing Bochs exception ordering. It invokes no build,
runtime observation, firmware, or source change.

## Reconciled transition facts

| Edge or component | Evidence | Classification |
| --- | --- | --- |
| T176 S23 machine-composition image | Its manifest has `deferredStartupPlan: false`, `machineComposition: true`, one replacement (`cpu\\exception.o`), and retained `main.o`. | It is a direct-entry image and **does not** satisfy the native-POST prerequisite. Its later stack result cannot diagnose the machine BOP component. |
| Native POST/deferred lifecycle | T159 proves that a deferred `main.o` disables the direct execution plan, initializes hardware/POST, then enters the ordinary CPU loop. T160 built and observed that two-object form through full COMMAND materialization and normal `50:36` resume. | Bochs owns this lifecycle; it excludes adapter BOP providers and optional devices as the owner of the zero INT10-vector predecessor. |
| Combined source/build form | `New-T98S1CurrentAdapterEngineDerivative.ps1` independently accepts `-DeferredStartupPlan` and `-MachineComposition`. With both, it copies/rebuilds only `main.o` and `cpu\\exception.o`, adds the already-defined machine component objects, and retains the existing Bochs archives. | A declared, bounded combined form exists. It has not been built or observed in this record. |
| #UD ordering | In `cpu/exception.cc`, the exact deferred-startup consumer precedes the machine-composition probe. The former accepts only its checked option-ROM rendezvous; an unmatched later #UD proceeds to the machine probe. | The two concerns are ordered, not competing generic interceptors. This establishes source-level compatibility only. |

## Owner and negative-boundary conclusions

The first repair-relevant predecessor remains the direct image's missing native
POST lifecycle, not a missing adapter BOP service. T176 S23 therefore cannot
be used to infer that selector `06h`, its original SoftPC handler, RAM, PIC,
BIOS, or an optional device caused the T177 stack path.

The bounded combined form changes no BOP classification or service result. It
uses the existing native Bochs POST path for machine initialization and the
existing selector-blind machine-composition seam only after the exact deferred
rendezvous has been consumed. It does not synthesize IVT entries, introduce a
device, add an adapter provider, or assign DOS/OpenNT semantics to Bochs.

## Disposition

The **currently observed T176 S23 derivative does not satisfy** the proven
POST/INT10 prerequisite. A source-declared combined derivative is statically
compatible with that prerequisite, but no claim of link or runtime behavior is
made here. T177 S3 may decide whether one exact, no-source-change combined
build and one bounded observation have sufficient value; otherwise T177 closes
with this attribution and without a repair.

## Confidence

High for the object/flag and ownership classification: all relevant manifest,
generator, source-order, and prior native-POST evidence is retained. No
confidence is assigned to combined runtime reachability until a separately
admitted observation occurs.
