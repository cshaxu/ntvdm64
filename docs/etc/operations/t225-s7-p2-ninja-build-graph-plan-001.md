# T225 S7 P2 Ninja build-graph plan

P1 proved that incremental rebuilds are valuable; P2 adopts Ninja, rather than
the custom object cache, as the formal executor. The generator consumes only
audited manifests and writes a local graph beneath `build/t225-s7/`.

The graph uses MSVC x64 `/MT`, `deps = msvc` and `/showIncludes`. Each admitted
module emits a static library. Every fixture and CLI executable is a separate
link edge, so a requested target is linked afresh from the current module
libraries. A configuration manifest hashes the compiler identity, flags,
include order, CPU5/P-MMX projection, module source lists and forbidden-input
list; a changed identity regenerates the graph.

P2 starts with the already self-contained `bx-vdm` ABI fixture, then maps the
complete `bx-core`, `bx-mantle`, `bx-vdm`, `opennt-host` and `cli` closures from
the existing audited manifests. Missing module closure is explicit; no glob or
ambient source discovery is permitted.