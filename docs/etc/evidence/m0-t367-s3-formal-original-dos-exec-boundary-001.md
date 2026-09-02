# M0 T367 S3 — Formal original DOS EXEC boundary

The existing CPU40/x86 formal graph was rebuilt with:

```text
VsDevCmd.bat -arch=x86 -host_arch=x64
ninja -C build/M0-T365/S2/x86 -j 8 original-softpc-process.exe
```

Ninja reported `no work to do`; the selected product exists at
`build/M0-T365/S2/x86/original-softpc-process.exe` (3,119,616 bytes, recorded
mtime `2026-09-02T08:23:07.963Z`).  The no-work result is an incremental-build
result, not a new runtime claim.

`ninja -t query original-softpc-process.exe` proves that the final product
input closure includes:

- `app-machine-shell.lib`;
- `original-mvdm-dem.lib` (including `demdisp.obj` and `demhndl.obj`);
- `original-mvdm-command.lib`;
- `original-ccpu386.lib` and `softpc-bindings.lib`;
- `basesrv-bindings.lib`;
- `session.lib` and `broker.lib`.

The graph has no `src.old` input and no added DOS executable loader.  This
closes the formal boundary prerequisite for T367's one immutable original-MZ
workload observation.  It does not prove that guest `$Exec`, `MEM.EXE`, child
termination, parent return, or COMMAND `54:0B` has executed.
