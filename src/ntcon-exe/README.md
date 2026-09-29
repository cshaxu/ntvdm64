# Native Console Worker

NTCON is the Win32-text worker peer of NTVDM. It owns and remains attached to
an ordinary hidden Console. There is no ConPTY or private helper. Windows
supplies Console storage, line editing, native Console APIs and attached-client
semantics; NTCON supplies their finite backend binding.

## Ownership

- `main.c`: independent authenticated registration, request arrival, actual
  Console member sampling and worker-local I/O coordination. An idle worker
  can remain resident independently of its former frontend.
- `execution.c`: direct target creation/completion, request cleanup and
  final-presentation acknowledgment. Requester death does not kill its target.
  Completion-export rights are checked before starting the target.
- `launch_packet.c` and `launch.c`: copied packet codec and restricted local
  resource/process materialization. run16 also links the same creation body for
  GUI launch. NTKVM links only the codec, never creation or execution.
- `console_state.c`: actual Console geometry, cells/cursor, active-buffer
  capture, native input, unread-key draining and actual membership observation.
- `text_frame.c`: native cells to the shared bitmap-glyph text frame. Uses the
  admitted ROM font or transferred DOS font banks; no alternate NTKVM glyph
  mapper. Optional style bytes preserve underline. Grid flags are unsupported.
  Window mouse position/buttons and reverse-video cell composition also belong
  here. Only the copied frame is painted; hidden Console cells/caret/history
  and the screen returned to another worker remain untouched.
- `presentation.c`: native capture/seed/member-specific operations under its
  instance lock. Ordered transport, cancellation, frame chunks, ordinary input
  codec, activation and key-return encoding use `worker-base`.
- `channel_io.c`: native execution-pipe transfer. Its completed-I/O-versus-peer-
  death contract differs from the shared strict frontend client; it stays local.

Cross-component declarations live in `interface`. NTSRV owns authentication,
discovery and worker management; run16 owns submission/direct-result waiting;
NTKVM owns visible Console/Window, display state, routing and the single renderer.
Original DOS/WOW execution and cleanup remain with NTVDM's original mirrors.

## Handoff and lifetime

A DOS activation waits for native final capture and release before importing
the common screen. NTCON seeds its actual Console on activation and publishes
only text frames. A direct native target's completion is not its descendants'
completion: surviving attached clients retain interaction. Once native clients
are gone, unread keys are returned in reverse-prepended batches to preserve FIFO
ahead of newer frontend input; consumed keys are never replayed. Parent output
resumes only after the explicit final-presentation acknowledgment.

Member snapshots are observations, never authority to terminate arbitrary PIDs
or a substitute for admission synchronization. Explicit management close asks
the actual hidden Console to close and acknowledges confirmed closure. Launcher
death never invokes that operation. No process-tree kill or second scheduler.
Capture detects geometry changes but does not promise an atomic content snapshot
against unrelated concurrent native writers.

Logical text geometry comes from the frontend handoff or the running native
application, never host display dimensions. The invisible Console uses fixed
carrier font metrics, independent of its copied bitmap fonts; inherited state
is applied and read back before acknowledgment. A return preserves native
scrollback storage separately from the current logical viewport. See the
[S13 ledger](../../docs/etc/evidence/m0-t423-s13-text-geometry.md) for actual
mode, resize-race, pointer, history and bidirectional handoff evidence.

## Provenance and verification

Console operations selectively reuse project S8 `d253e55af` capture/host/input
mechanics. Glyph conversion is moved from the retired native frontend producer;
the duplicate NTKVM renderer, ConPTY parser, carrier and control protocol are
removed. No OpenNT Console-server shell is imported; no guest/shared lib changes.

The [S12 ledger](../../docs/etc/evidence/m0-t423-s12-ntcon-backend.md) retains
source dispositions, failed attempts and exact build/runtime evidence. Tests
cover real hidden-Console state, authenticated RPC, concurrency, transport
negatives, request cleanup, double-session faults, attached survivors, real
DOS/native handoff, nested chains and formal regressions. Compile/fixtures are
not substituted for those production tests. Publication and task status are
controlled solely by [CURRENT](../../docs/states/CURRENT.md).
