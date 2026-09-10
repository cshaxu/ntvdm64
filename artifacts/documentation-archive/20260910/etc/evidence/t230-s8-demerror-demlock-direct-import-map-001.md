# T230 S8 — OpenNT DEM Error / Lock Direct-Import Map

## Question

Can the original OpenNT owners of `50:32`, `50:33`, and `50:3F` be
introduced as source-shaped Direct code without retaining the former v1
provider plane or placing CCPU/SAS semantics in Bochs?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demerror.c`
- `src/opennt/base/mvdm/dos/dem/demlock.c`
- `src/opennt/base/mvdm/dos/dem/demdisp.c`
- formal manifest `tools/build/t225-s7-full-module-manifest.json`

The checked-in OpenNT source baseline is revision
`d450bfc1b7711234072eebfdfa23735ea509e7c5`; source notices remain in each
directly imported file.

## Imported shape and seam

The original translation units are mirrored directly at
`src/bx-vdm/bop/opennt/dem/demerror.c` and `demlock.c`.  Other than their
include surfaces, their function order, fixed layouts, register flow and
failure decisions are retained.

`src/bx-vdm/bop/shim/demerror_lock_shim.c` replaces only unavailable historic
composition:

- CCPU/SAS register and checked guest-RAM access reuse the existing neutral
  `demhndl_shim` call context.
- `50:32` retains the original three-byte `VHE` and 18-byte `SYSDEV` layouts
  in TLS-backed fixed mirrors, remembering their original DS:DX and DS:BX
  addresses.  The hard-error layout is flushed through checked guest RAM
  before the active call ends; no host pointer enters a guest layout.
- `50:33` retains `demRetry`'s original save/restore then callback sequence.
  The historical all-service function-pointer table cannot yet be called
  through typed ingress, so every unbound table slot explicitly sets
  `AX=ERROR_CALL_NOT_IMPLEMENTED` and `CF=1`.  It never dereferences NULL or
  reports a fabricated retry success.  Binding an actual retry target remains
  whole-DEM dispatcher work in S10.
- `50:3F` retains the original `GETHANDLE(BX:BP)`, `LockFile` / `UnlockFile`,
  and `demClientError` order.  The host handle is a pointer-width-neutral
  Direct token resolved by the existing neutral token seam.

The native-object-name/symbolic-link helpers retained by `demerror.c` are
declared as explicit unavailable compatibility functions.  They are reached
only while deriving an owning drive letter for a hard error; unsupported
resolution falls back to the original `-1` drive result.  No selector
recognition, legacy-v1 policy, Readonly, Overlay, or Virtual behavior enters
the imported files.

## Focused local Direct witness

The formal MSVC x64 `/MT` Ninja graph compiled the complete `bx-vdm` static
library and linked the focused fixture.  The fixture exercised:

| Entry | Original body | Observed local contract |
| --- | --- | --- |
| `50:32` | `demSetHardErrorInfo` | registers the fixed VHE/SYSDEV guest layouts through the persistent checked-RAM seam, resumes with `CF=0` |
| `50:33` | `demRetry` | restores its saved record and reports the currently unbound original callback slot as `ERROR_CALL_NOT_IMPLEMENTED`, `CF=1` |
| `50:3F` | `demLockOper` | resolves one opaque `BX:BP` token to a real temporary-file handle; both original `LockFile` and `UnlockFile` success paths return `CF=0` |

This is local Direct source parity only.  It does not claim an INT 24 UI,
cross-package retry dispatch, selector ingress, or native guest execution.

## Verification

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T230-S8\direct-import-r3
ninja -C O:\repos.hobby\ntdos64\build\M0-T230-S8\direct-import-r3 `
  bin/t230-s8-demerror-lock-direct-import-fixture.exe
& O:\repos.hobby\ntdos64\build\M0-T230-S8\direct-import-r3\bin\t230-s8-demerror-lock-direct-import-fixture.exe
```

Observed output:

```text
T230 S8 direct OpenNT demerror/demlock import: hard-error setup, retry failure and file lock contracts verified
```

## Follow-up

S9 imports `demmisc.c`.  S10 alone may bind retry callbacks to the full
source-shaped DEM dispatcher and conduct bounded native observation.
