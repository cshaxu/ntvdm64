# M0 T368 S3 — Formal COMMAND boundary closure

## Executed formal command

```text
ninja -C build/M0-T365/S2/x86 -j8 original-softpc-process.exe
ninja -C build/M0-T365/S2/x86 -t query original-softpc-process.exe
ninja -C build/M0-T365/S2/x86 -t query original-mvdm-command.lib
ninja -C build/M0-T365/S2/x86 -t query basesrv-bindings.lib
```

The incremental product build returned `ninja: no work to do.`  It did not
configure another graph, use `src.old`, or run the product.

## Final-input proof

`original-softpc-process.exe` directly selects:

- `original-mvdm-command.lib`;
- `basesrv-bindings.lib`;
- `session.lib` and `broker.lib`;
- original DEM/XMS/DPMI32 and CPU40 SoftPC libraries.

The COMMAND archive contains `obj/command/cmddisp.obj` and
`obj/command/cmdmisc.obj`, alongside the remaining original COMMAND cohort.
The Base VDM binding archive contains
`obj/adapter-basesrv/base_vdm_client.obj`,
`obj/adapter-basesrv/base_vdm_local.obj`, and
`obj/adapter-basesrv/base_vdm_broker.obj`.

## Product identity

`build/M0-T365/S2/x86/original-softpc-process.exe`

SHA-256:

```text
eb7214703f7ec9562a84dcba37d9ed5b17c9b194cc9eeac3be7098cb34a5d43d
```

## Result and limit

The source-selected original COMMAND record route and its existing Base
VDM/session binding are final CPU40/x86 product inputs.  This proves link/input
closure only.  It does not prove that the product reaches guest `GotCom`,
`EXTERNAL`, the INT 21h EXEC transaction, child termination, or parent return;
those remain the sole subject of S4's one frozen observation and later owner
packages.
