# T422 S1 WOW32 audit closure

## Result

S1 closes the whole-package audit and implementation design for the selected
WOW32 provider.  It does **not** claim a running WOW16 desktop or acceptance
of `WRITE.EXE`, `WINMINE.EXE`, or `SOL.EXE`; those remain the explicit end
conditions of S2--S8.

## Reconciled scope

| Surface | S1 result | Evidence |
| --- | --- | --- |
| Original provider | 77 selected source bodies, original RC and 27 exports are selected; all 10 tables / 1,786 entries are assigned (182 local, 936 original-unimplemented, 668 original host thunks). | [formal graph](m0-t422-s1-executable-closure-graph.md), [dispatch audit](m0-t422-s1-dispatch-definition-audit.md) |
| Registration and callbacks | All 21 input and 20 output slots have one receiver. The 72 active `CallBack16` sites and 3 USER native-to-guest gateways are assigned to S2--S7; static x86 ABI proof is retained separately from runtime proof. | [coverage ledger](../operations/wow32-recovery-coverage-ledger.md), [closure graph](m0-t422-s1-executable-closure-graph.md) |
| Original versus autonomous footprint | 71 source mirrors normalize byte-identically; six semantic mirror deltas remain. The provider has 16 original non-MVDM files / 19 objects / 4,674 lines, 21 finite DLL bindings / 2,977 lines, and two worker-local bindings / 180 lines. | [mirror delta](m0-t422-s1-normalized-wow32-mirror-delta.md), [binding inventory](m0-t422-s1-autonomous-binding-inventory.md) |
| Direct USER data | Current SHAREDINFO/TEB/clock allocation is insufficient: no production desktop, WND/CLS, typed handle or metrics producer exists. The absence is retained as a passing negative audit, not hidden behind a fixture. | [direct-data matrix](m0-t422-s1-direct-data-owner-matrix.md), [client-view audit](m0-t422-s1-client-view-contract-audit.md) |
| Address/lifetime design | Original `SetDesktop`, `DESKTOPVALIDATE` and `REBASE*` establish client `TEB.pDeskInfo`, nonzero delta, server-form desktop bounds/links and one subtract-on-read relation. Normal/zero/overflow arithmetic is reproducibly checked; B1--B5 and exactly-once task cleanup are fixed. | [translation fixture](../../../tests/observation/verify-wow-client-view-translation-contract.ps1), [bootstrap/teardown](m0-t422-s1-wow-bootstrap-teardown-order.md) |
| Boundaries | USER/CSRSS/Win32k, kernel VDM monitor and a second scheduler/TLS/CCPU owner remain prohibited. Dynamic and public API edges have finite per-family receivers. | [boundary disposition](m0-t422-s1-boundary-disposition.md), [binding removal matrix](m0-t422-s1-binding-removal-matrix.md) |

## S2 admission-ready contract

S2 is one complete USER client-view/task/window/message/callback packet.  It
must implement B1--B5 in the existing worker only: construct a source-pinned
CCPU client allocation; publish client `pDeskInfo`, delta and server-form
desktop-heap relations only after a complete typed root object exists; wire
the actual original task exit to the exactly-once nonzero task cleanup branch;
and reject partial publication, stale reuse, callback destruction and worker
loss.  It owns IN-07/08/18/19/20, OUT-01/02/04--08/11/12/15--17 and all
non-menu USER direct-data rows.  Its closure requires a fresh x86 build, all
existing DOS regressions and real immutable USER/WOW16 evidence; native
fixtures cannot substitute for that evidence.

No S2 implementation may add a USER server, a second mapping manager, a
second scheduler/TLS owner, raw host pointers in guest state, or guest-media
changes.  Any retained binding must be measured against this S1 baseline.

## Verified S1 commands

```powershell
& .\tests\observation\verify-wow32-s1-formal-graph.ps1 -RepositoryRoot (Get-Location).Path
& .\tests\observation\verify-wow32-dispatch-definition-closure.ps1 -RepositoryRoot (Get-Location).Path
& .\tests\observation\verify-wow32-registration-surface.ps1 -RepositoryRoot (Get-Location).Path
& .\tests\observation\verify-wow-client-view-translation-contract.ps1 -RepositoryRoot (Get-Location).Path
& .\tests\observation\verify-wow32-client-view-gap.ps1 -RepositoryRoot (Get-Location).Path
```

The first four produce positive audit/build-layout markers.  The last emits
`WOW32_CLIENT_VIEW_GAP_CONFIRMED` and is intentionally a negative result: it
proves S2 still has real work rather than granting false acceptance.
