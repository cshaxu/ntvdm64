# M0 T313 S2 P9 — Firmware resource mirror cleanup

## Finding

The reached original firmware-resource mirror
`mvdm-host/softpc.new/host/src/nt_rez.c` contained two project-added statements:

```c
type = 0; // To stop unreferenced formal parameter errors
```

They appeared once in `host_read_resource` and once in
`host_write_resource`.  All three pinned OpenNT source baselines omit both
statements.

## Disposition

Both statements are deleted.  They were ordinary warning suppression, not a
width correction, an adapter binding, a mapping-manager boundary, or an
original semantic requirement.  The original unused-parameter warnings remain
visible under the selected x86 build, together with the original old-style and
CRT-deprecation diagnostics.

The existing registered `MVDM-HOST-DIV-133` resource retry correction remains
separate: it preserves the source's retry/error ordering while retaining the
already resolved session resource path.  This cleanup does not modify it.

## Verification

The fresh incremental formal graph command completed:

```text
ninja -C build/M0-T313/S2/r3 obj/host/nt_rez.obj original-softpc-host-roots.lib
```

The source and containing original host-root archive rebuilt successfully.
The emitted `C4100` warnings are expected evidence that no unrelated warning
suppression remains in these two resource functions.
