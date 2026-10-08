# M0 T437 S8 — released add-on utility consolidation

## Question

Can the Win1.01 and installed Win3.1 tools consume the single released copies
of `PIF.EXE`, `HASH.EXE`, and the mouse drivers without retaining duplicate
tool-directory payloads or broadening the approved guest-image changes?

## Change

- `tools/win101-setup/APPLY.CMD` and `UNAPPLY.CMD` resolve
  `assets/release/PIF.EXE` and `HASH.EXE`. Apply copies those exact released
  files into media `PATCH`, where `SETUP.CMD` needs them after tool execution.
  The two duplicate tool-directory binaries were removed.
- `tools/win31-launch/APPLY.CMD` resolves `PIF.EXE`, `HASH.EXE`, and
  `MOUSE31.DRV` from `assets/release`; none is retained in its tool directory.
  CMD now owns recovery copies, root `MOUSE.DRV` replacement, profile/config
  creation, and launch scripts.
- `PATCH386.EXE` replaces the old mixed-purpose `WIN31LAUNCH.EXE`. Its sole
  interface is `--apply --krnl <file> --win386 <file>` and it accepts only the
  two approved retail/candidate identities. It does not create backups or
  alter mouse/configuration/PIF state.
- First installed originals are retained under the owned Win3.1 `PATCH`
  directory as `KRNL386.ORIG`, `WIN386.ORIG`, and `MOUSE.DRV.ORIG`.

No guest-image range changed: `PATCH386.EXE` retains the approved SFT/DOSMGR
byte contracts and verifies each complete candidate hash before replacement.

## Verification

```powershell
cmd /d /c "src\addon\win31-launch\BUILD.CMD build\M0-T437-S8\r001\PATCH386.EXE"
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\pif_hash_tool_test.ps1 `
  -Pif assets\release\PIF.EXE -Hash assets\release\HASH.EXE `
  -BuildRoot build\t437s8\r3 -ReleaseRoot assets\release
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win101_setup_cmd_test.ps1 `
  -ToolRoot tools\win101-setup -BuildRoot build\t437s8\r5
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win31_launch_profile_test.ps1 `
  -InstallRoot build\t437s8\r4 -ProfileDirectory build\t437s8\r4\PATCH
```

All passed. The Win3.1 fixture was built from the approved exact identities:
it restored the two retail byte ranges from an existing checked candidate,
confirmed their retail hashes, then exercised a fresh `APPLY.CMD` run. It
verified both candidate hashes, all three recovery files, generated
CloseOnExit PIFs and a second idempotent Apply. Candidate-only invocation of
`PATCH386.EXE` left its inputs byte-identical; malformed invocation returned
64.

## Boundary

This is portable installed-tree preparation only. It does not prove Win3.1
enhanced-mode runtime success and does not affect the separately queued
ordinary-Setup post-copy protected-mode recovery.
