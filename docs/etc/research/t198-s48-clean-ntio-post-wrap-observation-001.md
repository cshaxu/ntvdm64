# T198 S48: clean NTIO observation after real-mode IP repair

## Result

The S47 real-mode sequential-IP repair removes the former false generic-UD
frontier.  A clean current-source rebuild now reaches the already admitted
`SVC_DEMGETDRIVES` (`50:0F`) result and stops only because the retained S23
observation bridge deliberately converts that successful resume into a finite
controlled stop.

This is execution progress, not a claim that the CLI NTVDM is generally
runnable.

## Reproducible witness

The following command was run from a new root:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S48NtioCleanRebuildProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s48-clean-ntio-r11 `
  -HostArchitecture x64
```

`r11` compiles the complete CPU5/Pentium-MMX `bx-core` closure, mantle,
current `bx-vdm` provider set, fixture and bridge with MSVC x64 `/MT`.
The historical S23/S25 files are read only as source-membership manifests;
the manifest explicitly records that no historical object is linked.

The locked byte-source input hashes and exit code are retained in
`artifacts/build/t198-s48-clean-ntio-r11/t198-s48-clean-ntio-observation.json`.
The fixture exits `2` because its old assertion intentionally expects the
pre-S47 `0000:0047` fault; that assertion is not an execution failure.

## Observed state

`run.log` records:

```text
status=4 observed-5011=1 observed-503b-resume=1 observed-drive-resume=1
observed-first-generic-ud=0 observed-stop=1 terminal=1:9346:743b
```

The bridge sets `observed_stop` immediately after a valid `50:0F` typed
resume.  The guest therefore reaches `9346:7439` (`C4 C4 50 0F`) and the
post-resume instruction position is `9346:743B`; it does not reach the former
spurious generic #UD at `0000:0047`.

OpenNT maps selector `50`, service `0F` to `SVC_DEMGETDRIVES`, dispatched by
`demdisp.c` to `demGetDrives`.  The existing source map
[`r20-dem-getdrives-contract.md`](r20-dem-getdrives-contract.md) identifies
the exact `9346:7439` call and its normal resume at `743d`.  The test bridge,
not Bochs or the service provider, elects to stop before that continuation.

## Narrow next admission

The next task may alter only the observation fixture/bridge to remove its
post-`50:0F` test stop and capture the first ensuing state.  It must retain
the same closed drive snapshot and provider disposition, and must not add a
BOP provider, device, firmware feature, or host capability merely to make the
guest progress.
