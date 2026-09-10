# M0 T310 S8 P4 — C-video generated mark-rule declaration closure

## Scope

This closure covers the seventeen original `S_*Mark*` declarations in
`softpc.new/base/cvidc/evidfunc.h`.

## Source finding

The historical header left each declaration with an implicit `int` result and
the table-facing parameter spelling.  The actual generated definitions in the
selected `sevid*.c` files uniformly have this private rule ABI:

```c
IUH rule(IUH v1, IUH v2, IUH v3, IUH v4)
```

The table-facing typed mark callbacks are a separate contract.  The existing
source-derived `Generate-CvidcTypedTables.ps1` carrier is the only component
that adapts a generated rule to that table contract.

## Resolution

`MVDM-HOST-DIV-121` changes only the seventeen mirror declarations to the
actual generated four-`IUH` function shape.  It does not change a generated
rule body, table order, mark behavior, guest identity, mapping-manager route,
or controller selection.

## Verification

The formal Ninja generated EVID-table carrier was forced to rebuild on both
architectures:

- `build/M0-T310/S8/p1-machine-source/x64/cvidc-mark-declaration-abi.log`
- `build/M0-T310/S8/p1-machine-source/x86/cvidc-mark-declaration-abi.log`

Neither log contains `C4431` from `evidfunc.h`.  Unrelated original
non-width C-video diagnostics remain visible.
