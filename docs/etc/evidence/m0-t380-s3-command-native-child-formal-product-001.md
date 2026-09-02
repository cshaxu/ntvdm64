# M0 T380 S3 — COMMAND native-child formal product confirmation

## Formal graph

The current formal CPU40/x86 graph at `build/M0-T379/S2/formal-x86` was run
through its required MSVC wrapper with target
`original-softpc-process.exe`. Ninja returned `no work to do`, proving that
no production source has changed since the last successful formal link.

`ninja -t query original-softpc-process.exe` identifies the final original
COMMAND archive `original-mvdm-command.lib` and all required same-shaped
boundaries as inputs of the one process: `softpc-bindings.lib`,
`softpc-win32-bindings.lib`, `basesrv-bindings.lib`, `session.lib` and the
original CPU40/SoftPC libraries. The graph contains no app-native-child
launcher and no second executor.

## Fixed products

- `original-softpc-process.exe` SHA-256:
  `adbdfc3bf72fbf41798ab299ee64b489e35096b86727274552b4c30199a57e1f`.
- Its sibling original `VDMREDIR.dll` SHA-256:
  `74dfd1f242b2169f8bc589e677ad242196e8034c4404a4e9ae226b231ec52198`.

The latter remains a sealed runtime companion under T379's format-4 stage.
It does not alter the selected COMMAND-native-child route.

## Result

S3 closes. The current formal product is identity-proven and contains the
selected original COMMAND lifecycle. S4 may make exactly one frozen
non-debug request for `C:\Windows\System32\cmd.exe /c exit 37` through the
existing guest COMMAND route.
