# Original DOS file-service restoration

## Owner request and disposition

On 2026-09-12 the owner requested a separate candidate T package containing
two sequential S stages, placed first in [Queue](../states/QUEUE.md). This is
planning approval only: no numeric T is allocated and the active
[Status](../states/CURRENT.md) packet is unchanged. This proposal carries the
D31-D33 and D08 restoration scope from the
[T405 audit](../etc/evidence/opennt-replacement-audit-20260912.md); admission
must reconcile those rows so they are not implemented twice under T405.

## Objective and boundary

Recover original DOS file-handle and fast-read contracts in the existing
Win32/x86 CCPU40 worker, minimizing duplicate algorithms in adapters and
overlays. Keep the public entry `ntvdm32.exe <binary>` unchanged. Broker,
BaseSrv queue policy, process splitting, CPU execution changes and complete
WRITE recovery are not prerequisites or deliverables of this package.

Original DEM/VDD logic remains in `mvdm-host/dos/dem/demfile.c`. Any admitted
non-MVDM `base/ntos/vdm/x86/rdwr.c` semantic slice belongs under `opennt-host`
at its original-relative path, following the
[source policy](../etc/operations/policy/source-policy.md) and
[architecture rules](../rules/ARCHITECTURE.md). Adapters bind finite missing
mechanics; they do not own a rewritten file-service algorithm. Kernel trap
installation, ring-0 execution, CSR and the retired CPU30 profile are excluded.

## Planned stages

These are the two proposed S scopes within one future T, not active packets.
At admission, materialize the sequential working plan under `etc/operations/`
and place only the current S brief in Status.

### S1 - Original DEM/VDD handle lifecycle (D31-D33)

Restore original allocation, association, retrieval and release as one
coherent lifecycle, including `GetFreeSftEntry`, `VDDAllocateDosHandle`,
`VDDAssociateNtHandle`, `VDDRetrieveNtHandle` and `VDDReleaseDosHandle`.
Review the complete SFT/JFT access boundary and affected WOW32 `wkfileio.c`
callers before replacing `mvdm_vdd_sft_shadow.c` policy. Remove shadow-specific
caller commits together with the superseded provider, not in isolation.

Preserve original optional outputs, field ownership, reservation visibility,
reference counts and error ordering through checked synchronous guest access.
Do not replace snapshots with unbounded durable guest pointers or a new
generic shadow/transaction manager. Resolve the reached U07 memory-lifetime
and publication requirements locally; unrelated U07 families remain outside
this package.

Acceptance covers allocation/exhaustion/reuse, optional NULL outputs,
association immediately visible through the original API, invalid handles,
retrieval/release/reference counts, unrelated JFT changes not overwritten,
and acquisition/publication failures without unexplained partial updates.
Exercise affected VDD/WOW contracts through the selected x86 CCPU40 graph,
then regress ordinary COMMAND and EDIT file operations. Record runtime
reachability limitations rather than claiming all third-party VDDs tested.

### S2 - Source-shaped fast-read recovery (D08)

First audit the full original `NTFastDOSIO` boundary and attempt its finite
user-mode composition through the smallest same-shaped bindings. Preserve
source-owned handle eligibility, seek/no-seek, offset interpretation,
completion/EOF handling, returned byte count and CF fallback. Keep kernel
trap-frame/VDM-TIB mechanics outside the CCPU40 dispatch; prove instruction
advancement occurs exactly once. Retain the explicit fast-write refusal and
original slow-write fallback; implementing fast write is not in scope.

Replace the autonomous `demfastio.c` fast-read policy with the selected
original-owner route. If faithful finite reuse cannot compose, record the
exact dependency and seek owner disposition before selecting the original
CF-to-DEMREAD fallback as the final product policy. A compiled lookalike or
a claim of improved speed is not original-source recovery evidence.

Audit the shared ordinary `demRead` memory binding as well: successful host
I/O followed by failed guest writeback must not silently trigger an unsafe
second read. Cover file-position side effects, untouched buffer tails and
the reached U07 lifetime contract; disabling fast read alone does not settle
these issues.

Acceptance covers zero/short reads, EOF, seek/no-seek and large offsets,
standard/pseudo/non-disk handles, invalid guest ranges, pending completion
where applicable, failure after host I/O, CF fallback and byte-for-byte
results/file positions against the selected original contract. Regress
COMMAND/EDIT file workloads and record the same-baseline DOSX/WRITE loading
frontier without asserting that D08 is its root cause or that WRITE runs.

## Delivery and exit evidence

For each stage retain source identity and exact owner, outgoing dependency
dispositions, selected build/provider evidence, focused positive and negative
tests, and reviewed before/after diff plus overlay footprint. Revalidate the
implementation baseline at admission; static T405 evidence is not runtime
acceptance. An unresolved mandatory contract blocks closure rather than
becoming a successful unsupported row by omission.

Build and fixture products stay under the admitted `build/` run root. Publish
only a verified formal x86 executable to `build/output/ntvdm32.exe`, with a
deliberate test copy to `O:\ntvdm64\ntvdm32.exe`; runtime observations belong
under `O:\ntvdm64\logs\`. Each admitted S closes through review, governance
checks, commit and push under [Execution](../rules/EXECUTION.md). The T closes
only after both stages meet their approved contracts and superseded duplicate
providers/caller hooks have explicit removal or bounded retention evidence.
