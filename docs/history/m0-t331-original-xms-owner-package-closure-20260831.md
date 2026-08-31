# M0 T331 — Original XMS owner-package closure

T331 closed the selected original `xms.486` owner package. Original XMS and
SubAlloc code remains the policy and allocator owner; the only modern seam is
the session-bounded CPU40 SAS provider, which neither exposes host pointers
nor creates a second allocator.

The original move/memory boundary has paired x86/x64 focused evidence. The
selected product also links on both architectures without `/FORCE`: Win32/x86
retains the original process-wide `fprt.c` `printf` interposition, while Win64
privately binds the source-local CRT interposition surface to coexist with
UCRT. One unchanged fixed-container x86 observation exits `0xc0000005`, the
already-classified CPU40/worker integration boundary; it does not identify a
new XMS repair.

Evidence: [T331 S1](../etc/evidence/m0-t331-s1-current-original-xms-owner-abi-rebaseline-001.md),
[S2](../etc/evidence/m0-t331-s2-original-xms-cpu40-session-binding-001.md),
[S3](../etc/evidence/m0-t331-s3-original-xms-package-contract-matrix-001.md),
and [S4](../etc/evidence/m0-t331-s4-original-xms-formal-product-closure-001.md).
