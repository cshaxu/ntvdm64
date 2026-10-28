# DEM current-directory selected-object failure probe

`demdisp.c` dispatches `SVC_DEMSETCURRENTDIR` to original `demSetCurrentDir`. It rejects a non-drive-letter first character, calls `SetCurrentDirectoryOem`, and then updates the drive-specific `=X:` variable through `SetEnvironmentVariableOem`. On failure of the second call it sets CF; source does not attempt to restore the already changed process current directory. The modern host process owns its current directory; the OEM and guest boundaries adapt input and errors. This is original control flow, not proof that the source of an installed guest or current release package is identical.

An x86 fixture linked `build/.ninja/msvc-x86-release/worker/obj/dem/demdir.obj` SHA-256 `C896938C307E25F03D083142D04407D0316515E5B90ECAA89EA7BAE6D88A9F11` to `demcwd-selected-object-fixture.c` SHA-256 `3F744802484842A620BAECC9F9B61ED33DF3ADC6AABC3691451A9C6EF03C7631`. The first OEM seam called real `SetCurrentDirectoryA` within the test process; the environment update was a controlled failure. Guest memory/register and DEM error functions were seams. MinGW i686 GCC `-Wl,--gc-sections` returned success with `Warning: corrupt .drectve at end of def file`. The process returned 0 and reported:

```text
environment failure cf=1 cwd_changed=1 env_calls=1 errors=0
invalid drive cf=1 host_calls=1 errors=0
missing cwd cf=1 host_calls=2 error=2 errors=1
```

The first case demonstrates a real process-CWD side effect under an injected environment failure, but does not establish a production failure or that rollback would match original semantics. The third error seam records the Win32 error and synthesizes CF, not original `demClientError` behavior. The isolated fixture restored its own CWD and removed its temporary directory. Actual OEM encoding, real `=X:` variable failure, DOS guest dispatch, task ownership/reentry and installed binaries remain unverified. No guest, runtime source, user directory or shared ledger was changed.
