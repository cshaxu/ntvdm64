# Original physical-memory mapping contract audit and restoration

## Owner request and disposition

Admitted as M0 T406 on 2026-09-12; removed from candidate Queue. Only S1
research/design is active in Status. The owner requires its findings and
reduction design before approving repairs. See the
[S1 research/design](../etc/evidence/m0-t406-s1-physical-mapping-design.md).
Earlier candidate language below is retained as proposal history.

On 2026-09-12 the owner approved arranging an independent candidate T for
U02 at the head of [Queue](../states/QUEUE.md), before DPMI/XMS restoration.
This registers the package, not a second active task or numeric T allocation.
The current [Status](../states/CURRENT.md) remains authoritative. At execution
admission reconcile the U02 audit disposition with this package, freeze the
actual baseline and create the sole active S brief.

## Objective and source boundary

Resolve and restore the original contract behind the current
`mvdm_softpc_physical_mapping.c` prepare/set, map/unmap, translate/resolve
and cancellation paths. U02 is an unresolved semantic family, not a proved
duplicate implementation or a proved cause of DOSX/WRITE failure. The
[source audit](../etc/evidence/opennt-replacement-audit-20260912.md#s3-physical-mapping-policy-disposition)
records current behavior; it does not establish the required original result.

Apply the [source policy](../etc/operations/policy/source-policy.md) and
[architecture rules](../rules/ARCHITECTURE.md): original MVDM algorithms stay
in `mvdm-host`; separately admitted non-MVDM bodies belong to `opennt-host`
at original-relative paths. Bind only the proved unavailable mechanics.
Do not mistake this memory-address service for the retired generic x64
identity manager. No generic mapping/token manager, kernel shell, ring-0
mapping, CPU30 build, instruction-core change or broker implementation is
authorized by this proposal. The execution profile remains x86 CCPU40.

## Source-backed direction approved on 2026-09-12

The owner approved refining this proposal after a read-only follow-up. The
restoration has two concrete consumer paths, not a generic mapping redesign:

- EMS page windows: original `softpc.new/base/dos/emm_mngr.c` calls
  `host/src/nt_emm.c:host_map_page/host_unmap_page`, which call
  `VdmMapDosMemory/VdmUnmapDosMemory`. Preserve that original upper policy.
- External DIB storage: original `wow32/wdib.c` conditionally calls
  `host/src/nt_mem.c:VdmAddVirtualMemory` for host-allocated pixel storage.
  Guest and GDI access must share the same content; a detached copy is not
  equivalent. Determine actual per-file `i386` selection and runtime
  reachability before attributing a product failure to this route.

Current `VdmAddVirtualMemory` requires a prior
`mvdm_softpc_physical_mapping_publish` record. The follow-up search of src,
tests and tools found the explicit publication call in
`tests/mvdm-host/ccpu_bounded_execution_fixture.c`, but no production caller;
the original-shaped WOW calls still pass `pvBits` directly. This is a
source-visible integration gap to verify first, conditional on actual branch
selection, not a demonstrated WRITE root cause. The existing fixture supplies
the extra registration itself and therefore cannot prove original-caller
compatibility. If the route is selected, preserve the original complete entry
contract rather than teaching original callers a new registration protocol.

The original upper allocation/removal and EMS caller bodies are available.
The bounded source searches did not locate definitions of VdmMapDosMemory,
VdmUnmapDosMemory or VdmSetPhysRecStructs in the searched OpenNT source, nor
matching definitions in the searched OpenNT-4.5 MVDM sources. Original
nt_mem.c describes updating PhysicalPageREC.translation. These observations
do not prove source loss or a kernel owner: inspect original build/generated
inputs and available artifacts before claiming a missing implementation.
The expected direction is original upper policy plus a minimal proved CCPU
page binding, not a promise to delete the entire adapter or import a kernel
implementation that has not been identified.

Current `ccpu386/ccpusas4.c:c_GetPhyAdd` checks alias translation and external
backing before ordinary RAM address calculation. A successful direct byte
access proves only that path. Audit CPU, SAS and bulk/cross-boundary consumers
for consistent backing selection. After removal, an unresolved external
lookup can fall through to ordinary RAM; establish required address and
alias retirement behavior rather than assuming that fallback is correct.

## Proposed stages

### S1 - Original owner and complete mapping contract

Inventory actual callers, exports, state ownership and selected build paths,
then trace each operation to its original source owner. Establish whether
EMS, XMS/DPMI, SAS or other selected consumers require each behavior; do not
infer requirements from a function name. Compare original source, existing
bindings and focused observations before selecting a restoration design.

Prioritize the DIB entry/publication gap and EMS page-window contract, then
the common access and backing-lifetime boundary. Record a deterministic
original-shaped caller test which does not manually pre-publish backing.
Freeze the selected DIB compile branch, original owner evidence and expected
failure behavior before implementation. Inactive paths are classified
explicitly, not counted as currently exercised failures.

Explicitly settle: exact replacement versus partial overlap and precedence;
one-hop versus chained alias resolution and cycles if applicable; source and
destination bounds and page padding; backing availability at publication;
exact versus partial unmap; backing retirement; publication failure and
rollback; and teardown with live aliases or checked synchronous accesses.
The current prepend/first-match, single-hop and exact-span policies are
observations, not predetermined defects to reverse.

Alias chains, arbitrary overlaps and partial unmap are research cases, not
new feature requirements. Implement only source-proven consumer contracts;
do not generalize the mapping service to satisfy an invented test matrix.

Record each original function/path/hash, retained layout/order/failure rule,
finite outgoing dependency and current-provider disposition. Unknown matches
remain unknown. Report findings for owner review before semantic replacement;
an unproved contract does not authorize an invented replacement manager.

### S2 - Source-shaped restoration and integrated verification

Implement the reviewed original-owner route, retaining only demonstrated
modern bindings and deleting superseded policy together with affected caller
hooks. Preserve names and source shape wherever composable. If a current
behavior is proved necessary and equivalent, retain it with exact evidence
instead of changing it solely to reduce line count. A blocked dependency must
receive owner disposition, not a success-returning placeholder.

Implementation and acceptance order within this S is:

1. Restore the selected original external-memory entry contract, including
   non-DWORD-aligned input, allocation failure and paired removal, without
   requiring a project-specific publication step from the original caller.
   Prove bidirectional visibility between guest access and host DIB storage
   when that route is selected; a blocked WOW workload needs explicit scope
   disposition, not an inferred passing DIB result from COMMAND/EDIT.
2. Prove EMS map/switch/unmap/remap and two windows sharing one backing,
   including preserved data when switching away and back. Do not rewrite
   the original EMS manager merely to accommodate a new adapter policy.
3. Verify consistent CPU/SAS/bulk/cross-page reads and writes, bounds and
   page padding, backing retirement, failure rollback and teardown. A pointer
   fixture alone is not complete access-path evidence. Define who owns and
   frees the original backing and reject stale use according to that contract.
4. Resolve only additionally reached overlap/unmap/alias cases; keep
   unsupported or inapplicable cases source-backed and explicit.

Cover map/read/write alias visibility, repeat mapping, overlapping spans,
exact/partial unmap, chains, missing or retired backing, bounds/padding,
publication failure and cleanup with focused positive/negative tests against
the selected original contract. Inapplicable cases require a source-backed
explanation. Verify affected consumers in the formal x86 CCPU40 graph and
regress COMMAND/EDIT plus relevant EMS/XMS/DPMI workloads identified in S1.
Record the DOSX/WRITE frontier without treating full WRITE execution as this
package's exit condition. Check the reached U07 access-lifetime boundary;
unrelated session redesign and U03 cache-selection policy remain out of scope.

## Sequencing and closure

This package precedes the
[DPMI/XMS candidate](proposal-dpmi-xms-contract-restoration-001.md) to provide
a tested mapping contract; it does not replace that package's allocator or
transition work. Keep broker restructuring independent. A discovered direct
dependency is named and reconciled explicitly rather than expanding into a
repository-wide audit or silently starting another active S.

Retain source identities, actual provider selection, contract tests, runtime
baseline comparisons and separate counts for removed autonomous code,
reverted mirror diff, added original source and necessary binding code.
Build/fixture outputs stay under `build/`; only verified formal publication
is copied to `O:\ntvdm64\ntvdm32.exe`, with observations under
`O:\ntvdm64\logs\`. Review, governance verification, commit and push follow
[Execution](../rules/EXECUTION.md). S1 research alone does not close the T:
the reviewed restoration or proved-equivalent retention and S2 verification
must be complete, with mandatory unresolved contracts explicitly blocking
closure unless the owner changes the scope.
