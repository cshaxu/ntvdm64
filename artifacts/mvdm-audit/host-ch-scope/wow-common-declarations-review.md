# Selected WOW GDI, kernel and USER declarations

The [observer](wow-common-declarations-observe.py) and
[receipt](wow-common-declarations-evidence.json) record current and two
original hashes, every recorded dependency group, representative thunk table
identity and cached WOW32 map owners for four pending headers. Each file is
byte-identical to both compared OpenNT editions. Replay the observer from the
repository root and compare JSON; changed source or build snapshots will
legitimately change the result.

| File | Selected declaration and static dispatch example |
| --- | --- |
| `wgdi.h` | `WG32Arc` is registered by `wgtbl2.h` and mapped to `wgdi.obj`. |
| `wgdi31.h` | `WG32AbortDoc` is registered by `wgtbl2.h` and mapped to `wgdi31.obj`. |
| `wkernel.h` | `WK32GetDriveType` is registered by `wktbl2.h` and mapped to `wkernel.obj`. |
| `wuser.h` | `WU32BeginPaint` is registered by `wutbl2.h` and mapped to `wuser.obj`. |

Each exact header path appears in 88 current x86 WOW32 dependency groups,
largely through `precomp.h`. Those groups and object names are retained in
the receipt. The representative declaration, table registration, definition
and map owner are positive evidence of the selected source role; header
inclusion alone does not prove every declaration participates in a call.

Recommend **partial** for these four file roles. No new execution was
performed: actual guest thunk dispatch, every API's behavior, error returns,
resource ownership and task teardown remain separate obligations. A cached
map is not proof that the currently installed release matches or executed it.
Original source-text retention is 100%; no behavioral-completion percentage
can be inferred. No guest, product source, build or shared ledger was changed.
