# M0 T310 S11 — CCPU effective-address owner-contract review

## Question

Can the selected CCPU `effective_addr(seg, offset)` route retain its original
fixed-width guest-linear contract on both host architectures without restoring
the excluded NT kernel VDM/MONITOR process-pointer alias?

## Original contract and selected path

`base/ccpu386/cpu4gen.h` maps `effective_addr` to
`c_effective_addr(IU16, IU32)`, and `c_main.h` publishes the same external
shape.  The direct original CCPU addressing body (`c_addr.c`) computes Intel
offsets only; the selected external segment-to-linear route is the named
adapter.  The historical `v86/monitor/i386/sas.c` implementation returned a
`Sim32GetVDMPointer` host alias and remains outside the selected profile.

The selected callers—including BIOS, disk, DOS/EMS, input and video packages—
consume the value as an `IU32`/`sys_addr` guest-linear quantity before using
their own SAS operation.  They receive no host pointer.  The separately owned
`c_sas_touch` physical-pointer operation and `c_VirtualiseInstruction`
machine-transaction operation are not enabled or altered by this packet.

## Disposition

`adapter-mvdm-host-out/softpc/mvdm_softpc_effective_address.c` remains the
single same-shaped adapter-backed provider.  It retains original real/VM86
`selector << 4` arithmetic, uses the selected original CCPU segment cache for
cached protected selectors, and uses the original descriptor-reader contract
for other selectors.  No mapping-manager identity is appropriate: the input
and output are numeric guest values.  No native pointer, MONITOR, kernel VDM
or Bochs route is selected.

The focused CCPU Ninja manifest now explicitly links this already-admitted
provider, so the runtime proof exercises the same formal selected closure
rather than relying on an accidental unresolved external.

## x86/x64 evidence

Fresh MSVC `/MT` x64 and x86 146-node selected CCPU graphs linked
`ccpu-bounded-execution.exe`.  On both targets the focused fixture verified
real-mode `1234:5678 -> 000179b8` and `0000:8000 -> 00008000`, then continued
through original SAS access-table/vector RAM checks, bounded CCPU/FPU
execution and `returned-recursive`.  The reviewed adapter compiled with no
effective-address width or callable-contract diagnostic; unrelated original
source diagnostics remain visible for their own clusters.
