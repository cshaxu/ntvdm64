# T200 S27: native CLI-to-engine handoff

## Boundary

The retained `src/cli/ntdos64_run.c` launches a separate legacy engine and
passes profile/root/launch/drive information in `NTDOS64_ADAPTER_*`
environment variables.  `src/cli/ntdos64_bochs_engine.c` then validates a
bundle and starts `bochs.exe`.  That route is retained evidence, not the
native x64 composition.

`src/cli/ntdos64_native_cli.c` is the new direct path.  It owns only CLI
argument parsing, BYOB profile/target admission, launch-plan encoding, and
drive-policy parsing.  It copies those values into
`bx_ntvdm_engine_request_v1`, calls `bx_ntvdm_engine_run_v1`, prints its typed
terminal, and maps only the engine terminal category to process success or
failure.  It does not use a child process, environment transport, Bochs
configuration, or guest-memory access.

## Reproducible result

The following command compiled the CLI, engine entry, current composition
objects, and admitted Bochs backend closure under MSVC x64 `/MT`, then ran the
admitted source-built `TARGET.EXE` profile:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S27NativeCliProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s27-native-cli-r3 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s26-current-composition-r1
```

It passed with:

```text
ntdos64-native: terminal=5 detail=1
```

The probe record explicitly retains `environmentTransport=false` and
`legacyBochsShell=false`.  Terminal `5/1` is the current bounded controlled
COMMAND exhausted-plan result established by S26; it is not a DOS exit-code
transport or a general compatibility claim.

## Limit

The legacy MinGW runner remains retained and is not rewritten by this S.  The
new CLI is an independently reproducible native path, not yet a broad
distribution target, interactive shell, WOW path, or normal DOS-result ABI.
