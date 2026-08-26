# app

CLI and final composition component. Existing app code is recovery evidence
until its owner/dependency review; provider algorithms do not belong here.

## M0 T273 S3 minimal machine shell

`machine_shell.{c,h}` is the admitted minimum app assembly: a caller-owned
`session` owns teardown, while the shell opens and closes the opaque
`adapter-bochs` machine lifecycle. It does not load a guest, select a BOP,
provide an OpenNT service, expose a Bochs type, or create a worker/scheduler.
