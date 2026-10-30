# Current package: prebuilt DPMI and XMS witnesses

Three SHA-256-identified existing DOS probes ran in independent copies of the
same sealed package, without rebuilding guest binaries. The `prebuilt-int16`
guest reported 16-bit interrupt return, its fault-return negative control,
and hardware IRQ return. `prebuilt-int32` reported the corresponding 32-bit
markers and hardware IRQ return. `prebuilt-xms` reported forced relocation,
data preservation and release. Each Run16 exited zero. Package identities,
probe hashes, console markers and results are retained in each subdirectory;
the test-only runner uses a short PATH and the Z: mapping was removed.

These are bounded live guest observations for these prebuilt probes, not
proof of every interrupt, exception, device or allocator condition. Their
source build provenance is not established merely by matching marker names.
No guest image, product source or installed image was modified.
