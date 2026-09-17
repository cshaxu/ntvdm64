# M0 T419 S5: management binding and compact run16 entry

## Question

Why did DTASKMGR show an unavailable BaseSrv while a product COMMAND worker
was live, and why did the historical compact spelling `run16 command/c ver`
fall through to the native host shell?

## Inputs and procedure

The compared clients were `src/dtaskmgr/main.c` and the passing focused
fixture `tests/adapter-basesrv/dtaskmgr_rpc_test.c`. The latter opens a real
self process handle with `OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
SYNCHRONIZE, FALSE, GetCurrentProcessId())`; the former had sent the
non-duplicable pseudo-handle returned by `GetCurrentProcess()`.

The formal x86 build root is
`build/M0-T419/S5/dtaskmgr-real-handle-x86-001`. Its selected product targets
were rebuilt after the two source changes. The existing authenticated fixture
was then connected to a live owner COMMAND session using its `--existing`
mode. The established clean-Console entry matrix covered `ver`, `mem`,
`command/c ver`, bare `command/c`, nested `command/c command /c ver`, and
`cmd/c ver`. The established runtime matrix covered direct MEM and interactive
COMMAND MEM/reentry; EDIT is separately retained as its native Console
interaction case.

## Repair and boundary

BaseSrv's authenticated peer-PID check is unchanged. DTASKMGR now retains one
real self-process handle for the lifetime of its binding, passes it to both
`TaskSnapshot` and `TerminateWorker`, and closes it at shutdown. The Console
heading is exactly `NTVDM Task Manager  |  BaseSrv <state>`.

Termination remains a two-step action: `K`/Delete selects the request and
`Y` confirms it. The next refresh visibly reports accepted, cancelled, or the
returned error; task disappearance remains owned by the server-side worker-exit
cleanup. The worker's own Connect attachment deliberately has only
query/synchronize rights. When a worker claims its launch reservation, BaseSrv
now takes a same-access duplicate of the launcher-prepared worker handle for
the server watch. That is the existing broker-owned termination capability;
DTASKMGR never receives it.

`run16` now recognizes a slash embedded in its first token only when the
prefix is a resolvable bare package image. It separates the image and the
first option, reconstructs the command text with a separating space, and then
calls the same selected classifier and launch path as the spaced form. This is
product CLI compatibility, not an OpenNT classifier change or a general shell
parser. A drive-qualified or backslash-qualified path is not reinterpreted,
and no COMMAND-only branch exists.

## Observations

- The live authenticated fixture returned a real row:
  `WORKER sequence=2 task=0 kind=1 state=2 image=O:\winnt\ntvdm.exe`.
  Thus BaseSrv had retained and projected the worker; the former DTASKMGR
  failure was the client pseudo-handle, not a missing broker record.
- The clean-Console entry matrix passed: `ver=0`, `mem=0`,
  `command/c ver=0`, bare `command/c=0`, nested
  `command/c command /c ver=0`, and `cmd/c ver=0`.
- The established runtime matrix passed: `direct-mem=0`, `mem-repeat=1`,
  `nested-mem=1`, `command-c=0`, and `edit=1`. These are the original COMMAND
  exit conventions already recorded by the project, not new success codes.
- The isolated BaseSrv lifecycle fixtures passed: `Check/Update/Get/Exit`,
  management worker-exit cleanup, and authenticated empty-broker
  snapshot/version-rejection/absent-termination checks.
- A controlled product `run16 command` registered one worker. The authenticated
  dtmgr fixture printed that row and reported
  `PASS: authenticated DTASKMGR RPC accepted selected live worker termination`.
  The selected `ntvdm.exe` worker exited. The test-created broker was then
  explicitly ended before the independent product matrix, so it could not
  contaminate that matrix. This directly disproves the earlier
  `ERROR_ACCESS_DENIED` path; it does not claim an idle-broker lifetime rule.
- The existing EDIT regression passed with result `1` after this repair.

## Deployment and follow-up

The selected built files were copied to `O:\winnt`:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `441683C611E3A63FA2C744C51320FC4F2EA54B2021E4F53436CCCB0E6F805F9D` |
| `basesrv.exe` | `709AFF2CE9A462A4A2A6CC92617152431DDD1A27FA5BFB769EECA7811BE49E9D` |
| `ntvdm.exe` | `F90F2D92390E30A01A5D6E23D0D1A94E6C4879B7E3F59740E0D7D307A22B0174` |
| `dtmgr.exe` | `D1390149BF739D71F01410EB62F5D542ABE460FA27EFEC4E95E8CDCBC7409EB4` |

The former `dtaskmgr.exe` was removed only after the replacement was present.
The source ownership is now `src/run16-exe/`, `src/basesrv-exe/`,
`src/ntvdm-exe/`, and `src/dtmgr-exe/`; every active build helper was rebased
to those component paths. Runtime observations are under `O:\winnt\logs`. Owner visual confirmation
of live termination remains the final T419 acceptance item; S5 does not claim
that a worker has exited until the broker refresh removes its row.
