# M0 T400 S9 — bounded WRITE segment burst sampling

The x86 Console observer ran the unchanged staged WRITE workload with the
S8-proven `0x172c` selector and a 1 ms host wait cadence.  It uses only
`ReadProcessMemory` against CPU40 globals and writes no target memory.

```text
result=timeout
elapsed-ms=30313
cpu40-samples-attempted=1930
cpu40-samples-readable=1929
cpu40-samples-coherent=1853
cpu40-cs-probe-selector=0x172c
cpu40-cs-probe-cadence-ms=1
cpu40-cs-probe-hits=0
```

The final ordinary state remains the later modal-wait CS `0x021f:0xaebc`.
No hit is **not** evidence that WRITE segment 5 or `FInitMemory` did not run:
the loader/initialiser can execute before the process becomes stably readable
to an external sampler.  This closes the non-mutating external-sampling route
and selects no runtime repair.

The next required witness is a separately-built, explicitly diagnostic x86
CPU40 image that records a bounded `CS=0x172c` IP sequence without changing
the decoded instruction, guest state or production artifact.
