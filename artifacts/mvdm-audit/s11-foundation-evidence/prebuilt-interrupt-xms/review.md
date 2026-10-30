# Current package: prebuilt DPMI and XMS witnesses

Three SHA-256-identified existing DOS probes ran in independent copies of the
same sealed package, without rebuilding guest binaries. The `prebuilt-int16`
guest reported 16-bit interrupt return, its fault-return negative control,
and hardware IRQ return. `prebuilt-int32` reported the corresponding 32-bit
client-frame markers and hardware IRQ return. `prebuilt-xms` reported forced relocation,
data preservation and release. Each Run16 exited zero. Package identities,
probe hashes, console markers and results are retained in each subdirectory;
the test-only runner uses a short PATH and the Z: mapping was removed.

The `prebuilt-int32` marker does not establish a 32-bit code segment. The
retained disassembly of that exact hash shows AX=1 selected at the DPMI entry
and execution continuing as 16-bit instructions, consistent with the earlier
documented 32-bit client-frame variant. It does not exercise the separate
`CODE32` descriptor and far-transfer path in the source. The latter remains
unverified by this run; marker names alone cannot distinguish those modes.

These are bounded live guest observations for these prebuilt probes, not
proof of every interrupt, exception, device or allocator condition. Their
source build provenance is not established merely by matching marker names.
No guest image, product source or installed image was modified.
