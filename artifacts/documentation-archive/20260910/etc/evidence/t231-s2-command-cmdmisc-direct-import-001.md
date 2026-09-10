# T231 S2 OpenNT `cmdmisc.c` Direct-import witness

## Question

Can the original OpenNT `cmdComSpec` (`54:02`) and `cmdGetCurrentDir`
(`54:04`) bodies execute in the current x64 `/MT` composition without adding
a new COMMAND provider or retaining a COMMAND v1 route?

## Inputs

- `src/opennt/base/mvdm/dos/command/cmdmisc.c` through the exact T231 S1
  import record;
- admitted mirror `src/bx-vdm/bop/opennt/command/cmdmisc.c`;
- `src/bx-vdm/bop/shim/command_misc_shim.{h,c}`;
- `tests/bx-vdm/t231_s2_command_misc_direct_import_fixture.c`;
- the fixed-width exception, CPU-state and typed-resume ABI sources.

## Procedure

The admitted translation unit replaces only its unavailable NT4 include
closure with `command_misc_shim.h`.  It uses a translation-unit guard to omit
the unrelated queue, registration, console and startup bodies; those bodies
remain unmodified in their original positions and have no emitted substitute.
The shim validates the fixed real-mode call, reads a bounded COMSPEC C string
through checked RAM, exposes only the original register spellings and maps
the result through the typed resume record.  For current-directory lookup it
uses the original `=X:` environment and OEM Win32 calls against the actual
host drive.

The focused MSVC x64 `/W4 /WX /MT` build compiled `cmdmisc.c`, the shim and
the fixture in `build/M0-T231-S2/command-misc-direct-r1/manual/`, linked with
the required `kernel32.lib` and `user32.lib`, then ran:

```text
T231 S2 direct OpenNT cmdmisc import: COMSPEC and current-directory ABI verified
```

## Observations

- `54:02` first call retained the original `COMSPEC=` shared-storage update
  and AL result; its repeat-call ordinary return did not manufacture a CPU
  delta.
- A non-NUL COMSPEC span exceeding the original 64-byte source capacity was
  rejected before entering the original body.
- `54:04` initialized/read host `C:` through its original `=C:` environment
  convention, copied the result into checked guest `DS:SI`, and returned the
  original clear-CF success.  An invalid drive returned CF with AX zero.
- No `bop-v1` object or dispatcher entered this focused build.

## Interpretation and limits

This is local Direct source parity for two original functions, not COMMAND
family closure, engine ingress replacement or native guest proof.  The formal
Ninja manifest now admits the same source and fixture, but a full graph run
is deferred to the T231 family closure because its first clean root rebuilds
the complete five-module closure.  That deferral does not change the local
MSVC compile-and-run witness above.

## Follow-up

S3 admits the neighboring original `cmdSetInfo` registration/context contract
and `cmdSaveWorld` ordinary no-op, then replaces their old product routes.
