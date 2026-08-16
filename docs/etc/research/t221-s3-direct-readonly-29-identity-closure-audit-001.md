# T221 S3 P68 — Direct/Readonly 29-identity closure audit

## Question

Does the admitted `dem_whole_provider` package satisfy the T221 exit matrix for
all 29 selected DEM identities in Direct and Readonly, while retaining—not
extending—the frozen Overlay/Virtual seams?

## Inputs and method

The original-owner, ABI and recovery-rung analysis is the T221 S1
[DEM recovery ledger](t221-s1-dem-recovery-ledger-001.md).  This audit maps
those owners to the final normal package-session fixture and P57--P67 family
evidence.  The fixture is compiled as one MSVC x64 `/MT` composition; it
exercises the same registry, ingress and provider session rather than calling
per-service helpers directly.

| Owner family | Identities | Direct disposition/evidence | Readonly disposition/evidence |
| --- | --- | --- | --- |
| `demfile.c`, `demdir.c` namespace/path | `01,03,04,05,06,12,17,22,44` | self-owned create/write/delete, create-new, rename, metadata, mkdir/rmdir and check-path succeed (P57/P59) | declared-image open/query succeeds; all mutations reject `CF/AX=5` (P58/P63) |
| `demhndl.c` handles | `00,02,08,16,1E,27` | seek/read/write/time/commit/close succeed on opaque direct tokens (P57/P65/P67) | valid seek/read/time/commit/close succeed; write/set-time reject `CF/AX=5`; stale seek rejects `CF/AX=6` (P65--P67) |
| `demsrch.c` search | `09,0A,0B,0C` | pathname and FCB first/next continuation are checked; no-match is `CF/AX=18` (P61/P63) | same read-view continuation and no-match contract (P61/P63) |
| `demfcb.c` wildcard/file state | `07,20,2C,2D,2E,2F,30,31` | wildcard mutation, create/open/read/write/close/date/info use the selected direct view (P60/P62/P64) | read/open/close/date/info retain admitted view; create/wildcard/write reject before host mutation with `CF/AX=5` (P60/P62/P64) |
| `demhndl.c` + Redirector boundary | `47,48` | explicit unavailable/invalid-token terminal; no local handle is a pipe | same explicit unavailable terminal; no fallback |

## Observation

The final complete-family command was:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\171-direct-readonly-handle-matrix `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The focused fixture linked and its executable returned exit `0`.  It also
retains its pre-existing typed Overlay/Virtual cases, but this P adds no code
or assertion that changes either profile's behavior.

## Conclusion

All 29 selected identities have a source-reviewed owner, a single
`dem_whole_provider` route, and an explicit Direct/Readonly result in the
normal package-session regression.  Host pointers and raw Win32 handles remain
private; tokens are generation-checked; guest transfers remain checked; pipes
remain Redirector-deferred.

This closes **T221 S3's Direct/Readonly matrix only**.  It does not claim a
native guest trace, Redirector/pipe implementation, raw device/GSET recovery,
JFT/SFT reconstruction, or an Overlay/Virtual resumption.  The latter remains
recorded in `TODO.md` for separate admission.
