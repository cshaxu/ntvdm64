# T210 S1 — CLI host-context owner-package map

## Scope

This owner package is the selected-drive, current-directory and environment
state shared by DEM and COMMAND.  It is not an implementation task for the
single observed `50:1A` BOP.

| Original service/owner | Contract | Current route/workaround | Target disposition |
| --- | --- | --- | --- |
| DEM `50:13`, `demdir.c:demQueryCurrentDir` | Validate `AL` drive root and CDS path; invalid CDS resets to `X:\`, updates `=X:` and clears CF; invalid drive takes `demClientError` | `dem_cwd_service_v2` with adapter-private CWD context and synthetic/contained namespace state | original+seam direct selected-drive context; readonly/overlay/virtual through common profile |
| DEM `50:18`, `demdir.c:demSetCurrentDir` | Read `DS:DX` OEM DOS path; `SetCurrentDirectoryOem`; set `=X:`; source error/CF path on failure | source-derived CWD context updates; no direct host current-directory backend | original+seam with copied bounded path and session-owned Win32 directory context |
| DEM `50:1A`, `demgset.c:demSetDefaultDrive` | Verify `DS:SI` path drive agrees with `DL`; set current directory in one operation because NT has one thread CWD; set CF/AX=1 for mismatch | same CWD context; fixture reaches the source-shaped non-success result at exit `197` | same owner package as `13/18`, never an isolated default-drive patch |
| COMMAND `54:02`, `cmdmisc.c` COMSPEC bootstrap | consumes copied host context to establish command-side context | `command_host_context_v1`, adapter-owned copied record | preserve as a consumer; no ambient recapture |
| COMMAND `54:04`, current-directory response | publish selected context directory through bounded guest write | `cmd_current_dir_service_v1` falls back to synthetic root when no copied context | migrate to one selected-drive/CWD source of truth |
| COMMAND `54:0F`, environment bootstrap | publishes bounded double-NUL environment | copied COMMAND host context / bootstrap state | migrate environment projection to the same session host-context owner |

## Original source and modern API availability

| Historical surface | Modern normal-process verdict | Permission/scope | Treatment |
| --- | --- | --- | --- |
| `SetCurrentDirectoryOem` -> `SetCurrentDirectoryW` / `RtlSetCurrentDirectory_U` | available through public Win32 `SetCurrentDirectoryW` | changes only this CLI process's current directory; normal filesystem ACL/share errors apply | usable through a session-owned capability; no host installation or registry mutation |
| `GetFileAttributesOem` -> `GetFileAttributesW` | available | selected path and ordinary access/error state | direct CWD validation / DOS-error mapping |
| `SetEnvironmentVariableOem` -> `SetEnvironmentVariableW` | available | process environment only | do not mutate ambient process state per BOP; carry `=X:` state in the bounded session and publish through the original caller contract |
| `Get/SetEnvironmentVariable` and environment block APIs | available | process/child context; size/error handling required | CLI admission captures an explicit environment; COMMAND receives a copied bounded block |
| `GetLogicalDrives`, `GetDriveTypeW` | available | ordinary user-mode enumeration | already captured through CLI include/exclude policy; no ambient guest drive access |

No required API is deleted or unavailable.  There is no `ntvdm64` or host
system rebuild dependency.  The unavailable historical CCPU/SAS guest-pointer
operations remain replaced only by existing checked bx-vdm gathers/writes.

## Profile and ownership matrix

| Mode | CWD state owner | Host effect | Required result shape |
| --- | --- | --- | --- |
| direct | session-owned selected real drive plus bounded path | selected Win32 CWD operation after validation | original DOS success or mapped Win32 failure |
| readonly | same session CWD only when pointing to admitted root; no mutation of backing host files | no host mutation | original/profile rejection, never synthetic success |
| overlay | overlay-owned per-drive CWD records | no backing-host write | overlay state result with ordinary DOS path validation |
| virtual | virtual-volume CWD records | no real drive operation | virtual profile result only |

The host process CWD is not an ambient source of truth.  A CLI admission
captures/validates selected context once; the host-context provider owns the
copied paths and the policy-selected Win32 operation.  DEM and COMMAND are
consumers, and bx-core/bx-mantle never see a drive, path, BOP or OpenNT name.

## Current workaround action

- Retain the fixed-width `command_host_context_v1` record as a consumer ABI.
- Migrate synthetic-root answers and duplicate `dem_cwd_context_v1` state
  into one session host-context provider.
- Replace per-service fallback failures only through the complete `13/18/1A`
  package provider.
- Keep virtual boot volume strictly opt-in; it must not replace selected real
  C: in direct mode.

S2 is admitted only after this map: it will define the shared provider state,
Win32 seam, profile enforcement and full `13/18/1A` plus COMMAND consumer
regression.  A trace is allowed only once that package-level result exists.

