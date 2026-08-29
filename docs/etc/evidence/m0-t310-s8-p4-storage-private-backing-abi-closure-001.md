# M0 T310 S8 P4 — fixed-disk private backing ABI closure

## Scope

This closure covers only the private aligned scratch-buffer backing in original
`src/mvdm-host/softpc.new/host/src/nt_fdisk.c`.  It is part of the current
whole-tree x86/x64 static ABI audit for `mvdm-host` and `mvdm-support`.

## Source finding

The original source stored the result of `malloc()` in `DWORD disk_buffer_pool`
and reconstructed a `PBYTE` after alignment.  That was a native x86 pointer
coincidence.  The value is neither a guest address nor an externally visible
handle: it is private host heap backing used only by `get_aligned_disk_buffer`.

## Disposition

`MVDM-HOST-DIV-069` keeps the source's lazy allocation and alignment order,
but stores the backing as `PBYTE` and performs the alignment calculation as
`uintptr_t`.  No mapping-manager identity is used or created.

## Verification

Forced formal Ninja object builds succeeded for the same selected source on
both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/storage-forced-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/storage-forced-x86.log`

Neither log contains the former `C4311` pointer truncation or `C4312`
fixed-width-to-pointer conversion at this backing path.  Unrelated historical
diagnostics remain visible and are not suppressed by this closure.
