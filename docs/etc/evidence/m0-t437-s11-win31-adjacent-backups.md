# M0 T437 S11 — Win3.1 adjacent backups and recovery

## Correction

The earlier Win3.1 launch tool stored replacements' originals beneath
`PATCH\*.ORIG`. That contradicted the project convention: a replaced file's
original belongs beside it with a `.BAK` suffix.

## Change

`APPLY.CMD` now retains:

- `SYSTEM\KRNL386.EXE.BAK`;
- `SYSTEM\WIN386.EXE.BAK`;
- `SYSTEM\MOUSE.DRV.BAK`.

It restores a retail executable into its original name before giving it to
`PATCH386.EXE` for its checked transformation. A repeat Apply accepts only a
matching checked candidate plus adjacent retail backup. It migrates compatible
legacy `PATCH\KRNL386.ORIG`, `WIN386.ORIG`, and `MOUSE.DRV.ORIG` files once,
then removes those legacy copies.

`UNAPPLY.CMD` verifies candidate/release identities and the required adjacent
backups before moving each `.BAK` file back. It removes only the six generated
profile files from `PATCH`; unknown owner content remains untouched.

## Verification

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win31_launch_apply_recovery_test.ps1 `
  -ReferenceRoot build\t437s10\r1\i -BuildRoot build\s11\r3
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win31_launch_mouse_path_contract_test.ps1
```

Both passed. The first fixture copied only required owner-tree inputs into
`build`, then proved first Apply, repeat Apply, PIF/profile integrity, legacy
recovery migration, and authenticated Unapply. No owner installation was
modified.

## Boundary

This changes tool recovery ownership only. It does not establish Windows 3.1
enhanced-mode runtime success or alter guest/add-on bytes.
