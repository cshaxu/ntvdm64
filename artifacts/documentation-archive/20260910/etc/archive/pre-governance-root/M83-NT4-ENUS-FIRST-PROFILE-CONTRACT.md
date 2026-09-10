# M83 NT4 EN-US First-Profile Contract

Status: version-locked adapter contract, not executable admission, 2026-08-08.

## Purpose

This contract joins M60-M82 into the single future profile named
`nt4-en-us-command-smoke-v1`. It defines the boundary above a governed machine
backend and below `ntdos64-run`'s CLI handoff. It does not implement a BOP or
SVC dispatcher, select an unproven machine default, or admit execution before
M70 and the bootstrap-cut checklist are satisfied.

## Contract Layers

```text
ntdos64-run invocation
  -> validated BYOB identity profile and root
  -> NT4 EN-US profile adapter
  -> opaque governed-backend C ABI
  -> configured software machine
```

`ntdos64-run` remains responsible only for CLI parsing, direct PE handoff, and
passing the validated `--byob-profile` / `--byob-root` pair to an explicit
engine. The profile adapter owns DOS policy. Core owns only neutral machine
semantics and never accepts profile names, file paths, DOS records, service
numbers, host handles, or command text.

## Static BYOB Identity Input

The existing `ntdos64-byob-profile-v1` validation result is required before
machine creation. The fixed target is x86 / `en-US` /
`nt4-en-us-command-smoke-v1`; it must select these required identity-verified
components:

| Role | Canonical file |
| --- | --- |
| `ntio` | `NTIO.SYS` |
| `ntdos` | `NTDOS.SYS` |
| `command` | `COMMAND.COM` |

The manifest supplies only names, sizes, hashes, feature declarations, and a
non-empty compatibility group. It contains no guest bytes, host drive letter,
host path discovery rule, external handle, or executable command. Optional
components remain unavailable unless the profile feature declaration and a
separate trace-backed capability admission both permit them.

## Invocation and Session Input

The engine receives this already-validated transport:

```text
--byob-profile <manifest> --byob-root <root> -- <DOS/NE target> [args...]
```

The adapter must make a copied session request from it before first guest
write. The request has four explicit policy groups:

| Group | Required form | Prohibited historical shortcut |
| --- | --- | --- |
| drive map | bounded `A`-`Z` entries, each with explicit root, containment policy, media class, and one-based boot-drive selection | host drive enumeration, registry boot-drive lookup, process CWD inference |
| directory | one contained current directory per configured DOS drive | `SetCurrentDirectory` and host `=X:` mutation |
| environment | bounded OEM multi-string plus explicit 16-bit COMSPEC record | `GetEnvironmentStrings`, inherited ambient COMSPEC/WINDIR, process environment mutation |
| command | copied target/argument request and bounded result/re-entry policy | BaseSrv, PIF broker, automatic engine discovery, host-process injection |

The contract intentionally fixes shapes and isolation requirements, not values
for unproven early machine state. A session request that lacks an established
source/trace value must reject with `bootstrap_input_unproven` rather than
fill zeroes or a product-style default.

## Required Guest Protocol Closure

The future adapter is responsible for the following source-established
protocols when reached. This is a required ownership list, not evidence that
every branch is reached before a live trace proves it.

| Guest transport | First-profile responsibility |
| --- | --- |
| `BOP 12h`, `BOP 15h` | profile memory quantity result only; no controller-specific policy in core. |
| `BOP 50h / SVC 11h` | identify and atomically publish selected NTDOS at guest `DI:0000`. |
| `BOP 50h / SVC 0Dh,0Fh,1Bh,32h,46h` | boot-drive, contained drive count, typed DOS-data registration, VHE registration, and DPB publication. |
| `BOP 54h / SVC 05h,02h,0Fh,01h` | SCS registration, COMSPEC, bounded environment capacity negotiation, and first `CMDINFO` command fetch. |
| command completion | preserve a finite command result/re-entry decision and session cleanup; M66 is the historical reference. |

All BOP marker recognition must use the configured decode facility. The
adapter may return only `unhandled`, `handled-resume`, `stop`, or `fault` under
the core-owned rule. It may not change CPU mode, segments, control registers,
memory mappings, devices, IRQ state, or instruction ownership.

## Guest State Discipline

1. The only guest image placement before execution is validated NTIO at
   `0070:0000`, with the virtual-interrupt reservation at `0070:0014`, under
   the bootstrap-cut ordering.
2. NTDOS, environment, DPB, and later command records use validated copied
   spans and a paused ordinary-RAM all-or-none transaction.
3. `SVC_DEMSETDTALOCATION` and `SVC_CMDSETINFO` create typed, session-owned
   registrations, never retained RAM pointers. Registration invalidates on
   reset, stop, fault, memory-generation change, or machine destruction.
4. Guest fields retain their fixed historical layouts: `DOSWOWDATA` is 36
   bytes, `SCSINFO` is 207 bytes, `CMDINFO` is 36 bytes, and a DPB record is
   35 bytes. A host ABI must serialize these fields deliberately rather than
   cast host C structures into guest memory.

## Backend Admission

No engine may instantiate this contract until the M70 evidence package exists:

1. generic configured decode observation with an independent first-party
   consumer and focused core tests;
2. generic paused ordinary-RAM multi-span transaction with an independent
   first-party consumer and focused core tests; and
3. a version-negotiated copied-value `extern "C"` ABI with opaque machine,
   registration, and transaction ownership.

The profile adapter may then be a separate MinGW wrapper/mantle component. It
must not compile OpenNT/NTVDMx64 historical sources into the normal runner or
require those sources at runtime.

## Explicitly Deferred

The first profile excludes, until an original trace reaches a separately
admitted policy:

- XMS/HMA, HIMEM, EMS, DPMI, DOSX, protected-mode transition, and WOW;
- redirector, network, named-pipe, lock, FAT/absolute-disk, and FDC/HDC media
  semantics;
- live keyboard, mouse, video, printer, serial, PIT, RTC/CMOS, and console
  event-thread behavior;
- debugger/symbol services, PIF/configuration-file synthesis, and temporary
  host files; and
- raw Win32 handles, process-wide environment/CWD changes, registry lookup,
  system-directory discovery, injection, or persistent guest-byte storage.

## Admission and Evidence Result

The first executable acceptance remains bounded:

```text
valid BYOB set + valid session request
  -> source-backed bootstrap trace
  -> original service ownership through the profile adapter
  -> COMMAND.COM /C owned target
  -> finite exit result + redacted trace
```

Until BC-10 through BC-15 gain source/trace-backed values, the correct result
is a named stop. A successful profile run proves only this fixed contract; it
does not prove arbitrary NTDOS versions, a general NTVDM replacement, or any
deferred device feature.

## Authority

M60-M82 are source and behavior evidence. M69 and M70 control backend scope.
`NT4-ENUS-BOOTSTRAP-CUT-CHECKLIST.md` controls pre-entry admission. M78
controls outer CLI transport.
