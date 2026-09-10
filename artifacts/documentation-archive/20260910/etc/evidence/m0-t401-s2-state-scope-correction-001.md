# M0 T401 S2 — data-state scope correction

## Correction

The S2 observation found `fMessageInzFailed == 0` in the uniquely bound WRITE
DGROUP during a Kernel31 GPF terminal. This proves only that execution did not
reach either source statement which writes that flag:

- `MmwCreate`'s `Error:` epilogue; or
- `MdocCreate`'s `GetDC(...) == NULL` error branch.

It does **not** prove that `MmwCreate` or its `FInitMemory()` call was never
entered. A General Protection Fault can interrupt either routine before it
returns to the normal error epilogue. Accordingly, any statement that S2
eliminated the whole `MmwCreate`/`FInitMemory` direction is withdrawn.

The corrected disposition is narrower: no normal `fMessageInzFailed`-setting
exit explains the observed GPF. A source-adjacent pre-fault contract is still
required before any repair can be considered.

This correction supersedes the over-broad phrase in the current STATUS S2
closure note as well as any summary derived from it. It does not change the
run inputs, observer result, or the separate low-memory-dialog evidence.
