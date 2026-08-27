# M0 T280 S15 — DEM host-identity facade evidence

## Recovery ladder

1. Direct original reuse fails: original `dos/dem/demsrch.c` writes its native
   `PFFINDLIST` directly into a 32-bit DTA/FCB field.
2. The selected smallest facade is `adapter-softpc`'s
   `mvdm_host_identity_*` surface. It preserves the 32-bit stored value,
   lookup-before-use and release-before-free contract while the session's
   `host_resource` mapper owns native pointer identity.
3. No Bochs intrusion is needed.
4. The facade is the registered source-derived replacement; no provider
   behavior is newly authored.

`FFindId` is not mapped: it remains original numeric search data. Guest DTA
and FCB copies are not performed by this facade and continue to require the
separate session `guest_memory` lease path.

## Formal verification

The following commands generated the disposable Ninja graphs and ran their
fixtures using MSVC `/MT /W4`:

```text
New-T280S15MvdmHostIdentityNinja.ps1 -Architecture x64
ninja -C build/M0-T280/x64-mvdm-host-identity
mvdm_host_identity_fixture.exe
New-T280S15MvdmHostIdentityNinja.ps1 -Architecture x86
ninja -C build/M0-T280/x86-mvdm-host-identity
mvdm_host_identity_fixture.exe
```

Both architectures compiled `mapping_manager.c`, `guest_memory_lease.c`,
`session.c`, `mvdm_host_identity.c` and the fixture, linked successfully, and
the fixtures exited zero. The fixture proves no-bound-session refusal,
publish/resolve and reverse-lookup round-trip, stable republish identity,
session isolation, release/stale refusal and session-disposal invalidation.

No DEM body, BOP route, guest trace or guest-memory mapping was built or
enabled.
