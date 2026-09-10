# R2 BYOB Resource Loader Caller Contract

Status: R2.2 source-derived admission record, 2026-08-07.

## Historical Caller

The NT4 historical sources establish the first NTDOS image-acquisition edge:

```text
NTIO.SYS
  -> BOP 50 / SVC_DEMLOADDOS (11h)
  -> MS_bop_0
  -> DemDispatch(11h)
  -> demLoadDos
```

`demmisc.c:demLoadDos` receives the guest destination in `DI:0000`, converts
it through the historical `GetVDMAddr`, opens `ntdos.sys`, then repeatedly
uses the original `ReadFile` loop in 16 KiB chunks. It retains responsibility
for guest placement, byte order, load progression, error transition, and the
return to NTIO. A modern loader must not replace those responsibilities.

## Historical Incompatibility

`dem.c:DemInit` constructs `pszDefaultDOSDirectory` with
`GetSystemDirectory`. `demLoadDos` appends `\\ntdos.sys` to that global path.
That was correct for the original installed NTVDM product but is forbidden for
the non-invasive BYOB profile: it discovers an ambient system location and
would make the experiment depend on host-installed files.

## Admitted Seam

The first possible R2.2 modern-host shim is a **per-session runtime-root
provider**. It may provide the already preflighted, user-selected BYOB root to
the historical `DemInit` ownership boundary instead of `GetSystemDirectory`.
It must meet all of these conditions:

1. input is explicit per runner session; it has no global fallback and no
   ambient system-directory lookup;
2. it accepts only the fixed-profile metadata approved before guest-memory
   mutation;
3. it preserves `demLoadDos` as the caller of `CreateFileOem`, `ReadFile`,
   `CloseHandle`, `GetVDMAddr`, and `TerminateVDM`;
4. its source overlay, if one becomes necessary, changes only root acquisition
   and maintains original path ownership/lifetime;
5. all diagnostic output contains role, size, hash, and guest address only;
   it redacts local host paths and never copies BYOB bytes into the workspace.

## Not Yet Admitted

- no BYOB file read, manifest implementation, image placement, or bootstrap
  execution is authorized by this record;
- no direct call to `demLoadDos` is an acceptable substitute for NTIO's SVC;
- no `GetSystemDirectory` hook, import patch, registry setting, system file,
  injected DLL, or private API is permitted;
- `TerminateVDM` remains a future explicit session-stop seam, not a local
  success/failure stub.

## Required Evidence Before Implementation

1. a selected manifest passes the existing profile rejection contract before
   any guest write;
2. the prepared-entry trace reaches original `SVC_DEMLOADDOS` from an NTIO
   prefix rather than a synthetic direct call;
3. a bounded success and missing-file failure prove that original `demLoadDos`
   owns placement and its failure path reaches the documented runner stop
   boundary;
4. the resulting process touches only the explicit BYOB root and its own
   memory/diagnostic output.
