# M0 T338 S3 — Original configuration path-capacity transfer

## Exact predicate

The original `cmdconf.c::ExpandConfigFiles` calls
`GetShortPathNameA(systemRoot, achSysRoot, sizeof(achSysRoot))`, where
`achSysRoot` has 64 bytes. Its next condition explicitly treats a returned
length of 64 or greater as zero; the original invalid-system-file branch then
calls `RcErrorDialogBox(ED_BADSYSFILE, ...)` and terminates the VDM.

The previously fixed stage gave its session system root the exact path
`O:\\repos.hobby\\ntvdm64\\build\\M0-T337\\S3\\package-contract-r3\\mvdm`.
The external observer recorded both its ordinary and short-path lengths as
64. This selects that exact original predicate. It is not a missing
`config.nt`, `autoexec.nt` or `COMMAND.COM` condition.

## Same-media confirmation

`Stage-OriginalSoftpcRuntime.mjs` created a separate immutable stage at
`build/r338`; no input bytes or product source changed. Its MVDM root was 38
characters long. The one bounded, console-owning x86 observation recorded:

- `50:11`, `50:3B`, `50:0F`, `50:1B`, `54:05`, then `50:3D` on the unchanged
  original console protocol;
- a rebased original frame chain of `ErrorDialogBox -> RcErrorDialogBox ->
  demExitVDM -> DemDispatch -> MS_bop_0 -> c_cpu_simulate -> host_start_cpu`.

Thus the shorter byte-identical container crosses the normal configuration
path and reaches the previously attributed original `50:3D -> demExitVDM`
transfer. It does not prove execution of a declared DOS child, parent return
or COMMAND `54:0B` completion.

## Product disposition

The immediate T338 recovery is the source-defined packaging precondition:
the executable-relative MVDM system root used by original configuration must
fit the historical 63-character usable short-path capacity. T338 makes no
unsafe source rewrite, synthetic fallback, guest rewrite or configuration
bypass. A general long-install-path product disposition remains explicit
release/package work; it cannot be silently solved by changing the original
COMMAND algorithm.

## Transfer

The earliest runtime owner is again the existing original
`50:3D -> demExitVDM` DEM/guest-control transfer. The next vertical-slice
package may work only from the short-root, byte-identified container and must
not reopen configuration processing as a BOP leaf backlog.
