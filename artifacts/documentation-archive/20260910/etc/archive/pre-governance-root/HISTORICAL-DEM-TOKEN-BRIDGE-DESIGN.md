# Historical DEM Token-Bridge Design

## Decision

M27's token is an x64-safe replacement for the historical value stored in the
NTDOS SFT.  It must not be cast to `HANDLE` and passed through the original
Win32 calls.  The first bridge is instead a narrow, provenance-visible source
adaptation inside the existing four original functions:

```text
demOpen        : GetVDMAddr(DS:SI) -> OEM path/drive decode -> token_open
demRead        : AX:BP token + DS:DX guest buffer -> preflight -> token_read
demChgFilePtr  : AX:BP token -> token_seek
demClose       : AX:BP token -> token_close
```

`DemDispatch`, `apfnSVC`, `MS_bop_0`, the SVC byte/IP advance, the NTDOS SFT,
PSP, file position, and EXEC code remain original and unchanged.  The bridge
is not a replacement selector table or an adapter that returns a completed DOS
operation outside DEM.

## Why an Alias HANDLE Is Rejected

`demOpen` serializes `HANDLE` into `BP:AX` in `demfile.c:319-320`, while
`demClose`, `demRead`, and `demChgFilePtr` reconstruct it with `GETHANDLE` in
`demhndl.c:51`, `124`, and `374`.  A 64-bit native handle cannot survive that
transport.

More importantly, simply treating the token as a fake `HANDLE` is not safe:
`demClientErrorEx` in `demerror.c` calls `GetDriveLetterByHandle` for hard
errors.  That function uses `NtQueryVolumeInformationFile` and `NtQueryObject`
on its argument.  A token would be an invalid native object.  No global API
interposition or fake-handle table is admitted.

## Exact Historical Call Sites

The original dispatcher already maps the four relevant selectors without a
local substitute:

| Selector | Original owner | Bridge work |
| --- | --- | --- |
| `SVC_DEMCHGFILEPTR` | `demdisp.c:99` -> `demChgFilePtr` | replace `SetFilePointer` call only |
| `SVC_DEMCLOSE` | `demdisp.c:101` -> `demClose` | replace optional seek and `CloseHandle` only |
| `SVC_DEMOPEN` | `demdisp.c:117` -> `demOpen` | replace the normal-file open/size branch only |
| `SVC_DEMREAD` | `demdisp.c:121` -> `demRead` | replace optional seek and `ReadFile` only |

NTDOS's normal SFT path proves the token remains an opaque 32-bit value.  For
example, `file.asm:212` invokes `SVC_DEMOPEN`, then `file.asm:218-219` stores
`BP:AX` in `sf_NTHandle`; `handle.asm:455-466` reloads that same pair before
`SVC_DEMREAD`; `handle.asm:138-140` reloads it before `SVC_DEMCLOSE`; and
`handle.asm:695-698` reloads it before `SVC_DEMCHGFILEPTR`.  No DOS assembly
change is needed or allowed.

The initial profile excludes FCB I/O, lock, file-time, commit, write, named
pipe, and directory/FCB enumeration.  These paths also consume the SFT value,
but they are outside the four-operation admission.  If a later trace reaches
one, it requires its own token-aware host-operation extension before it may
run.  A normal first command must fail closed rather than feed its token to a
raw-handle operation.

## Required Bridge ABI

The bridge is private to the historical host island and calls the M27 table
through fixed-width C functions.  Before M27's wide relative path interface,
it must perform exactly these conversions:

1. copy the NUL-terminated OEM pathname from the validated guest `DS:SI`
   range without retaining a guest pointer;
2. decode its DOS drive and map it through the immutable contained namespace;
3. convert the root-relative component to UTF-16 without best-fit fallback;
4. call `open_read` only for the read-only `demOpen` mode admitted by the
   profile;
5. publish M27's token through the existing `setBP(low16)` / `setAX(high16)`
   and size through existing `BX:CX` ordering, with `DX=0` and `CF=0`.

For reads, the backend-facing portion must preflight the full `DS:DX,CX` guest
write range before host I/O and commit only the bytes read.  If translation or
range validation fails, it must set a DOS-mappable error and enter the
original `demClientError` path without a partial guest write.  M27 itself does
not own guest-memory translation.

For any error after token lookup, the bridge must preserve the original
`demClientError` register/error behavior but supply `INVALID_HANDLE_VALUE` and
the token's recorded DOS drive for hard-error classification.  This keeps
`GetDriveLetterByHandle` from receiving a synthetic token.  For a path-open
failure, the existing filename drive remains the error-context source.

## Source and Link Boundary

The modified form, when admitted, must live as an explicitly named historical
bridge overlay separate from the current compiler-only OpenNT overlays.  It
must include the provenance of its base source, the exact changed call sites,
and a test target that links neither into `ntdos64-run` nor a default target.
It cannot be hidden behind a `HANDLE` macro, a global Win32 hook, or a silent
replacement of `demfile.c` / `demhndl.c` in the generic historical object
target.

## Outcome

M28 is complete as a bridge-design record.  The remaining execution-side
precondition is not another file API: it is a neutral guest-memory lease and
transaction contract for `demRead`, plus an execution backend that can reach
the original DEM chain.  Neither is supplied by M28.
