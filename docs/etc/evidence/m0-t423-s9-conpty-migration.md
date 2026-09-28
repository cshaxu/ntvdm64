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

## Native input records and parser candidate audit

The same x86 probe now has four additional cases. Final run
`t423-s9-conpty-contract-r5.log` passes all seven cases; build log
`build/M0-T423/S9/conpty-lifecycle-build-r5.log` has no warnings.
Source SHA256 824EC3D1C69FC8395F35DCFDC082CD251E8CA183E3E4DF798F44ECB9D7E395E0;
EXE SHA256 D3A0BF5C8A2F720072478C7C0B7BC78681C696C593371068E9CBFC562CDB350E.

- Win32 keyboard sequence: the real native client receives Ctrl/F1 down/up,
  exact VK/scancode, repeat count, Unicode and modifier state.
- SGR mouse: actual MOUSE_EVENT records preserve down at (10,6), drag at
  (11,7), then release with zero buttons, including the movement flag.
- Focus sequences: actual FOCUS_EVENT true/false reaches the client.
- Unread records: after consuming A down/up, the attached client peeks and
  reads only the still-queued B down/up. This proves local queue access, NOT
  frontend reclamation across a DOS handoff. That integration remains open.

The test disables cooked/processed input for these raw-record cases; it does
not claim cooked editing, Ctrl+C/Break, Windows Terminal physical input,
arbitrary mouse combinations or all Unicode input are verified. r4 passed
with test signedness warnings and misleading union-field printing for mouse;
r5 corrects printing/types and reruns all cases without altering assertions.
The protocol source is Microsoft's
[Win32 input specification](https://github.com/microsoft/terminal/blob/main/doc/specs/%234999%20-%20Improved%20keyboard%20handling%20in%20Conpty.md).
It describes keyboard encoding, not universal INPUT_RECORD serialization.
Mouse/focus were therefore independently tested, not inferred from the spec.

### Reusable terminal candidate

Official source archive:
https://www.leonerd.org.uk/code/libvterm/libvterm-0.3.3.tar.gz
SHA256 09156F43DD2128BD347CBEEBE50D9A571D32C64E0CF18D211197946AFF7226E0.
It was downloaded and path-validated before extraction solely under
`build/M0-T423/S9/libvterm-audit`; no production source import. LICENSE is MIT,
copyright Paul Evans (2008), requiring notice retention. The nine src/*.c
units, two public headers and internal headers/generated encoding includes
are the candidate compile closure; CLI tools/tests are not runtime inputs.
The nine C files contain 4,827 nonblank lines, not a zero-footprint dependency.

The original sources compile unchanged with MSVC Win32/x86 /MT /std:c11
/W3 /D_CRT_SECURE_NO_WARNINGS and the upstream include directory. Build-local
`compile-r1.log` retains upstream signedness/narrowing warnings, not errors.
There are no GUI/toolkit or POSIX runtime dependencies in this selected closure.

Checked-in admission test `tests/observation/conpty_terminal_candidate_test.c`
links those nine objects with /MT /std:c11 /W4. It passes 20 assertions in
`t423-s9-terminal-candidate-r2.log`: one-byte input fragmentation, CJK width,
combining character, RGB attributes, single cursor reply, alternate-screen
restoration, scroll, resize preservation and SGR mouse press/release output.
The initial link attempt r1 failed because the test's C hexadecimal escape
accidentally consumed the following 'e'; splitting the literal fixed the
test, not the upstream source. Final test link log has no warnings.
Test source SHA256 295ED814F439D86C87423E46EFD4C5E37128A4807D090E8BABA6FF1FAA1CF5F4;
EXE SHA256 A5EEBF4D0D487362898CCA65F5B767DC54058BA6DFEE4D879F6FFA38006C393F.

Important non-pass: state.c consumes DECSET/DECRST 9001 without notifying
the unrecognised-CSI fallback; keyboard.c has no Win32 key-record encoder.
The test explicitly asserts/reports that limitation. A naive fallback
callback cannot recover it. Before selection, audit an upstream-supported
parser interception boundary or the Microsoft parser closure; do not patch
shared libraries or add a duplicate terminal screen model to hide this gap.

The source review also confirms ntkvm is attached to its visible Console,
whereas the inner run16 may be attached to the native backend. Public Console
handles are not generic cross-process transferable handles; see
[Microsoft Console handles](https://learn.microsoft.com/en-us/windows/console/console-handles).
Thus attached-caller unread-record capture is a candidate finite handshake,
not permission to make the worker own a Console, add another helper, or claim
that a disconnected byte-stream reader knows which keys were consumed.
