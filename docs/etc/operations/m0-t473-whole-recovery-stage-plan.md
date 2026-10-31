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
| S13 | Investigate and repair direct `run16 winmine` black background and excess window width; see the bounded scope below. |
| S14 | Recover identified DOS core, startup and services (previously S13). |
| S15 | Recover identified device, timing, input and presentation owners (previously S14). |
| S16 | Recover identified WOW execution, task, conversion and cross-boundary services (previously S15). |
| S17 | Recover identified USER, message, menu, control and dialog owners (previously S16). |
| S18 | Recover identified GDI, resource and other system-service owners (previously S17). |
| S19 | Complete remaining independently bounded owner packages from S10 (previously S18). |
| S20 | Remove local providers and selections displaced by this T's original recoveries; report mirror and non-original footprint separately (previously S19). |
| S21 | Re-audit host C/H source and composition, complete all required product and capability regressions, publish, commit and push for T acceptance (previously S20). |

### S13: WINMINE background and window geometry

Owner reports that direct `run16 winmine` produces black background areas and
excess width. Audit these independently, including their combined effect; do
not special-case WINMINE or alter guest binaries. Original WINMINE requests a
gray class brush. The current worker GDI alias allocator can return small
handles in the same range that `GETWNDCLASS16` interprets as system colors.
This is a proven code-level ambiguity; the brush and alias actually used in
the reported run remain unobserved. Trace registration, erase, painting and
bitmap/DC ownership before attributing the black pixels. Consider reserving
the conflicting alias range only after evaluating original handle semantics,
capacity, reuse, system-color values and lifecycle.

In one recorded 8×8 run the same HWND was requested at 154×240 and observed
at 237×240: `build/t473-s11/geometry-input-diagnostic/run/windows.txt` and
`build/t473-s11/geometry-input-diagnostic/run/probe.txt` respectively.
Another run observed the expanded width at creation; these are separate
runs. Capture requested and actual dimensions, DPI, system metrics,
`WM_GETMINMAXINFO` and `WM_WINDOWPOSCHANGING` inputs and outputs in one
isolated run. Trace original USER sizing and restore the demonstrated owner;
do not hard-code a window width or discard valid minimum-size semantics.

Exit requires real Win16 before/after witnesses for background, full client
and board pixels, dimensions and redraw, including difficulty changes,
occlusion, minimize/restore, gameplay and restart; test actual GDI aliases,
system-color pseudo-brushes, reuse, failure and lifecycle, plus another Win16
window and applicable DOS/WOW/Win3.x regressions. Prior three-cell readiness
and interaction checks alone do not certify complete painting. Record
uncovered paths, both mirror and adapter diffs, coherent cached build, Full
gates, ten-component publication, hashes, commit and push.

**Stage-number translation:** reports and worklists written before this
insertion retain their historical labels. References to then-current
S13–S20 now mean S14–S21 respectively; S11 and the NTCON S12 are unchanged.
Older source-role ledgers may predate the earlier NTCON insertion as well:
their original S12–S19 scopes map to current S14–S21. Apply these mappings
by scope and document date; never renumber historical evidence in place or
mistake a historical S13 for the new WINMINE S13. All inherited obligations
remain in their original owner scopes.

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
assigning any proved DDE or USER defect to S16/S17. The owner moved further
SOL and WOW multi-window performance optimization to the separate
[queued proposal](../../proposals/proposal-wow-multiwindow-performance-recovery-001.md);
latency is no longer an S11 closure gate. S19 owns HUNTER and standalone tools/debuggers,
with CPU prerequisites in S11 and launch/PIF integration in S14. S21 rebinds
normal, failure and teardown evidence to the actual coherent release package.
S19's debugger scope includes the selected original WOW remote-entry path:
the [S11 handoff](../../../artifacts/mvdm-audit/s11-debug-context-candidate.md)
records its held first-entry experiment, session and execution ownership,
context restoration, failure and teardown requirements. S11's ordinary WOW
task debug-context handoff does not claim that remote path is connected.

The NIDDB instance-retirement and allocation-rollback candidates identified
during S10 are held outside this T after the owner clarified that internal
defects in connected components are not boundary recovery. Their [bounded
evidence](../../../artifacts/mvdm-audit/host-ch-scope/niddb-allocation-rollback-review.md)
and saved patches remain traceable; actual VxD provider reachability is a
separate original-connection question. The internal changes are not published.

S10's [native-source checkpoint](../../../artifacts/mvdm-audit/native-source-role-review.md)
retains per-file S11 execution/translation and S15 BIOS obligations without
accepting new runtime behavior. The separate
[slave-BIOS EOI source finding](../../../artifacts/mvdm-audit/shard-0/slave-bios-eoi-source-review.md)
belongs to S15 and still requires an actual PIC-state witness before repair;
it is not the already delivered PIC reinitialization correction. The held
SOL Options candidate failure remains an unclassified runtime invariant
failure until its cause is proved, rather than being assigned to a later
control stage merely from the application that triggers it.

S15 additionally owns the [proved VDD uninstall and DMA boundary defects](../../../artifacts/mvdm-audit/vdd-native-role-review.md), including all-slot ownership retirement and channel-8 rejection before array access. S10 records their strict counterexamples without changing the runtime.
S15 also owns the scoped [fixed-disk failure review](../../../artifacts/mvdm-audit/nine-way/part-8/fdisk-open-close-failure-review.md): establish actual-object failure propagation and one-time handle/count retirement with a disposable fixture before choosing an original-source change. The recorded source risks are not yet reproduced guest failures.

S17 owns the isolated [EDIT word-break buffer counterexample](../../../artifacts/mvdm-audit/shard-2/wordbreak-boundary-review.md); real guest reachability and the length contract remain to be verified.
The [selected host failure handoff](../../../artifacts/mvdm-audit/cross-369/selected-host-reproduced-failures-handoff.md) and [cross-shard handoff](../../../artifacts/mvdm-audit/cross-369/cross-shard-runtime-failures-handoff.md) record historical stage labels; their pre-NTCON S13–S16 scopes now map to current S15–S18. Each later stage must rebind its tested artifact and preserve the recorded distinction between controlled fixtures and live guest outcomes.
S16/S17 retain the [cursor/icon alias allocation and cleanup boundaries](../../../artifacts/mvdm-audit/cross-369/cursor-cache-disposition.md): both unchecked allocation sites and lock/mapping failure ownership require controlled failure and lifecycle witnesses. The separate [historical DDEML thunk review](../../../artifacts/mvdm-audit/cross-369/ddeml-thunk-disposition.md) leaves actual guest DDEML registration, peer interaction, callbacks and retirement in S16.
S16 additionally retains the [unselected DDEML thunk source risks](../../../artifacts/mvdm-audit/s10-followup-wow.md): guest mapping across a native call, full binding table and missing lookup returns. Prove actual guest/provider reachability before treating these as current-product defects or choosing an implementation.
S18 retains the [fax provider and notification boundary](../../../artifacts/mvdm-audit/s10-followup-fax.md): establish actual separate-driver applicability, then test mapping, peer failure, modal dispatch and cleanup. The original potential busy wait is a conditional source finding, not a reproduced current-product defect.
S14 owns the isolated [DOS label boundary](../../../artifacts/mvdm-audit/shard-1/dem-directory-review.md); guest reachability and safe volume semantics remain to be verified.
S14 additionally retains the [FCB deletion failure branches](../../../artifacts/mvdm-audit/nine-way/part-8/dem-fcb-delete-review.md) as unconfirmed source risks. Exercise the actual DEM object with controlled attribute and enumeration failures before deciding whether a mirror exception is justified.

S14 also owns the [FCB open metadata failure review](../../../artifacts/mvdm-audit/nine-way/part-8/dem-fcb-open-review.md): validate acquired-handle publication and retirement through the actual selected DEM object before deciding a mirror exception.

S11/S15 own the [CCPU and C-VID VideoVector layout boundary](../../../artifacts/mvdm-audit/host-ch-scope/ccpu-evidgen-review.md): establish actual cross-object accesses, ownership and ABI before any repair; different declarations alone do not prove an executed defect.

S11/S15 also retain the [B067/B068 BIOS fallback and enhanced-mode selector boundary](../../../artifacts/mvdm-audit/s11-foundation-evidence/b068-selected-boundary/review.md). S11 owns selected execution identity; S15 owns device, provider and reset evidence, with S16 for any demonstrated Win16 callback effects. The owner's exclusion of unrelated CCPU-internal and 486 repairs remains in force; a conditional source finding is not a guest failure or permission to rewrite it.

S15 also owns the [printer fast-path boundary witness](../../../artifacts/mvdm-audit/s11-monitor-printer-boundary.md): prove actual direct-I/O reachability and the selected TIB buffer/flush contract before choosing a repair. S11 scalar-port tests alone do not prove this device-specific path.
