# Contained Namespace And Command Service Ledger

Status: M24 source-derived command-service contract record, 2026-08-08.

## Scope

This ledger turns the M22/M23 command cut into explicit historical guest
buffer and disposition requirements. It is a derivation record, not an
implementation of command services and not a license to expose raw Win32
handles, ambient directories, or historical global session state.

## Fixed Historical Command ABI

`src/opennt/base/mvdm/inc/cmdsvc.h:16-34` fixes the command service selector
set. The first-cut relevant selectors are:

| Selector | Historical role | First-cut disposition |
| --- | --- | --- |
| `SVC_GETCONFIGSYS` (12) | write CONFIG filename to guest buffer | required, contained synthetic path |
| `SVC_GETAUTOEXECBAT` (13) | write AUTOEXEC filename to guest buffer | required by COMMAND initialization, contained policy path |
| `SVC_GETINITENVIRONMENT` (15) | size/copy environment into guest segment | required |
| `SVC_CMDCOMSPEC` (2) | announce permanent guest COMSPEC | required metadata acknowledgement |
| `SVC_CMDGETNEXTCMD` (1) | fill first-command communication record | required |
| `SVC_CMDGETCURDIR` (4) | return a drive's current directory | required: NTDOS requests it when a CDS needs synchronization |
| `SVC_CMDSETINFO` (5) | register SCS/binary/FDACCESS guest bindings | required before command protocol; bindings must be checked, not raw pointers |
| `SVC_GETSTDHANDLE` (6) | obtain historical standard-handle bridge | defer until the command's redirection/stream path reaches it |
| `SVC_CMDINITCONSOLE` (9) | historical console/event setup | required selector acknowledgement, but not an event-thread/UI requirement |
| `SVC_GETKBDLAYOUT` (14) | decide whether command installs KEYB.COM | required fixed-profile response: no additional layout installation |
| `SVC_RETURNEXITCODE` (11), `SVC_CMDEXITVDM` (0) | report completion/termination | required bounded runner outcome |

`SVC_CMDCHECKBINARY` (7) and `SVC_CMDEXEC` (8) are a separate native-image
classification/handoff path. They are not bootstrap requirements and are not
included in the first DOS-command service surface.

## Guest Communication Records

### Configuration And AUTOEXEC Paths

Both historical functions receive `DS:DX` pointing to guest storage.
`cmdGetConfigSys` and `cmdGetAutoexecBat` document a 64-byte minimum receiver
in `src/opennt/base/mvdm/dos/command/cmdconf.c:51-120`. The old host expands
files into temporary Windows-directory files, converts their filenames to OEM,
and writes those names through `GetVDMAddr`.

A future profile must instead prevalidate the guest buffer and atomically
write a bounded, NUL-terminated contained DOS pathname. That pathname denotes
a session-local policy object, not a host file path. It must be stable for the
whole boot; missing/empty policy must have an explicit DOS-visible outcome.

### Initial Environment

`cmdGetInitEnvironment` (`cmdenv.c:171-330`) receives `ES:0000` and `BX` as
the destination size in paragraphs. The historical response is a retry-style
size protocol:

```text
BX == 0                 no supplied environment
BX <= supplied capacity success, bytes copied at ES:0000
BX > supplied capacity  no success copy; BX is required paragraphs
```

The copied data begins with the permanent COMSPEC entry and is followed by a
double-NUL terminated DOS environment. Historical code builds this from the
ambient process environment, removes/reformats selected entries, and performs
OEM conversion. The modern profile must supply an immutable pre-encoded
environment record, including the validated guest COMSPEC, and never enumerate
or mutate the host process environment.

### First Command

`CMDINFO` in `cmdsvc.h:96-121` is a packed guest communication record. It
contains at least:

```text
EnvSeg, EnvSize, CurDrive, NumDrives,
CmdLineSeg, CmdLineOff, CmdLineSize, ReturnCode,
bStdHandles, pRdrInfo, CodePage, fTSRExit, fBatStatus,
ExecPathSeg, ExecPathOff, ExecPathSize, ExecExtType
```

`cmdGetNextCmd` receives a pointer to that record at `DS:DX`
(`cmdmisc.c:46-63`). It may negotiate environment capacity through carry and
`AX` (`cmdmisc.c:208-225`), then writes command text via guest segment:offset
pointers, updates the command/environment fields, and sets carry clear on
success (`cmdmisc.c:284-364`). The historical implementation imports BASE
command/session state, PIF behavior, ambient directories, and raw standard
handle data. Those are explicitly excluded from the research profile.

The first-cut profile therefore needs one validated immutable command record:

- fully qualified DOS path and a command tail within the historical guest
  buffer limit;
- selected drive and current directory drawn from the contained namespace;
- declared code page for fixed EN-US behavior;
- no PIF, batch-reentry, shell-out, or repeat-command behavior;
- a profile-local stream description in place of a host pointer/handle.

All guest pointer ranges must be checked before mutation, and a retry response
must leave command text/metadata unmodified. This is a future generic
checked-memory transaction requirement, not a license for arbitrary service
handlers to modify CPU mode or machine state.

### Current Directory

Historical `cmdGetCurrentDir` takes `DS:SI` and `AL` (drive index), returns a
directory or carry-set `AX=0/1` errors (`cmdmisc.c:517-577`). It falls back to
environment entries and `GetDriveTypeOem`. The contained replacement must
answer only from M19's immutable namespace record: known mounted drive and
declared current directory succeeds; unknown/unmounted drive has the documented
carry-set failure. It must not query host drive topology or update host
environment variables.

### Completion

`cmdReturnExitCode` accepts `DX` exit code, `AL` current drive, and `BX:CX`
redirection-info pointer (`cmdexec.c:590-649`). Its historical carry-set result
means re-enter with another command; carry clear ends a shell-out cycle.
`cmdExitVDM` simply terminates the historical process (`cmdexit.c:16-30`).

For the bounded single-command profile, the only valid completion outcomes are
`completed(exit_code)`, `cancelled`, or `faulted`. No reentry/shell-out is
admitted. The runner captures the guest-reported exit code without a historical
event thread, console title change, or process-global termination.

## Stream Boundary

The original `SVC_GETSTDHANDLE` expects a `PREDIRCOMPLETE_INFO` pointer encoded
in `AX:BX`, selector in `CX`, and returns a raw 32-bit handle split over
`BX:CX` plus file size in `DX:AX` (`cmdredir.c:225-315`). That ABI cannot be
directly retained on a modern 64-bit host without pointer truncation and
ownership hazards.

It is therefore explicitly **not** a bootstrap convenience API. A later shim
must introduce a profile-owned stream capability that has fixed ownership,
lifetime, error, and redaction rules, then translate it at the narrow historical
boundary only after exact DOS redirection callers are proven. The initial
command fixture may avoid redirection and use a minimal console/device path;
it may not forge a host handle value.

## Boundary Result

M24 confirms a narrow division:

- profile/mantle owns named DOS paths, environment bytes, first command,
  namespace records, stream policy, and completion policy;
- a future generic execution core owns only checked guest memory, constrained
  register/result commits, transition/resume, and stop/fault reporting;
- historical command ABI adapters own selector-specific packing only and must
  remain outside core;
- native Win32 process dispatch, stream redirection, PIF/WOW, registry,
  keyboard/mouse/EMS/HMA, and shell-out remain separate later cuts.

The next research task is to trace the DOS file/open/EXEC implementation that
consumes these records, so the contained namespace can be stated as real DOS
operations and failure semantics rather than merely named paths.
