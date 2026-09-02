# M0 T360 S4 — formal original-SystemRoot package closure

The existing formal CPU40/x86 Ninja graph rebuilt the affected app library and
product in four actions. The new product is:

```text
build/M0-T359/S2/formal-x86/original-softpc-process.exe
SHA-256 2A36708E39559472019527A16F14170A0B48B65D843F9D4D0A0906FEE890B1E5
```

`Stage-OriginalSoftpcRuntime.mjs` then created the fresh format-3 package:

```text
build/M0-T360/S4/original-systemroot-package
```

Its manifest records sixteen immutable media assets. A direct SHA-256 pass
verified all sixteen and confirmed that no destination begins with the retired
`mvdm/` layout component. The package is ready for T360 S5's one frozen
observation; no guest process was started by this closure.
