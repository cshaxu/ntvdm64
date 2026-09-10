# T225 S8 DOSBox Completion Runner 001

## Purpose

`Invoke-DosBoxBatchWithCompletion.ps1` is a build-host lifecycle shim for
historical DOS tools.  DOSBox can return control to PowerShell before its
emulated child has flushed output, so host-process exit is not an adequate
completion signal for a source-build stage.

## Contract

The runner accepts only an already existing directory beneath repository
`build/`, a relative batch filename, and a relative completion-marker path.
It refuses paths outside that directory, missing batches, existing completion
markers, and unavailable DOSBox.  It mounts the stage as DOS `C:`, calls the
batch, and waits at most the declared timeout for the batch-owned marker.
It does not provide DOS command arguments, interpret generated files, copy
guest artifacts, or change OpenNT/Bochs/adapter behavior.

## Verification

A fresh NTIO message-bootstrap stage used the original OpenNT 4.5 `BUILDIDX`
and `NOSRVBLD` commands followed by `echo done > DONE.OK`.  The runner observed
the marker within its 30-second bound.  Post-completion SHA-256 checks proved
all `MSBIO.CL3..6` files exactly equal their managed OpenNT inputs.

## Disposition

The runner is admitted solely as a `tools/build` host-process completion shim
for staged historical DOS tool invocations.  Original batch commands retain
all tool semantics.  The next NTIO stage must use the same explicit marker
contract and independently verify every declared build output.
