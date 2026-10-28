# DEM directory selected-object probe

Original DOS `dos/v86/doskrnl/dos/path.asm::$MKDIR` translates the path, passes `DS:DX` and explicitly zeroes BX/SI before `SVC_DEMCREATEDIR`; `$RMDIR` forwards `SVC_DEMDELETEDIR`. `demdisp.c` routes these services to `demCreateDir`/`demDeleteDir`. The original DEM functions map the guest address, call the OEM adapter for the host directory operation, clear CF on success and pass failures to `demClientError`. Windows owns the actual directory; DEM owns argument translation and error forwarding. Source review does not establish that the installed immutable guest was built from these exact sources.

Current `src/mvdm/dos/dem/demdir.c` and OpenNT's version match SHA-256 `7013B557F8D89527E719484C032BCB3BEFE8BF6F79CBD7C97EAE49C737E1B9AC`. A dedicated x86 executable linked the existing selected object `build/.ninja/msvc-x86-release/worker/obj/dem/demdir.obj` SHA-256 `C896938C307E25F03D083142D04407D0316515E5B90ECAA89EA7BAE6D88A9F11` with `demdir-selected-object-fixture.c` SHA-256 `52A79995F60C3EE107369CEC66F9F1C475D4CCFC62B05119FBB6C10B67E39ADE`. MinGW i686 GCC with `-Wl,--gc-sections` reported `Warning: corrupt .drectve at end of def file` but succeeded. The fixture's OEM seams call real Win32 ANSI CreateDirectoryA/RemoveDirectoryA on an isolated temporary directory; registers, guest address, and error handler are controlled seams.

The create and delete success paths cleared CF; repeated create and repeated delete reached the error seam with real errors 183 and 2. A direct synthetic call with BX=1 returned before calling the host create function and left CF unchanged. The executable returned 0 and printed:

```text
duplicate create cf=1 error=183 errors=1
missing delete cf=1 error=2 errors=2
extended-attributes cf=9 create_calls=2 errors=2
```

The nonzero-BX behavior is an original early return marked as unsupported extended attributes; ordinary `$MKDIR` explicitly sets BX/SI to zero, so this test does not show an observable guest defect. Neither the guest, DEM dispatcher, actual OEM conversion nor the original `demClientError` ran; the seam sets CF and records the native error. Non-ASCII conversion, guest pointer lifetime, directory semantics, set-current rollback, current package identity, and real guest error translation are not established. No product, guest image or shared ledger changed. Prior historical source and bounded label observations remain in `shard-1/dem-directory-label-audit.md`.
