# T179 S1 guest-teardown predecessor map 001

## Scope and inputs

This read-only map joins the original NTDOS `msctrlc.asm` teardown sequence,
the retained normal-return trace
`artifacts/analysis/t176-s19-share-normal-return-lifecycle-observation-001`,
and the existing Bochs stack audit. It does not infer a missing write or alter
an exception/BOP path.

## Proven chain

| Order | Fact | Owner | Confidence |
| --- | --- | --- | --- |
| 1 | `50:3C` resumes at `0032:5333` with zero CPU delta. | Existing contained DEM lifecycle provider, preserving original guest continuation. | High |
| 2 | Original `msctrlc.asm` then frees the child arena, performs `DOS_ABORT`, restores `CurrentPDB`, clears termination state, loads `SS:SP` from the parent PDB user-stack field, restores world state, and executes `DOIRET`. | Original NTDOS guest teardown. | High for source order; medium for every runtime state value. |
| 3 | The retained trace later executes `C4 C4 06` at `073B:0740`; Bochs reports real-mode vector 06 with target `073B:0740`. | Guest IVT state selects a self-referential target; adapter has passed it through. | High |
| 4 | Each vector delivery consumes six bytes of `SS:SP`, from the observed parent-context `95AB:A0C9` down to `95AB:0001`. | Bochs architectural real-mode exception-frame delivery. | High |
| 5 | The next frame push wraps to `FFFFh`, fails the normal SS limit check and reaches third exception. | Bochs stack mechanics. | High |

## Exact unproven predecessor

The trace does **not** contain the instruction or write that installed (or
left installed) IVT vector 06 as `073B:0740`. The self-targeted vector is
therefore an observed guest-machine state, but its first writer is unproven.
Neither the source sequence nor the trace proves that parent-stack restoration
itself wrote the vector or made an invalid stack value.

The original SoftPC `illegal_op_int` handler is the correct historical owner
of selector `06h` after that vector is reached; its later linked machine island
is not evidence that it was reached in this normal-return trace. The adapter
is correctly selector-blind/pass-through here and must not manufacture a
vector, consume the BOP, or fix the stack.

## Result

The first unproven post-teardown input is the **provenance of the vector-06
guest state before the self-loop**, not a generic Bochs stack defect, a DEM
service failure, or CLI result transport. S2 may compare this with the
original SoftPC vector/handler installation contract and classify whether a
future diagnostic can observe the first writer without changing semantics.
