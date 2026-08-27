# M0 T280 S15 — DEM host-identity facade

Recover the smallest adapter-softpc facade needed by original `demsrch.c` to
persist a native `PFFINDLIST` as a 32-bit surrogate.  The original source has
no call-shaped equivalent: NT4 stored its x86 process pointer directly in the
DTA/FCB.  Direct reuse therefore fails on x64 and violates the project's
pointer-transport rule.

The facade shall use only `session_thread_current()` and that session's
existing `host_resource` mapping-manager instance.  It publishes, resolves
and releases opaque native values.  It returns a fixed-width surrogate, never
a host pointer, and does not acquire guest memory, close resources, select a
DEM service, or interpret an `FFindId`.

The interface is an explicitly registered source-derived replacement: it
preserves the original DTA/FCB storage width, lookup-before-use and
release-before-free contract where original direct pointer storage cannot
compose.  A focused x86/x64 fixture must prove no-bound-session failure,
publish/resolve identity, release/stale failure, distinct session isolation,
and disposal invalidation.  The original `demsrch.c` body is not modified in
this S; that follow-on must record its local DIVERGENCE markers separately.
