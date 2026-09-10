# M0 T365 S2 — Original `.COM` DEM file-lifecycle lease closure

## Scope and result

S2 closes the selected low-dependency host boundary for the original guest
DOS `$Exec` `.COM` route.  It does not alter the guest loader, PSP/MCB logic,
parent-return path, file-selection policy, or a BOP result.

The complete selected lifecycle is now:

```text
guest DS:SI pathname --bounded copied lease--> original demOpen
  -> original CreateFileOem / file-size / CF-AX handling
  -> session host-resource identity in AX:BP
  -> original demRead seek / ReadFile / CF-AX handling
       --one bounded writable lease, commit only on success-->
       guest DS:DX bytes
  -> original demClose CloseHandle then identity release
```

`demOpen` copies at most `MAX_PATH` bytes.  An inaccessible or unterminated
guest pathname returns through original DEM error direction with
`ERROR_INVALID_ADDRESS`; it is never retained as a host alias.  `demRead`
leases exactly `CX` bytes.  Read failure, named-pipe failure and lease failure
discard the bounce buffer; only the original success path commits it.

## Source and binding evidence

- Original guest `$Exec` remains owner of `.COM` load/PSP/parent lifecycle,
  as recorded by the closed S1 owner ledger.  S2 touches only its immediate
  host DEM open/read/close dependency.
- [demfile.c](../../../src/mvdm-host/dos/dem/demfile.c) retains the original
  pathname canonicalization, named-pipe branch, `CreateFileOem`, file size,
  `mvdm_host_identity_publish_words`, CF and AX/BP order.  Its former
  `GetVDMAddr(DS:SI)` alias is replaced by a copied `mvdm_guest_location`.
- [demhndl.c](../../../src/mvdm-host/dos/dem/demhndl.c) retains original
  redirector branch, seek, `ReadFile`, broken-pipe, `demClientError`, CF and
  AX order.  Its former `GetVDMAddr`/`Sim32FlushVDMPointer`/
  `Sim32FreeVDMPointer` sequence is one bounded lease acquired before the
  synchronous call, committed only at the original success exit.
- `demClose` already retains the source order `CloseHandle` then
  `mvdm_host_identity_release`; it is the same session mapping manager used by
  `demOpen` for the historical AX:BP handle shape.
- `MVDM-HOST-DIV-195` in `src/mvdm-host/README.md` registers both exact
  mirror modifications.  No new mapper, file provider, loader or BOP route
  was introduced.

Static extraction after the change proves:

- the `demOpen` body has no `GetVDMAddr`, and calls
  `mvdm_guest_location_copy_c_string` plus the existing handle publisher;
- the `demRead` body has no `GetVDMAddr`, `Sim32FlushVDMPointer`, or
  `Sim32FreeVDMPointer`, and has explicit acquire / discard / commit calls;
- the `demClose` body releases the existing host-resource identity.

## Focused local evidence

`tests/adapter-mvdm-host-out/softpc/t365_s2_dem_file_lifecycle_lease_fixture.c`
uses a real temporary local file and one session.  It verifies:

1. DS:SI pathname bytes copy through the bounded string lease;
2. the host file is published and resolved only as a session identity;
3. a synchronous `ReadFile` into the lease commits the exact payload to
   guest memory;
4. an invalid 16:16 range cannot obtain a lease and does not change already
   committed guest bytes; and
5. source-shaped close order is host `CloseHandle` followed by identity
   release.

The reproducible generator is
`tools/build/New-T365S2DemFileLifecycleNinja.mjs`.  With MSVC x86 environment:

```text
node tools/build/New-T365S2DemFileLifecycleNinja.mjs O:\repos.hobby\ntvdm64 O:\repos.hobby\ntvdm64\build\M0-T365\S2\dem-file-lifecycle-x86
ninja -C O:\repos.hobby\ntvdm64\build\M0-T365\S2\dem-file-lifecycle-x86 -j 4
dem_file_lifecycle_lease_fixture.exe
```

completed with exit code 0.

## Formal product selection

The regenerated CPU40/x86 graph at `build/M0-T365/S2/x86/build.ninja` selects:

- `src/mvdm-host/dos/dem/demfile.c` and `demhndl.c` into
  `original-mvdm-dem.lib`;
- `mvdm_host_identity.c` and `mvdm_guest_location.c` into
  `softpc-bindings.lib`; and
- both archives into `original-softpc-process.exe`.

The external formal command completed with exit code 0:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 ... -BuildRoot build/M0-T365/S2/x86
ninja -C build/M0-T365/S2/x86 -j 8 original-softpc-process.exe
```

Warnings in unrelated original CPU40 source remain visible and were neither
suppressed nor reclassified by S2.

## Explicit limits

This evidence is local source/binding evidence only.  It does not claim MZ
relocation, guest child execution, parent return, native child execution,
Redirector completion, a BOP result, or a product runtime observation.  Those
remain governed by later T365 packets.
