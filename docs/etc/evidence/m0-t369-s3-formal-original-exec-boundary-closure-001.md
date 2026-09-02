# M0 T369 S3 — Formal original EXEC-boundary closure

The formal CPU40/x86 product was rebuilt outside the sandbox with the existing
MSVC x86 environment:

```text
cmd.exe /d /s /c "call build\\M0-T366\\S2\\declared-dos-record-x86\\msvc-x86.cmd ninja -C build/M0-T365/S2/x86 -j 8 original-softpc-process.exe"
```

Ninja reported `no work to do`; therefore the current selected source/object
closure is already the final product.  The same formal graph was queried.

`original-softpc-process.exe` directly takes the current `app-machine-shell.lib`,
the original SoftPC CPU40/device archives, `original-mvdm-dem.lib`,
`original-mvdm-command.lib`, Base VDM/session/broker bindings and the original
CCPU archive.  `app-machine-shell.lib` includes
`obj/app/package_layout.obj`, so the current executable-relative SystemRoot
layout is a final product input.

The COMMAND archive includes the original `cmddisp.obj`, `cmdexec.obj` and
`cmdmisc.obj`; the DEM archive includes original `demdisp.obj`, `demhndl.obj`
and the full reached DEM owner set.  No query input selects `src.old`, a host
DOS executable loader, or a synthetic child/parent provider.

This is a final-link fact only.  It does not claim that a guest COM/MZ body
has executed; S4 owns one fixed observation with the rebuilt observer.
