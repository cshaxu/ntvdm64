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

## Execution-context audit and correction

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

Reusing the existing heartbeat thread remains a candidate, not proved safe:
it requires a separately copied frame, original ICA-safe extraction, transport
outside the guest lock, preservation of the heartbeat's absolute deadline,
and an explicit quiescence/ordering proof for suspend/final paint/release.
The current production handoff assumes no such concurrent producer. No
implementation has been installed and no candidate has been published.

## Recovery ladder and approved exception boundary

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

## Follow-up and delivery limits

Resolve the publication execution context before changing production code.
If safe reuse requires changing the current single-producer ownership or
adding a thread, report that expansion for renewed owner approval. Focused
tests must prove no guest-clock advancement, no transport in IRQ, retained
final frame, no input starvation and suspension/shutdown ordering. Only then
build and publish all eight files with a coherent recovery/hash record; run
Console17/Window17/WOW and affected handoff/lifecycle tests afterward.

This record is admission/source evidence only. It is not S5 capability
closure, a performance improvement claim, or T429 acceptance.
