# Contained DOS Namespace Dossier

Status: M46 design dossier, 2026-08-08. This is the resource contract for the
M43-M45 one-command Lane P path. It does not implement a filesystem, alter
DEM dispatch, or authorize an ambient host drive mapping.

## Purpose And Boundary

The first runnable profile needs a DOS-visible namespace, but not a general
PC filesystem. Its sole job is to make already-selected BYOB roles visible to
the original NTDOS/DEM file path after the correct historical loader owns each
transition:

```text
NTIO preload                         -> NTIO owns execution
NTDOS selected resource              -> original demLoadDos owns load at DI
COMMAND.COM selected resource        -> original NTDOS/DEM open and EXEC
CONFIG.SYS profile resource           -> original NTDOS open/read/parse
AUTOEXEC.BAT profile resource         -> original COMMAND open/read
one .COM/.EXE command resource        -> original COMMAND/NTDOS open and EXEC
```

The namespace neither exposes host paths to the guest nor makes a host file
operation a DOS operation outside original DEM ownership.

## Source And Prior-Contract Basis

| Requirement | Authority |
| --- | --- |
| NTDOS remains a live-DI `SVC_DEMLOADDOS` transition, not a preload. | `BYOB-BOOTSTRAP-RESOURCE-OWNERSHIP.md`; `HISTORICAL-DEMLOADDOS-RESOURCE-BRIDGE-DESIGN.md` |
| Normal files retain original DEM dispatch/SFT/PSP/EXEC ownership. | `HISTORICAL-DEM-TOKEN-BRIDGE-DESIGN.md` |
| The x64-safe normal-file minimum is original `demOpen`, `demRead`, `demChgFilePtr`, and `demClose` using opaque 32-bit tokens. | `HISTORICAL-DEM-TOKEN-BRIDGE-DESIGN.md` |
| A read never receives a raw guest RAM pointer. | `DEM-READ-GUEST-MEMORY-BRIDGE-ADMISSION.md` |
| CONFIG/AUTOEXEC and first target are declared logical resources, not host discovery results. | M43, M44, M45 dossiers |

## Immutable Mount Model

One session owns one immutable table of DOS path to resource-role bindings.
It is constructed after BYOB validation and before any guest instruction. A
binding records only:

```text
canonical DOS path, role, resource identity (hash/size), read-only byte source
```

It does not retain a guest buffer, a raw host handle, an ambient current
directory, a host drive letter, a device path, or a host path visible to the
guest. The resource loader may retain a private rooted location only long
enough to reopen and revalidate the selected file, as M30 already requires.

The initial profile has exactly one declared drive. Its root and every path
use the M43 8.3-safe canonical OEM grammar, are case-normalized under a fixed
OEM profile rule, contain no wildcard/device/`.`/`..` component, and resolve
only by exact table lookup. Long-name fallback, DOS PATH search by the host,
current-directory inheritance, UNC, volume GUID, reparse-point traversal, and
physical-drive syntax are forbidden.

The table contains at minimum:

| Role | DOS visibility | Selection rule |
| --- | --- | --- |
| `ntio` | not a normal file before its entry preload | fixed BYOB identity |
| `ntdos` | not a normal file before original `demLoadDos` | fixed BYOB identity |
| `command` | selected fixed COMSPEC path | fixed BYOB identity |
| `config` | M43 declared CONFIG path | fixed profile resource |
| `autoexec` | M43 declared AUTOEXEC path | fixed profile resource |
| `target` | M44 declared absolute `.COM` or `.EXE` path | one session CLI/profile request, verified below selected root |

The `target` request must be normalized and bound before guest execution. A
CLI spelling is input to profile construction, never a guest-visible host path.
The future runner may let the user choose a rooted mount and a target below it,
but must reject root escape before it creates this table.

## Original DEM File-Operation Join

For normal file roles, only the following original-operation bridge is
admitted:

| Original owner | Namespace action | Result transport |
| --- | --- | --- |
| `demOpen` | exact canonical lookup and read-only token open | existing `AX:BP` opaque token and existing file-size register order |
| `demRead` | read token into host-owned staging | M29 all-or-none guest write transaction |
| `demChgFilePtr` | seek token within immutable resource | existing DEM register result/error path |
| `demClose` | retire token | existing DEM result/error path |

`demLoadDos` is intentionally separate: it uses the selected `ntdos` resource
and its live `DI` placement transaction, as M31 defines. It is never modelled
as a normal namespace `OPEN`.

All open modes other than the source-proven read-only normal-file mode fail
through the original DEM error ownership. File create, write, truncate, lock,
commit, date/time mutation, FCB I/O, pipes, printer/serial/console device
names, directory enumeration, wildcard search, and media/block I/O have no
binding and no fallback. A first command reaching one is an explicit trace
stop, not a reason to add a private substitute.

## Error, Lifetime, And Diagnostics

An absent path, a role identity mismatch, unsupported access intent, failed
root revalidation, token exhaustion, cancellation, or transaction failure
must enter the existing original DEM error path with its normal DOS-level
ownership. The adapter does not return a fabricated success, pass a token as a
native handle, show an old dialog, or alter SFT/PSP state.

Tokens are per-session, opaque, and invalid after close/reset/stop. Resource
bytes are private to the immediate original operation and are discarded after
that operation. Traces record service/role identity, hash prefix, length,
read/seek count, disposition, and stop category; they never record guest image
bytes, absolute host paths, command contents beyond an approved redacted
identity, or host handle values.

## Preconditions And Fixtures

Runtime admission still requires the M29 guest-write transaction and a real
execution backend capable of reaching original DEM. This dossier neither
weakens M36's core-adoption gate nor permits a local executor.

- valid role opens produce opaque tokens and only original DEM register
  results;
- reads/partial reads/seeks/closes preserve M28/M29 atomicity and token scope;
- an invalid DOS path, case/normalization mismatch, root escape, missing role,
  write intent, device name, or stale token reaches a bounded error/stop;
- `ntdos` can only be consumed by the original `demLoadDos` fixture, while
  COMMAND/config/autoexec/target can only use the normal DEM fixture;
- no fixture uses host drive discovery, process current directory, registry,
  system directory, temporary files, named pipes, or direct device APIs.

## Deferred

More drives, writable scratch media, CONFIG device drivers, batch command
search, directory enumeration, `PATH`, redirection, LFN, network redirector,
mouse/keyboard files, and all external Win32 program handoff remain separate
capability dossiers. They must demonstrate their original owner and containment
boundary before joining this namespace.
