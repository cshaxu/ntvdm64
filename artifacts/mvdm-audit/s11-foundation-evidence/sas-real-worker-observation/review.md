# SAS tail breakpoint: real DOS worker observation

A 64-bit debugger started Run16 using the existing fresh-identity short-path
stage and observed its descendants from process creation. The selected input
PE/map binds both SAS tails to the same `int 3` at RVA `0x1782e0` (see the
[static review](../../shard-3/sas4gen-vector-audit.md)). The worker was
observed at relocated base `0x00510000`; the observer watched
`0x006882e0` for both ordinary and WX86 breakpoints. The unchanged staged
`MEM.EXE` was launched by Run16; worker, launcher and enclosing debugger run
all exited zero, and no target breakpoint was observed. A separate no-debug
same-setup control returned exit zero. Captured events, stage manifest,
image hashes, observer source and run wrapper are retained here.

Earlier trials failed before this result. Source and stack evidence identified
a real debugger protocol issue: `STATUS_VDM_EVENT` is an original VDM event;
the worker's separate WOW64 `DebugBreak` was preceded by its same-thread
`NTVDM:BOP_DBGBREAKPOINT` debug string. Original BIOS table maps 5B to
`MS_bop_B`, which emits this string and calls the original `DbgBreakPoint`;
the observed stack returns through the selected NTVDM `DbgBreakPoint` and
CCPU BOP-dispatch call. The observer continues only that explicitly marked,
same-thread BOP break; other noninitial exceptions retain the ordinary
unhandled disposition. It does not replace guest behavior. The module/RVA
analysis is in the preceding [non-pass record](../sas-debug-startup-nonpass/review.md).

This is a bounded observation of a single DOS launch through worker exit,
with debugger presence and fresh package identity. It is not proof of a MEM
output payload, every DOS/WOW workload, startup before process creation,
non-CCPU builds, external DLL consumers or general tail unreachability. The
sealing changes package identity; static map proof concerns the matching
pre-seal input, not independently read mapped instruction bytes. No product
or guest binary was edited or rebuilt.
The retained event text normalizes line endings and trailing whitespace; raw
output remains in the task build directory.
