# T210 S2 — session host-context provider design and regression admission

## Question

What single adapter-owned provider can preserve the reachable OpenNT
selected-drive/current-directory/environment contract for the DEM and COMMAND
owner packages without giving either package an ambient process-state source,
creating a separate readonly/shadow implementation, or exposing a host handle
to guest code or Bochs?

## Inputs

- The complete `50:13`, `50:18`, and `50:1A` bodies in
  `src/opennt/base/mvdm/dos/dem/demdir.c` and `demgset.c`.
- The T210 S1 owner map and the completed T200/S31 mutation/disposition
  ledger.
- The completed T201 profile ABI and overlay lifecycle.
- Existing adapter records: `bx_ntvdm_command_host_context_v1`,
  `bx_ntvdm_dem_cwd_context_v1`, `bx_ntvdm_dem_cwd_service_v2`, host-drive
  policy, and host namespace.

## Source contract

`demQueryCurrentDir` validates the requested drive root, checks the CDS path,
and resets an invalid CDS path to `X:\\` while updating `=X:`.  It reports an
invalid drive through the normal DEM error path.  `demSetCurrentDir` reads a
full OEM drive path, calls `SetCurrentDirectoryOem`, then updates `=X:`.
`demSetDefaultDrive` verifies that `DS:SI[0]` agrees with `DL + 'A'`; on a
mismatch it sets CF and AX=1.  Its one-shot current-directory call exists
because the historical NT host has one thread current directory.

The modern equivalents are public `SetCurrentDirectoryW`,
`GetFileAttributesW`, `GetLogicalDrives`, `GetDriveTypeW`, and environment
block APIs.  They require only ordinary user-mode permissions.  None is a
removed/private Win32 API, and none requires a system-file, kernel, service,
or installation-registry change.

## Provider boundary

S3 will introduce one `bx_ntvdm_session_host_context_v1` provider in
`src/bx-vdm`.  It is same-island, session-owned state—not a Bochs bridge
record—and has exactly these owned values:

| State | Source | Lifetime | Consumer |
| --- | --- | --- | --- |
| admitted drive mask and roots | CLI policy plus host-drive snapshot | session | DEM namespace/path consumers |
| selected drive and one bounded relative CWD per admitted drive | explicit CLI admission and successful provider operation | session | DEM `13/18/1A`, COMMAND `54:04` |
| bounded projected environment, including the logical `=X:` entry | CLI admission plus successful provider operation | session | COMMAND `54:0F`, DEM query/set logic |
| copied COMMAND projection | generated from provider state | per publication | COMMAND `54:02/04/0F` |
| common mutation profile and overlay token/lifetime | T201 profile registration | session | all operations |

It owns no BOP selector, guest pointer, host pathname pointer, raw `HANDLE`,
Bochs object, callback, or externally retained allocation.  DEM receives an
already-classified service byte and bounded gathered input; COMMAND receives a
fresh copied projection.  `bx-core` and `bx-mantle` do not see any drive,
path, environment, BOP, DOS, or OpenNT term.

## Win32 seam and mode enforcement

The provider has a narrow private Win32 seam.  CLI admission captures the
explicit initial drive/environment projection once; no BOP recaptures the
ambient current directory or environment.  For direct mode, a validated,
admitted full path may be submitted to `SetCurrentDirectoryW`, and the
provider updates its copied state only after that call succeeds.  It maps the
returned Win32 error through the existing DEM error/result path.  The process
operation is intentional normal CLI host integration, not host installation
or an implicit startup side effect.

| Mode | `13` query/reconcile | `18` set CWD | `1A` set drive/CWD | COMMAND projection |
| --- | --- | --- | --- | --- |
| direct | validate admitted root/path with host namespace; reset stale CDS to root | validate then issue the one explicit Win32 CWD operation | reject `DL`/path mismatch with CF+AX=1; otherwise same direct operation | copied provider state |
| readonly | validate admitted state; no backing-file mutation | reject according to profile; no synthetic success | same rejection after mismatch check | copied admitted state only |
| overlay | provider/overlay-owned CWD state | update overlay state after normal DOS path validation | update selected drive and overlay CWD | copied overlay projection |
| virtual | virtual-volume state only | virtual provider operation only | virtual provider operation only | copied virtual projection |

The default-off virtual boot volume remains an explicit virtual profile
feature; it is never an implicit replacement for an excluded real C:.

## Migration actions

| Existing surface | S3 action | Reason |
| --- | --- | --- |
| `bx_ntvdm_command_host_context_v1` | retain as the copied COMMAND consumer ABI | bounded, pointer-free publication record already matches the boundary |
| `bx_ntvdm_dem_cwd_context_v1` | migrate its per-drive state into the provider, then delete or reduce it to a private compatibility wrapper | it duplicates session CWD/profile ownership |
| `bx_ntvdm_dem_cwd_service_v2` | replace its context/namespace dependency as one `13/18/1A` provider integration | preserves its checked gathers/transaction shape but removes endpoint-local policy |
| `cmd_current_dir_service_v1` synthetic-root fallback | delete | a missing context must not fabricate a directory success |
| host namespace/root handles | retain adapter-private and use only for admitted root/path validation | it is the existing checked direct-host capability seam |

No S3 change may add a selector-specific profile choice, mutate Bochs,
invent DOS filesystem semantics, or preserve a synthetic success fallback.

## Whole-family regression admission

S3 must source-build a single MSVC x64 `/MT` provider fixture and one bound
DEM/COMMAND integration fixture.  It must cover:

1. Direct-mode `13`, `18`, and `1A`: root, valid child, stale CDS reset,
   invalid/excluded drive, `DL`/path mismatch (CF+AX=1), ordinary Win32
   failure mapping, and no state commit on failure.
2. Readonly, overlay, and virtual modes for the same three services, proving
   profile owner/class enforcement and that direct host mutation is neither
   attempted nor silently reported as success.
3. One source of truth: a successful provider change appears identically in
   COMMAND `54:02`, `54:04`, and `54:0F` copied publication; no consumer
   recaptures ambient state or uses a synthetic root.
4. Bounded OEM conversion, bad guest strings, gather/aperture failure,
   non-admitted drive, `..`/reparse escape rejection, environment capacity,
   double-NUL validity, and no host handle/pointer in guest-visible output.
5. Release/teardown zeroing of owned copied state and the T201 overlay
   lifetime.  The tests must not need a native execution trace.

Only after this package regression passes may one native trace be used as an
integration observation; it cannot create another endpoint task.

## Procedure and observations

Read the original DEM bodies and the current adapter implementation, then
compared state ownership against the fixed T201 owner/class ABI.  The current
adapter has bounded gather/write mechanics and a COMMAND copied-record
consumer, but it splits CWD state across `dem_cwd_context_v1`, host namespace,
and a synthetic COMMAND fallback.  That split is the migration target; it is
not evidence that the whole owner package is already recovered.

## Interpretation and confidence

High confidence.  The historical source gives the ordering and failure
contract, and the required Win32 surface is public and present.  The proposed
provider unifies only session host context; it neither expands Bochs nor
implements a DOS kernel.  S3 is now bounded implementation work.  Other DEM,
COMMAND, Redirector, XMS, DPMI, Debugger, WOW and machine packages remain
separate owner packages.

## Follow-up

Admit T210 S3 for this provider and its complete `13/18/1A` plus
`54:02/04/0F` regression.  Do not use trace to choose individual services.
