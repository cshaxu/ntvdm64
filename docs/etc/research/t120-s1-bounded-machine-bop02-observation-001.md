# T120 S1 bounded machine-BOP `02h` observation 001

The single permitted observation used frozen T119 r3 executable SHA-256
`F58EB208CADF1A9D8B2A33ECA6465A793E850EF72A8A00B60A787B0AE9250389`, one
million benchmark instructions and a 15-second watchdog. Evidence is retained
at `artifacts/analysis/t120-s1-bounded-machine-bop02-001/`.

The watchdog terminated the process (`exitCode: 1`). The log records the
existing startup transaction and three commits, then the known native prefetch
veto at physical `0xAF6AF`. It records zero BOP identity observations because
the T119 r3 exception compile enabled machine composition but not the existing
read-only BOP listener.

Therefore this run neither proves nor disproves reachability of selector
`02h`, and it makes no handler-call or resume claim. The decisive composition
finding is that direct start reintroduces the already-known aperture lifecycle
boundary before an observable BOP identity. T121 is limited to combining the
existing deferred-POST path and listener with the unchanged machine seam;
there is no second run in T120.
