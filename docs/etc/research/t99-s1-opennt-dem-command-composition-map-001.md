# T99 S1 OpenNT DEM/COMMAND composition map 001

## Decision

The first provider implementation slice is **profile-backed read-only boot
namespace composition**. It is a component-sized replacement boundary derived
from original OpenNT `cmdconf`, DEM normal file I/O, and pathname-search
components; it is not a collection of trace-hit BOP handlers.

The current T98 execution reaches `54:0C` only after successful startup. In
original OpenNT that service is paired with `54:0D`, and `cmdconf.c` creates
temporary CONFIG/AUTOEXEC files then returns their pathname through a
64-byte `DS:DX` buffer. The returned pathname is subsequently consumed by
ordinary DOS services. Consequently, neither service may be independently
declared a completed provider merely because it writes a pathname.

## Reached-family reconciliation

The immutable T98 S3 trace records the following DEM/COMMAND identities. The
table assigns them to their original component rather than to an adapter
recognizer.

| BOP | Original handler/module | Family disposition |
| --- | --- | --- |
| `50:11` | `demLoadDos`, `demmisc.c` | Existing one-time NTDOS bootstrap exception; retain separately, not part of namespace slice. |
| `50:3B` | `demIsDebug`, `demmisc.c` | CLI non-debug session policy; retain source-derived result pending later provider migration. |
| `50:0F` | `demGetDrives`, `demgset.c` | Profile drive-inventory component; no host filesystem projection. |
| `50:1B` | `demSetDTALocation`, `demgset.c` | Namespace/search locator prerequisite; copied registration, not a retained raw pointer. |
| `50:32` | `demSetHardErrorInfo`, `demerror.c` | Historical error-registration prerequisite; no retry/INT 24 policy is admitted. |
| `54:05` | `cmdSetInfo`, `cmdmisc.c` | Historical raw guest-pointer registration; must be replaced with copied launch/session state or remain unavailable. |
| `50:46` | `demGetDPBList`, `demgset.c` | Profile drive-descriptor materialization; no storage/device ownership transfers. |
| `50:0D` | `demGetBootDrive`, `demgset.c` | Profile boot-drive policy; original registry query is explicitly not imported. |
| `50:21` | `demIOCTL`, `demioctl.c` | Raw-media/IOCTL component; outside first read-only namespace slice. |
| `54:0C`, `54:0D` | `cmdGetConfigSys`, `cmdGetAutoexecBat`, `cmdconf.c` | Paired boot-file producer; included in first namespace slice. |
| `50:09`, `50:0B` | `demFindFirst`, `demFindNext`, `demsrch.c` | Paired pathname search; included through the existing immutable profile snapshot. |
| `50:0A`, `50:0C` | FCB search in `demsrch.c` | Same source module but distinct FCB ABI; expressly excluded pending FCB buffer/PDB contract closure. |
| `50:18`, `50:00`, `50:16`, `50:02` | `demOpen`, `demChgFilePtr`, `demRead`, `demClose` in DEM file-I/O modules | Included as the paired normal DOS read-handle consumer, not individually. |

The trace also includes selectors `12` and `15`; they are historical
machine/BIOS-class forms, not DEM or COMMAND provider candidates. Their
existing narrow outcomes remain outside this task's provider scope.

## Original composition blockers

| Original component | Required historical facilities | T99 contained replacement boundary |
| --- | --- | --- |
| `cmdconf.c` | Temporary host files, OEM/Unicode conversion, `GetVDMAddr`, error dialog and `TerminateVDM` | Profile has immutable CONFIG/AUTOEXEC bytes and placements; adapter copies a bounded DOS pathname result and uses its controlled-stop/error policy, never a host temporary path. |
| DEM normal file I/O | CCPU/SAS registers, guest pointers, Win32 handles/file APIs and per-process tables | Fixed-width copied requests/results plus opaque adapter resource tokens bound only to profile-declared content; no host handle crosses the ABI. |
| `demsrch.c` pathname search | DTA/PDB globals, NT directory enumeration and process association | Existing copied DTA registration and immutable four-entry metadata snapshot; no ambient host directory enumeration. |
| FCB search | Different FCB byte layout, `DS:SI`/`ES:DI`, PDB and byte-52 semantics | Deferred; it may not be silently routed through pathname search. |

No listed original dispatcher is independently CLI-linkable: `DemDispatch` and
`CmdDispatch` both depend on `softpc.h` register/pointer semantics, while the
handlers above additionally bind Win32/NT host lifecycle. `nt_bop.c` remains a
routing reference only.

## Provider order and implementation gate

For this one composition family, provider precedence remains:

1. independently composable original implementation — **blocked** by the
   historical facilities listed above;
2. original structure with contained CLI capability adaptation — **selected**:
   preserve paired service ordering, buffer width, carry/error outcome and
   lifecycle while substituting the finite profile namespace;
3. source-derived rehost — allowed only for the documented contained adapter
   provider surface, with each deviation recorded;
4. unavailable/deferred — FCB, write/create/rename, raw IOCTL and host process
   functions remain here.

The next S may implement only this complete producer/consumer family:
paired `54:0C/0D`, normal read-only open/seek/read/close, and existing pathname
`50:09/0B` integration under one profile namespace owner. It must replace or
withdraw the corresponding legacy endpoint path as a group; it must not add a
selector recognizer, host-directory fixture, host path/handle, DOS/BIOS rule,
or FCB fallback.
