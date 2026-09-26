# T423 S3 Hidden Console Ledger

## Baseline and scope

S3 is admitted by 584a51a35 after S2 closure. The unchanged production baseline
is d0f856a50 with the six-file manifest recorded in the S2 ledger. This record
is an implementation audit, not a claim that a hidden backend is connected.
Root run16 owns presentation; native Windows owns Console behavior. Original
MVDM/BaseSrv continues to own execution, worker selection and reentry.

## Selected and rejected recovery material

Read-only reference: codex/t423-original-reference-20260925 at 286d54a30.
Files inspected through git show, not copied wholesale:

| Reference file | Disposition |
| --- | --- |
| src/run16-exe/native_console_capture.c | Reuse the active CONOUT$ open and before/read/after geometry validation pattern. Reject fixed viewport-only storage as the complete S3 scrolling contract. |
| src/run16-exe/native_console_helper.c | Reuse finite hidden Console creation and per-stream Console replacement mechanics, subject to tests. Do not copy owner-death execution policy or final-frame timeout as current acceptance. |
| src/run16-exe/native_console_channel.c | Do not import the worker-owned renderer contract. Its mutex-protected shared frame/input transport duplicates the new root channel ownership and imposes a 256-event queue policy. |
| src/product-abi/native_console_text.h | Reject fixed 160x54 whole-frame limit. Native Console geometry, viewport origin, cursor, attributes and bounded cell tiles must remain explicit; do not impose a guest text-mode limit on native programs. |

These are project-owned prototype mechanics, not original OpenNT algorithms.
Modern native Console implements the original external Console services;
importing CSRSS/USER server or building another terminal parser is unnecessary.
S2's copied-cell and input records can be reused where their exact semantics
match, but its worker request channel must not be reinterpreted as a native
backend without typed endpoint admission.

## Current source findings

- run16 main.c creates/waits native CUI and COMSPEC fallback directly in the
  visible Console. GUI does not acquire a DOS frontend. Both native launch
  branches must use one backend entry, not receive separate implementations.
- frontend_scope.c distinguishes root ownership using an inherited restricted
  event verified by BaseSrv. A joined launcher does not start another pump.
  This is the existing presentation capability, not worker-selection authority.
- base_service.c RetainFrontendRoot checks peer identity, event-object equality
  and live root, independently of physical Console membership. It can retain
  presentation association across a hidden Console without a caller PID token.
- service_bind_existing_console currently determines execution Console keys
  from authenticated live processes' physical membership. A new hidden Console
  will not match the original DOS parent by this mechanism alone. Reusing the
  frontend capability as a Console key would conflate I/O and execution.
- Native target completion remains the launcher's direct process wait/result.
  Backend disconnect cannot manufacture task success or authorize tree kill.

## Implementation constraints for the next production change

1. A root retains its visible Console. The hidden backend must have its own
   process-local Console attachment; never FreeConsole/AllocConsole on the
   root frontend. Use run16's private helper role, not a seventh product EXE.
2. Native children inherit their actual Console group normally. Do not allocate
   another backend merely because a new native image starts. Preserve explicit
   new/detached Console requests and file/pipe standard handles independently.
3. The backend opens current CONOUT$ for active-buffer capture and sends bounded
   cells plus geometry/cursor metadata directly to root. Input/control/resize
   have ordered acknowledgments and finite cancellation; snapshot coalescing
   cannot discard input or control events. Scrolling is required, not just the
   last visible screenshot. S3 output is visible Console; S4 later adds Window.
4. Cross-Console execution association needs a separate authenticated binding
   to the original logical Console context. Audit an existing parent connection
   and a verified helper process/Console group; never accept a naked key/PID or
   derive this binding from shared root frontend alone. Original srvvdm still
   decides READY/BUSY, queues and reentry. Exact RPC admission and rundown tests
   must precede wiring this path.
5. Startup resource rollback ends at successful handoff. Preserve non-root
   launcher/native target independence and root session-close behavior from S2.
   Broker/worker failures and final-output acknowledgement remain distinct from
   the native target's actual process exit code.

## Finite S3 checklist

- [x] Current launch and reference implementation owners audited above.
- [ ] Hidden backend creation/streams/active buffer and copied input contract.
- [ ] Authenticated cross-Console execution-context binding and negative tests.
- [ ] One native launch path, formal root frontend composition and cleanup.
- [ ] Cooked/raw, Unicode, cursor/color/resize/scroll, mouse and Ctrl+C/Break.
- [ ] Redirected/aliased streams, EOF, fast exit and failed startup.
- [ ] Proposal chains A/B: actual interaction at every level, one root frontend,
      no competing readers, exact direct completion and usable returned parent.
- [ ] Fault isolation, cumulative DOS17/WOW headless gates, coherent six-file
      publication, reviewed minimal diff and clean pushed state.

Physical desktop observation follows the owner waiver; unit/logic evidence is
required instead. No S3 functionality is marked complete by this initial audit.

## Hidden attachment preserves inherited capability: measured prerequisite

Extended the existing tests/app/frontend_capability_inheritance_test.c rather
than adding another transport. The child now asserts that the root PID is not
in its Console process list and that its Console window is not visible. The
existing direct/CMD/nested-CMD paths still prove inherited SYNCHRONIZE-only
event rights (SetEvent denied) and completion 37.

The new --rebind-helper path starts on the root's Console, asserts that actual
membership, then calls FreeConsole/AllocConsole only in the helper, whose
startup specifies SW_HIDE. After separation it asserts root absence and an
invisible window. It creates CMD with ordinary Console inheritance; the final
child asserts that helper is a member of its hidden Console, while root is not,
and uses the same inherited restricted event. Root signals that event after
the child readiness handshake and receives completion 37 through CMD/helper.
This is real Windows Console membership/inheritance, not a mocked membership
answer, but it does not yet register an execution association with BaseSrv.

Build: MSVC Win32/x86 /MT /W4, frontend_capability_inheritance_test.c linked
with user32.lib; object and executable reside under
build/M0-T423/S3/console-inheritance. The S2 control observer runs test.exe on
an unswitched private desktop with a 45000-ms deadline. Evidence prefix
O:/winnt/logs/m0-t423-s3-console-inheritance: exit 0 and all four actual PASS
lines, including inherited visible membership -> hidden helper -> CMD child.
No user desktop manipulation, product/guest change or publication replacement.

This supports a smaller binding candidate: authenticate and capture the
helper's genuine original Console membership before detaching, then preserve
that logical execution context across the backend's physical attachment.
Current service_bind_existing_console already caches a connection's original
key. It must not be invoked via a fake CheckVDM/placeholder DOS task merely to
obtain that key. A finite authenticated setup operation and its lifetime rules
are still required, including helper loss while native descendants survive.
Frontend object possession alone still cannot select a worker. The next
production change must prove that binding/rundown through service and RPC tests.
