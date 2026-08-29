# M0 T310 S8 P4: selected CCPU access-table ABI closure

## Scope

This is the four-row `SPC-CCPU-ACCESS-TABLE` cluster for
`softpc.new/host/src/nt_cprgs.c`.

## Finding and disposition

The historical ledger retained four `C4057` function-pointer assignments for
selected register readers.  The source already carries
`MVDM-HOST-DIV-046` and `MVDM-HOST-DIV-047`: selected generated declarations
and table slots state the original callbacks' real `IU16 -> IUH` shape rather
than relying on NT4 old-style declaration compatibility.

## Verification

After cleaning the exact formal object, both formal Ninja configurations
rebuilt `obj/host/nt_cprgs.obj` successfully.  Neither x64 nor x86 emitted
`C4057`.  No source change, suppression, adapter, or mapping-manager route was
needed; the worklist rows now record the existing selected-access-table
closure.
