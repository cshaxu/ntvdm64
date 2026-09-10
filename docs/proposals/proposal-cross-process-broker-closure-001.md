# Cross-process broker closure

## Status and objective

This is the unadmitted proposal for Queue candidate 2.  It may begin only
after the applicable single-process WOW16 owner contract has closed.  Its
objective is to recover the required BaseSrv-style registration, command
queue, notification, and disconnect-cleanup behavior through public IPC while
returning the MVDM process to a source-shaped worker role.

The proposed standalone product has three executables:

| Program | Proposed responsibility | Must not own |
| --- | --- | --- |
| `run16.exe` | The user-facing application entry point: validate that the requested file exists, classify its image/command kind, start or connect to `basesrv.exe`, submit the launch request, create an `ntvdm.exe` worker only when the broker's selection result requires one, and register that worker.  It is the CLI realization of the otherwise system-supplied `CreateProcess`/BaseClient path. | Guest execution, reusable-worker selection, a generic shell implementation, or private CSRSS behavior. |
| `basesrv.exe` | The product broker: accept copied launch and worker-registration records, own stable worker IDs, command records, availability/notification and disconnect cleanup, and select an existing ready worker or require creation of a new one. | Guest, COMMAND, SoftPC, WOW, raw guest/native pointers, and a reimplementation of CSRSS or kernel VDM. |
| `ntvdm.exe` | The background, original-shaped MVDM worker.  It obtains its next command through the source-shaped `GetNextVDMCommand` client boundary, initializes the selected original MVDM runtime, executes the assigned DOS/Win16 work, and reports lifecycle state. | User-facing target classification, CLI policy, broker ownership, and independent command-line launch semantics. |

`ntvdm32.exe` remains the current product baseline.  These names describe a
future admitted topology; this proposal does not rename, split, build, or
replace the current executable.

## Proposed launch and registration path

1. `run16.exe` receives a target and preserves the required raw command tail,
   environment and standard-stream identity for the selected route.
2. It performs only product-level existence and image-kind classification.
   A native PE target follows ordinary public `CreateProcess`; a selected DOS,
   PIF, or Win16 target becomes a copied broker launch request.
3. `run16.exe` locates or starts the one product broker for its user/session,
   then submits the request.  A successful connection to an already-running
   broker is not an error and does not start a duplicate server.
4. `basesrv.exe` atomically records the request and either assigns a ready
   compatible worker or returns the source-shaped equivalent of “worker not
   present.”  Only in the latter case does `run16.exe` create `ntvdm.exe`.
5. The new worker registers its process-local capability/state through
   `run16.exe`'s launch reservation and the public broker endpoint.  The
   broker commits the registration, publishes availability, and delivers the
   copied command record.  A failed launch, registration, disconnect, or
   timeout deterministically removes the reservation and record.

This deliberately keeps physical worker creation in the explicit standalone
BaseClient/CLI side (`run16.exe`) while keeping worker selection, command
ownership, and lifecycle coordination in the BaseSrv side (`basesrv.exe`).
It does not assert that a broker itself creates every worker.

## `BOP 54:08` native-child route

For the original `cmdExec32`/`BOP 54:08` route, `run16.exe` replaces this
product's `cmd.exe /c` helper hop: it receives the original-shaped request,
classifies the target, and forwards the appropriate native or VDM launch.  It
is not a new general-purpose `cmd.exe` parser.  Command-shell grammar,
redirection and COMMAND ownership remain with the original owner unless a
separate admitted package proves otherwise.  In particular, the route must
not retain guest pointers or defer use of a guest command tail after the
source-shaped lease ends.

## Broker lifetime and cleanup policy

The broker is a single product instance **per user/session endpoint**, not a
system-wide controller.  A second `basesrv.exe` first checks the product
mutex/endpoint; when the first instance is healthy it exits without disturbing
it.  Discovery/control of unrelated processes is prohibited.

The following are proposed product retention policies, not claims about
historical OpenNT behavior:

- A worker may be reused only after it explicitly reports a broker-defined
  ready/idle state through the copied wire contract.
- The default idle lease is one minute.  At expiry `basesrv.exe` asks an
  eligible idle worker to exit and removes it if its connection then closes.
- A worker is ineligible for that reaper while it has an assigned or queued
  command, a launch reservation, pending parent wait/re-entry, active guest
  work, an interactive-console lease, outstanding request/notification, or
  incomplete registration/disconnect cleanup.
- When no registered workers, launch reservations, queued commands, or live
  client operations remain, `basesrv.exe` exits cleanly rather than remaining
  resident.  A later `run16.exe` invocation starts it again.

The admission packet must choose the exact worker state transitions, the
graceful-exit/forced-cleanup limit, and focused positive/negative evidence.
It must also decide whether an interactive `COMMAND.COM` session ever advertises
the reusable idle state; a quiet interactive console must not be silently
destroyed merely because it exceeds one minute.

## Source shape and IPC boundary

The source-recovery starting point is the relevant OpenNT Base VDM
client/server contract (including the BaseSrv VDM registration and command
selection material) rather than a new general process manager.  The active
packet must identify the exact original owner paths and audit the required
four rungs: direct composition, smallest source-shaped public-IPC facade,
registered intrusion, then new behavior only as a last resort.

`GetNextVDMCommand` stays at the `ntvdm.exe` call site in its original-shaped
role.  Its standalone implementation becomes a client proxy that serializes
only fixed-width copied command values to `basesrv.exe`; the broker performs
the corresponding server-side queue/selection work.  No raw pointer, native
handle, CRT object, guest address lease, or process-local identity crosses the
wire.  The wire is versioned, sized, per-user access-controlled, and has
explicit status, lease, cancellation, notification, and disconnect rules.

## Boundary and non-goals

The broker owns no guest, COMMAND, SoftPC, WOW provider, USER, CSRSS, kernel
VDM, or generic process-pool behavior.  It does not make x64 a build target,
does not introduce host mutation, and does not substitute cross-process
transport for the prerequisite single-process WOW16 lifecycle closure.

## Admission and exit evidence

An admitted package must provide:

1. an original-owner and four-rung recovery ledger for the selected Base VDM
   client/server slices and every new public IPC seam;
2. a versioned fixed-width wire schema and focused tests for registration,
   ready-worker selection, no-worker creation request, command delivery,
   notification, disconnect, malformed messages, and per-user access denial;
3. lifecycle evidence that duplicate `basesrv.exe` startup converges safely,
   the one-minute idle policy reaps only eligible workers, and an empty broker
   terminates without orphaned records; and
4. real x86 runtime evidence for the selected brokered DOS/Win16 rows, while
   preserving the source-owned worker startup and `GetNextVDMCommand` call
   shape.
