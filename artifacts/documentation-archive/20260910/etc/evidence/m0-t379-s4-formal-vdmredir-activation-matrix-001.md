# M0 T379 S4 — Formal VDMREDIR activation matrix

## Scope

This record closes the product assembly gate only.  It does not report a
guest run; S5 alone may make the one frozen `REDIR.EXE` observation.

## Inputs and identities

- Formal CPU40 parent input:
  `build/M0-T379/S2/formal-x86/original-softpc-process.exe`, SHA-256
  `adbdfc3bf72fbf41798ab299ee64b489e35096b86727274552b4c30199a57e1f`.
- Formal original DLL input:
  `build/M0-T379/S2/formal-x86/VDMREDIR.dll`, SHA-256
  `74dfd1f242b2169f8bc589e677ad242196e8034c4404a4e9ae226b231ec52198`.
- Declared immutable local-resource workload:
  `build/output/dos/REDIR.EXE`, staged as `system32/REDIR.EXE`, SHA-256
  `b6e9fad4df3117914d31f24c97a3aa8f525d3b5d7cc79d3c0151d70cfacac3a7`.

The fresh stage is `build/M0-T379/S4/runtime`.  Its format-4 manifest carries
the parent as `ntvdm.exe`, the sibling runtime companion `VDMREDIR.DLL`, and
the immutable media set.  A Node SHA-256 verification of every product,
companion and media manifest row returned `all: true`.

## Parent identity and original loader contract

The original host export definition names the parent module `ntvdm.exe`.
`LoadVdmRedir` in original
`src/mvdm-host/softpc.new/host/src/nt_bop.c` calls
`SafeLoadLibrary("VDMREDIR")`, then resolves the nine parent-used exports by
their original names.  It is therefore not sufficient to place the DLL beside
an arbitrarily named executable: the DLL's import table must bind to the
loaded parent identity.

The stage deliberately gives the selected formal parent the original runtime
name `ntvdm.exe`; its descriptive build-tree filename remains a build artifact
only.  `VDMREDIR.DLL` is staged as a sibling companion, not guest media.  PE
import inspection of that staged DLL reports exactly:

```
KERNEL32.dll
NETAPI32.dll
ntdll.dll
ntvdm.exe
```

Thus normal Windows application-directory DLL resolution can find both the
originally named parent already loaded as `ntvdm.exe` and the companion
`VDMREDIR.DLL`, without static-linking `VrDispatch` or creating a second
SoftPC executor.

## Formal build and source-shaped failure direction

The S2 formal x86 Ninja graph already linked the original DLL through its
original `vdmredir.def` and generated parent import library.  S4 reran the
stage and identity matrix only; it did not change a provider body or run the
product.

The absent/invalid-companion direction remains original source behavior:
`MS_bop_7` records its first failed `LoadVdmRedir` attempt as state 2,
sets `CF=1` and `AX=ERROR_INVALID_FUNCTION`, then advances the guest IP by
one.  The new companion-aware manifest validator rejects a missing or
hash-mismatched `VDMREDIR.DLL` before an observation is admitted; it does not
replace this guest-visible original failure path.

## Result

The formal product matrix is closed.  The parent module identity, DLL identity,
original dynamic-loader/export surface, workload identity and retained source
failure direction are all fixed.  S5 may now make exactly one bounded,
non-debug observation using this stage.  This record makes no claim that a
`57:xx` service, named pipe, remote network route or arbitrary Redirector
workload completed.
