# M0 T356 S4 — Fixed command observation

## Frozen inputs

- Formal CPU40/x86 product source:
  `build/M0-T356/S3/formal-x86/original-softpc-process.exe`
- SHA-256:
  `c92e4cc21f872adc6abc766da94f583fdb751b416d1ed19ce73c457b8328fa65`
- Declared command: `--command EXIT`.
- Declared media root: `O:\ntvdm\mvdm`.
- Observer: the existing non-debug, console-owning
  `tests/observation/console_startup_observer.c`, compiled as an x86,
  task-owned observation binary.

## One observation

The observer was invoked once with the formal executable, the fixed root and
an eight-second bound.  It produced:

```text
container=console-owning-nondebug
result=exited
exit=0x00000041
timeout-ms=8000
fixed-system-root=O:\ntvdm\mvdm
image-identity=unavailable
```

There was no exception, BOP, DEM or guest-progress report.  This is an exit,
not a timeout, and it is not evidence that guest COMMAND, NTDOS `EXEC`, or
`EXIT` ran.

## Source attribution

`0x41` is app-defined `APP_STARTUP_MEDIA_REJECTED` in
`src/app/entry.c`.  `main` returns it before selecting/opening the SoftPC
shell when either `app_package_layout_set_process_media_roots` or the original
COMMAND-root admission check fails.

The product deliberately derives its package roots from its own image path:

```text
src/app/package_layout.c
app_package_layout_set_process_media_roots()
  -> GetModuleFileNameA(NULL, executable_path, ...)
  -> app_package_layout_set_media_roots(...)
  -> <image directory>\mvdm and <image directory>\mvdm\softpc
```

The fresh formal image was in `build/M0-T356/S3/formal-x86`, which has no
adjacent `mvdm` directory.  The observer's current directory and its separate
`O:\ntvdm` root argument cannot alter the image-relative app contract.  Thus
the observed media rejection is source-defined application package assembly,
not an original MVDM/SoftPC/DOS terminal and not a BOP result.

## Disposition

This S4 observation is immutable and will not be retried with changed inputs.
The exact successor owner is the existing product-stage contract:
`tools/build/Stage-OriginalSoftpcRuntime.mjs` creates a new package containing
the formal product as `original-softpc-process.exe` and its unchanged sibling
`mvdm` media; `tools/observation/ObserveSoftpcStartup.mjs` then replaces only
that staged executable for a fixed observation.  A separately admitted
package-stage observation task must use that contract before any guest-execute
or graphics claim can be made.
