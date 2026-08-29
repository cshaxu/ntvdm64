# M0 T310 S8 P4 — SoftPC system-interrupt native contract closure

## Scope

This closure covers the selected original host system-interrupt sources:
`nt_eoi.c`, `nt_inthk.c`, `nt_reset.c` and `nt_timer.c`.

## Result

- Existing DIV-050 already closes the timer remainder and PIC bridge contracts
  without changing their original timer/interrupt ordering.
- The three original `VdmInstall*IntHandler` registrations now carry named,
  typed internal function-pointer contracts instead of converting callbacks to
  `PVOID`. These are control-flow callbacks, not handles or guest addresses;
  the mapping manager is intentionally not involved.
- The original `strlen`-derived parser length remains native `size_t`.

## Verification

After cleaning the selected objects, formal MSVC/Ninja rebuilt them on both
host targets:

```text
x64: ninja obj/host/nt_timer.obj obj/host/nt_eoi.obj obj/host/nt_inthk.obj obj/host/nt_reset.obj
x86: ninja obj/host/nt_timer.obj obj/host/nt_eoi.obj obj/host/nt_inthk.obj obj/host/nt_reset.obj
```

Neither run emitted C4057, C4152 or C4267 for the selected x86/x64 contracts.
Remaining diagnostics are explicitly non-width warnings (`C4100`, `C4101`,
`C4102`, and an x86 macro redefinition) and remain visible.

`dpmiint.c` is not yet a formal S8 Ninja input. Its source call sites use the
same three named registration declarations; its independent current compile
is blocked earlier by the existing `_LDT_ENTRY` duplicate declaration in the
unselected DPMI source package, not by the typed registration interface.
