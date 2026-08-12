# M80 NT4 Startup Service Closure

Status: completed static source closure, 2026-08-08.

## Scope

This is a source-reachable closure for fixed NT4 EN-US from NTIO entry,
through NTDOS initialization, to COMMAND's first command fetch. It is not an
execution trace: branch predicates, profile choices, and device responses
remain unobserved until a governed backend exists.

## Ordered Bootstrap Spine

| Order | Guest call | Historical service | Future owner |
| --- | --- | --- | --- |
| 1 | `doskrnl/bios/msinit.asm` `init` | `BOP 50h` / `SVC_DEMLOADDOS` (`11h`), `DI` = NTDOS destination segment | Wrapper selects immutable NTDOS and publishes it through the governed RAM transaction. M61. |
| 2 | `doskrnl/dos/msinit.asm` `DosInit` | `SVC_DEMGETDRIVES` (`0Fh`) | Contained drive-map policy returns a copied drive count. M63. |
| 3 | same `DosInit` | `SVC_DEMSETDTALOCATION` (`1Bh`) | Session records validated guest locations; raw historical host pointers do not cross the ABI. |
| 4 | same `DosInit` | `SVC_DEMSETHARDERRORINFO` (`32h`) | Wrapper error coordinator holds copied, validated VHE/device-chain references. M64. |
| 5 | same `DosInit` | `BOP 54h` / `SVC_CMDSETINFO` (`05h`) | Session records validated `SCSINFO` references. M68. |
| 6 | same `DosInit`, after device initialization | `SVC_DEMGETDPBLIST` (`46h`) | Drive policy publishes a bounded DPB list through the RAM transaction. |
| 7 | `doskrnl/bios/sysinit1.asm` | `BOP 12h`, `BOP 15h`, `SVC_DEMGETBOOTDRIVE` (`0Dh`) | Memory quantity and boot drive are profile data. M72. |
| 8 | `sysinit1.asm` `ProcessConfig` | `SVC_GETCONFIGSYS` (`0Ch`) | Explicit optional profile file/stream policy. M67. |
| 9 | COMMAND `rdata.asm` / `tcode.asm` | `SVC_GETINITENVIRONMENT` (`0Fh`), then `SVC_CMDGETNEXTCMD` (`01h`) | Profile creates a contained environment and one initial command record. M67-M68. |

The ordering is source dependency, not proof that every row is unconditionally
reached in every profile. Command rows begin only after COMMAND loads.

## Conditional and Rejected Calls

| Source call | Role | First `/C` treatment |
| --- | --- | --- |
| `SVC_ISDEBUG`, `SVC_DEMSYSTEMSYMBOLOP` | Debuggee/symbol branches | Rejected; diagnostics optional. |
| `SVC_DEMWOWFILES` | WOW configuration | Out of scope. |
| `SVC_CMDINITCONSOLE`, `SVC_GETKBDLAYOUT` | Console/video and keyboard layout | Trace-gated; neither creates a bootstrap device requirement. |
| `SVC_CMDCOMSPEC` | Permanent COMMAND reports COMSPEC | Profile metadata; no ambient host environment mutation. |
| `SVC_CMDEXITVDM`, `SVC_RETURNEXITCODE` | Command terminal path | Required only at bounded command-result boundary. M66. |

## Pointer Capture and DPB Consequences

`demSetDTALocation` (`dos/dem/demgset.c`) converts `DS:AX`, `DS:DX`,
`DS:CX`, and `DS:SI` to retained host pointers. `cmdSetInfo`
(`dos/command/cmdmisc.c`) similarly retains `SCSINFO` and companion locations.
They prove ordering and shared-state dependencies in the original stack, but
are not an acceptable external ABI. A future wrapper retains only opaque,
session-owned checked references and revalidates each later access through
core routing.

`demGetDPBList` writes a linked DPB sequence at `ES:BP`, filling only drive,
unit, and next-link fields; when there are no eligible drives it leaves `BP`
unchanged. A first contained profile may expose no physical disk image. A
profile that exposes a drive must publish the DPB bytes as coordinated guest
state, which is an application of M70 candidate B, not a third core facility.

## Boundary Result

This closure confirms, rather than enlarges, M70:

1. NTDOS placement, environment publication, and DPB construction require
   checked all-or-none ordinary-RAM publication.
2. Historical BOP transport requires configured decode observation with
   core-owned finite results.

Service selection, drive mapping, CONFIG policy, environment content, error
state, and command/result rules remain wrapper or future-mantle policy.
CMOS, RTC, FDC, HDC, display, keyboard, XMS, and EMS are not proven bootstrap
requirements here.

## Source Basis

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`
- `src/opennt/base/mvdm/dos/dem/demgset.c`
- `src/opennt/base/mvdm/dos/command/cmdmisc.c`
- `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm`
- `src/opennt/base/mvdm/dos/v86/cmd/command/tcode.asm`

Related records: M61-M68 and M70-M75.
