# T221 S3 P32: Virtual private-token foundation

## Change

The Virtual private-root view now has one selector-blind open/create backend.
It opens only entries in the private store, adopts its backend object under a
distinct `VIRTUAL_FILE` opaque session-token kind, and preserves the existing
create/open and scalar-error boundary. It has no host namespace or resolver
parameter.

The existing overlay file object is reused only as a bounded in-memory file
and share/offset implementation. The Virtual token kind prevents a later
Direct or Overlay handle path from accepting it by accident.

## Verification

Run `tools/probe/Invoke-T221S3DemVirtualNamespaceProbe.ps1` with the fresh
build root `build/M0-T221-S3/105`.

The MSVC x64 `/MT` probe passed both private-root and private-token fixtures.
The latter proves `CREATE_NEW`, opaque kind validation, write/readback, stale
token rejection, `OPEN_EXISTING`, and the handled `ERROR_FILE_NOT_FOUND`
outcome. It uses no host I/O and no guest execution.

## Limits

This backend is not bound to a DEM BOP yet. In particular it does not alter
the T220/T221 `50:3C` PDB/JFT/SFT lifecycle limitation: virtual token teardown
will be attached only through a separately evidenced owner lifecycle, not by
extending the existing Direct-only cleanup shortcut.
