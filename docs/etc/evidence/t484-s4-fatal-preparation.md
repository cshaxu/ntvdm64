# T484 S4 DPMI fatal preparation and obsolete injection retirement

## Retained coverage and preparation

The existing exact #GP dialog, Ignore/new-thread/same-fault and Terminate
assertions remain useful. Register the unchanged independently authored
dpmi_fatal_exception.asm as FATAL16.COM/FATAL32.COM in the existing native
CMD fixture graph. The runner uses the shared CMD alias and physical-stage
owners, a verified package manifest and the existing isolated process scope.
It no longer copies a probe into a supplied runtime or assumes V: mapping.
Explicit Run16 --wait verifies request completion, not launcher handoff alone.
Reports remain in build/dpmi-fatal-<prefix> unless an explicit log root is given.

The native dialog helper inspects and controls only the owned worker PID on
the current desktop. No Computer Use or manual interaction is involved. This
is process/package isolation, not a private-desktop coverage claim.

## Retired obsolete test

Commit74344f75b (T483 S5 P17) removed the added CPU40 PM-info allocation block
from DpmiPassPmStackInfo. The current owner sets LockedPMStackSel and resets
LockedPMStackCount; it no longer calls DpmiAllocateVirtualMemory or publishes
the historical allocated36-byte record. The old diagnostic's macro replacement
therefore has no call to intercept. Keeping it cannot prove allocation refusal.

Delete dpmi_pm_allocation_failure.c, its worker_trace target, the diagnostic
owner expectation and the runner's allocation-only options/assertions. No
production code is changed and no general out-of-memory coverage is claimed.
Historical evidence remains intact. This fulfills the retrospective retirement
requirement rather than carrying an obsolete migrated target forward.

## Verification

Initial owned Userenv process12208,2026-11-24T18:55:55Z to18:58:48Z, exit0:
cached full build and eight actual guest cases (16/32-bit client frames,
direct/twice-nested COMMAND, Terminate/Ignore then Terminate) pass. Each case
checks the exact fault IP; Ignore requires a different dialog thread at the
same fault. Worker/launcher completion is observed before owned cleanup.
All11 raw host images and22 published hashes are unchanged.

Final owned Userenv process11116,2026-11-24T18:59:41Z to19:02:33Z, exit0,
repeats cached full build and all eight cases after obsolete-injection removal.
All eight pass; fault IPs are00000140/00000141 respectively. All four Ignore
cases prove a new dialog thread at the same fault. All11 raw host images and22
dual-publication hashes remain unchanged. Exact-identity publication reuse is
recorded, not a new Full pass or a product fault-handling fix.

The regenerated worker build.ninja has no pm-allocation-failure target.
Standalone diagnostic owner audit process23108 passes15 retained variants;
that is PE/MAP ownership evidence, not runtime acceptance for those15 variants.
Parser and diff checks pass; final documentation/tracker gates precede commit.

## Progress

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease10 to9. One obsolete diagnostic source, its producer and its
exclusive assertion mode are retired. No original guest is modified or rebuilt;
assets/winnt.zip is untouched. S4 and final S5 acceptance remain open.
