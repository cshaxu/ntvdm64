# ConPTY migration boundary ledger

## Question and baseline

Replace the project-owned hidden Console/helper with ntkvm-owned ConPTY,
without losing accepted DOS/native nesting, input return, retirement, mouse
or GUI startup contracts. Baseline is S8 production d253e55af and delivery
record 6ef96410d; the verified seven-file package remains at O:/winnt.
No migration candidate is published and no runtime equivalence is claimed.

## Initial source review

Procedure: inspect native_console_backend/host/view/frontend headers and
implementations, session_service retirement, the selected frontend README,
and existing ConPTY observation fixtures. Search src/tests/tools for
ConPTY, pseudoconsole and terminal-parser references. Results:

| Existing owner | Disposition | Required replacement proof |
| --- | --- | --- |
| native_console_backend / native_console_host private helper channel | Replace, then delete superseded channel/entrypoint | Direct ConPTY creation/launch, redirected and aliased handles, cancellation, final output drain |
| native_console_view snapshot polling | Replace native capture/presentation, retain visible Console ownership contract | Incremental stream to complete terminal state, Console and Window rendering, cursor/attributes/scrolling |
| native_console_view reclaim_input_to | Cannot simply delete | Existing code removes unread INPUT_RECORD batches from the backend and prepends them before newer DOS input; prove an equivalent ConPTY handoff before changing this contract |
| native_console_host MEMBERS and session_service retirement | Cannot substitute direct-target completion | Current GetConsoleProcessList excludes the helper and retains a session with attached descendants; backend EOF/session lifetime must be independently tested |
| native_request_client/io and authenticated broker delivery | Reuse | Process/resource ownership and two separated character sessions must remain validated |
| DOS console_channel/video and Window input queues | Reuse | DOS bypasses ConPTY; preserve mouse, mode changes and source retirement |
| Existing command_stream_observer and console_terminal_observer | Reuse as test infrastructure only | They do not provide a production VT screen parser or prove unread-input reclamation |

The search found no production ConPTY/VT parser in the selected frontend.
That is a repository-local finding, not a claim that no reusable upstream
component exists. Existing observer job-object cleanup is test-only and must
not become product launcher-death policy.

## External contract evidence

Primary references inspected for this audit:

- [Microsoft pseudoconsoles](https://learn.microsoft.com/en-us/windows/console/pseudoconsoles):
  transport is UTF-8 and the underlying multi-client Console still exists.
- [Microsoft ClosePseudoConsole](https://learn.microsoft.com/en-us/windows/console/closepseudoconsole):
  closure sends CTRL_CLOSE_EVENT to connected clients; output can continue.
  The page distinguishes pre-24H2 blocking closure from build 26100 onward.
  A direct target exiting therefore does not authorize closing a backend
  while other attached clients still need it. Output draining and session
  retirement require separate proof; ClosePseudoConsole is not a harmless
  replacement for terminating only the old helper.
- [libvterm upstream](https://www.leonerd.org.uk/code/libvterm/): C99,
  toolkit-independent terminal parsing with callbacks is a reusable candidate.
  No source imported yet. License contents, pinned source, MSVC x86 build,
  Unicode/state/input coverage and Microsoft-specific input extensions remain
  unverified. An upstream feature description does not pass these checks.

Original NT4 Console server code is historical ownership evidence, not a
ConPTY implementation. Full CSRSS/Console-server rehosting remains outside
the admitted boundary. Prefer a reusable terminal parser plus the smallest
frontend transport binding, not a second hand-written general terminal.

## Next verification and acceptance ledger

- [ ] Pin and inspect a reusable terminal source/license/build closure before
  import; compare against the Microsoft terminal implementation boundary.
- [ ] Build a checked-in x86 ConPTY contract probe: direct target versus
  attached descendant lifetime, output drain/EOF and explicit session close.
- [ ] Prove raw/cooked keys, releases, mouse/control events and unconsumed
  input return, including native-to-DOS handoff. Do not replay a shadow input
  queue without knowing which events the target actually consumed.
- [ ] Select one terminal-state/query-reply owner and prove split UTF-8/VT,
  Unicode cells, resize, scroll/alternate screen and both display paths.
- [ ] Migrate production as a complete backend closure, remove replaced
  helper and duplicate renderer, retain authentication and execution owners.
- [ ] Run the complete proposal regression/publication gates and report
  deleted/new/imported footprint separately.

Confidence: the old ownership and lifetime/input-return dependencies are
directly source-proven. ConPTY replacement equivalence is not yet proven.
The immediate action is capability tests, not speculative product deletion.
