# r23 FCB Search Capability Boundary

## Trigger and owner

The next observed source-built NTIO boundary is `C4 C4 50 0A`:
`SVC_DEMFINDFIRSTFCB`.  `host/src/nt_bop.c` routes the `50` selector through
`DemDispatch`; OpenNT's `demdisp.c` maps service `0x0a` to
`demFindFirstFCB` in `dos/dem/demsrch.c`.  Its paired continuation is
`SVC_DEMFINDNEXTFCB` (`0x0c`).  It is therefore not an independently useful
one-shot query.

## Source-derived contract surface

`demFindFirstFCB` receives:

- `ES:DI`: wildcard-bearing OEM full path;
- `DS:SI`: packed guest `SRCHBUF` destination/state;
- `AL`: ordinary versus extended FCB selector;
- `DL`: search attributes.

It converts the OEM path, filters attributes, allocates or retains a host
search entry, fills the FCB result, stores an opaque search identity in guest
reserved fields, and returns CF status.  `demFindNextFCB` rereads those fields,
validates their relationship to a retained host entry, performs the next
enumeration step, updates the same buffer, and releases state on exhaustion or
mismatch.  The original code additionally handles volume-label search and
maps native failures to DOS error behavior.

The authoritative packed definitions in `dos/dem/dosdef.h` make the guest
state nontrivial: `SRCHBUF` contains an FCB prefix followed by `DIRENT`; the
`DIRENT` carries both a pointer-sized historical `pFFindEntry` field and a
32-bit `FFindId` in the DOS-reserved area, then attributes/time/date/size.
These fields cannot be exposed as host pointers by a typed adapter ABI.

`FillFCBSrchBuf` writes more than the reserved state: it normalizes the result
to an 8.3 space-padded name/ext pair, clears current-block and record-size,
writes low file size, converts last-write time/date, and fills the embedded
directory name/ext, attributes, time/date and size.  The source initializes
both reserved fields before first search, writes them only when retained
look-ahead state exists, and clears them on next-search failure.  The
path-style family uses the same retained `FFINDLIST` through `SRCHDTA`, so the
two families cannot safely receive independently invented token schemes.

The source-built NTDOS consumer in `v86/doskrnl/dos/search.asm` allocates a
53-byte `SEARCHBUF`, passes it as `DS:SI`, then copies its continuation bytes
back into the caller's FCB.  It does not dereference the historical host
pointer.  Consequently a fixed-width adapter-owned cookie plus search ID can
occupy the two historical reserved fields, provided the paired next operation
validates both and no host pointer is ever written into guest RAM.  This is a
source fact, not an authorization to implement only the first operation.

## Boundary decision

The adapter must replace the historical pointer with an adapter-owned opaque
search token and validate it against its own bounded session table.  No Bochs
code may inspect path bytes, search attributes, wildcard rules, tokens or
result layouts.  Guest strings and result buffers require a separately
versioned sequence of bounded read and atomic write transactions; the existing
single-read and multi-write mechanisms can be considered, but they do not by
themselves admit filesystem semantics.

A correct capability also needs one profile-owned host namespace/root and a
fixed mapping from admitted DOS drive numbers to it.  A guest path, service
byte or CLI flag must never become an unrestricted host path.  Enumeration
order, short-name mapping, attributes, volume label behavior, error mapping,
token exhaustion and cleanup at process/session termination must be decided
for the complete `FindFirst/FindNext` family before either service is made
resumable.

## CLI admission finding

The existing `--byob-root` is not a candidate namespace root.  The CLI uses it
only to identity-validate the selected `NTIO.SYS`, `NTDOS.SYS` and
`COMMAND.COM` artifacts, and the profile schema deliberately forbids paths.
Reusing it as C: would collapse immutable guest-input admission and mutable
host-file capability into one authority domain.  The search capability instead
needs an independently validated namespace descriptor: a profile-selected
relative root/drive policy, a separately supplied host base accepted by the
CLI, and a child-only immutable handoff to the adapter.  The descriptor must
reject reparse traversal and retain no raw guest-controlled path.

## Explicit non-admissions

This evidence does not admit a Bochs memory/device/firmware change, a direct
adapter memory mapping, a host directory API, a general DOS filesystem, or a
standalone `50:0A` response.  The retained DTA locator registration is useful
context only; it does not authorize consuming that locator or synthesizing an
FCB result.

## Required next evidence

1. Complete the source trace through the paired path-style search services and
   FCB cleanup owner, including exact fields written by `FillFCBSrchBuf`.
2. Establish the fixed-width guest layout actually consumed by the source-built
   NTDOS image, including the correct 16-bit surrogate for the historical host
   pointer field.
3. Specify a complete namespace/search transaction ABI and negative cases.
4. Obtain user-visible acceptance of that capability contract before code or
   any additional Bochs intrusion is proposed.
