# T95 S6 Intact Native Build Observation 001: External Timeout

## Result

The one admitted foreground invocation of the intact native target was started
with the recorded MSVC x86 environment.  It did not reach a Bochs compiler,
archive or linker diagnostic.  The execution host terminated the foreground
command after approximately 65 seconds.

At inspection time no `nmake`, `cl` or `link` process remained.  The retained
log ends during the original CPU archive compilation at `xsave.cc`; it has no
NMAKE terminal error, archive command, final link command or target output.

## Preserved evidence

- `intact-native-container-build-001.log` contains the exact pre-timeout
  compiler sequence.
- 74 original `.o` files exist in the copied fresh root.
- No archive file, `ntdos64-native-container.exe`, or produced link map exists.
- The log has no adapter or OpenNT path.

## Classification

**External execution-time-limit interruption.**  This neither proves that the
native target builds nor identifies a Bochs source/toolchain failure.  The
copied root and partial objects are immutable evidence and must not be resumed
or repaired; the packet prohibited retry in the same root.

## Next gate

Any complete target observation requires a new fresh root and a separately
admitted detached/monitored execution mechanism that can preserve one command's
identity and final exit status without the foreground timeout.  It must still
be one invocation, have no automatic retry, and must not run the resulting
executable.
