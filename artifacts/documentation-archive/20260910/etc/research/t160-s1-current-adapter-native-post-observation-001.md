# T160 S1 current-adapter native POST observation 001

## Question

Does the T159 current-adapter deferred composition build within the exact
two-object Bochs limit and reach native-POST guest execution?

## Inputs and procedure

`New-T98S1CurrentAdapterEngineDerivative.ps1 -DeferredStartupPlan` generated
`artifacts/build/current/t160-native-post-current-adapter-r1`. Before linking,
its manifest and makefile proved exactly `main.o` and `cpu\\exception.o`, the
direct/deferred pair `0/1`, and no archive/device prerequisite. One MSVC/x86
link produced a 2,580,480-byte executable with SHA-256
`F1CAE4F9A3E0ACD9029F9F0CB79B98A3998A49A1A8B8954FDD490314B17694C9`.

One deferred run used profile SHA-256
`3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982`, CA800
option-ROM SHA-256
`E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB`, a
one-million benchmark, and a 15-second watchdog. No second run occurred.

## Observation

`observation.json` records `watchdogTerminated: false`, `exitCode: 1`, 31 BOP
observations, and seven commits. There is neither a pre-CPU execution-plan
marker nor an aperture panic. The relevant ordered path is:

```text
50:11 -> NTDOS transaction (0x6cd2 bytes)
... normal DOS/COMMAND flow ...
50:42 -> 0x003e-byte fast read
50:42 -> 0xc4d0-byte full COMMAND.COM read
50:02 -> close
50:36 -> accepted resume at 0032:5a70
cpu loop quit -> benchmark exit 1
```

The terminal source sweep after this run exceeded its ten-second command
limit, but did not affect the build/run. Existing T141/T152 records classify
`50:36` as `SVC_DEMENTRYDOSAPP`, a guest-transfer boundary, not a new adapter
provider request.

## Interpretation and follow-up

T160 proves the present T154 adapter links into native POST/deferred lifecycle
and reaches normal source-owned execution after full COMMAND materialization.
It excludes the T158 direct-before-POST failure. It neither proves CLI
completion nor justifies a provider/device repair. T161 may make one longer,
unchanged-binary observation to locate the first post-`50:36` boundary.
