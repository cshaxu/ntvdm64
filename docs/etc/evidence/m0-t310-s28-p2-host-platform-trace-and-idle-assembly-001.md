# M0 T310 S28 P2 — host-platform trace and idle assembly

## Scope

This record closes two reached selected x86 CCPU40 link edges only.  It does
not claim that the historical debugger product, an idle scheduler policy, or
integrated machine execution is complete.

## Source-shaped disposition

- `trace_file` is defined by the original SoftPC source
  `mvdm-host/softpc.new/base/debug/trace.c`.  The candidate graph now selects
  that one original translation unit as `original-softpc-base-trace.lib`.
  It deliberately does **not** select `mvdm-host/dbg`: that is the distinct
  MVDM debugger/CSR product and its `dbg.c` requires private debug transport.
- `ActivityCheckAfterTimeSlice` has no C/C++ definition in either approved
  OpenNT source tree.  The imported NTVDMx64 patch
  `mvdm-softpc-patch/patches/common/fmstubs.c` supplies its retained empty
  body.  It is selected to preserve the patch's existing link contract, not
  represented as a recovered idle policy.  The original callers
  `nt_unix.c` and `nt_eoi.c` retain their call order.

## Formal evidence

- Fresh graph: `build/M0-T310/S28/formal-host-platform-x86-r8`.
- `original-softpc-candidate` completed 377 selected Win32/x86 CCPU40 edges.
- `original-softpc-forced-closure.dll` was produced with the original
  trace-only library and the retained patch evidence library whole-archived.
  Its log has no unresolved `trace_file`, `ActivityCheckAfterTimeSlice`,
  `RegisterConsoleVDM`, or `ExitVDM` symbol.

## Remaining boundary

The current empty `ActivityCheckAfterTimeSlice` body must not be expanded by
this platform-binding packet.  A later owner package may replace it only after
it proves the intended SoftPC idle/event behaviour and keeps the same public
call shape.  The obsolete `adapter-mvdm-host-out/monitor/host_idle.c` no-op is
not evidence for that behaviour and is outside this P's selected linkage.
