# T473 whole recovery stage plan

Owner approved this dependency-ordered plan. CURRENT alone admits the active
S; later entries are planned scopes, subject to subdivision or empty-scope
closure after the complete cross-audit. No source or capability may disappear
from the T objective through renumbering.

| Stage | Scope and exit |
| --- | --- |
| S9 | Complete generic USER identity, callbacks and lifetime audit; deliver and verify any repair. |
| S10 | Reconcile both DOS16 and WOW32 audits with host C/H sources, selected edition, actual build/dispatch and evidence; classify applicable files as full/partial/excluded and estimate mirror-diff retention. Other file types serve as dependencies or evidence, not individually classified scope. |
| S11 | Recover proven execution, scheduling, callback, identity and lifetime foundations. |
| S12 | Repair NTCON Console / Window input-source key lifecycles; [scope and acceptance](m0-t473-s12-ntcon-input-source-handoff.md). |
| S13 | Recover identified DOS core, startup and services (former S12). |
| S14 | Recover identified device, timing, input and presentation owners (former S13). |
| S15 | Recover identified WOW execution, task, conversion and cross-boundary services (former S14). |
| S16 | Recover identified USER, message, menu, control and dialog owners (former S15). |
| S17 | Recover identified GDI, resource and other system-service owners (former S16). |
| S18 | Complete remaining independently bounded owner packages from S10 (former S17). |
| S19 | Remove local providers and selections displaced by this T's original recoveries; report mirror and non-original footprint separately (former S18). |
| S20 | Re-audit host C/H source and composition, complete all required product and capability regressions, publish, commit and push for T acceptance (former S19). |

The owner also assigned package-scoped NTSRV isolation to S10. Its reviewed
candidate passed selected independent-package, DOS, native CUI, WOW and
control gates and was published as the coherent ten-image baseline; see
`artifacts/mvdm-audit/package-isolation-review.md`. This delivery does not
close the remaining S10 source audit or T-wide recovery and regression gates.

Each implementation stage covers original entry, initialization, behavior,
callbacks, failures and retirement, with original-source traceability and
necessary normal, negative and teardown tests. An unchanged previously
accepted capability can reuse applicable evidence; a file's presence or
source identity alone is not execution or behavioral proof. Preserve guest
immutability, approved host boundaries, and the user's minimum-diff policy.
Unrelated project-wide minimization remains in the next queue package.

The owner clarified that this T audits and restores connection boundaries
and their necessary closure, not independent internal defects in components
already connected. Historical findings below are leads rather than automatic
repair obligations: first establish an in-scope connection or an independently
admitted observed failure. This applies to every stage and supersedes older
unconditional repair wording. 486-specific repairs remain excluded.

S10 retains the detailed [recovery worklist](../../../artifacts/mvdm-audit/recovery-worklist.md)
and [stage handoff](../../../artifacts/mvdm-audit/s10-s11-s19-handoff-reconciliation.md).
The [C/H reconciliation checkpoint](../../../artifacts/mvdm-audit/s10-host-ch-reconciliation-summary.md)
records bounded source-role decisions and the evidence freshness limitations.
S11 retains correctness triage of the observed SOL Options failure before
assigning any proved DDE or USER defect to S15/S16. The owner moved further
SOL and WOW multi-window performance optimization to the separate
[queued proposal](../../proposals/proposal-wow-multiwindow-performance-recovery-001.md);
latency is no longer an S11 closure gate. S18 owns HUNTER and standalone tools/debuggers,
with CPU prerequisites in S11 and launch/PIF integration in S13. S20 rebinds
normal, failure and teardown evidence to the actual coherent release package.

The NIDDB instance-retirement and allocation-rollback candidates identified
during S10 are held outside this T after the owner clarified that internal
defects in connected components are not boundary recovery. Their [bounded
evidence](../../../artifacts/mvdm-audit/host-ch-scope/niddb-allocation-rollback-review.md)
and saved patches remain traceable; actual VxD provider reachability is a
separate original-connection question. The internal changes are not published.

S10's [native-source checkpoint](../../../artifacts/mvdm-audit/native-source-role-review.md)
retains per-file S11 execution/translation and S14 BIOS obligations without
accepting new runtime behavior. The separate
[slave-BIOS EOI source finding](../../../artifacts/mvdm-audit/shard-0/slave-bios-eoi-source-review.md)
belongs to S14 and still requires an actual PIC-state witness before repair;
it is not the already delivered PIC reinitialization correction. The held
SOL Options candidate failure remains an unclassified runtime invariant
failure until its cause is proved, rather than being assigned to a later
control stage merely from the application that triggers it.

S14 additionally owns the [proved VDD uninstall and DMA boundary defects](../../../artifacts/mvdm-audit/vdd-native-role-review.md), including all-slot ownership retirement and channel-8 rejection before array access. S10 records their strict counterexamples without changing the runtime.
S14 also owns the scoped [fixed-disk failure review](../../../artifacts/mvdm-audit/nine-way/part-8/fdisk-open-close-failure-review.md): establish actual-object failure propagation and one-time handle/count retirement with a disposable fixture before choosing an original-source change. The recorded source risks are not yet reproduced guest failures.

S16 owns the isolated [EDIT word-break buffer counterexample](../../../artifacts/mvdm-audit/shard-2/wordbreak-boundary-review.md); real guest reachability and the length contract remain to be verified.
The [selected host failure handoff](../../../artifacts/mvdm-audit/cross-369/selected-host-reproduced-failures-handoff.md) and [cross-shard handoff](../../../artifacts/mvdm-audit/cross-369/cross-shard-runtime-failures-handoff.md) record historical stage labels; their later-stage assignments now shift by one (S13–S16 become S14–S17). Each later stage must rebind its tested artifact and preserve the recorded distinction between controlled fixtures and live guest outcomes.
S15/S16 retain the [cursor/icon alias allocation and cleanup boundaries](../../../artifacts/mvdm-audit/cross-369/cursor-cache-disposition.md): both unchecked allocation sites and lock/mapping failure ownership require controlled failure and lifecycle witnesses. The separate [historical DDEML thunk review](../../../artifacts/mvdm-audit/cross-369/ddeml-thunk-disposition.md) leaves actual guest DDEML registration, peer interaction, callbacks and retirement in S15.
S15 additionally retains the [unselected DDEML thunk source risks](../../../artifacts/mvdm-audit/s10-followup-wow.md): guest mapping across a native call, full binding table and missing lookup returns. Prove actual guest/provider reachability before treating these as current-product defects or choosing an implementation.
S17 retains the [fax provider and notification boundary](../../../artifacts/mvdm-audit/s10-followup-fax.md): establish actual separate-driver applicability, then test mapping, peer failure, modal dispatch and cleanup. The original potential busy wait is a conditional source finding, not a reproduced current-product defect.
S13 owns the isolated [DOS label boundary](../../../artifacts/mvdm-audit/shard-1/dem-directory-review.md); guest reachability and safe volume semantics remain to be verified.
S13 additionally retains the [FCB deletion failure branches](../../../artifacts/mvdm-audit/nine-way/part-8/dem-fcb-delete-review.md) as unconfirmed source risks. Exercise the actual DEM object with controlled attribute and enumeration failures before deciding whether a mirror exception is justified.

S13 also owns the [FCB open metadata failure review](../../../artifacts/mvdm-audit/nine-way/part-8/dem-fcb-open-review.md): validate acquired-handle publication and retirement through the actual selected DEM object before deciding a mirror exception.

S11/S14 own the [CCPU and C-VID VideoVector layout boundary](../../../artifacts/mvdm-audit/host-ch-scope/ccpu-evidgen-review.md): establish actual cross-object accesses, ownership and ABI before any repair; different declarations alone do not prove an executed defect.

S11/S14 also retain the [B067/B068 BIOS fallback and enhanced-mode selector boundary](../../../artifacts/mvdm-audit/s11-foundation-evidence/b068-selected-boundary/review.md). S11 owns selected execution identity; S14 owns device, provider and reset evidence, with S15 for any demonstrated Win16 callback effects. The owner's exclusion of unrelated CCPU-internal and 486 repairs remains in force; a conditional source finding is not a guest failure or permission to rewrite it.
