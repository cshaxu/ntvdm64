# M0 T310 S8 P4: Copy/Fill Current-Source Revalidation

## Scope

This record revalidates the seven historical ABI diagnostics for the selected
original `softpc.new/host/src/copy_fnc.c` unit.

## Result

- The active source already applies registered `MVDM-HOST-DIV-064`: only
  private host address alignment uses `uintptr_t`; the original four-byte
  fill value remains `unsigned int`.
- The active fill count is `size_t`, so it remains a private native length and
  does not cross a guest or component identity boundary.
- `RtlFillMemoryUlong` has an explicit source-shaped `NTAPI` declaration.
- No mapping-manager identity is applicable: every reviewed address is a
  short-lived private SoftPC backing pointer.

## Verification

Fresh formal Ninja compilation of `obj/host/copy_fnc.obj` succeeded with exit
code zero for both x64 and x86. Neither build emitted the recorded C4244,
C4311, C4312 or C4013 diagnostic for this unit. No new source change was
needed; non-width warnings elsewhere remain visible.
