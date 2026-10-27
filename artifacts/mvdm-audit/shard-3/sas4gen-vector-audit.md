# SAS x86 generated vector and unavailable tails

## Selection and source identities

The shard owns `softpc.new/base/cvidc/sas4gen.h`, a byte-identical
original reference whose SHA-256 is
`ECDA9CC4909EC3B945F90441A1EC441ADAF807DCF6F419523C80C334E87067A0`.
The x86 worker include ordering instead picks the separately retained
`src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h` (SHA-256
`ABC942914E4EA58F17D4549C3FCD6753500B012B83056B33B285C4DCEE79008B`).
Ninja VALID dependency data records that selected generated header for 84
selected MVDM worker source objects; it does not record the original C-VID
header path for those sources. The generated x86 file adds `sas_touch`,
`VirtualiseInstruction`, `IsPageInstanceData` declarations and vector slots
to the original C-VID reference. These alter the shared `SasVector` layout,
so independently switching one compilation unit to the original header is
unsafe. Prior T411 and `opennt-replacement-audit-20260912.md` record this
NTVDMx64-generated carrier and its provenance; they do not establish that
the appended runtime slots have working implementations.

## Concrete lifecycle and linkage

Original `ccpu386/sascdef.c` declares `c_sas_touch` and
`c_VirtualiseInstruction` and places both into `cSasPtrs`. Both pinned
OpenNT and OpenNT-4.5 CCPU sources retain these exact declarations and
initializers. Original `ccpusas4.c::sas_init` calls `SasSetPointers`, which
copies the `cSasPtrs` record to `Sas` using `sizeof(struct SasVector)`.
The existing C-VID binder publishes `Cpu.Sas = &Sas` and binds the independent
`Sas_overwrite_memory` slot to the real no-cache-invalidation CCPU leaf.
Thus two questionable function pointers are published to the shared vector;
they are not only unused textual declarations. The added final
`IsPageInstanceData` vector member is omitted from the CCPU initializer and
therefore zero-initialized.

The production x86 `sascdef.obj` relocation table references both actual
fallback symbols. `ntvdm.exe.map` locates both in
`softpc-ccpu-vector-defaults:fmstubs_vector_defaults.obj`; disassembly of
that object confirms each executes `int 3`, followed by a zero/NULL return
if a debugger or exception handler resumes it. This is not a normal
implementation and must not be called a success stub. The retained NTVDMx64
`ntvdmpatch/patches/common/fmstubs.c` has the same `int 3`/zero convention;
that identifies provenance but does not prove original OpenNT capability.

## Reachability evidence and uncertainty

A search of current `src/mvdm` and `src/ntvdm-exe` text for both function
names found CCPU vector initialization, selected fallback definitions and
generated declarations, but no direct C/ASM call site. The analogous search
of both pinned OpenNT MVDM trees found the CCPU vector entries but no
provider bodies/call sites outside generated header variants. This is
source-search evidence, not proof against numeric vector indexing, an
indirect function pointer, generated code, assembly, another component or
a less obvious macro spelling. The known `IsPageInstanceData` consumer in
original `base/debug/yoda.c` is under `#ifndef CCPU`; it is not evidence of
a reachable selected-CCPU call.

The S9 r10 product and guest tests establish ordinary workloads function;
they do not contain an observation that counts or traps these two vector
slots. The existing selected fallback preserves an explicit breakpoint if
reached; the eventual exception outcome depends on the debugger/exception
handler, so process termination is not asserted without a runtime witness.

## Minimum credible recovery

1. Preserve the coherent current vector/layout and original CCPU code during
   diagnosis. Inventory all code that receives `Cpu.Sas` or accesses the
   SAS vector, including generated/numeric accesses and selected binaries;
   use actual object references and a test-only trap witness to prove direct
   or indirect calls and initialization/teardown, not only `rg`.
2. If a supported operation reaches either tail, trace that operation back to
   a documented original provider/contract and search pinned matching
   sources. Reuse an exact compatible provider if found; otherwise design the
   smallest host-resource adapter with explicit behavior, failures and
   lifetime. A zero/NULL success substitute is unacceptable.
3. If complete consumer and runtime evidence proves the tails unreachable
   for the accepted CPU40 profile, retain their explicit failure boundary or
   consider removing them only as an atomic generated-header/vector/initializer
   ABI simplification with a demonstrated line reduction and full regression.
   No deletion is justified yet, and reachable behavior cannot be silently
   excluded just because ordinary applications have passed.

The original reference header remains **pending** under the T-wide
source/capability reconciliation. No product, guest or installed file was
modified by this audit.
