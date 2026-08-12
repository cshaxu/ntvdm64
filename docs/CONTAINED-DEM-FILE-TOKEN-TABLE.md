# Contained DEM File-Token Table

## Scope

This is the M27 implementation proof for the host-operation seam admitted by
`HISTORICAL-DEM-FILE-SHIM-ADMISSION.md`.  It is a modern, default-disabled
fixture only.  It is not linked to `ntdos64-run`, the historical DEM object
set, a BOP handler, or a guest execution path.

The implementation is `src/archived/trace-probes/contained_dem_file_tokens.c` with its private ABI
in `src/archived/trace-probes/contained_dem_file_tokens.h`.  It intentionally exposes only:

```text
session_init(root)
open_read(relative path) -> token32, size32
read(token32, explicit/current offset) -> bytes
seek(token32, offset, DOS-compatible origin) -> offset32
close(token32, optional final offset)
session_teardown()
```

It does not implement DOS path canonicalization, an SVC, a BOP, an SFT, EXEC,
file creation, write, directory enumeration, named pipe, CD-ROM fallback, or
ambient drive discovery.  Those remain the historical callers' responsibility
or later, separately admitted profile policy.

## Token Contract

Each token is structured as `session-id:16 | generation:8 | slot+1:8`.
The table holds at most 64 regular files per session.  The native `HANDLE`
remains private to the slot and is never copied into the token.

* token zero is invalid;
* token lookup checks session ID, slot range, open state, and generation;
* close clears the slot before closing the native handle;
* teardown closes every open native handle and clears the whole session;
* a closed or torn-down token cannot become valid again without a matching
  session ID and generation.

This matches the evidence that NTDOS transports the historical file value via
`AX:BP`, while making the 64-bit host object unobservable to guest or
historical 32-bit ABI code.

## Containment Contract

Configuration accepts one non-reparse-point directory root.  Open accepts only
nonempty relative paths: absolute paths and paths containing a drive colon are
rejected.  The candidate is normalized with `GetFullPathNameW`, checked for a
component-boundary root prefix, and rejected when an existing path component
is a reparse point.  Files open read-only with `GENERIC_READ` and
`FILE_SHARE_READ`; files larger than the 32-bit historical `demOpen` size
contract are rejected.

The actual historical bridge must provide the OEM-to-wide conversion and
guest-memory preflight before calling this table.  M27 deliberately does not
pretend that a host buffer is a guest address.

## Verification

`contained-dem-file-token-test` is `EXCLUDE_FROM_ALL`.  It was built and run
successfully on 2026-08-08 using the recorded MinGW-w64 CMake cache:

```text
cmake --build artifacts/build/current/default-mingw-verify \
  --target contained-dem-file-token-test
contained-dem-file-token-test.exe
```

The test creates an owned temporary root and verifies:

1. open/read returns the expected content and a nonzero opaque token;
2. explicit seek and current-position read preserve cursor behavior;
3. a token from session A fails in session B with `ERROR_INVALID_HANDLE`;
4. `..\\escape.txt` fails with `ERROR_ACCESS_DENIED`;
5. close invalidates the old token, and a new session gets a different token;
6. teardown invalidates the previous session's token.

A subsequent default build completed without building this target.  The target
appears in CMake's help list only as an explicit selectable target, alongside
`ntdos64-run`; default output did not list
`contained-dem-file-token-test`.

## Next Boundary

M27 proves the x64-safe resource ownership and containment half of the seam.
It does not make historical DEM runnable.  The next task must specify the
smallest provenance-visible replacement of the direct Win32 operations inside
the original `demfile.c` / `demhndl.c`, including OEM pathname conversion,
register result preservation, guest-memory transaction preflight, and original
`demClientError` inputs.  It may not replace `DemDispatch` or DOS file logic.
