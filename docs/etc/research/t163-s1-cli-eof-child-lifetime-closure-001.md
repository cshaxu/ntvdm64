# T163 S1 CLI EOF child-lifetime closure 001

`src/cli/ntdos64_bochs_engine.c` now supplies its noninteractive native-Bochs
child with inheritable `NUL` stdin rather than inherited console input. This
is CLI-only process policy; no handle crosses the CLI/adapter ABI and no
Bochs, guest, BOP, device, or adapter behavior changes.

The existing MinGW CLI build compiled the shim and fake-Bochs probe.
`ntdos64-bochs-engine-policy` passed: its probe requires stdin to be readable
with zero bytes and propagates exit 47, proving EOF and child-exit propagation.
An anonymous pipe was rejected because a writerless Windows pipe produces
`ERROR_BROKEN_PIPE`; `NUL` provides the required successful zero-byte read.

Runtime remains separately admitted.
