# T422 S1 WOW32 dispatch-definition audit

## Question

Do all original dispatch-table entries in the selected 77-source WOW32
composition have an explicit source disposition, and does every selected host
thunk point to a body selected in that composition?

## Procedure

`verify-wow32-dispatch-definition-closure.ps1` parses all ten original
`*tbl*.h` tables.  It treats `LOCALAPI` and `UNIMPLEMENTEDAPI` as their
original table dispositions, not as missing source.  For each remaining entry
it requires a body-form definition in one of the selected original WOW32 C
inputs; a header declaration alone does not satisfy the check.

## Observation

The check resolves all 1,786 table entries:

| Disposition | Count | Meaning |
| --- | ---: | --- |
| original `LOCALAPI` | 182 | guest-side/local implementation; no WOW32 host body is expected |
| original `UNIMPLEMENTEDAPI` | 936 | selected original profile explicitly reports the historical unsupported path |
| selected host thunk | 668 | every entry has an original selected C body |

The exact per-table totals are pinned by the verifier: KERNEL 310, USER 539,
GDI 491, keyboard 138, sound 19, Shell 104, Winsock 152, ToolHelp 3,
multimedia 3 and common dialogs 27.

## Interpretation

There is no unresolved selected thunk symbol hidden behind a dispatch-table
entry.  This does not promote an entry to functionally complete: each body
still needs its owner S's binding, normal/failure/reentry/teardown test and,
where applicable, real immutable guest coverage.  The 936 original
`UNIMPLEMENTEDAPI` rows remain source-defined historical behavior; they are
not a license to replace them or a new product defect.

The dispatch table is now a finite entry inventory rather than an inferred
surface.  S2 owns the USER/task-related rows, S3 GDI, S4 resources/menu,
S5 DDE, S6 KERNEL/DOS and S7 every remaining table family.  Mixed USER/GDI
tables keep their individual stable capability owner in the coverage ledger;
table membership does not override it.

## Reproduction

```powershell
& .\tests\observation\verify-wow32-dispatch-definition-closure.ps1 `
  -RepositoryRoot (Get-Location).Path
```

Expected output contains `WOW32_DISPATCH_DEFINITION_CLOSURE_OK`.
