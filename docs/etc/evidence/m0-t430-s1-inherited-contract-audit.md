# T430 S1 inherited CCPU40/V86 contract audit

## Question, inputs and procedure

Owner admits the queued audit and requests reusing the other session's
build-directory results. Initial review is source/document-only; no product
execution, process termination, production modification or deployment.
Accepted baseline is5b9931b8e/T429 S8, owner closure930a0f2d8.

Located build/research-ccpu40-v86-guest-contract-20260929. Read audit-snapshot.json,
audit-delivery.md, proposal-compliance-20260930.md, unmet-boundary-register-20260930.md,
gap-handoff-matrix-20260930.md, real-gap-triage.md and saved identity headers.
The research directory is retained unchanged, not a build dependency.

## Initial source conclusions

| Inherited material | Reuse decision and limitation |
| --- | --- |
| G01–G17 contract families and guest/receiver tables | Reuse stable IDs and finite owner boundaries; validate source hashes and actual selection before adopting conclusions. |
| Claimed11,817 aggregate keys,3,673 guest edges,23 host ABI and15 control boundaries | Historical static census, not runtime equivalence. Verify CSV consistency independently; no global rescan merely to reproduce counts. |
| September29 snapshot99c458b4b and September30 graph/media updates | Different package from T429. Root-level EXEs, ntkvm names and observer startup stages require current owner/path mapping. |
| H01/H02 launch-profile errors5/87 | Historical environment observations, not current CCPU/DOSX defects. Accepted matrices supersede general claims that DOS cannot start; exact old profiles remain separate. |
| K01 directory-query fallback | Missing VdmQueryDir binding/slow DEM fallback claim requires current wiring review. No ordinary FCB failure proved. |
| K02–K04 redirector capacity, async completion and CD-name copies | Source-risk hypotheses; recheck implementation and targeted negatives, not already reproduced product failures. |
| G01 original COMMAND environment defect | Preserve immutable-guest TODO disposition, not CPU/adapter repair. |
| O01 SOUND table limitation | Conditional original-source limit; do not infer WINMINE currently fails or used these ordinals. |
| W01 WRITE initialization OOM | Historical aggregation/frontier, not first failing provider. Retain investigation owner; no CPU/heap/WOW attribution from dialog text. |

The prior compliance report itself says the candidate is not closed. Static
catalogs are useful, but candidates and receivers do not prove actual calls,
returns, failure or cleanup. Passed workloads cover only their actual
contracts and unchanged inputs, not every unobserved interface family.

## Current identity checks — r001/r002

The reproducible read-only verifier is
`tools/audit/Verify-InheritedGuestContractEvidence.ps1`; run from repository
root with `powershell -NoProfile -ExecutionPolicy Bypass -File
tools/audit/Verify-InheritedGuestContractEvidence.ps1 -OutputRoot
build/M0-T430/S1/r002`. Select a fresh run root for repetition. No product
startup, cleanup, inherited script execution or old report overwrite occurs.

| Check | Actual result |
| --- | --- |
| Aggregate tables |11,817 unique family/interface keys;3,673 unique guest edge keys;23 ABI symbols;15 broker keys;17 unique exact G01–G17 IDs. Historical claims confirmed, not runtime proof. |
| Source identity |455 inherited records represent452 unique paths:373 records same,25 changed,57 missing old paths. Results in source-identity.csv; missing does not prove missing behavior. |
| Original selection |All18 original source-group arrays equal the validated current formal cache M0-T427/S2/r001/source-manifest.json, with no added/removed entries. Current x86 CCPU40 and130 CCPU sources retained; selection is not code equivalence. |
| Media candidates |56 old-path/system32 candidates:36 unchanged,19 absent,1 changed (root mysmb16.exe). Candidate counts are not distinct guest count. Names/locations alone never prove unchanged guest. |
| Accepted host publication |All8 actual O:/winnt/system32 hashes match the accepted S8/r011 manifest. No deployment or runtime performed. |
| Audit mechanics |PowerShell parse and positive verifier pass. Out-of-build output and reuse of sealed output are rejected before writes. Inherited inputs are SHA256-pinned in r002/inherited-inputs.csv. |

57 missing records are5 interface,11 old ntcon,38 ntkvm,1 NTSRV,
1 run16 and1 worker-base path. Prior naming/common moves explain why path
existence alone is insufficient; each relevant boundary must be mapped using
history/current production ownership, not basename matching.

The38 inherited media rows have36 unchanged mapped old paths. Of the two
unmatched paths, mysmb16.exe differs; the old system32/WRITE.EXE is absent.
An explicit follow-up hash check finds O:/winnt/WRITE.EXE exactly matches that
old WRITE hash08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B.
Thus this WRITE path is a proven relocation, not changed media. Actual runtime
image selection is still separate from static file identity.

Current source review independently confirms K01's binding has definitions
but no production caller in src/ntvdm-exe; NtVdmControl returns unsupported
without it. Original demsrch.c::FileFindReset explicitly falls back to slow
enumeration. K02's vrnetapi.c::VrGetUserName still compares Unicode wcslen
against CX-1 before OEM copying. These are retained source observations,
not new guest failure reproductions. K03/K04, actual source-drift semantics,
media provenance and reached-contract/test mapping remain pending.

Governance and relative links pass after admission. Unrelated proposal hash
AE9DB32FD31A87EFC07FF895DF66CB3FFDFD17ECBED6B1FBE4391152A2ADF9AB is
preserved and excluded. S1 is active; these results do not close it.

## Next bounded reconciliation

Freeze current source/build/media identities, verify CSV schemas/uniqueness,
isolate changed/missing inputs and connect the selected boundary to existing
tests/evidence. Explicit unreached items are not fabricated profile exclusions.
Do not rerun scripts in place: they may overwrite sealed evidence, hard-code
old paths or invoke guests. Review them first; fresh reports belong below
build/M0-T430/S1. This is initial review, not S1 closure or runtime proof.

## Superseding S1 conclusion — 2026-10-04

Owner now requests S1 closure and subsequent non-WOW completion, with known
WOW gaps assigned to the queued WOW32 owners. The paragraphs above retain
the initial findings; this section supersedes their pending-S1 status.
S1 closes a bounded audit, not functional equivalence or repair.

Replayed the verifier into fresh build/M0-T430/S1/r003. All census, selected
group and accepted publication checks retain the r002 results. The new
boundary-reconciliation.csv assigns all25 changed and57 missing records to
existing current receivers and pins their current hashes. These82 rows
include duplicate inherited records; a boundary successor is not a claim
that the old file body was simply renamed or its behavior is equivalent.
No product was launched, rebuilt, stopped or redeployed.

### Source-drift disposition

Read `git diff --name-status -M 99c458b4b HEAD -- src` and bounded body diffs
for c_main, keyba, COMMAND, DEM and OEM directory wrappers. Naming/common
migration is supported by T424 S7–S12; current system roots by T427; latest
publisher/input/runtime by T429. Existing evidence remains tied to its exact
inputs, not retrospectively assigned to the September snapshot.

| Drift group | Current ownership and disposition |
| --- | --- |
|5 interface records | common/protocol and common/codec; versioned declarations retained, not missing services. |
|38 old ntkvm records | NTCON frontend/renderer; deleted native control/bootstrap wrappers now common RPC/NTSRV spawn. Existing libraries preserve original rename provenance; host control is not VGA execution. |
|11 old ntcon records | NTVWM execution/presentation/hidden Console; launch codec and I/O client now common. Original name referred to a worker, current name to the frontend. |
|NTSRV membership, run16 probe, worker-base client | frontend_registry authenticated identity, removal of launcher probe, common/console client respectively. Deleted sampling/probe code is not a missing CPU/guest provider. |
|c_main | Per-instruction observers gated to diagnostic builds by T429 S2. Old observer counts cannot establish current release reachability; interpreter ownership unchanged. |
|keyba | Added pending-IRQ reset at session reset, not the C0 response or output-read deassert repairs. Both registered defects still present. |
|COMMAND cmd/cmdkeyb/cmdredir, DEM, OEM process | Product system-directory naming/binding and original host temporary-directory fallback restored in T427; cmdmisc projects completed WOW startup environment (DIV324). That WOW-specific behavior is not blanket non-WOW equivalence. |
|mouse_io, nt_graph/nt_event/nt_fulsc/nt_reset/nt_bop/nt_pif | Current software VGA, natural pointer/input/route hooks and source-owned final frame, plus product paths. Accepted T425/T429 evidence is representative current proof; old full-frame/host-timer assumptions superseded. |
|NTSRV/main, run16 scope/main, worker-base connection, NTCON main/text_frame | Central registration/control/receipt/retirement and common event-input/publication integration. T424/T428/T429 evidence covers the selected product paths; renderer geometry does not change instruction/descriptor ABI. |

### Current finite receiver ledger

“Selected” here proves a receiving implementation, not execution of every
catalog entry. Unreached dynamic vectors/ports and fixture-only cases remain
unproved; no new exhaustive CPU/service census is a closure requirement.

| Family | Original/current selected receiver | Evidence/disposition |
| --- | --- | --- |
| G01 BOP decode | ccpu386/c_main.c → base/bios/bios.c | Original decode and selected tables; representative guest calls only. New stack profile S2, not an all-opcode proof. |
| G02 DEM | host/src/nt_bop.c → dos/dem | Current COMMAND/MEM/EDIT matrix; directory reset K01 goes to S5. |
| G03 WOW | MS_bop_1 → wow32 dispatch, selected WOW page/task bindings | Separate WOW32 W01/W02/W03 owners; current app frontiers do not prove all thunks. |
| G04 XMS | MS_bop_2 → original XMSDispatch/SAS, softpc/mvdm_a20.c and mvdm_xms_memory.c | Existing XMS normal/negative evidence identity reviewed in S6. |
| G05 DPMI | MS_bop_3 → dpmi32 dispatcher/25 original handlers | Prior direct/nested DOSX proof is historical; invalid IDT/PM-stack/fault proof still limited. S6. |
| G06 CMD | MS_bop_4 → original CmdDispatch, command_process_compat → NTSRV/run16 | Current nested/product handoff proof T429; no recreated guest scheduler. |
| G07 redirector | MS_bop_7 → LoadVdmRedir/VrDispatch and bounded guest-copy providers | Selected DLL and dispatch confirmed; K02–K04 source risks S4, not all-network runtime success. |
| G08 other BOP | bios.c and original BIOS/EMS/mouse/NTIO tables | Selected static receivers retained. Dynamic VDD/third-party entries remain conditional, not fabricated missing providers. EMS proof S6. |
| G09 software INT | original reset/IVT and c_intr.c, guest rewrites, PM IDT | Runtime vector target is dynamic; source/byte census not execution proof. Relevant transition tests S2/S6. |
| G10 IRQ/fault | ICA/c_intr/nt_inthk, original DPMI interrupt handlers | Distinct source/exception origin retained. CPU/PIC tests S2/S3/S6; WOW callback-specific work handed off. |
| G11 port I/O | c_main/ios.c → selected original device tables | Device semantics retained; C0/IRQ1 S3. Unreached dynamic ports not all certified. |
| G12 modes/descriptors | original dpmi32/modesw/dpmiselr and ccpu386 descriptor/control owners | Concrete CALL/RETF/IRET stack-width debt S2; selected non-WOW transition proof S6. |
| G13 mappings/leases | SAS/nt_mem plus mvdm_softpc_physical_mapping/guest_memory and session leases | Bounded adapter exists, not universal alias proof. Non-WOW failure/teardown S6; WOW shared aliases W02/W03. |
| G14 A20/EMS/XMS | original SAS, emm_mngr/nt_emm/XMS with bounded bindings | Existing source/guest witnesses reused conditionally; EMS mapping/return evidence S6. |
| G15 video/input | original VGA/keymouse → NTVDM adapter → worker-base publication/input → common client → NTCON | T425/T429 actual Console/Window/EDIT200 and final-frame proof. Original VGA bit5 limitation retained, no speculative repair. |
| G16 lifecycle | original nt_event/nt_reset/execution/termination → NTSRV, shared shutdown/input | T429 real lifecycle/handoff cases; guest mapping/resource failure coverage S6. Source ownership not moved to broker. |
| G17 host services | original Sim32/VDM memory owners, bounded mapping/monitor providers | K01 production query binding absent with explicit slow fallback. Other conditional monitor-only services remain profile-limited, not silently certified. S5/S6. |

### Updated gap judgments

Current source rechecks: K01 demsrch.c::FileFindReset explicitly falls back
after unsupported NtVdmControl; rg finds no production query-bind caller.
K02 vrnetapi.c::VrGetUserName still compares wcslen against CX-1 before OEM
conversion. K03 mvdm_redirector_async.c::mvdm_redirector_async_complete writes
error/count words before acquiring/copying payload. K04 VrGetCDNames explicitly
documents valid writable buffers and **no error return**; its helper clears
and copies destinations in sequence with short-circuit failure. Thus K04's
missing CF/AX is not itself a regression, and K03's required atomicity has not
yet been established against the original implementation. S4 must compare
original ordering and test invalid-address behavior before choosing a fix.

Current CALL/outer RETF/outer IRET still use operand-size SP/ESP after loading
new SS. IRET's separate VM return already uses set_current_SP: do not conflate
these branches. keyba C0 still only marks code_to_send_valid; output-buffer
read still deasserts IRQ1 only under JOKER. S2/S3 own the finite source-based
profiles, not guessed explanations for WRITE or mouse latency.

H01/H02 remain historical pre-guest launch-profile observations. Accepted
ordinary product paths supersede general startup-failure claims; detached/
pipe profiles are not newly promised or repaired here. Original COMMAND
large-environment and DOSX debug/reallocation limits, MEM-size attribution,
external COM requirements and VGA bit5 debt stay explicit in TODO. No original
guest patch or unproved peripheral expansion is added.

WOW G03/shared-view, WRITE first-provider investigation and original SOUND
limits have exact receivers and test obligations in the
[successor plan](../operations/t430-non-wow-contract-plan.md#known-wow-handoff).
This is a recorded handoff, not recipient implementation or WOW acceptance.

### Closure and verification limits

S1's frozen finite ledger and input reconciliation are complete. S2–S7 are
planned non-WOW completion, replacing the original audit-only sequence by
owner direction. Neither source selection nor the existing representative
matrices proves all11,817 keys/3,673 candidates equivalent. Missing dynamic
evidence is retained explicitly with bounded owner follow-up.

Verifier positive r003 passed; sealed-output/out-of-build rejection and
PowerShell parser checks also passed. Verify-DocumentationGovernance.ps1,
Test-DocumentationRelativeLinks.ps1 and git diff --check passed. No new runtime
result or deployment is claimed. The unrelated side-session proposal remains
excluded. T430 stays open and S2 awaits sequential admission.
