# T429 S5 software-video publication admission and source audit

## Request and baseline

The owner closes delivered S4 and admits S5 to restore timely software-VGA
publication, with a minimal registered mirror exception. Build and focused
verification precede coherent early publication for owner testing; the full
product regression follows publication and still precedes production P closure.
S4's pushed delivery and unchanged production0654f5e0b remain the recovery
baseline. Early publication is not full validation or S closure.

## Source observations

At main9d970e08b, original `mouse_io.c::mouse_int2` ends its pointer-update
path with `host_flush_screen()` and expressly documents smooth response.
`nt_graph.c::nt_flush_screen` returns immediately for the CCPU software
FULLSCREEN route under existing DIV-322. `nt_graphics_tick` publishes completed
text at the original two-tick boundary. Original heartbeat interval is
54925us. This establishes a source-defined deferral, not measured physical
mouse-to-pixel latency or a universal frame-rate limit.

Existing S3 r008 sample5 EDIT200 instrumentation measures queue-to-IRQ take,
not guest handler completion: median5920us, P957327us, maximum66413us over61
consumed entries;143 further moves were merged by the existing queue. Frame
transfer median5690us/P957240us over17 publications. One last-take-to-next-
transfer-start gap is approximately174ms. It is not a causally matched final
pointer-pixel observation. No fresh runtime test is claimed here.

## Superseded r001 execution-context audit and correction

The earlier proposed "wake the existing publication thread" assumed a thread
which is not present in this product. Current text assembly and publication
run synchronously on the original guest/video execution context. The ordinary
NTVDM heartbeat thread advances original devices and posts CPU_TIMER_TICK;
it is not an independently quiesced frame publisher. Its alertable wait treats
USER_APC as termination. QueueUserAPC is therefore not a drop-in display wake.

The existing Console watcher handles broker closure and input-ready/rearm. It
does not own or snapshot original VGA/painter state. Moving synchronous video
transport into that watcher risks postponing input notification and broker
close, so it is not an approved substitute publisher.

Using extra CPU_TIMER_TICK notifications would run the original timer event,
not just extract a frame; it cannot serve as a display-only wake. Merely
restoring the original flush body can call project Console/graphics transport
from a mouse IRQ and revive the prior accumulation problem.

The owner approves existing-heartbeat reuse for copied transmission and
pause/final-paint ordering. Further source audit corrects another assumption:
ICA does not cover all CCPU VGA writes. Extraction on heartbeat is rejected.
Original guest/video execution instead runs the original painter and makes
an owned copy. Heartbeat sends only that copy, never reading VGA/CPU state.
An endpoint-owned FIFO retains every actual completed text publication without
deduplication. A short separate queue lock is never held across pipe transport.
Pause disables queue admission, waits for the claimed send, drains older copies,
then retains the existing final-frame/ownership-release acknowledgement. A
display wake consumes only the remaining original heartbeat deadline, never
calling time_tick or posting an extra CPU_TIMER_TICK. Graphics remains on its
existing path. Candidate is not yet published or verified.

## Initial admission recovery ladder (superseded by owner below)

1. Retain original mouse/VGA algorithms and `host_flush_screen` call sites;
   direct synchronous reuse is blocked by cross-process frontend transport
   under the IRQ/guest execution boundary.
2. Retain original painter through an NTVDM-local display-only notification
   adapter; its safe existing execution context must be established first.
3. Owner approves minimal mirror hooks and registration of their precise
   changed expressions once the execution model is established. This is not
   authority for a new thread, CPU algorithm, guest clock, scheduler, transport
   channel or lifecycle policy. No new divergence ID is claimed as implemented.
4. No new VGA/mouse algorithm, frame dedup, or speculative replacement is used.

## Initial admission limits (superseded by owner below)

Resolve the publication execution context before changing production code.
If safe reuse requires changing the current single-producer ownership or
adding a thread, report that expansion for renewed owner approval. Focused
tests must prove no guest-clock advancement, no transport in IRQ, retained
final frame, no input starvation and suspension/shutdown ordering. Only then
build and publish all eight files with a coherent recovery/hash record; run
Console17/Window17/WOW and affected handoff/lifecycle tests afterward.

This record is admission/source evidence only. It is not S5 capability
closure, a performance improvement claim, or T429 acceptance.

## Owner supersession: independent event-driven publication

The owner rejects the unpublished heartbeat/FIFO candidate above. Its source
edits were reverted; r001 remains abandoned build evidence, never published.
The newly approved implementation is one NTVDM-owned publisher thread. Original
guest/video execution extracts complete immutable copies; the publisher never
reads VGA/CCPU state and never advances guest time. Latest pending state replaces
older unsent state. Complete descriptor/payload comparison uses the last
successful send, including font, palette and cursor state.

Dirty events wake the publisher. A one-shot timer enforces maximum50Hz/20ms
ordinary delivery; there is no periodic idle wait. Blocking/release stops
admission, waits for a claimed send and drains the latest copy without the cap,
before the retained synchronous final-paint/ownership acknowledgement.
Endpoint teardown cancels transport and joins the publisher before releasing
its context. NTCON and worker-base have no new deduplication.

50Hz is a maximum publication cadence, not a guest VGA refresh clock or a
guarantee that arbitrary VGA writes are extracted within20ms. Original normal
video-tick/mode-settle checks remain. Mouse flush restores owner-context local
painting and copied publication instead of deferring to that normal tick.
The mirror exception is MVDM-HOST-DIV-326; nt_timer.c remains unchanged.

r002 x86 production build and production-linked publisher unit pass. The unit
covers latest-of200, minimum20ms interval, unchanged idle, palette-only change,
forced final drain, resume invalidation and sticky failed sends. Focused real
product tests pass in r004: Console4, Window4 and real EDIT200, with actual
guest completion, return to MEM and mouse input acknowledgement. Eight-file
early publication passes all hashes in r005/published-manifest.json; recovery
is r005/recovery. Only ntvdm.exe changes (SHA256
33AFD28A68E3B653B02AAB6152E21558F0F5031CEAF85828BF7CE1654ED5612C).
Full product regression and owner hand testing remain pending; this is not
S5 closure or an end-to-end physical-latency improvement claim.

## Final source and lifetime review

Independent diff review after early publication finds two finite owner-local
edges: re-created channels must retain client->stop cancellation, and text
mouse flush after mode retirement must rearm copied admission before painting.
Both are implemented without changing original guest algorithms or common
transport. No guest-frame pointer enters the publisher. Palette uses its own
short lock; publisher state lock is never held across I/O. The existing channel
lock serializes transport, and endpoint teardown cancels then joins before
freeing that lock/context. Stop/worker-shutdown wins over dirty/timer waits.

The pure production publisher fixture has a tracked reproducible runner:
`tests/observation/verify-software-video-publisher.ps1 -BuildRoot
build/M0-T429/S5/r013 -CompilerWrapper build/M0-T427/S4/r049/msvc-x86.cmd`.
It passes the send/failure cases and50 create/stop/join cycles with exact handle
count and borrowed shutdown handle retained. r008 also records20 repeated
earlier fixture passes. The original production state-copy fixture now supplies
explicit native boundary substitutes for its whole adapter object and adds
local paint/publish order, mode-settle refusal, block/resume, mode-retirement
rearm and text/graphics route checks. It still tests all eight EGA banks,
downloaded glyphs, split cursor, dimensions and refusal without mutation.
It is a native supplement, not guest acceptance. `console-video-test.exe`
passes original atomic commit/cancel/style/configuration/identity/EOF tests.

Pinned OpenNT and OpenNT-4.5 files are identical for both changed mirror owners:
nt_graph.c SHA256 E08E6414C0261656B67A97F839256EE4BA6565F84CE6799197F92B98436131D9;
nt_event.c SHA256 7F555A87BA029627D6811C0F7B96964BB8A96AF90C60C89012975A4AC5442076.
r008 retains complete pinned-source diffs, not a claim of zero existing diff.
This S adds only the registered local flush and block/resume hooks (at most
three executable lines/insertion); new mechanics stay in NTVDM adapters.
nt_timer.c, guest source/media, CPU heartbeat, NTCON and worker-base are unchanged.

r006 is a non-pass caused by invoking the established WOW runner in Windows
PowerShell5, which lacks ProcessStartInfo.ArgumentList; no target ran. The
required PowerShell7 r007 passes all product gates (197530ms). r011/r016 retain
subsequent candidate results separately. An occupied global broker rejects
later suite admission before any test starts; no assertion is weakened. The
final dependency-selected package relinks VDMREDIR.dll through existing library
dependencies. r019/runtime seals that final combination; subsequent final
gate/publication records, not earlier candidate runs, decide closure.

## Final-package gates and retained failed attempt

The sealed r019 package passes Product in r020 (202516ms): WOW frontiers
66726ms, Console17 62473ms and Window17 67429ms, with separate cleanup.
The ordinary retained WOW application frontiers do not regress; this is not
general Win16 usability. r026 passes all eight unchanged integration contracts:
native, nested Console, nested Window, modern EDIT, cooked outer-CMD return,
frontend close, unexpected worker loss and two independent sessions. Actual
direct exit23, cooked return19 and infrastructure failure1067 are checked.

r012 is retained as a non-pass, not overwritten by r026. Its nested Window
attempt timed out in a guest illegal-instruction modal before the DOS prompt
(CS03f4/IP20f7, opcode bytes63 6f 72 72 65). The same PowerShell process had
previously run Product. Native API evidence in r027 proves that restoring an
absent environment variable with SetEnvironmentVariable(name,NULL) leaves an
empty-but-present entry on this host. The observer tests presence, not a
nonempty value: leaked SHORT_HISTORY unexpectedly selected the five-row
fixture. The Product runner now uses Env-provider removal for originally
absent entries, matching the existing S4 handoff harness fix. This is test
environment restoration only; assertions, cases and deadlines are unchanged.

Fixed comparisons r022/r023 (standard layout) and r024/r025 (explicit short
history) pass on both unchanged baseline and candidate. They establish no
reproduced candidate-only regression, but do not prove the root cause of the
illegal guest instruction. It is not attributed to immutable guest code, not
waived as an original limitation and not reclassified as passed. The final
pipeline additionally checks that Product leaves SHORT_HISTORY truly absent
before running the original integration contracts in the same process.

Remaining interpretation limits are explicit: extraction still belongs to the
original software-video owner; the new thread cannot observe arbitrary VGA
writes independently.50Hz limits successful publication, not guest execution
or universal20ms physical latency. Owner-context tests cover cursor/font/bank
state and text/graphics routing; they do not establish every graphics guest.
Physical RDP movement/clipping and display latency remain owner-waived or
unobserved, not passed. No1000-move stress requirement is introduced; the real
EDIT200 contract is retained. The unexplained r012 modal is retained for future
diagnosis if reproduced under the corrected documented test environment.

## Reproduction and build identity

All outputs are below build/M0-T429/S5. Formal x86 dependency closure uses
build/M0-T427/S2/r001/run-ninja-parallel.cmd with targets run16.exe, ntsrv.exe,
ntcon.exe, ntvdm.exe, ntvwm.exe, ntmon.exe and VDMREDIR.dll. r014's final
no-work log confirms the sealed package is current. WOW32.DLL is the unchanged
sealed original-provider input, not an unavailable cache target or substituted
DLL. r014/verified-package.json pins every package member. MSVC14.43,
SDK22621, x86 /MT and original CCPU40 remain selected.

Reproducible focused publisher verification:

```powershell
& tests/observation/verify-software-video-publisher.ps1 `
  -BuildRoot build/M0-T429/S5/r030 `
  -CompilerWrapper build/M0-T427/S4/r049/msvc-x86.cmd
```

The root must be fresh; use another unused build run to reproduce. r030
passes against the final source. Full product uses the retained PowerShell7
Invoke-ProductVerification.ps1, Suite Product, r019/runtime, T427 S2 r001
cache, T427 S4 r049 Console/Window observers, T425 S9 r008/G7.COM and S3 r009
WOW baseline. r028/sequence.ps1 records the exact continuous command; r029
retains the original eight-case integration invocations. All runtime phases
are serial against the single BaseSrv endpoint, use only the owned Z: alias
and remove it. Process cleanup validates package path and process identity,
not a process-name-wide kill. Original guest media/configuration is retained.

Source review verifies complete zero-initialized descriptions and copied
payloads, explicit publisher ownership and borrowed shutdown ownership,
bounded latest-state replacement, no state lock across transport, sticky
failure, and cancellation after channel reopen. NTCON/worker-base/nt_timer.c
have no diff. The other session's five-line proposal change is preserved and
excluded from this S delivery.

The corrected continuous sequence passes Product r028 (197014ms: WOW66427,
Console17 60915, Window17 65001, plus setup/cleanup), verifies absent
SHORT_HISTORY restoration, then passes all eight integration cases in r029
without leaving that PowerShell process. r029/results.json retains per-case
results and times; r014/final-sequence.log retains ordered output. This is a
fixed whole-pipeline verification after the test cleanup change, not reduced
assertions or retry-to-success inside an observer. r012 remains a separate
failed attempt with the causal limitation above.

## Final coherent publication

r018 passes final-package Console4, Window4 and the retained real EDIT200
workload (burst-records=200, input-sink-acknowledged=yes), followed by menu exit
and MEM at the same prompt. r017 publishes all eight r019 members to
O:/winnt/system32 with every source/published hash equal. It retains the
previous installed candidate in r017/recovery; the preceding fully validated
S3 package also remains in r006/runtime of S3 and the early r005 recovery.
Only NTVDM and its dependency-selected VDMREDIR link differ from that baseline:

| Artifact | Final SHA256 |
| --- | --- |
| ntvdm.exe | F3589AA37BB3FFB4621B3C580B5E637C693196637AEC9AAD286B80B2C17B3C5A |
| VDMREDIR.DLL | AA9F579E88721F2A23E94CF509679EED4044E3A668FE44A8CF311468957D801A |

The remaining six hashes are preserved in r014/verified-package.json and
r017/published-manifest.json. No fixture/instrumented EXE is a published
product member, and no diagnostic configuration is installed. The final
publication replaces the early candidate, not a mixed component set.

r021 passes actual published Console/Window COMMAND, MEM, EDIT and native
smoke, with all eight hashes verified before and after. Its runtime logs are
O:/winnt/Logs2/t429-s5-r021-*. Build r021 retains the verified published manifest;
r014/final-sequence.log retains the ordered continuous verification/publication.
Final diff review, documentation governance and relative links are delivery
gates. This closes bounded S5 implementation/delivery, not T429 manual
acceptance, physical RDP latency, universal graphics compatibility or the
unexplained historical r012 failure. No later S/T is automatically admitted.
