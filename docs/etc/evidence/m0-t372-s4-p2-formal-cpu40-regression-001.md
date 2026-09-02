# M0 T372 S4 P2 — Formal CPU40 regression

The S4 local-source graph initially exposed a duplicate declaration of
`FSCTL_QUERY_FAT_BPB_BUFFER`.  The diagnosis was precise: modern public
`winioctl.h` already owns the exact OpenNT type, while an original host source
such as `nt_fdisk.c` reaches the adapter `ntioapi.h` without directly including
that public declaration.

The correction is a source-binding repair, not a rewritten carrier:
`adapter-mvdm-host-out/win32/include/ntioapi.h` now includes public
`winioctl.h` and no longer declares a private duplicate.  Original source sees
the original type spelling and SDK layout; no Redirector, SoftPC or guest
behavior changes.

External formal CPU40/x86 validation:

```text
ninja -C build/M0-T371/S2/formal-x86 -j 8 \
  original-softpc-candidate original-softpc-process.exe
```

completed all 452 actions and linked `original-softpc-process.exe`, including
`original-mvdm-redir.lib`, `redirector-bindings.lib`, the parent original
`ntvdm.def` export table and `original-softpc-process-import.lib`.

The build retains pre-existing original-source warnings.  No warning is hidden
or treated as a substitute for the S4 owner-lifecycle proof.
