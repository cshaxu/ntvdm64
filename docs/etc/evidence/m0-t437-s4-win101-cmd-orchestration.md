# M0 T437 S4 — Win1.01 CMD installer orchestration

## Question

Can the Win1.01 media patch remain small and inspectable while letting the
original Setup own assembly of `WIN100.BIN`?

## Inputs

- Released `MOUSE101.DRV`, `PIF.EXE`, and `HASH.EXE` from `assets/release/`.
- The tracked payload in `tools/win101-setup/`.
- Disposable flat-media fixtures created by
  `tests/component-integration/win101_setup_cmd_test.ps1`.

## Procedure and observations

`APPLY.CMD` asks for a media directory and copies exactly `SETUP.CMD`,
`README.TXT`, `MOUSE.DRV`, `SETVER.EXE`, `PIF.EXE`, and `HASH.EXE` into its
`PATCH` directory. It removes the superseded PowerShell/template payload if a
prior patch is present.

`SETUP.CMD` copies the flat original media into a private `PATCH\TEMP`,
replaces only that temporary `MOUSE.DRV`, and invokes the original `SETUP.EXE`
through `run16`. It records the true launcher result and removes `TEMP` on both
normal and negative return. Only after Setup returns does it ask for the actual
installed tree and create `PATCH\WIN.CMD`, `WIN101.PIF`, `CONFIG.NT`, and
`AUTOEXEC.NT`; the profile resolves `run16` from `PATH`.

Focused commands:

```powershell
cmd /d /c "src\addon\pif\BUILD.CMD build\M0-T437\S4\r002\utilities\PIF.EXE"
cmd /d /c "src\addon\hash\BUILD.CMD build\M0-T437\S4\r002\utilities\HASH.EXE"
powershell -NoProfile -ExecutionPolicy Bypass -File tools\build\Stage-AddonUtilityRelease.ps1 -PifBuild build\M0-T437\S4\r002\utilities\PIF.EXE -HashBuild build\M0-T437\S4\r002\utilities\HASH.EXE
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\pif_hash_tool_test.ps1 -Pif assets\release\PIF.EXE -Hash assets\release\HASH.EXE -BuildRoot build\M0-T437\S4\r002\pif-hash -ReleaseRoot assets\release
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win101_setup_cmd_test.ps1 -ToolRoot tools\win101-setup -BuildRoot q
```

Both focused tests passed. The package test exercises a successful original
Setup stand-in and an exit-37 failure stand-in; it checks retained package
files, release identity, generated installed profiles, result propagation, and
temporary-media cleanup. Its compact `q` root is intentional: historical PIF
string fields are bounded, so real media and installed paths must be short,
physical DOS-compatible paths.

## Interpretation and boundary

The orchestration and cleanup contract is proved without modifying original
media in place. The stand-in cannot prove that a particular original Win1.01
Setup guest run completes; that remains a manual guest acceptance boundary.
No PowerShell script, JSON/template input, or persistent work directory is
needed in media or installed `PATCH` directories.

## Follow-up

Retain the original-Setup guest run as the acceptance step. Any failure that
requires directly patching an immutable guest core image requires new task
admission.

## Superseded media handling

S5 supersedes this record's temporary-media `MOUSE.DRV` handling. The current
owner-approved model replaces the selected media-root file in place and keeps
its original bytes as `MOUSE.DRV.BAK`; see the S5 evidence record.
