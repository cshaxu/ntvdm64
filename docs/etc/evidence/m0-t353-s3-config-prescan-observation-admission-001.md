# M0 T353 S3 — CONFIG pre-scan observation admission result

## Formal link

The existing selected CPU40/x86 formal graph rebuilt exactly the affected
adapter object, `softpc-bindings.lib`, and `original-softpc-process.exe`:

```text
[1/3] mvdm_softpc_termination.c
[2/3] softpc-bindings.lib
[3/3] original-softpc-process.exe
```

The only production change makes the already decoded, default-off BOP ingress
record append to the existing child-only report when the observer supplies a
path.  It is registered under `MVDM-HOST-DIV-164`; it does not read guest
memory or alter BOP, CPU, guest, file, mapping or session state.

## Invalid observation

One S3 launch was attempted against the new immutable 39-character-root stage
`build/T353S`.  The requested report path was under `build/M0-T353/S3`, but
that parent directory had not been created before the observer launch.
Consequently the observer had no writable main report or child report path and
could not preserve result/marker evidence.  The product child may have run,
but this attempt has **no attribution value** and must not be compared with
the frozen sequence.

S3 permits only that one observation.  It makes no continuation conclusion.
S4 must pre-create a new report directory and make one fresh observation using
the same staged immutable media and the already linked product; it must not
change product sources, media, command arguments, system-root length or the
observer.
