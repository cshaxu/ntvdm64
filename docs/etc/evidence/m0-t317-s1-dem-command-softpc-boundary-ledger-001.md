# M0 T317 S1 — DEM/COMMAND SoftPC vertical-slice boundary ledger

## Question

What complete original cohorts must be recovered, in what dependency order,
to make the first DEM/COMMAND vertical slice source-shaped under the selected
x86 SoftPC CCPU40 machine route?

## Inputs and procedure

- Read the selected original `dos/dem/sources` and `dos/command/sources`
  manifests: they select 16 DEM and 11 COMMAND translation units.
- Read `demdisp.c` (73 SVC entries) and `cmddisp.c` (17 SVC entries), then
  map each selected unit to its shared state and outgoing dependency family.
- Cross-check Base VDM forms against `BASEVDM-001` through `BASEVDM-018`,
  current session/mapping-manager ownership, the T316 monitor closure, and the
  CCPU40 formal graph. No retired Bochs, CPU30 or kernel-VDM route is counted
  as an available provider.

## Cohort ledger

| Cohort | Original translation units | Required boundary | Disposition | Ordered recovery |
| --- | --- | --- | --- | --- |
| DEM common/dispatch | `dem.c`, `demdata.c`, `demdisp.c`, `demmsg.c` | SoftPC register forms; common DEM guest descriptors | Original bodies selected; register/lease calls belong to `adapter-mvdm-host-out/softpc` | S4 after command bootstrap proves Base VDM/session state |
| DEM namespace/search | `demdir.c`, `demsrch.c` | Host path/current directory, `VdmQueryDir`, DTA guest lease | Original bodies selected; current monitor `VdmQueryDir` binding is direct; mapping and public file APIs are named adapters | S5 after DEM common |
| DEM file/handle/FCB | `demfile.c`, `demhndl.c`, `demfcb.c`, `demlock.c` | Host file handles, SFT/JFT/DTA leases, Redirector calls | Original local-file bodies selected through SoftPC mapping and session handle identity; named-pipe/Redirector branches remain exact later-owner branches | S6 after namespace/search; Redirector branches are not locally reimplemented |
| DEM errors/misc | `demerror.c`, `demgset.c`, `demmisc.c` | DOS error data, host time/config, guest lease | Original bodies selected; public Win32 and existing SoftPC adapter only | S5 with namespace/search where shared DOS data is available |
| DEM DASD/device | `demdasd.c`, `demioctl.c`, `demlabel.c` | `host_simulate`, INT13/device/PIC/floppy semantics | Original bodies remain selected but live machine/device path is owned by later SoftPC device and VDD/Redirector packages | Explicit later-owner branch; not a first vertical-slice prerequisite |
| COMMAND shared state/dispatch | `cmd.c`, `cmddata.c`, `cmddisp.c` | SoftPC register forms, original `VDMINFO` records | Original bodies selected; no app dispatcher may replace the table | S2 foundation |
| COMMAND Base VDM/bootstrap | `cmdmisc.c`, `cmdenv.c`, `cmdconf.c`, `cmdkeyb.c` | `GetNextVDMCommand`, capacity/retry/re-entry, command/environment guest copies, console/key-layout capability | Original bodies selected; Base VDM local protocol is the required bounded adapter; guest locations use mapping leases | S2 — first implementable cohort |
| COMMAND child lifecycle | `cmdexec.c`, `cmdexit.c` | `CreateProcess`, suspended worker, re-entry balance, child completion, parent return | Original bodies selected; local same-shaped Base VDM/session worker route is required. PSP/DOS parent return is a later guest owner, not a substitute lifecycle | S3 after S2 |
| COMMAND streams/PIF | `cmdredir.c`, `cmdpif.c` | Stream identity, pipe/console endpoints, PIF/console product shell | Original local stream shape through mapping/adapter remains selected; remote pipes and full PIF shell remain Redirector/host-capability later owner paths | S3 only for local child endpoints; later owners for excluded branches |

## Boundary conclusions

- Base VDM command broker and `NtVdmControl` are distinct. `cmdmisc.c` and
  `cmdexec.c` use the former; DEM directory search uses the latter only for
  `VdmQueryDir`. Neither may use the other as a generic dispatcher.
- Every `GetVDMAddr`/`Sim32*VDMPointer` form is a SoftPC/session mapping lease
  boundary. It is not a reason to create a DEM- or COMMAND-private mapper.
- `host_simulate` is an original SoftPC CCPU40 entry form. Its DASD/INT13
  callers require the later whole device package; a bounded child/command
  cohort must not claim those device semantics.
- The original 27 translation units already appear in the selected formal
  source graph. That demonstrates source composition only, not linked service
  behavior; S2 begins with the first stateful cohort rather than forcing all
  dispatcher entries through fixture stubs.

## S2 admission

S2 owns the complete COMMAND shared-state and Base VDM/bootstrap cohort:
`cmd.c`, `cmddata.c`, `cmddisp.c`, `cmdmisc.c`, `cmdenv.c`, `cmdconf.c`, and
`cmdkeyb.c`, plus only the reached same-shaped Base VDM/session/SoftPC adapter
surfaces. Its acceptance must prove original request/capacity/retry order and
source-shaped unavailable outcomes; it must not start `cmdexec`, DEM file I/O,
or any BOP trace.

## Confidence and follow-up

High confidence for cohort ownership and order: the mappings come directly
from both original source manifests and tables. S2 must re-read each selected
function's concrete Base VDM and mapping call contract before implementation;
the ledger intentionally does not claim runtime completion.
