# Lane P Set-Info And COMSPEC Dossiers

Status: M41 design dossiers, 2026-08-08. These are Lane P-only evidence packets under M40. They do not admit an overlay implementation.

## LP-01: Set-Info Binding

| Field | Dossier |
| --- | --- |
| Historical source/function | `base/mvdm/dos/command/cmdmisc.c:591-601`, `cmdSetInfo` |
| Dispatcher/entry | Original `cmddisp.c` table entry; NTDOS `msinit.asm:346-352` supplies `DS:DX` SCS record, `DS:BX` DOS-binary byte, `DS:CX` FD-access word |
| Lane | P only; historical direct-pointer body is not preserved |
| Profile input | none; service binds guest-owned storage only |
| Required backend | copied logical-address context and M33 multi-span validation transaction |
| Historical body region | three `GetVDMAddr` borrows and retained raw pointers |

### Contract

The adapter reads the packed `SCSINFO` record at `DS:DX` as copied bytes and preflights three writable bindings: `SCS_ToSync` at packed offset 206, the byte at `DS:BX`, and the word at `DS:CX`. `SCSINFO` is exactly `64 + 128 + 14 + 1` bytes under source pack(1). All four ranges must translate through ordinary RAM in one transition epoch.

Success stores opaque guest-address bindings and their epoch; it changes no guest byte and no register. A later command transaction revalidates every binding rather than retaining a pointer. Reset, resume, stop, fault, or session destruction clears them.

### Failure

The original function has no error return. A malformed record, provider/ROM/hole overlap, overflow, failed translation, or stale epoch therefore becomes the declared Lane P transition fault/stop result. It retains every previous binding and does not report service success.

### Fixtures

- valid disjoint bindings retain opaque addresses only;
- a bad final byte/word retains all previous bindings;
- a record crossing a non-ordinary route fails without guest mutation;
- reset/resume invalidates bindings;
- trace asserts original dispatcher/function entry and `lane_p` identity.

## LP-02: COMSPEC Announcement

| Field | Dossier |
| --- | --- |
| Historical source/function | `base/mvdm/dos/command/cmdmisc.c:635-650`, `cmdComSpec` |
| Dispatcher/entry | Original table entry; permanent COMMAND invokes it from `dos/v86/cmd/command/init.asm:1121-1128` with `DS:DX` |
| Lane | P only for copied string transport and fixed terminal policy |
| Profile input | selected guest COMSPEC DOS path and declared terminal-pause result |
| Required backend | bounded copied logical guest read at current transition epoch |
| Historical body region | `GetVDMAddr` string operations plus direct process-global command metadata |

### Contract

The adapter reads a NUL-terminated path from `DS:DX` through a bounded copied guest read. It must match the profile-selected guest COMSPEC path under declared fixed-profile comparison rules. The path excluding NUL is at most 63 bytes because later original `cmdexec.c:249-250` copies it into `SCS_ComSpec[64]`.

On success the adapter records only session metadata equivalent to source `COMSPEC=` value and length. It performs no host environment update. It stages the profile-declared `AL` terminal-pause result as the sole register output; no other GPR, flag, mode, or segment state changes.

### Failure

Missing NUL within the bound, malformed guest address, profile mismatch, overlength path, stale epoch, or unavailable terminal capability yields Lane P fault/stop before metadata or register mutation. It may not substitute host `COMSPEC`, system-directory search, or another guest command path.

### Fixtures

- matching bounded path records one redacted role/length fact and commits AL;
- mismatch and overlength preserve metadata and registers;
- malformed/non-ordinary input has no mutation;
- terminal result changes only AL;
- trace proves original dispatcher/function entry, `lane_p`, and no host environment access.

## Shared Isolation

Both dossiers require M36/M37 before any implementation. Future overlays are adapter-local, default-disabled, absent from Lane H and generic runner links, and may not invoke a selector through a parallel dispatcher.
