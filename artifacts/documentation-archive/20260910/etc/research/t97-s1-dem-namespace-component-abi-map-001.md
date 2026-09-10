# T97 S1 DEM Namespace/File/Search Component and ABI Map

## Question

Which complete OpenNT DEM component family owns the first CLI profile's
namespace, file, and search behavior, what exact BOP contracts enter it, and
can any original handler be composed without restoring CCPU/SAS/NTVDM host
composition?

## Inputs And Method

The audit reads the local approved OpenNT sources only:

- `base/mvdm/inc/dossvc.h` and `base/mvdm/dos/dem/demdisp.c` for selector
  numbers and original dispatch ownership;
- `demfile.c`, `demhndl.c`, `demdir.c`, `demsrch.c`, and `demfcb.c` for the
  component family and ABI; and
- the frozen T96 adapter endpoints plus existing readonly-namespace and
  search transaction code solely as implementation inventory.

`50:42` is intentionally outside the provider candidate set: `dossvc.h`
names it `SVC_DEMFASTREAD`, while `demdisp.c` dispatches it to
`demNotYetImplemented`. It is a real-path boundary, not an alias for
`demRead` (`50:16`).

## Original Component Map

| Original module | Callable DEM services | Original contract surface | First-profile disposition |
| --- | --- | --- | --- |
| `demhndl.c` | `00` seek, `02` close, `08` file times, `16` read, `1E` write, `27` commit | A 32-bit Win32 `HANDLE` is split across `AX:BP`; read takes `DS:DX`, `CX`, optional `BX:SI` seek and returns `AX`/CF. It uses `GetVDMAddr`, `Sim32FlushVDMPointer`/`Sim32FreeVDMPointer`, `ReadFile`, `SetFilePointer`, extended error state, and optional VDM redirector. | `00/02/16` are a single contained read-only file-token candidate. `08/1E/27` are deferred: time mutation/write/commit are not in the profile. |
| `demfile.c` | `01` chmod, `03` create, `05` delete, `12` open, `17` rename, `22` create-new, `44` check-path | `12` reads a path from `DS:SI`, accepts mode in `BL`, and returns split host handle in `AX:BP`, length in `BX:CX`, pipe flag in `DX`, and CF. It invokes `CreateFileOem`, redirector/named-pipe helpers, VDD DOS-handle state, and `Sim32GetVDMPointer` helper paths. | Only `12` can join the read-only token candidate after an exact restricted-mode/error map. All create/delete/rename/chmod operations are explicitly unavailable under immutable resources. `44` awaits its complete pathname/error contract; it is not admitted as an open substitute. |
| `demdir.c` | `04` create-dir, `06` delete-dir, `13` query-current-dir, `18` set-current-dir | `13` reads a guest CDS via `DS:SI`, tests host attributes, and reports CF; `18` consumes `DS:DX` and calls host process current-directory state. | `04/06/18` unavailable: mutation or ambient host CWD. `13` is a separate immutable projected-CWD candidate, coupled to COMMAND `54:04`; it is not a host-directory call. |
| `demsrch.c` | `09` path find-first, `0A` FCB find-first, `0B` path find-next, `0C` FCB find-next, `3C` terminate-PDB | Path find-first reads `DS:DX`, attributes in `CX`, and the DTA registered through `50:1B`; it fills `SRCHDTA` and continuation words. Find-next consumes that DTA. FCB forms consume `DS:SI` search buffer and `ES:DI` pathname. The source owns search ordering, DOS error normalization, process/PDB association, VDD termination notification, Unicode/NT directory APIs, and `NtVdmControl`. | `09/0B` are a paired pathname-search candidate only with source-shaped DTA and fixed immutable enumeration snapshot. `0A/0C` remain deferred until FCB success *and no-match* ABI, PDB ownership, and byte-52 behavior are closed. `3C` remains owner/deferred; it must not be used as session cleanup policy. |
| `demfcb.c` | `07` delete-FCB, `20` rename-FCB, `2C` create-FCB, `2D` open-FCB, `2E` close-FCB, `2F` FCB I/O, `30` date, `31` file-info | FCB handlers use guest FCB/DTA structures, split Win32 handles, `GetVDMAddr`/`Sim32*`, and file mutation/attribute APIs. | All deferred as one FCB compatibility family. They cannot be silently enabled merely because pathname search becomes available. |

Every listed service is dispatched by the original `apfnSVC[]` table in
`demdisp.c`; no selected member is independently composable as modern CLI
code. The common blockers are not a missing include: `get*/set*` register
macros and `GetVDMAddr` require historical CCPU/SAS-facing state, `Sim32*`
requires historical mapped-pointer lifetime, and the handlers bind to Win32
host handles, redirector/VDD state, or NT internal directory control. Directly
compiling `nt_bop.c` or any of these handlers would reintroduce the rejected
intrusive composition.

## Contract Edges For S2/S3

The minimal coherent first-profile closure is not FASTREAD and not a generic
host filesystem:

1. A CLI-admitted, immutable resource snapshot maps only declared guest files
   (`COMMAND`, `CONFIG.SYS`, `AUTOEXEC.BAT`, target) to a declared drive.
2. A contained token provider replaces only the source's split-handle ABI for
   `50:12`, `50:00`, `50:16`, and `50:02`. It must preserve the original
   service's documented CF/register failure result while never exposing a
   Win32 handle or host path to guest memory.
3. Path `50:09`/`50:0B` receives a complete copied DTA contract, fixed
   enumeration snapshot, DOS error normalization, and paired continuation;
   it must not borrow FCB failure behavior.
4. FCB forms, writes, mutations, pipes/redirector, volume labels, arbitrary
   drives, host CWD, and raw directory APIs remain unavailable or deferred.

Existing `bx_ntvdm_readonly_namespace_v1` and
`bx_ntvdm_dem_readonly_file_service` demonstrate parts of item 2 for the
four fixed BYOB images. Existing `bx_ntvdm_host_namespace_v1` and
`bx_ntvdm_search_*` demonstrate parts of item 3. They are **not** admitted
providers: the T96 legacy register freezes the direct file recognizer; the
search code has no DEM-plane runtime integration; FCB no-match behavior is
explicitly unresolved; and neither group establishes the complete original
component failure contract. They are implementation inventory for S2/S3, not
authority to add another handler.

## S1 Conclusion

There is no independently composable original OpenNT provider for this family.
The admissible next work is bounded source-derived rehosting through the
existing DEM plane, beginning with an immutable resource/token design (S2)
and the paired pathname search ABI (S3). It must either replace/withdraw the
frozen endpoint as a single provider outcome or retain a source-proven
unavailable result; it may not add a fifth direct BOP recognizer, revive
`50:42`, or modify Bochs.

## Confidence And Follow-up

Confidence is high for service ownership, dispatch numbers, direct ABI inputs,
and hard composition edges because they are read directly from the cited
modules. The exact DOS error map, DTA byte layout, canonical pathname rules,
and read-only mode matrix require S2/S3 source-level audit before any runtime
provider is admitted.
