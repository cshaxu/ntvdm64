# T99 S2 boot-namespace provider state 001

## Implemented internal boundary

`bx_ntvdm_boot_namespace_provider_v1` is now the single adapter-local owner
of the finite profile-backed boot namespace. Its state owns only:

- the existing immutable read-only files (COMMAND, CONFIG, AUTOEXEC, target);
- the v4 DOS metadata snapshot used by pathname search; and
- the pathname search transaction state.

It exposes paired CONFIG/AUTOEXEC pathname production and normal DEM
open/seek/read/close operations through that one state. It retains no host
path, host handle, guest pointer or Bochs object.

## Focused evidence

The new `bx-ntvdm-boot-namespace-provider-test` source-built and passed under
the current local CMake toolchain. It establishes this contained chain:

```text
54:0C CONFIG pathname -> 50:12 open -> 50:16 read -> 50:02 close
```

The test also asks the provider to prepare `50:0A` and verifies rejection, so
FCB search cannot accidentally inherit pathname or read-handle semantics.

This is a provider-state/unit result only. Runtime dispatch still contains the
older frozen endpoint paths, and the whole family has not yet been migrated
through the common runtime/session flow. No engine or runtime evidence is
claimed by this record.
