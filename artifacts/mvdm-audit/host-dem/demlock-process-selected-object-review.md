# DEM lock cross-process selected-object probe

The original DEM `demLockOper` translates guest register values to a host handle, byte offset and length and calls Windows `LockFile`/`UnlockFile`; the original error path delegates to `demClientError`. The host owns process, handle and byte-range lock lifetime, and the guest source assigns the DOS SFT handle. See `demlock-selected-object-review.md` for source identity and dispatch boundaries.

`demlock-process-selected-object-fixture.c` SHA-256 `185A9E18748A1432A0C043E2D8CD392C29134FC5EE13D5D2054423D106C5B860` linked the selected `demlock.obj` SHA-256 `AD3E46A85624561D249A0FA31B475C8F34781A817A1B4973FBFBC3A6005BFD48` into an x86 executable, with controlled CPU register and error seams and real Win32 file operations. The parent and its test-owned child independently opened a temporary file. Child acquired a one-byte lock and signaled an event; parent observed a conflicting lock, then terminated and waited for that test child, and acquired and released the same lock. No external process was touched. MinGW i686 GCC with `-Wl,--gc-sections` reported `Warning: corrupt .drectve at end of def file` but succeeded. Exit 0, output:

```text
cross-process conflict cf=1 error=33 errors=1
after owner exit cf=0 errors=1
```

This confirms host-OS lock release when that separate test process dies, through the selected DEM function. It does not demonstrate guest task termination within a surviving worker, actual DEM error conversion, original SVC dispatch, or installed image identity. The temporary file and processes were cleaned up; no guest or shared ledger changed.
