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
- [x] Build a checked-in x86 ConPTY contract probe: direct target versus
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

## Real x86 lifetime probe

Checked-in test: `tests/observation/conpty_lifecycle_test.c`. This authored
probe starts one leader and an inherited-Console leaf inside ConPTY; named
events gate leaf completion. It never launches the product, guest, UI window
or owner desktop. Cleanup owns only the exact probe processes/session.

Reproduction from the repo root: initialize VS2022 BuildTools VsDevCmd.bat
with `-arch=x86 -host_arch=x64`, then run
`cl /nologo /MT /W4 /Fo:build/M0-T423/S9/conpty-lifecycle.obj
/Fe:build/M0-T423/S9/conpty-lifecycle.exe
tests/observation/conpty_lifecycle_test.c`. All outputs remain below build.
Run the resulting executable without arguments, redirecting its report to a
fresh O:/winnt/Logs2 path. Nonzero return fails the fixture. It requires the
ReleasePseudoConsole export; absence is a failed capability, not a skip/pass.

Inputs: host kernel32 file version 10.0.26100.9549; MSVC Win32/x86 /MT /W4.
Final build log `build/M0-T423/S9/conpty-lifecycle-build-r3.log` has no warning.
Source SHA256 6A5A2B447C471AFC3B4F58B9175F83B56EE5B128482A8A5474084F6C3EC2C891;
EXE SHA256 73EEC3F69B05B4D3C274C4AF903441934DFFB55E8291669C2F04AC51A1C41F1D.

`O:/winnt/Logs2/t423-s9-conpty-lifecycle-r3.log` passes three cases:

| Case | Actual assertion/result |
| --- | --- |
| descendant-lifetime | Leader exits 37 while leaf stays alive; release leaf yields 23 and final screen marker; retaining HPCON prevents natural EOF (WAIT_TIMEOUT 258 before close). |
| explicit-session-close | Leader already exited; closing ConPTY delivers actual CTRL_CLOSE_EVENT to leaf, whose handler exits 91; capture drains and reaches EOF. |
| released-natural-retirement | ReleasePseudoConsole does not kill the live leaf; after leaf exits 23, output reaches EOF before ClosePseudoConsole; final marker remains captured. |

This is more precise than equating direct process completion with session
completion. The first case is the retained-HPCON negative control. Microsoft
documents exactly this ownership loop and its Windows 11 24H2 solution in
[ReleasePseudoConsole](https://learn.microsoft.com/en-us/windows/console/releasepseudoconsole).
The API remains dynamically discovered in this test, not a newly imposed
product minimum or a production import. Still prove backend recreation across
empty native intervals, outstanding authenticated admissions and input return
before using this mechanism in the frontend.

Retained failures: r1 correctly failed both capture assertions although
leader/leaf statuses were right. Probe stdout followed the supervisor's file
redirection instead of the attached Console. r2 made the selected screen
output explicit through CONOUT$ and passed the initial two cases. r3 adds
release/natural-EOF and reruns all three. Thus actual ConPTY output, not the
outer log or process exit alone, is required. No product behavior was changed.
