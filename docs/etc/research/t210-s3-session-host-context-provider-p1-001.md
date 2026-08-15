# T210 S3 P1 — session host-context provider substrate

## Question

Can direct, readonly, overlay and virtual selected-drive/CWD state be owned by
one adapter session provider before the DEM and COMMAND consumers migrate to
it?

## Inputs

- T210 S2 provider design.
- The original DEM CWD/selected-drive source bodies.
- T201 mutation profile/overlay ABI.
- Existing rooted host namespace and COMMAND copied context ABI.

## Change

`bx_ntvdm_session_host_context_v1` now owns bounded per-drive state,
environment projection, profile resolution, overlay lifetime and a fresh
`bx_ntvdm_command_host_context_v1` projection.  Direct mode validates an
admitted namespace directory and then invokes the normal CLI-process
`SetCurrentDirectoryW` operation; it commits copied state only after success.
No guest pointer, raw handle, absolute host path or Bochs value enters the
record.

The work also repairs `bx_ntvdm_host_namespace_v1_directory_exists`: an
admitted root is valid for CWD validation, but is not a nonempty child path.
Previously its lower-level child-open contract incorrectly made every root
validation fail.  Nonempty paths retain the existing strict 8.3/reparse-safe
relative-open contract.

## Procedure and observation

Executed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T210S3SessionHostContextProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t210-s3-session-host-context-r5
```

The fresh MSVC x64 `/MT` compile and run passed.  The fixture uses the actual
current host drive only to construct its explicit admitted-root test fixture;
it restores the process CWD before exit.  It proves direct root validation and
commit, readonly refusal, overlay and virtual isolated state, valid bounded
environment, and consistent COMMAND copied projection.

The existing CWD service regression was also rebuilt and run successfully at
`artifacts/build/t210-s3-legacy-cwd-r1`.  This verifies that the root fix
preserves the retained legacy consumer while its planned migration remains
pending.

Earlier r1--r4 attempts are retained negative evidence.  r1 compiled but
returned 1.  The targeted diagnostic found `ERROR_PATH_NOT_FOUND` from direct
root validation; the defect was the namespace root/child conflation above,
not an unavailable Win32 API.  r5 passes after the repair.

## Interpretation and follow-up

This is provider substrate, not host-context owner-package closure.  Existing
`dem_cwd_context_v1`, `dem_cwd_service_v2`, boot-namespace plumbing and the
COMMAND current-directory fallback still have to migrate as one S3 follow-up.
No native guest trace was run or admitted.
