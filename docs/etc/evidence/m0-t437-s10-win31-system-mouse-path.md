# M0 T437 S10 — Win3.1 installed mouse-driver path

## Diagnosis

`tools/win31-launch/APPLY.CMD` preflighted and later mutated
`<root>\MOUSE.DRV`. A read-only listing of the owner-provided installed tree
showed the existing Windows 3.1 driver at
`<root>\SYSTEM\MOUSE.DRV`; there is no root-level driver. The tool therefore
failed before it reached its binary-identity or profile work.

## Repair

All four operations now use `SYSTEM\MOUSE.DRV`:

1. installed-tree preflight;
2. current driver hash;
3. preservation to `PATCH\MOUSE.DRV.ORIG`;
4. copy of the released `MOUSE31.DRV` replacement.

`PATCH\MOUSE.DRV.ORIG` remains an owned recovery file; its location did not
change. No guest or released add-on bytes changed.

## Verification

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win31_launch_mouse_path_contract_test.ps1
```

Passed. The test checks the preflight list, hash input, backup input, and
replacement destination, and rejects a remaining root-level input path.

An additional build-owned fixture copied the required installed files from the
owner tree read-only, ran `APPLY.CMD` with standard-input redirection, and
proved that the replacement was written to `SYSTEM\MOUSE.DRV` while
`PATCH\MOUSE.DRV.ORIG` preserved the original driver hash. No owner-installed
file was written.

## Boundary

This proves the launcher’s installed-path contract, not enhanced-mode runtime
success. Applying the tool to an owner installation remains an explicit
mutable user action.
