# Project Status

## Current Work

**No active M/T/S packet.** Owner closes M0 T433 with “t收口 队列给我看看”.
[T433 closure](../history/m0-t433-native-components-x64-closure.md) retains
S1–S7 briefs, evidence, limits and final integration audit. Planned S8 is folded
into this owner-directed closure, not a separately executed implementation.
No successor T/S is admitted; [Queue](QUEUE.md) owns candidate order.

## Current Technical Baseline

T433 S7 production d1aa8336c3940e84879d6bc3c625a2ea5175ced1 and closure
31a1487e41657e07d8727b1078c9fc36d0a2f2c8 are committed/pushed.
Final build/M0-T433/S7/r030-final-runtime matches O:/winnt/system32:
APP0.0.433 / RPC41 / I/O25, ten images, unchanged guest/configuration.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll |

NTVDM retains original CCPU40; one NTVWM executes both native widths with
matching Hooks. NTSRV remains task/lifecycle/I/O authority; NTCON stays
worker-neutral. Paths, CLI/search and fixed copied protocols remain.
S7 fixes independent-PIF parent recovery at its existing authenticated boundary;
original worker/ConsoleRecord identities and DOS/WOW execution are unchanged.

[S7 evidence](../etc/evidence/m0-t433-s7-mixed-batch-tests.md) records
Full14/Console17/Window17/WOW, both-width service29, mixed batches16/typeahead6,
exact missing-command10 and actual WOW BAT/UI3. r038 coherent publication
preserves S6 recovery; r039 actual deployed DOS/32/64 smoke and ten hashes pass.
Closure rechecks installed hashes read-only; Z: is removed.

No physical/RDP/gameplay or universal compatibility claim. WINMINE reaches
visible UI; SOL/WRITE retain independent known frontiers. Long-path/default
geometry/large-environment limits and the unclassified rapid-interactive
observation remain as recorded in [TODO](TODO.md) and closure evidence.

## Reusable Build Inputs

All disposable/reusable outputs remain below build/. Native service:
build/M0-T433/S7/r028-native-final. Worker: build/M0-T433/S2/r001.
Frontend: build/M0-T433/S3/r001. Monitor: build/M0-T433/S4/r001.
Launcher: build/M0-T433/S5/r002-native. Hook64: build/M0-T433/S2/r002-hook64.
Formal x86 fixture/worker cache: build/M0-T427/S2/r001.
Use actual consumer ABI and manifests; never mix object/CRT widths or use
diagnostic/superseded dual-worker images as production inputs.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T433 | Selective native migration, mixed-width BAT coverage and PIF parent recovery; owner-directed closure on delivered evidence. [Closure](../history/m0-t433-native-components-x64-closure.md). |
| T432 | Single native worker, dual Hooks and broker I/O release authority. [Closure](../history/m0-t432-single-worker-dual-hook-closure.md). |
| T431 | Accepted Hook32 baseline. [Closure](../history/m0-t431-native-hook32-closure.md). |
| T430 | Accepted non-WOW contract repairs/proofs. [Closure](../history/m0-t430-non-wow-contract-closure.md). |

## Next Admission

Await owner direction. The queue head is NTSRV-owned read-only task traces and
NTMON hierarchy. This is a candidate, not active work or a numeric T allocation.

## Recent Governance

Owner-directed T433 closure is documentation-only: preserve the full previous
status in indexed history, recheck the existing ten-image publication read-only,
and run governance/relative-link/diff gates before commit/push. No product
change, process termination, new test run or successor admission is authorized
by this closure. Candidate order remains unchanged.
