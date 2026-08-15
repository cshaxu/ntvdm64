# T216 S1: Guest Executable-Image Visibility Source/ABI/Failure Map

## Question

What complete, source-first package is required for SYSINIT to load the
profile-declared `COMMAND.COM`, and for the same declared namespace to remain
coherent for the first target and optional terminal image, without turning a
`50:3D` observation into a leaf implementation?

## Inputs

- OpenNT `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`,
  `dos/file.asm`, `dos/handle.asm`, `dos/search.asm`, and
  `base/mvdm/inc/dossvc.h`.
- OpenNT host ownership: `dos/dem/demdisp.c`, `demfile.c`, `demhndl.c`,
  `demsrch.c`, and `demfcb.c`.
- Current `bx_ntvdm_readonly_namespace`, boot namespace provider/plane,
  direct DEM whole provider, namespace/handle/search partitions, and profile
  search snapshot.
- T215 S6 source-built causal observation.

## Original Contract

SYSINIT obtains configuration first, then opens the command interpreter,
seeks to determine its size, and invokes DOS EXEC.  Any failure falls through
`comerr` to `SVC_DEMEXITVDM`.  This is ordinary DOS open/handle/EXEC behavior,
not a special `50:3D` service contract.

OpenNT's dispatcher assigns the host portions to one DEM owner package:

| Service | Original DEM owner | Current copied ABI needed by declared image | Original/family outcome |
| --- | --- | --- | --- |
| `50:12` open | `demfile.c:demOpen` | copied `DS:SI` OEM pathname and mode/share inputs; returns opaque handle in AX:BP and size registers | open/read succeeds or DOS-domain error/CF |
| `50:00` seek | `demhndl.c:demChgFilePtr` | opaque handle, origin, CX:DX offset | 32-bit position or invalid-handle/parameter CF |
| `50:16` read | `demhndl.c:demRead` | opaque handle, requested CX and checked guest destination | copied bytes plus AX/CF result |
| `50:02` close | `demhndl.c:demClose` | opaque handle | CF only on success; error/CF on invalid handle |
| `50:08` times | `demhndl.c` file-time path | opaque handle and get/set mode | DOS time/date, or access/error CF |
| `50:09/0B` path find first/next | `demsrch.c` | copied path/DTA/PDB request and 43-byte result | declared metadata match or no-more-files layout |
| `50:0A/0C` FCB find first/next | `demsrch.c` | copied FCB/search-buffer/DTA request | FCB result/no-more layout |
| `50:2C/2D/2E/2F/30/31` FCB file operations | `demfcb.c` | FCB/SFT layout and opaque handle state | remains a separate FCB subfamily, not interchangeable with handle tokens |

`50:01/03/04/05/06/17/22` are namespace mutation/metadata paths in
`demfile.c`/`demdir.c`.  The declared images are immutable profile inputs:
their exact names must return the ordinary readonly failure instead of falling
through to an identically named host file.  `50:1E` write and `50:27` commit
likewise use the already-defined readonly outcomes.  This is not a virtual
drive: non-declared paths retain the caller-selected real-host provider mode.

## Current-State Map

| Declared image | Namespace state | Search state | Direct DEM open/handle state | Disposition |
| --- | --- | --- | --- | --- |
| `COMMAND.COM` | `readonly_namespace.files[0]` owns bytes, path, metadata | snapshot entry 0 exists | **not claimed** by `startup_namespace`, so `50:12` falls into host Win32 open | missing essential seam |
| `CONFIG.SYS` | files[1], T204-owned bytes when bound | snapshot entry 1 exists | exact path is claimed by S5 overlay | already attached special startup subset |
| `AUTOEXEC.BAT` | files[2], T204-owned bytes when bound | snapshot entry 2 exists | exact path is claimed by S5 overlay | already attached special startup subset |
| first target | files[3] owns bytes/path/metadata | snapshot entry 3 exists | not claimed; direct host fallback | same missing seam |
| optional terminal target | files[4] owns bytes/path/metadata | snapshot entry 4 exists | not claimed; direct host fallback | same missing seam |

The apparent contradiction is real and is the package defect: the boot
provider initializes a coherent all-file readonly namespace and search
snapshot, but `bx_ntvdm_dem_whole_provider_v1_set_startup_namespace` names it
as a two-image startup subset.  `match_startup_path` and `owns_token` restrict
namespace/handle interception to indices 1 and 2.  In parallel, the installed
direct DEM search partition enumerates `host_namespace`, whereas the retained
boot plane's snapshot-aware search path is bypassed after the whole provider
is installed.  A declared image can therefore be listed by one plane yet be
invisible to SYSINIT's actual open route.

## Recovery-Ladder Decision

1. **Direct original source:** unavailable as a modern in-process object.
   `demfile.c`, `demhndl.c` and `demsrch.c` depend on CCPU/SAS register and
   address conversion, historical VDM/global state and old host composition.
   Linking them would not be a bounded CLI composition.
2. **Smallest compatible seam:** selected.  Existing source-derived readonly
   namespace, copied DEM result layouts, and direct provider partitions already
   preserve the relevant OpenNT owner, handle layout, ordering and failure
   rules.  Generalize their *declared immutable-image* predicate from only
   indices 1/2 to every ready declared namespace file; inject the existing
   profile snapshot into the direct search partition.  Do not duplicate a DOS
   loader, DOS EXEC, filesystem, or selector recognizer.
3. **External intrusion:** not applicable.  No Bochs/core/mantle change is
   required.
4. **New behavior:** prohibited for this package; no source gap justifies it.

## S2 Implementation Boundary

S2 may make one provider-owned all-declared-image overlay family for open,
seek/read/fast-read, close, times, search, and readonly mutation refusal.  It
must preserve opaque typed tokens and checked guest transactions; it must not
add an EXEC BOP, inspect unbounded guest memory, create host files, alter
nonmatching host paths, or treat FCB file operations as a handle-token alias.
The FCB *search* snapshot can be admitted with the same image metadata;
FCB open/read/write requires its existing source/ABI subfamily to be assessed
explicitly before claiming executable-loader coverage.

## Verification Plan

The first regression creates a profile-owned `COMMAND.COM` and target images
whose names do not exist on the host drive.  It proves `50:12 -> 50:00/16/02`
and `50:09/0B` resolve the declared bytes/metadata, while exact mutation
attempts fail without host side effects and a nonmatching path still routes to
the selected host provider.  A bounded native trace follows only after that
whole package test passes.

## Confidence And Follow-up

High confidence: T215 S6 directly observes the current `50:12` failure after
the configuration sequence, and code inspection proves its declared
`COMMAND.COM` bytes are excluded by the two-index predicate.  The exact
SYSINIT sub-operation that produces terminal CF is not asserted.  Admit S2 as
the above one whole provider seam; do not add a `50:3D` handler.
