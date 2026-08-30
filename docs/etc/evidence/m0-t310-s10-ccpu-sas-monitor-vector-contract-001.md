# M0 T310 S10 — CCPU SAS monitor-vector contract review

## Original chain

The selected original CCPU profile installs memory access through
`sas_init -> SasSetPointers(&cSasPtrs) -> memcpy(&Sas, ...)` in
`base/ccpu386/ccpusas4.c`. `base/ccpu386/sascdef.c` supplies the matching
source-generated `cSasPtrs` initializer, while `base/cvidc/sas4gen.h` is the
selected `SasVector` callable contract.

## Disposition

- All ordinary RAM, move, fill, scratch, physical access and virtual-selector
  slots retain their generated `IU8`/`IU16`/`IU32`/native-pointer contracts and
  original CCPU providers. No adapter replaces a SAS provider.
- `Sas_overwrite_memory` remains the registered `MVDM-HOST-DIV-060` original
  unavailable slot. It is not silently enabled or converted into a monitor or
  Bochs operation.
- Guest physical addresses remain `IU32`; native `IU8 *` results stay inside
  the original SAS callable boundary. The existing mapping-manager seam is
  used only by the separately owned external physical binding path.

## x86/x64 evidence

Fresh incremental MSVC `/MT` x64 and x86 Ninja builds both passed. The bounded
original CCPU program verifies after `sas_init` that `Sas` contains the
original `cSasPtrs` RAM read/write and physical-address providers, that the
unavailable overwrite slot remains null, and that an ordinary vector-mediated
RAM store/read returns the original byte. Both executions then reached
`returned-recursive` through the original bounded CCPU path.
