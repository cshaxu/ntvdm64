# M0 T310 S16 P4 — original firmware resource-write root

## Original behavior

`cmos.c` calls `host_write_resource` only after CMOS was read and changed.
Original `nt_rez.c` first resolves the resource through `host_find_file`, then
writes it. If that open fails, its retry historically used bare `name`, which
can create a same-named file in the process current directory rather than the
already selected resource location.

## Selected recovery

`MVDM-HOST-DIV-133` changes only that retry operand to the existing resolved
`hff_ret` path. It preserves the original function name, parameters, lookup,
first open, create/retry, write, close and `host_error` ordering. The selected
app/session firmware-root route therefore applies to both resource reads and
the original CMOS/profile persistence path.

ROM/BIOS code inputs remain immutable. This change does not create a writable
ROM route; it retains the original CMOS/profile lifecycle without an
uncontrolled current-directory side effect.

## Verification

The formal selected Win32/x86 CCPU40 Ninja graph rebuilt the changed original
`nt_rez.c` member and `original-softpc-host-roots.lib` successfully. Original
source-form CRT warnings remain visible; no warning was suppressed and no new
resource provider was introduced.
