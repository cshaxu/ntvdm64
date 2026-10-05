# Project Status

## Current Work

**No active M/T/S packet.** Owner accepts and closes T429 on 2026-10-04.
S1–S8 are closed; no following T or S is admitted. See the
[T429 closure and retained packets](../history/m0-t429-performance-worker-io-closure.md),
[ordered candidate queue](QUEUE.md) and [debt disposition](TODO.md).

## Current Technical Baseline

Accepted production delivery: 5b9931b8e, pushed to main/origin.
The coherent eight-file set build/M0-T429/S8/r008/runtime remains published
at O:/winnt/system32: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe,
ntmon.exe, WOW32.DLL and VDMREDIR.DLL. MSVC14.43/SDK22621, x86 /MT CCPU40;
APP0.0.427/RPC38/I/O25 unchanged. S8/r011 pins publication/recovery hashes;
r013 confirms published Console/Window smoke and all-eight hashes.

Both workers use worker-base publication and event-driven input machinery.
The publisher compares immutable complete state against its last successful
commit, with a maximum50Hz cap. Native20ms sampling only captures active
hidden Console output. NTVDM original input processing remains in place;
NTCON remains format-driven without display deduplication. NTSRV retains
connection/lifecycle authority. No helper or new wire is added.

[S8 verification](../etc/evidence/m0-t429-s8-shared-event-input.md) passes
focused fixtures, Console17/Window17/WOW, EDIT200, all8 integration cases and
published smoke. Physical RDP latency, matched SoftPC comparison and universal
mouse-speed improvement are not claimed. Native capture/final-capture retry
and recorded limits remain; host scrollback is not promised.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T429 | Owner-accepted performance and shared worker I/O; [closure](../history/m0-t429-performance-worker-io-closure.md). |
| T428 | Owner-accepted worker interface unification; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |

## Recent Governance

Owner-directed T429 closure is documentation-only. Retained detailed packets
move to history rather than being discarded. Queue order is unchanged; its
head remains the bounded CCPU40/V86 guest-contract audit. Unrelated proposal
modifications remain untouched and excluded. No implementation may proceed
until the owner admits the next packet.
