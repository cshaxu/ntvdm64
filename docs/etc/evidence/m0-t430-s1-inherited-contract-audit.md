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
