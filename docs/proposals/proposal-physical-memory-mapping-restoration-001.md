# Original physical-memory mapping contract audit and restoration

## Owner request and disposition

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

## Proposed stages

### S1 - Original owner and complete mapping contract

Inventory actual callers, exports, state ownership and selected build paths,
then trace each operation to its original source owner. Establish whether
EMS, XMS/DPMI, SAS or other selected consumers require each behavior; do not
infer requirements from a function name. Compare original source, existing
bindings and focused observations before selecting a restoration design.

Explicitly settle: exact replacement versus partial overlap and precedence;
one-hop versus chained alias resolution and cycles if applicable; source and
destination bounds and page padding; backing availability at publication;
exact versus partial unmap; backing retirement; publication failure and
rollback; and teardown with live aliases or checked synchronous accesses.
The current prepend/first-match, single-hop and exact-span policies are
observations, not predetermined defects to reverse.

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
