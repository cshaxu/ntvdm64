# T161 S1 extended native POST observation 001

## Inputs and procedure

T161 verified the immutable T160 executable, profile, and CA800 option-ROM
hashes, then made its one permitted 10-million/30-second deferred observation.
No input, source, build, configuration, provider, or device changed.

## Observation

The observer wrapper timed out during its process-tree cleanup and reported
`Access denied`; consequently it did not write `observation.json`. This is a
wrapper limitation, not a clean watchdog result. The retained `bochs.log`
nevertheless records a complete terminal sequence and no emulator process was
present after the wrapper returned.

After normal `50:36` resume, execution continues through COMMAND-side work,
including selector `54` services and a later finite `50:42` read/close cycle.
Its final trusted boundary is:

```text
50:36 accepted resume at 0032:5a70
... guest command execution ...
FE observed at 0d00:0100
adapter controlled stop accepted
cpu loop quit / quit_sim called with exit code 1
```

The ending is neither a prefetch panic nor a direct-entry/INT10 failure. It
does not itself prove that a controlled stop is a successful CLI completion;
that classification needs the source-defined target/stop contract.

## Interpretation

The first post-`50:36` boundary is the existing controlled-stop BOP, not a
missing BOP provider or Bochs device. No repair is admitted from this trace.
The wrapper's missing JSON remains a bounded evidence limitation.

## Follow-up

T162 must statically correlate `TARGET.COM`, the CLI launch declaration, and
the existing controlled-stop provider with the original expected result and
failure semantics. It must not run, alter the stop provider, or convert exit
code one into a success claim without that evidence.
