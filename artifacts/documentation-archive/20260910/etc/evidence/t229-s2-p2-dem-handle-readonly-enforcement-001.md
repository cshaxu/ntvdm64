# T229 S2 P2 — DEM handle mutation Readonly enforcement

## Question

Do the local-file implementations of `50:08 demFileTimes` (set-time) and
`50:1E demWrite` reject Readonly at the shared DEM owner boundary, or merely
rely on a Win32 handle to fail after a host mutation API has been attempted?

## Source contract

`src/opennt/base/mvdm/dos/dem/demhndl.c` defines:

- `demFileTimes`: `BL=1` converts `DX:CX` DOS date/time and calls
  `SetFileTime`; ordinary failure is routed through `demClientError`.
- `demWrite`: performs `WriteFile`, and the zero-length case performs the
  equivalent EOF mutation.

Both are host-side mutations. In this CLI, policy is an explicit product
requirement: Readonly must deny them without depending on ambient handle
permissions. The original ordinary access-denied result remains `AX=5, CF=1`.

## Repair

`src/bx-vdm/bx_ntvdm_dem_handle_partition_v1.c` now resolves the shared file
view before either host mutation path:

- `demFileTimes` set-time requires `FILE_METADATA` admission.
- `demWrite`, including the zero-length truncate form, requires
  `NAMESPACE_CONTENT` admission before seek, `WriteFile`, or `SetEndOfFile`.

Direct retains the original host operation. Overlay/Virtual do not gain
behavior from this change: their private paths remain selected by the existing
whole-provider routing. (The spelling in this sentence is descriptive only;
no Virtual behavior is admitted.)

## Verification

The formal whole-provider fixture owns the temporary file and invokes each BOP
through ingress, provider selection, session and mechanical transport:

| Service | Direct observation | Readonly observation |
| --- | --- | --- |
| `50:08`, `BL=1` | timestamp set succeeds | `AX=5, CF=1` before `SetFileTime` |
| `50:1E`, 3 bytes | file becomes `TWO` | `AX=5, CF=1`; file remains `ONE` |

Commands run on 2026-08-18:

```powershell
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b bin/t225-s19-dem-package-lifecycle-fixture.exe
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct pdb-lifecycle
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly pdb-lifecycle
```

All exited `0`; `git diff --check` passed.

## Scope limit

P2 closes only the observed missing policy gates. It is not evidence that the
complete handle or DEM owner package is finished. File open/read/seek/commit,
FCB variants, namespace mutation variants and search still require their
complete owner-family conformance sweep.