# M0 T261 S1 — OpenNT host source-truth audit

## Question

Does the current `src/opennt-host` tree retain the relative layout, filenames
and contents of its admitted OpenNT host source, after only the component root
is changed?  If not, what exact recovery/relocation work is required?

## Inputs and procedure

- Compared every live non-README file below `src/opennt-host` with
  `refs/opennt` by relative basename and SHA-256.
- Inspected current file headers, original-path comments, formal module manifest
  and T259 host/machine interface catalog.
- Treated byte-identical original content as direct-import evidence only; a
  related function name or a source comment is not content identity.

## Current-state result

There are 50 live production files: 26 C sources and 24 headers.  Only three
are byte-identical to their OpenNT originals.  The other 47 are project-owned
composition, source-derived capability, or compatibility seams.  Therefore the
current owner-family layout is **not** an acceptable final OpenNT-host mirror.

| Class | Count | Current fact | Required final disposition |
| --- | ---: | --- | --- |
| `DIRECT` | 3 | Exact SHA-256 OpenNT content. | Move to re-rooted historical relative path unchanged. |
| `COMPOSE` | 16 | Project wrapper/session/BOP composition around an imported source or caller. | Move to `app`, `opennt-bop`, or an adapter unless an original host path can be recovered. |
| `HOST-REHOST` | 24 | Host-facing code derived from a named OpenNT provider/call contract. | Recover original host translation unit first; otherwise keep only under an individual recovery exception. |
| `ADAPTER-CANDIDATE` | 7 | Win32/SoftPC/CCPU mechanics currently mixed into host source. | Move behind `adapter-win32` or `adapter-softpc` with the original call shape. |

The count is an audit classification, not a claim that each listed source is
already recovered or behavior-complete.

## File ledger

`Original/source basis` is the first source path or interface family that the
next S must prove in detail.  `Target action` is binding for layout work; a
different action needs an amended audit row before code changes.

| Current file(s) | Class | Original/source basis | Target action |
| --- | --- | --- | --- |
| `command/nt_pif.c` | DIRECT | `base/mvdm/softpc.new/host/src/nt_pif.c` | Move unchanged to `softpc.new/host/src/nt_pif.c`. |
| `command/nt_pif.h` | DIRECT | `base/mvdm/softpc.new/host/inc/nt_pif.h` | Move unchanged to `softpc.new/host/inc/nt_pif.h`. |
| `command/nt_uis.h` | DIRECT | `base/mvdm/softpc.new/host/inc/nt_uis.h` | Move unchanged to `softpc.new/host/inc/nt_uis.h`. |
| `command/command_opennt_pif_parser.c` | COMPOSE | `nt_pif.c` direct composition. | Give it a distinct composition filename outside the original path; it must include/compile the unchanged original only. |
| `command/command_pif_shim.c`, `.h` | HOST-REHOST | `softpc.new/host/src/nt_pif.c`, PIF error/config contract. | Replace with smallest source-shaped PIF facade or move non-host ABI to an adapter; do not retain generic name. |
| `command/command_config_shim.c` | HOST-REHOST | `nt_pif.c` config/autoexec contract. | Recover original call order in a re-rooted host source or register a specific unavailable dependency. |
| `command/command_binary_shim.c` | ADAPTER-CANDIDATE | `RtlNtStatusToDosError` caller contract. | Move same-shaped NTDLL behavior to `adapter-win32`; retain only host policy at the caller. |
| `command/command_environment_shim.c` | HOST-REHOST | `dos/command/cmdenv.c` environment contract. | Keep original provider in `opennt-bop`; make any session environment seam source-shaped and individually registered. |
| `command/command_misc_shim.c`, `.h` | HOST-REHOST | `dos/command/cmdmisc.c`; `nt_lpt.c`; BaseSrv/CSR notifications. | Split original host calls by `nt_lpt`/event owner; move Win32/NTDLL mechanics to adapter and leave explicit unavailable VDD/CSR routes. |
| `command/command_native_session_shim.c`, `.h` | COMPOSE | COMMAND `cmdexec`/event lifecycle. | Move BOP/session continuation to `opennt-bop` or `app`; only same-shaped public-API calls may remain behind adapters. |
| `config/config_complete_shim.c` | COMPOSE | SoftPC configuration-complete notification. | Classify against original `config.c`/BOP caller; move route to `opennt-bop` or recover original host body. |
| `dem/bx_ntvdm_host_drive_policy.c`, `.h` | HOST-REHOST | DEM drive projection / current-directory contract. | Retain only if source audit proves no direct original host body; register as host capability exception. |
| `dem/bx_ntvdm_host_namespace.c`, `.h` | HOST-REHOST | DEM namespace/search contract. | Retain only if source audit proves no direct original host body; register as host capability exception. |
| `dem/dem_common_shim.h`, `dem_drive_policy_shim.h` | COMPOSE | imported DEM declaration closure. | Eliminate generic wrapper name; publish only exact original declarations or move to owning adapter. |
| `dem/dem_direct_host_session.c`, `.h` | COMPOSE | DEM dispatcher/session composition. | Move BOP/session control to `opennt-bop` or `app`; no generic host-session implementation remains. |
| `dem/dem_native_session_shim.c`, `.h` | COMPOSE | direct DEM ingress and generic-UD continuation. | Move to `opennt-bop` ingress or `app`; it is not an OpenNT host translation unit. |
| `dem/demdasd_ioctl_shim.c`, `.h` | HOST-REHOST | `dos/dem/demdasd.c`; `spckbd.asm`; SoftPC I/O contract. | Recover original host call seam; transfer CPU/IVT/machine conversion to `adapter-softpc`. |
| `dem/demdir_shim.c`, `.h` | HOST-REHOST | `dos/dem/demdir.c`. | Recover original host directory calls and move Win32 implementation to `adapter-win32` only when necessary. |
| `dem/demdisp_shim.c`, `.h` | COMPOSE | `dos/dem/demdisp.c`. | Move dispatch invocation to `opennt-bop`; keep imported `demdisp.c` as the provider owner. |
| `dem/demerror_lock_shim.c`, `.h` | ADAPTER-CANDIDATE | `dos/dem/demerror.c`, `demlock.c`; CCPU/SAS state. | Recover original source bodies in `opennt-bop`; move copied CPU/memory mechanics to `adapter-softpc`. |
| `dem/demfile_shim.c`, `.h` | HOST-REHOST | `dos/dem/demfile.c`; file/redirector calls. | Recover original host file APIs with public Win32 facade; reserve protocol behavior for Redirector. |
| `dem/demgset_shim.c`, `.h` | ADAPTER-CANDIDATE | `dos/dem/demgset.c`; CCPU/SAS mappings. | Move mapping/copy mechanics to `adapter-softpc`; leave source caller in `opennt-bop`. |
| `dem/demhndl_shim.c`, `.h` | HOST-REHOST | `dos/dem/demhndl.c`; host-handle contract. | Use the one handle manager through an original-shaped host seam; no new table. |
| `dem/demmisc_shim.c`, `.h` | HOST-REHOST | `dos/dem/demmisc.c`; `nt_msscs.c`, VDD and floppy calls. | Recover direct public Win32 portions; retain individually documented VDD/FDC unavailable branches. |
| `dem/demsrch_fcb_shim.c`, `.h` | HOST-REHOST | `dos/dem/demsrch.c`, `demfcb.c`; `nt_msscs.c`. | Recover search/FCB host calls; retain VDD callback decline as a named exception only. |
| `redir/redir_session_shim.c`, `.h` | COMPOSE | `VDMREDIR` loader/completion contract. | Move route/lifecycle to Redirector owner package; no generic host session remains. |
| `top_level/top_level_nosupport_shim.c`, `.h` | HOST-REHOST | `softpc.new/host/src/nt_error.c` (`MS_bop_9`). | Recover re-rooted original host path first; retain public-dialog/session-stop divergence only in the imported body or its facade. |
| `xms/xms_shim.c`, `.h` | ADAPTER-CANDIDATE | `xms.486/{xms.c,xmsa20.c,xmsmisc.c}` and SAS/A20/IVT calls. | Keep XMS provider in `opennt-bop`; move A20/IVT/RAM mechanics to `adapter-softpc`. |
| `xms/xms_native_session_shim.c`, `.h` | COMPOSE | XMS generic-UD/session continuation. | Move BOP/session routing to `opennt-bop` or `app`; no generic host session remains. |

## Dependency and ordering result

1. Establish exact original-path and content identity before moving a source.
2. Extract all adapter-bound calls (`Rtl*`, public Win32, SAS/CCPU/IVT/A20)
   before importing a host body; otherwise an imported file would retain a
   hidden generic shim dependency.
3. Relocate BOP ingress/session code before DEM/COMMAND body cleanup, so
   `opennt-host` stops owning selector/continuation composition.
4. Recover direct host providers by original path, preserving their failure
   behavior; only then delete superseded shims.
5. Rebuild the full static closure once, after all include/manifest paths are
   coherent.  This is layout validation, not BOP enablement.

## Confidence and limitations

High confidence: file count, byte identity and the three direct PIF imports.
Medium confidence: source-family attribution for the 47 non-identical files,
because their current names collapse several historical callers.  S2 must
perform function-level call-site mapping against `refs/opennt` before any row
is implemented.  No row is permitted to claim direct source reuse based only
on a semantic resemblance.
