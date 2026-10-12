# M0 T443 S16 — special-case closure audit

## Scope and method

This is a documentation/governance audit after the owner transferred H08 to
the queued `WOW32 fonts, text and metafiles` package.  It introduces no
production code, protocol, guest or mirror change.  Each H01--H10 row below
was checked against the T443 ledger, its named production owner and its
focused closure evidence.  Historical old S7--S10 paragraphs are superseded
planning text, not active work.

## H01--H10 disposition matrix

| Row | Final T443 disposition | Evidence / unique owner |
| --- | --- | --- |
| H01--H03 | Delivered: COMMAND target/tail heuristics were removed from the project adapter; the BOP 54:08 service calls product-root `system32\\run16.exe` with the original tail, without changing `COMSPEC`. | [S3](m0-t443-s3-command-comspec-contract.md), [S12](m0-t443-s12-command-service-run16-routing.md), and [S12 closure](../../history/m0-t443-s12-command-service-run16-routing-closure.md). |
| H04 | Retained: one unclassified command-text route reaches Hooked host `COMSPEC /c`; no project shell grammar or internal-command list was added. | [S5 fallback disposition](m0-t443-s5-comspec-fallback-retain.md). |
| H05--H06 | Delivered: Hook preserves native `CreateProcess(application, command)` inputs and applies bounded two-field selection only to legacy redirection; Run16 retains its separate one-string CLI contract. | [S6 Hook boundary](m0-t443-s6-hook-createprocess-boundary.md). |
| H07 | Retained with a bounded limitation: production uses `NtAlertThread`; the absent-export `QueueUserAPC` fallback is not represented as general equivalence. | [S13 disposition](m0-t443-s13-ntalertthread-disposition.md). |
| H08 | Transferred, not implemented in T443: host GDI font tracking/search and guest resource lookup belong to the single queued WOW fonts/text/metafiles provider owner. | [S1 ledger](m0-t443-s1-special-case-contract-ledger.md), [queued owner](../../proposals/proposal-wow32-fonts-text-metafiles-001.md#t443-h08-transfer). |
| H09 | Delivered with H05--H06: the Hook's two-field candidate boundary is independently bounded; no Run16 `MAX_PATH` CLI rule was spread into native CreateProcess. | [S6 Hook boundary](m0-t443-s6-hook-createprocess-boundary.md). |
| H10 | Delivered: only the initial route-selection query can defer an unbound route; rejected publication errors propagate instead of becoming a successful paint. | [S14 publication contract](m0-t443-s14-frontend-publication-error-contract.md). |

## Current release identity

On 2026-10-11, the documentation-only audit rechecked all ten manifest rows.
Each `assets/release` SHA-256 and deployed `O:/winnt/system32` SHA-256 equals
the current `assets/release/release-manifest.json` value.  The immediately
preceding repair commit is `543546d9b`, which republished the coherent x86
`ntvdm.exe`/`VDMREDIR.DLL`/`nthook32.dll` trio after its actual WINMINE
depth/isolation witness passed.

## Outcome and remaining boundary

No unresolved T443-owned special-case implementation remains.  H08 is not
claimed passed as a font capability: it is explicitly assigned to the queued
original-owner package, which must perform the real font/path/lifetime tests
before changing a provider.  The active task is ready for owner T closure
approval after this audit; no full product regression was re-run because this
S changed documentation and task ownership only.
