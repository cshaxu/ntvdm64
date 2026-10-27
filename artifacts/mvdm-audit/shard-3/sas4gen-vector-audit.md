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

## Product-binary vector layout and noninteractive probe design

The selected PE32 `SasVector` has 58 pointer-sized fields. In the generated
x86 layout the zero-based indices / byte offsets are: `Sas_touch` 54 /
0xd8, `SasVirtualiseInstruction` 56 / 0xe0, and `IsPageInstanceData`
57 / 0xe4. `sas-vector-pe-probe.py` independently parses the exact r10
NTVDM PE sections, reads the `cSasPtrs` initializer using product map
symbols, verifies both first two slots point at a breakpoint instruction and
checks the third is zero. The two named functions are COMDAT-folded to the
same address (`0x5781e0` before PE relocation); `EDL_fast_bop` is distinct.
The recorded JSON is a **static binary proof**, not a runtime-use result.
The linked image's `sas_init` copies 58 DWORDs from that initializer into
`Sas`, and the vector binder publishes its address.

A one-pass dumpbin disassembly of the exact hash-matched NTVDM image found no
absolute instruction references to the three tail field addresses, no direct
call/jump to the merged fallback address, and no obvious register-indirect
call with displacement 0xd8/0xe0/0xe4. It found the `Sas` address in its
initializer copy and vector publication. This bounded textual/binary
observation does not eliminate two-stage indirect loads, computed indices,
other DLL consumers, alternate process images, or uncovered modes. The
large disassembly was temporary and removed; the PE probe and JSON are
retained.

A noninteractive **reachability** witness should reuse the existing private
worker/guest observer and `tests/observation/wow_sol_options_probe.c`'s
`DebugActiveProcess`/`WaitForDebugEvent` lifecycle, against one fully staged
and hash-pinned package. Attach only to the observer-owned `ntvdm.exe`;
handle the debugger's initial synthetic breakpoint separately; compare each
subsequent first-chance breakpoint against that process's relocated NTVDM
module base plus the map RVA. Log process/thread IDs, exception address,
x86 stack/return address, current image hash and workload, preserve the
existing exception disposition, detach and verify normal termination.
Because both tails fold to one address, a hit proves at least one tail was
entered, while per-tail attribution needs the return call site/arguments or
a separately compiled test-only vector wrapper. No hit proves only those
attached workload intervals. An attach after process creation cannot cover
early worker startup; do not report full-lifetime non-reachability without
an explicit pre-start attach mechanism. A test-only fixture that replaces
the two vector members with counting wrappers after `sas_init`, forwarding
to original tails only in a deliberately controlled negative case, can
prove vector dispatch and caller attribution without changing production or
guest; it remains supplemental to the actual-worker observer.

## Full field scan and metadata distinction

`sas-vector-fields.csv` indexes all 58 fields, their x86 offsets and the
first exact lexical `Sas.<field>`/`Sas-><field>` references across repository
C, header and assembly text. For the two breakpoint fields, references are
only generated header macro definitions; for `IsPageInstanceData`, the only
non-header direct field expression is the `#ifndef CCPU` YODA path already
noted. In selected CCPU builds the public generated `sas_touch` and
`VirtualiseInstruction` macros expand directly to the corresponding
`c_*` symbols; under non-CCPU PROD they use the vector members; other
non-CCPU builds declare ordinary functions. These preprocessor branches have
different reachability and should not be conflated.

The original reference `cvidc/c2cpusad.h` has a 55-name `SasVectorNames`
table, whose last entry `IOVirtualised` is index 54; the selected generated
x86 struct inserts `Sas_touch` at 54 and moves `IOVirtualised` to 55. This
is a metadata-layout mismatch worth tracking, not a demonstrated runtime
index bug. Repository source search finds no consumer of `SasVectorNames`;
`GenerateCvidcCpuBinding.mjs` reads only the first two metadata groups,
`CpuVectorNames` and `CpuPrivateVectorNames`, not this third SAS name list.
The checked NTVDM export list also does not expose names `Cpu`, `Sas`,
`c_sas_touch` or `c_VirtualiseInstruction`. Neither observation proves that
a pointer cannot escape through another API or a dynamic module.

## Isolated execution witness and its limit

The existing `tests/mvdm-host/ccpu_bounded_execution_fixture.c` was copied
to an isolated build-owned test source and changed by
`sas-vector-runtime-fixture.patch`. After original `sas_init` the fixture
saves the two actual vector entries, independently invokes both fallback
addresses in SEH as negative controls, installs counting wrappers that would
forward any vector call to the original breakpoint, and runs the original
fixture's bounded SAS scalar/string/move/fill, A20, and external physical
page mapping operations. Both controls raised `EXCEPTION_BREAKPOINT`, the
bounded portion exited zero and reported `touch=0 virtualise=0`; identities
and exact limitations are in `sas-vector-runtime-result.json`. The original
vectors were restored before SAS teardown. This does not detect calls that
bypass the vector via the CCPU direct macros, cover initialization before
wrapper install, run an actual guest application, or prove all paths safe.

An earlier version continued into the full original fixture and failed at
its unrelated timer/PIC INTACK check (`vector=-1`); that is recorded as a
non-pass and was not converted into a successful full fixture. The narrow
SAS witness stops before CPU/PIC initialization. Fixture links required the
existing `opennt-base-client.lib` plus a test-only `CsrPortHeap` declaration
and used the existing `/FORCE:multiple` test seams; it is not the shipped
NTVDM executable. Logs, binaries and derived complete source are disposable
under `build/t473-s10/shard-3-sas-vector/`; only the minimal test patch and
results are retained here. No product or guest file was modified.
