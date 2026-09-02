# M0 T366 S3 — Formal declared DOS record closure

## Procedure

```text
VsDevCmd -arch=x86 -host_arch=x64
ninja -C build\M0-T365\S2\x86 -j 8 original-softpc-process.exe
ninja -C build\M0-T365\S2\x86 -t query original-softpc-process.exe
ninja -C build\M0-T365\S2\x86 -t query app-machine-shell.lib
ninja -C build\M0-T365\S2\x86 -t query basesrv-bindings.lib
```

## Observation

The selected target was incrementally clean. The final process has direct
inputs `app-machine-shell.lib`, `original-mvdm-command.lib`,
`basesrv-bindings.lib`, `session.lib` and `broker.lib`.

`app-machine-shell.lib` contains `obj/app/launch_declaration.obj`.
`basesrv-bindings.lib` contains the exact selected local BaseClient,
BaseVDM-local and BaseVDM-broker objects. The final link also retains the
selected original CPU40 and original DEM archives.

## Interpretation

The S2 correction is a final product input, rather than a fixture-only
substitute. This remains formal link proof: it does not establish that any
guest command, `$Exec`, child, MZ image or parent return executes.
