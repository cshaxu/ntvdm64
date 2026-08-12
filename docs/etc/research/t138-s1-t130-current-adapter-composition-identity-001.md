# T138 S1 T130/current adapter-composition identity audit 001

## Comparison

T130's retained derivative manifest and T136 r4's inherited manifest have
identical hashes for every adapter and CLI source, `main.cc`, `exception.cc`,
the exception header, machine component, and retained machine archives. The
T130 binary copied into its observation bundle is SHA-256
`2E591BDC8FA846544B1EA9AEFBE2EE54643C0EDCB382F3D1C1063953CDDA2404`.

The only intentional engine difference is T136's new direct object:
`cpu_trace.obj`, compiled from `cpu.cc` with BX-TRACE-056 enabled, placed
before retained `cpu/libcpu.a`. The T136 map consequently resolves not only
`prefetch`, but also `cpu_loop`, `getICacheEntry`, `linkTrace`, `repeat`, and
`repeat_ZF` from that direct object.

## Result

T137's earlier exit at `50:36` is not an adapter, profile, ROM, BOP, provider,
or machine-component identity difference. It is an **object-granularity
non-equivalence**: Bochs 2.6 groups the desired prefetch function with core
CPU-loop symbols in one translation unit, so a direct prefetch override
changes the linked CPU object as a whole even when the source diff is only a
fault-only log.

This means the T136 run cannot compare control flow with T130 and cannot
source-classify T134's predecessor. A full CPU archive rebuild might restore
internal object consistency, but is explicitly outside the admitted minimal
closure and would require a new, independently justified build/semantic audit.

## Disposition

BX-TRACE-056 is retained as an implemented but **not behaviorally comparable**
diagnostic. No second run, BOP change, adapter correction, or CPU archive
rebuild is admitted. T139 records the return to the last reproducible T130
baseline and the evidence needed before any future post-handoff observation
can be reconsidered.
