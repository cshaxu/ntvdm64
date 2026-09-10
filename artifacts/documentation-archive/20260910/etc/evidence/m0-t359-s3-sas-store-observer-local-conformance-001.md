# M0 T359 S3 P1 — SAS-store observer local conformance

## Scope

This is a local test of the adapter-owned, default-off observation boundary
only. It does not execute a guest, alter guest media, select a BOP or prove
the COMMAND lifecycle.

## Reproducible graph

`tools/build/New-T359S2SasStoreNinja.ps1` generates the x86 Ninja graph at:

```text
build/M0-T359/S2/sas-store-observer-x86
```

The graph compiles the production observer, its focused fixture and strict
failing stand-ins for unrelated functions sharing the historical translation
unit. The stand-ins prove the fixture has no hidden session or guest-memory
dependency; they cannot make an uncalled path succeed.

Running `ninja -C build/M0-T359/S2/sas-store-observer-x86 test` succeeds
(`5/5` actions). The fixture proves all of the following:

- no report is produced with no configuration;
- an adjacent non-overlapping byte write is not reported;
- each configured overlapping write is recorded with its original address,
  width and scalar value;
- reporting itself does not require a session, mapping or guest lease.

## Formal product relationship

The separately generated `build/M0-T359/S2/formal-x86` graph linked
`original-softpc-process.exe` successfully in 428 actions with the production
observer selected. The focused fixture is intentionally not a substitute for
that link and the formal product is intentionally not a substitute for the
later one-run S4 observation.
