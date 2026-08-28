# app

CLI and final composition component. Existing app code is recovery evidence
until its owner/dependency review; provider algorithms do not belong here.

## M0 T273 S3 minimal machine shell

`machine_shell.{c,h}` is the admitted minimum app assembly: a caller-owned
`session` owns teardown, while the shell opens and closes the opaque
`adapter-bochs` machine lifecycle. It does not load a guest, select a BOP,
provide an OpenNT service, expose a Bochs type, or create a worker/scheduler.

## T302 Base VDM producer boundary

The app no longer owns a Base VDM command protocol. It may declare launch
input, but the copied command record, `VDMINFO` shape, dispatch ordering and
session binding belong to `adapter-mvdm-host-out/basesrv`.

## M0 T310 S3 selected backend composition

`machine_shell` composes only a session which made its single backend choice
while inactive. The app's omitted selection defaults to SoftPC; Bochs is an
explicit alternative. The selected Bochs path is the existing `adapter-bochs`
lifecycle. A selected original SoftPC path is explicitly unavailable until
T310 S4 imports its original machine composition; it never falls back to
Bochs. All later functional fixtures must cover both selected backends on x86
and x64; an unavailable backend is a recorded outcome, not a skipped matrix
cell.
