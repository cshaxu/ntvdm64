# M0 T376 S1 — External DOS EXEC owner and media admission

## Result

The current CPU40/x86 graph already contains the complete immediate original
DEM file cohort needed to begin ordinary DOS program loading.  The first
remaining package action is not a new BOP provider or loader: it is to bind
the existing source-shaped `demOpen`/`demRead`/`demClose` cohort to selected
immutable `.COM` and MZ media, prove its positive/negative local contract,
and add the selected MZ product to the executable-relative stage before a
single frozen observation.

## Selected immutable inputs

- `.COM`: `src/mvdm-guest/dos/v86/cmd/loadfix/LOADFIX.COM`, 1,131 bytes,
  SHA-256 `8e62654a49bd88c784c53f25e9fba13c641624d6a02214385115f66ccf1ecfdb`.
  It is an imported OpenNT prebuilt guest product.  With no argument its
  original source reports its no-parameter outcome and terminates through
  `INT 21h/AH=4Ch`; it does not become a host-side test loader.
- MZ: `src/mvdm-guest/dos/v86/cmd/fastopen/FASTOPEN.EXE`, 882 bytes,
  SHA-256 `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc`.
  It is an imported OpenNT prebuilt MZ product.  Its original `fastopen.asm`
  calls `INT 21h/AH=4Ch` with zero status, making it the smallest known MZ
  loader/relocation/parent-return candidate without a device or child-process
  dependency.

The current `O:\\ntvdm64` stage already carries the `.COM`; its format-3
manifest does not yet carry `FASTOPEN.EXE`.  S2 owns that immutable-stage
declaration change.  No media was copied or changed in S1.

## Current original path and bindings

1. Original guest `msproc.asm::$Exec` performs `$OPEN`; `file.asm` emits
   `SVC_DEMOPEN`.  Original `handle.asm` issues `SVC_DEMREAD` on its normal
   fallback and `SVC_DEMCLOSE`; the MZ path additionally uses
   `SVC_DEMCHGFILEPTR` for header/relocation positioning.
2. Original guest process ownership remains in `$Exec` and `$Exit`:
   PSP/MCB/environment and inherited SFT state are guest data; `msctrlc.asm`
   emits `SVC_PDBTERMINATE` before arena and parent restoration.  Neither an
   app loader nor `cmdReturnExitCode` participates in ordinary DOS child
   return.
3. `src/mvdm-host/dos/dem/demdisp.c` still maps the reached service entries to
   original `demOpen`, `demRead`, `demClose`, `demChgFilePtr` and
   `demTerminatePDB`.  The formal source manifest for
   `original-softpc-process.exe` selects `demfile.c`, `demhndl.c`, `demmisc.c`
   and the full original DEM dispatcher group.
4. `demOpen` copies its bounded DOS pathname through a session guest-memory
   lease before its original canonicalization and `CreateFile` flow; it
   publishes the native handle only through the existing `AX:BP` host-resource
   identity.  `demRead` takes one bounded writable lease for the unchanged
   synchronous `ReadFile` sequence and commits it only on success; `demClose`
   resolves and retires that same identity.  These are registered
   `MVDM-HOST-DIV-195` mirror changes, not a new provider.
5. The pre-existing T365 local fixture demonstrates the shared lease and
   handle-identity contract, but it does not itself execute the original DEM
   service bodies.  S2 must retain it only as support evidence and add the
   smallest source-body-level contract proof needed for the selected cohort.

## S2 cohort

S2 is the complete, already source-shaped **DOS external-file lifecycle
cohort**:

- original `demOpen`/`demRead`/`demClose` and current bounded mapping seams;
- original `demChgFilePtr` only as the MZ extension of the same handle
  identity contract;
- original `demTerminatePDB`/`HostTerminatePDB` disposition and ordinary
  guest parent return; and
- immutable stage declaration for `FASTOPEN.EXE`.

It excludes the kernel-only `SVC_DEMFASTREAD` fast path (original DOS already
falls back to `SVC_DEMREAD` on its `CF=1` result), non-DOS binary
`SVC_CMDCHECKBINARY`, Redirector, native-child COMMAND, VDD/debugger product
shells and every guest/media mutation.
