# M0 T310 S8 P4 — COMMAND MULTI_SZ pointer-width closure

## Source result

The original `cmdSetDirectories` scans its private host MULTI_SZ buffer for
per-drive `=X:` current-directory entries.  Two expressions converted that
same buffer pointer through `ULONG` merely to read byte two and advance to byte
four.  That was a 32-bit process-address coincidence and produced the x64
pointer-truncation diagnostics in the full-tree P4 ledger.

`cmdmisc.c` now uses the equivalent in-buffer forms `lpszzEnv[2]` and
`lpszzEnv + 4`.  The matching predicate, byte offsets, environment update
order and original error direction are unchanged.  This is private native
buffer arithmetic, so it neither creates a guest address nor uses the session
mapping manager.

The remaining `cmdexec.c` guest-address, opaque-handle, BaseSrv re-entry and
child-process lifecycle records are a separate complete original COMMAND
package.  This small closure does not claim that package is enabled or
runtime-complete.

## Validation

Formal Ninja rebuilt `original-mvdm-command.lib` on both host architectures:

- `build/M0-T310/S8/p1-machine-source/x64/p4-command-multisz-abi.log`
- `build/M0-T310/S8/p1-machine-source/x86/p4-command-multisz-abi.log`

Both archives complete successfully.  The x64 log contains no `C4311` or
`C4312` diagnostic for `dos/command/cmdmisc.c`; x86 follows the same source
path.  Other compiler-visible COMMAND diagnostics remain deliberately
unmodified pending their complete owner-package review.
