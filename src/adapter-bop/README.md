# adapter-bop

Selector-blind BOP ingress adapter.  It accepts the existing fixed-width
generic CPU exception record, verifies its mechanical identity, and forwards
the unchanged copied event to an `app`-registered route callback.  It owns the
typed completion hand-back used by the CPU, but not the interpretation of any
guest bytes.

It must not name or inspect a BOP selector/service, an OpenNT provider, or
DOS/WOW semantics.  `opennt-bop` owns route recognition and provider dispatch;
`app` owns the one-session binding lifetime.  This is project-authored adapter
code, not an imported OpenNT mirror.

## Boundary register

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `ADAPTER-BOP-001` | NT4 connected BOP dispatch directly to its product host. | The modern component graph must not make Bochs or its generic CPU entry selector-aware. | A same-runtime, `app`-owned route binding forwards only the copied event and typed outcome; the route remains in `opennt-bop`. | `bop_ingress_v1.c`, `bop_ingress_v1.h` |
