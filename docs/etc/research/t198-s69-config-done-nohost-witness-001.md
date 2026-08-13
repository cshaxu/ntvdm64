# M0 T198 S69: Config-Done No-Host-Capability Witness

## Result

The active composition now accepts only the source-observed `BOP 5E/AL=0`
ConfigDone event and resumes after three bytes.  It introduces no UMB, DASD,
raw-drive, GUI, VDD, or console backend.  The guest's subsequent `AllocUMB`
call remains guest-owned.

## Clean source-built witness

The complete MSVC x64 `/MT` run at
`artifacts/build/t198-s69-config-done-nohost-r1` advances beyond ConfigDone to:

```text
next=50:36 terminal=1:0032:5a72
```

This is a DEM service, `SVC_DEMENTRYDOSAPP`, not a new top-level BOP.

## Next frontier

`50:36` belongs to the already-inventoried DEM DOS-app entry/dispatch family.
The next task must compare its actual copied state with the active global DEM
plane and determine whether the re-entry is already closed or exposes a
session-lifetime gap.  It must not add a one-off handler merely because this
is a new point in the guest control flow.

