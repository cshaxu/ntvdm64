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

The WOW64 exception address comparison has a separate [positive and
one-byte-offset negative control](../sas-breakpoint-calibration/review.md).
It confirms the observed address convention on this host, without proving
that every workload reaches or avoids the target.

## Mapped-image check

A later fresh-isolation DOS run read the actual worker's memory at its create
process debug event. The mapped `cSasPtrs` initializer entries 54 and 56 both
contained the relocated target `0x003582e0`, and that target contained `0xcc`;
the independent breakpoint-address calibration already established how this
host reports an x86 `int3`. This reads the sealed, relocated process rather
than assuming its text bytes or pointer relocation from the pre-seal map.
The worker and launcher exited zero; no target hit was seen. The result is
one DOS run, not proof of all consumers or of the subsequently copied runtime
`Sas` vector's full lifecycle. `result-v11-summary.txt`, the stage manifest,
image hashes, observer source and raw trace hash are retained; the full trace
is in the active build directory. No product or guest was modified.

## Initialized live vector

In a further fresh-identity DOS run, the observer read both initializer
slots and the `0xcc` target from the actual relocated process at creation. At
the later original BOP 5B debug event, it also read the runtime `Sas` record:
slots 54 and 56 both equaled the same relocated target. Source `sas_init`
uses `SasSetPointers` to copy the initializer. The worker then completed and
Run16 returned zero, without an observed target break. The observer requires
both live-slot comparisons for success. The stage manifest, exact image hashes,
summary, source and full event-log hash are retained alongside this review.
This is one point after initialization, not proof that the slots never change
later or that any untested guest cannot reach them. No product or guest changed.
