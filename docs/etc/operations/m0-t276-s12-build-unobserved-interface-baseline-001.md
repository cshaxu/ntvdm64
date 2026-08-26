# M0 T276 S12 P10: Literal-build-unobserved source baseline

## Question

What does the ten-file `host/src` remainder mean when none is observed by the
literal original `sources` parser, and how is a false source-presence-to-runtime
inference prevented?

## Result

- `nt_aorc.c` and `nt_cprgs.c` are alternative CPU register/executor wrapper
  systems. They are Bochs-replacement exclusions, not a reason to create
  one adapter operation per historical register accessor.
- `nt_ertbl.c` and `nt_mess.c` are small original error-data tables. They stay
  exact mirror inputs until a reached error package and original build profile
  identify their actual consumer.
- `nt_hfx.c` is a historical stub set; it cannot become a default file/time
  provider. `nt_hunt.c` and `nt_smenu.c` are debugger/settings product UI
  paths, both profile-gated.
- `nt_term.c` is a cross-owner cleanup/termination sequence. It must be
  recovered only after the named session teardown owners are real; it cannot
  turn a session stop into a process kill.
- `nt_vflop.c` is a second virtual FDC and is excluded beside Bochs.
  `stf_conf.c` is a historical product configuration panel and stays gated by
  app policy and original build evidence.

## Decision

All ten retain `profile/build-unobserved` intent from P2. No original build
membership is invented and no auxiliary source joins a runtime/build graph.
The only future routes are the named complete owner packages, each of which
must first establish original build/profile reachability.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12BuildUnobservedInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 build-unobserved interface verification passed: 10 rows.`
