# M0 T198 S59: DEM Fast-Read Finite-Composition Witness

## Composition repair

The existing `bx_ntvdm_dem_fastio_provider_v1_dispatch` was added to the
boot-namespace plane's existing bulk-result path.  The DEM component map also
now assigns service `0x42` to the namespace component, rather than retaining
it as an original-noop endpoint; otherwise the provider was unreachable.

The repair does not add a new BOP ABI, Bochs behavior, host filesystem access,
device, firmware, or DOS image.  The provider reads only the already
materialized immutable namespace and uses the pre-existing checked ordinary
guest-RAM bulk action.

## Clean source-built witness

The following clean MSVC x64 `/MT` source-built run was performed from a new
build root:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S48NtioCleanRebuildProbe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s59-fast-read-composition-r2 \
  -HostArchitecture x64
```

The probe intentionally returns its legacy fixture status `2`; the emitted
run log is the witness.  It advances beyond the former fast-read boundary:

```text
next=5f:72 terminal=1:8dc8:45a8
```

`5f` is a three-byte top-level BOP selector.  The following `72` is the next
guest opcode, not a service number.

## Next frontier

OpenNT `bop.h` names selector `5F` `BOP_UNIMPINT`.  NTIO's `spckbd.asm`
issues it with `AX=VERSIONID` after constructing keyboard/interrupt handoff
tables; its `jc isk_int9` continuation explicitly accepts a carry failure.
The next task must audit the existing narrow `spckbd` completion provider and
its exact `VERSIONID` state before admitting it.

