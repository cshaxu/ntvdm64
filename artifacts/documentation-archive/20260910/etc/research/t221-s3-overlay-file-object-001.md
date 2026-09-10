# T221 S3: Overlay File Object

## Decision

The DEM whole provider now owns an `overlay_files` object in addition to its
existing COW/tombstone store.  It is a selector-blind, provider-private file
object: a bounded opaque token names a drive-relative file and a private byte
offset.  It has no BOP number, guest pointer, host pathname, or `HANDLE`.

The object receives only a copied base-file byte sequence from a future view
resolver.  Its first successful open materializes that sequence in the
session-private store; subsequent reads, writes, seeks, truncates and closes
use only the store.  Consequently a write cannot reach the host namespace.

## Scope and evidence

`Invoke-T221S3DemOverlayFileProbe.ps1` source-builds the store, object and
focused test under MSVC x64 `/MT`.  The test proves base-byte preservation,
copy-on-write replacement, seek/read, truncate, stale-token refusal and
creation of a new private file.  The current full whole-provider fixture also
builds and passes with the new object linked.

This is not yet a BOP subfamily closure.  There is intentionally no namespace
resolver, host-base reader, guest token adoption, enumeration merge, metadata
or tombstone/delete route.  Direct and Readonly behavior are unchanged;
Virtual remains unimplemented.  The next implementation step is to introduce
the provider-owned resolver and migrate all handle services through a typed
Overlay backend before any namespace/FCB/search route claims Overlay support.
