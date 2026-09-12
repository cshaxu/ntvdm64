# Cross-process broker closure

## Status and objective

This is the unadmitted broker proposal linked from Queue. The owner approved
broker-before-WOW-restoration sequencing on 2026-09-12; implementation still
requires admission after the preceding queued restoration packages. Its
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

## Proposed source-first sequencing amendment

The owner requested this design addition on 2026-09-12 after discussing D01
in the [original-owner audit](../etc/evidence/opennt-replacement-audit-20260912.md#consolidated-discussion-index-confirmed-owner-overlap).
The proposed alternative is to admit this broker package before continuing
the affected BaseSrv replacement work, establishing the real service-process
owner while restoring original source, rather than building an autonomous
broker first and replacing its policy later.

The owner approved this sequencing change on 2026-09-12; Queue now places
broker before the WOW/debugger restoration and final WOW16 workload packages.
This is candidate ordering, not implementation admission. At admission,
preserve the open audit/restoration task and its findings
at an explicit handoff; do not mark it complete or run two active S packets.
Broker transport is not a substitute for missing single-worker DOSX, DPMI or
WOW behavior. Resume the remaining owner recovery after the bounded broker
work, with failures attributed to their actual owners.

### Restore the original service lifecycle, not one function

The current local Base VDM provider uses one pending command slot and local
DOS/WOW ownership, wait/retry and termination decisions. Original
`srvvdm.c` uses Console/DOS and WOW records with task selection, parent
notification, dispatch state and command-payload ownership. Similar behavior
for one launch is not proof that these models are equivalent.

The replacement unit must include the required original lifecycle:
`BaseSrvCheckVDM` and its DOS/WOW branches, `BaseSrvUpdateVDMEntry`,
`BaseSrvGetNextVDMCommand`, and the applicable completion, exit-code,
rollback and record-release helpers. Keep original BaseClient wait/retry and
result-copy policy at the client boundary. Do not retain a second autonomous
task state machine synchronized with the original records. All of D01-D07
and D10 are included and must receive individual evidence-based dispositions;
the executable split alone closes none of those audit rows.

Single-session in-process composition of this original lifecycle remains a
possible implementation route: one session can populate the original record
model without replacing that model. An independent broker is preferred here
to avoid temporary same-process parent/wait special cases, not because
`srvvdm.c` inherently requires cross-process transport. Later separation
should change bindings rather than rewrite service policy. It may reduce
same-process-specific divergences but does not automatically remove CSR,
Console or USER boundary adaptations, and total project IPC code may grow.

### Included audit package: BaseSrv and application entry

On 2026-09-12 the owner requested that the entire BaseSrv/application-entry
audit group be included in this proposal as one coordinated restoration
package: **D01, D02, D03, D04, D05, D06, D07 and D10**. D10 is mandatory
scope, not an optional follow-up. This records candidate design scope only;
it does not admit implementation, close audit rows or reorder Queue.

| Audit point | Required restoration scope | Original owner and execution side |
| --- | --- | --- |
| D01 | Next-command selection, capacity/result handling, dispatch state and command-payload ownership. | `srvvdm.c:BaseSrvGetNextVDMCommand`, hosted in `basesrv.exe`. |
| D02 | PIF response capacity, title and output-field policy without consuming the task prematurely. | `srvvdm.c:BaseSrvFillPifInfo`, hosted in `basesrv.exe`. |
| D03 | Service-instance first-query state, serialized consumption and explicit restart semantics; no host boot-file mutation. | `srvvdm.c:BaseSrvIsFirstVDM`, hosted in `basesrv.exe`; detailed constraints below. |
| D04 | Current-directory set/get, consume-on-get ownership and capacity/failure handling. | `srvvdm.c:BaseSrvSetVDMCurDirs/BaseSrvGetVDMCurDirs`, hosted in `basesrv.exe`. |
| D05 | DOS task completion, parent notification, exit-code and record cleanup, with WOW exit capability explicitly accounted for. | Original BaseSrv exit/lifecycle slice, including `BaseSrvExitDOSTask`, hosted in `basesrv.exe`. |
| D06 | Reentry accounting and its interaction with native-child waits and completion; remove autonomous scheduling policy from the adapter. | `srvvdm.c:BaseSrvSetReenterCount` and its original callers. |
| D07 | Client capture/result-copy, wait and retry ordering, including insufficient buffers and cleared retry exit code. | Original BaseClient `vdm.c:GetNextVDMCommand` slice, on the requesting client/worker side, not a server-side replacement. |
| D10 | Original executable-format classification and error behavior; keep CLI existence checks, search, routing and worker creation distinct. | Original BaseClient `vdm.c:GetBinaryTypeW` slice in `opennt-host`, used by the `run16.exe` entry composition. |

One package does not mean one source component: original BaseSrv/BaseClient
algorithms remain in `opennt-host`; original MVDM callers remain in
`mvdm-host`; `app` supplies launcher/product composition; package-private
adapters and `broker` supply finite resource bindings and copied transport.
D07 runs where the API caller resides, including `ntvdm.exe`; it must not
be moved wholesale into the launcher merely because both use client code.

Implement the shared lifecycle together: classify/submit, select or
create/register a worker, acquire, execute/reenter, notify completion and
reclaim. Sequential stages and commits are allowed, but a stage must name any
remaining transitional provider and its removal condition. The completed
package must not retain parallel local and original task-policy state
machines, or leave an included audit point as an untracked later repair.

For each of the eight rows, retain the discussion/acceptance structure:
current selected implementation; original OpenNT implementation; evidenced
reason for divergence; exact restoration and superseded-code disposition;
positive, negative and runtime acceptance. In addition to per-row tests, the
package jointly verifies COMMAND/EDIT, `BOP 54:08` child return, standard
streams, parent exit codes, WOW requests and abnormal cleanup. Unavailable
contracts remain named limitations requiring explicit owner disposition, not
successful closure. Restoring WOW task delivery does not authorize moving
WOW32, DPMI, DOSX or WRITE internal behavior into the broker.

### Finite outgoing closure and ownership

- Retain the selected original BaseSrv bodies, structures, ordering and
  failure rules under `opennt-host/base/win32/server/`, including `srvvdm.c`.
  Original BaseClient slices also remain in `opennt-host`; MVDM consumers
  remain in `mvdm-host`. Executable placement in `basesrv.exe` does not move
  original algorithms into the project-owned `broker` source component.
- Bind CSR transport through a package-private `adapter-opennt-host` seam
  and bounded broker IPC. Materialize validated original request structures
  only for the local call and marshal copied results. Do not import CSR/CSRSS
  or create a generic substitute subsystem to satisfy unresolved symbols.
- Supply verified caller/worker identity and explicit event/resource
  ownership behind the reached original interfaces. Account for creation,
  duplication, notification, waiting and closure without fabricating a CSR
  process/thread object hierarchy or serializing native handles. The admission
  must resolve the permitted cross-process resource binding before claiming
  parent waits or standard-stream transfer work.
- Give Console-associated records an explicit registered identity contract;
  a process-local Console handle value is not a global wire identity. This
  does not require recreating Console Server or changing guest input policy.
- Audit WOWEXEC notification, process lookup, token/interactive-identity checks
  and USER callbacks individually. Select proven public bindings or record
  the exact unavailable operation; no success-returning empty substitutes.
  DOS closure does not establish shared-WOW closure.
- Keep one-shot exit and idle-retention decisions in product composition.
  Original task state determines completion; product policy determines when
  an eligible worker should exit. Remove direct product-session termination
  from the service command-acquisition policy when its replacement is proved.

### D03: first-VDM state and prohibited host-file mutation

The owner explicitly required this boundary on 2026-09-12: restoring the
first-VDM query must not create `C:\IO.SYS` or `C:\MSDOS.SYS`.

- **Current behavior:** `base_vdm_local_initialize` initializes
  `first_vdm_available` for each local session; its query consumes that flag.
  Independent workers can therefore each report first. This is not a query
  for the first command in a worker or a count of currently running VDMs.
- **Original behavior:** `BaseSrvIsFirstVDM` in `srvvdm.c` returns and clears
  the service-owned `fIsFirstVDM` flag. Within one service instance, the first
  query consumes it; worker exit does not reset it. The MVDM consumer is
  `scs_init` in `softpc.new/host/src/nt_msscs.c`, which calls
  `GetNextVDMCommand(NULL)` and conditionally calls `AddSystemFiles`.
- **Reason for divergence:** the no-broker composition moved service-global
  state into a per-session record. The query-and-clear operation resembles
  the original, but its ownership and lifetime do not.
- **Restoration:** execute the original service query with state owned by the
  `basesrv.exe` instance, reached through the client proxy, and retire the
  duplicated per-session flag. Prove serialized dispatch or provide the
  smallest registered synchronization binding so concurrent queries cannot
  both consume the first result. Do not assume the original function itself
  supplies locking.
- **Restart semantics:** the proposed flag resets on a new broker instance,
  including after automatic empty-broker shutdown. This is explicitly a
  service-instance lifetime, not a Windows-login-wide guarantee. Do not add
  persistent state or a resident helper merely to preserve the flag across
  restarts without a separately demonstrated consumer requirement.

The original `AddSystemFiles` body attempts to create missing boot-file
placeholders on the host system drive. That side effect is prohibited product
behavior, not part of D03 restoration. Preserve the explicit non-mutating
binding currently represented by `MVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION`;
restoring original call shape must not re-enable the mutating branch. Neither
launcher, broker nor worker may create, overwrite, truncate, delete or change
attributes of host `C:\IO.SYS` or `C:\MSDOS.SYS`, nor substitute IBM-named
boot placeholders or another host-drive root to evade this boundary. Existing
host files remain untouched. No elevated retry or installation-time workaround
is permitted. Retain the upstream body only as unselected source evidence;
the non-mutating disposition must be explicit rather than a claim that the
original file-creation operation succeeded.

D03 acceptance must cover repeated queries, different workers sharing one
broker, worker exit/replacement, concurrent first queries, and broker restart.
Exactly one query returns true per healthy broker instance; subsequent queries
return false, while transport failures retain a distinct failure disposition.
Source/build-selection evidence must show that the host-mutating branch is
excluded, with focused intercepted filesystem-call tests asserting no boot-file
mutation request. Real-host validation is observational only: never create or
remove actual system-root files to set up a test. Complete these checks before
closing D03; this query is not proof of DOSX or WRITE recovery.

### Proposed staged implementation and acceptance

These are planning stages, not allocated or active S packets. Admission may
map them to sequential S briefs without treating a stage as full T closure.

1. **Closure selection (proposed S1):** identify the exact original functions,
   records and outgoing dependencies for submission, registration, acquisition,
   completion and rollback; record each binding and superseded local provider.
   Prove a finite build boundary before implementing transport around it.
2. **Three-program composition (proposed S2):** connect `run16.exe`,
   `basesrv.exe` and `ntvdm.exe` to the selected original service lifecycle.
   Keep worker creation/registration on the launcher side and selection on
   the server side. Demonstrate that original bodies execute, not merely
   that the three programs link; remove replaced duplicate policy together.
3. **DOS lifecycle (proposed S3):** verify COMMAND, EDIT, `BOP 54:08` child
   launch and return, standard streams, exit codes, creation failure rollback,
   client/worker disconnect and abnormal exit. Include first acquisition,
   insufficient-buffer retry, pending DOS wait, and parent notification.
4. **WOW coordination and retention (proposed S4):** verify the selected WOW
   submission/acquisition/completion path and nonblocking empty-WOW response;
   complete single-instance startup and safe idle/empty-broker shutdown tests.
   Name any blocked guest/provider workload explicitly rather than calling a
   DOS-only result complete WOW support or using IPC to bypass that owner.

An empty command queue is not an idle worker: EDIT, interactive COMMAND or a
WOW task can still be running. Start the proposed one-minute timer only from
an explicitly eligible, completed-task state, and cancel it on renewed work.
Test arrival-versus-reaping and startup-versus-broker-exit races; no live
interactive session may be reaped solely because no new command is queued.

Before implementation admission, reconcile the three executable names and
publication layout with the current single-`ntvdm32.exe` build/deployment
authorities. This proposal does not authorize publishing new executables or
claim that the complete outgoing service closure has already been proved.

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
transport for original single-worker WOW16 behavior or its later workload acceptance.

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
