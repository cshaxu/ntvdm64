# COMMAND configuration system-root selection

Frozen `dos/command/cmdconf.c` is partial under DIV-158. The OpenNT
`ExpandConfigFiles` calls `GetWindowsDirectory`, then `GetShortPathNameA`
into the original 64-byte system-root buffer. Current source substitutes
`mvdm_softpc_system_copy_root` for that first call; the session owns the
selected root and the adapter clears its output on refusal. The unchanged
configuration/PIF selection then opens the selected CONFIG.NT or AUTOEXEC.NT;
a zero or too-long short-root result enters the original bad-system-file
dialog and termination path. The return from the first acquisition is not
the later root-length authority.

A local direct public-API probe on this host called `GetShortPathNameA` with
an empty string and with a nonexistent root: both returned 0 and error 3.
This supports the described error direction on this host but is not a
production worker or full failure-path test. `tests/session/softpc_media_resource_fixture.c`
covers the session root helper and small buffers; normal DOS startup evidence
does not independently prove every expansion, differing-drive root, invalid
root or 64-byte short-path boundary.

Retain the original short-path processing, PIF choice, configuration-line
expansion and failure order; verify those paths with current package identity
and independently authored inputs before declaring their behavior complete.
No code, guest, installed profile or product artifact was changed by this
source audit.
