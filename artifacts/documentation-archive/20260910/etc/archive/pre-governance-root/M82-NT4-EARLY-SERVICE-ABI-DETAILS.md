# M82 NT4 Early Service ABI Details

Status: source-derived early-service detail ledger, 2026-08-08.

## Scope

M80 establishes static startup order. This record fixes remaining early ABI
details for a future version-locked profile adapter. It complements M60, M63,
M64, M67, and M68; it does not authorize a local dispatcher or broaden core.

## COMMAND BOP Transport

The guest `CMDSVC` macro emits `C4 C4 54 <service>`. In
`softpc.new/host/src/nt_bop.c`, `MS_bop_4` reads one byte at real-mode
`CS * 16 + IP` through `sas_load`, calls `CmdDispatch(service)`, then
unconditionally advances IP by one.

`dos/command/cmddisp.c` has a 17-entry table (`00h` through `10h`). Its range
check exists only in `#if DBG`; an ordinary build indexes directly. A future
adapter must never reproduce that undefined behavior. Its profile dispatcher
must reject an invalid service before state publication. Core still owns byte
consumption, IP advance, CPU mode, and permitted register patching.

## SVC 0Dh: Boot Drive

`demGetBootDrive` has no guest input. Historical code queries a registry value
and accepts it only for a fixed host drive; otherwise it defaults to `3`. It
writes **AL only**, as a one-based value (`A=1`, `C=3`). `sysinit1.asm` stores
that AL result as the DOS default/boot drive.

Registry lookup and host-drive inspection are historical product policy. The
contained counterpart is a required one-based boot-drive value selected from
the configured DOS drive map. It must not inspect the host registry, enumerate
host drives, or infer a drive from the process CWD.

## SVC 1Bh: DTA and DOS Data Registration

Despite its older two-register comment, `demSetDTALocation` consumes four
locations with one common DS:

| Input | Historical retained meaning |
| --- | --- |
| `DS:AX` | four-byte far DTA location variable (`pulDTALocation`) |
| `DS:DX` | current-PDB word (`pusCurrentPDB`) |
| `DS:CX` | `DEMEXTERR` location (`pExtendedError`) |
| `DS:SI` | `DOSWOWDATA`, from which DEM derives the SFT head |

`DOSWOWDATA` has nine DWORD fields under two-byte packing (36 bytes);
`lpSftAddr` is its final field at offset `32`. Historical DEM converts all
four locations to raw VDM pointers and dereferences them from later
file/search/error paths. This proves a registration lifetime dependency, but
cannot cross a governed backend ABI.

A future adapter validates stated spans at registration, retains only typed
session-owned guest references, and revalidates every later access. Reset,
stop, mapping change, or memory-generation change invalidates registrations.
No RAM pointer or `GetVDMAddr` equivalent may cross the external ABI.

## SVC 46h: DPB List

`demGetDPBList` takes `ES:BP` as destination and returns BP at the first byte
after the generated list. The original x86 host `DPB` uses one-byte packing
and is 35 bytes:

| DPB byte offset | Historical write |
| ---: | --- |
| `00h` | `DriveNum` (`A=0`) |
| `01h` | `Unit`, same value |
| `1Bh` through `1Eh` | 32-bit far-style `Next` value |

For each local removable or fixed drive, historical code advances BP by 35,
writes only these fields, and links to the next nominal record. It terminates
the last link with `FFFFFFFFh`. With no eligible drive, BP is unchanged and
no bytes are published.

The contained profile derives its ordered list from its explicit drive map,
then publishes necessary field spans only after validating every destination.
The group of writes and final BP patch must commit all-or-none through M70
candidate B. This is not permission to cast a host structure into guest RAM,
or to move FDC/HDC/CMOS into core.

## SVC 05h: CMD State Registration

`cmdSetInfo` receives `DS:DX` for packed `SCSINFO`, `DS:BX` for the
`SCS_Is_Dos_Binary` byte, and `DS:CX` for the `SCS_FDACCESS` word. With
one-byte packing, `SCSINFO` is 207 bytes:

```text
00..3F   SCS_ComSpec[64]
40..BF   SCS_CmdTail[128]
C0..CD   PARAMBLOCK (14 bytes)
CE       SCS_ToSync
```

The old host caches pointers; later command dispatch writes `SCS_ToSync=FFh`.
M68 defines the separate 36-byte `CMDINFO`. The future adapter needs both
layouts, but uses checked session references and transaction-backed field
publication rather than cached raw pointers.

## Boundary Result

These facts reinforce the existing two M70 gaps: BOP 54 requires configured
decode observation with finite core-owned outcomes, and DPB/command/startup
publication requires a paused ordinary-RAM all-or-none transaction. They add
no generic core device, namespace, file, or CPU-mode facility.

## Sources

- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`
- `src/opennt/base/mvdm/dos/command/cmddisp.c`
- `src/opennt/base/mvdm/dos/command/cmd.h`
- `src/opennt/base/mvdm/dos/command/cmdmisc.c`
- `src/opennt/base/mvdm/dos/dem/demgset.c`
- `src/opennt/base/mvdm/dos/dem/demdasd.h`
- `src/opennt/base/mvdm/inc/doswow.h`
