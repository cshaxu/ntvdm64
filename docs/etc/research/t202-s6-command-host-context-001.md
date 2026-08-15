# T202 S6: COMMAND copied host-context foundation

## Scope

This record advances the admitted COMMAND whole-provider work only.  It does
not claim that `54:04` closes COMMAND, that all `=?X:` directory state is
implemented, or that a native guest trace has passed.

OpenNT's `cmdGetCurrentDir` in `base/mvdm/dos/command/cmdmisc.c` obtains the
per-drive `=?X:` directory variable, returns that value when present, returns
`X:\` for an admitted drive with no remembered directory, and returns
`AX=0, CF=1` for an invalid drive.  The historic output bound is
`MAXIMUM_VDM_CURRENT_DIR + 3`, where `vdmapi.h` defines the directory part as
64 bytes.

## Delivered boundary

`bx_ntvdm_command_host_context_v1` is an adapter-private, fixed-width record:

- it holds one selected drive and an OEM path of at most 67 non-NUL bytes;
- the record contains copied bytes only—no path pointer, `HANDLE`, callback,
  Bochs object, guest pointer or BOP selector;
- composition setup invokes `GetCurrentDirectoryW` at most once, converts it
  to the copied OEM record only when it belongs to the selected drive, and
  otherwise records that drive's root;
- BOP dispatch never reads or changes the process current directory.

The COMMAND package session owns a copy of that record.  `54:04` now consumes
it through the existing checked multi-write/mechanical-action path.  The
selected drive returns the copied directory; another admitted drive returns
its root; an excluded or invalid drive retains the source-shaped `AX=0,
CF=1` result.  The pre-existing legacy runtime and startup bridge pass no
context and therefore retain their old root-only behavior until they are
removed by a separately admitted composition migration.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-host-context-r5 `
  -Fixture boot-namespace
```

`t198-s16-bx-vdm-boot-namespace.json` records `linkExitCode: 0`,
`runExitCode: 0`, and `passed: true` under MSVC x64 `/MT`.  The bound-package
fixture installs `C:\NTDOS64` as copied COMMAND context, invokes `54:04`
through ordinary generic BOP ingress and checked guest RAM, and verifies the
exact copied write.  Its all-`54:00..10` ownership sweep remains in the same
fixture.

## Remaining package work

This is one context input, not a full COMMAND environment or host-CWD
provider.  Per-drive directory updates, `54:01` launch/return lifecycle,
console/keyboard, opaque stream operations, VDD/KEYB disposition, and child
VDM broker replacement remain governed by the T202 S5 source/ABI map and S6
whole-package regression plan.  No native trace was run for this change.
