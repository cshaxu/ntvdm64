# M0 T423 S15 — mouse ownership and native position

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Scope and cause

S14 was accepted and closed before S15 admission. S15 addresses two observed
Window-mode mouse defects without changing guest media, original MVDM code,
shared KVM libraries, or the DOS mouse movement contract.

1. Original `cmdexec.c` blocks the DOS event thread during native execution;
   `nt_event.c` detaches the DOS mouse menu; `nt_mouse.c` calls
   `ClipCursor(NULL)`. The compatibility RPC previously forwarded this
   directly to the host even while NTCON Window owned mouse capture. On the
   next input, the Window library observed that its clip had disappeared and
   released capture. NTCON now arbitrates that request: while its Window owns
   the clip, a DOS clip mutation is acknowledged without touching the host
   clip. Console mode retains the original forwarded request. Deliberate
   capture release remains a separate Window action.
2. NTW32 initialized its text square near the screen center and consumed only
   relative motion. Under absolute-position RDP input, the physical host
   pointer could reach the capture edge before the square did. NTCON now
   snapshots pointer and clip coordinates with each Window input event and
   converts native movement to absolute content pixels. The common pointer
   message adds action `CONSOLE_MOUSE_POSITION` (interface version 20); NTW32
   applies and clamps it using its current text viewport. DOS continues using
   its existing relative input path. The Win32 square remains available for
   CMD as well as modern EDIT; no executable-name heuristic was added.

## Build and tests

- Formal x86 build graph: `build/M0-T423/S15/formal-x86`; executable objects
  and links were produced with the graph's exact VS x86 compiler/linker
  commands. Ninja itself did not exit normally in this environment, so this
  is an exact-command formal-graph build, not a claimed successful Ninja run.
- Focused `frontend_window_mouse_test`, `ntw32_text_frame_test` (133 checks,
  zero failures), and `console_pointer_dispatch_test` passed. The updated
  `console_frontend_test` pointer-action assertions passed, but its full run
  could not pass in the non-interactive build terminal: the unrelated
  `SetConsoleScreenBufferSize` check returned error 87. This is not counted
  as a complete test pass.
- Candidate package through short `Q:` mapping: 17/17 Console routes, 17/17
  Window routes, and the three prior WOW frontiers passed. Normal paced
  DOS↔CMD handoff cases passed. The supplemental zero-delay
  `dos-native-typeahead` case failed both candidate and accepted S14 package
  under the same conditions; it remains a pre-existing separate input-order
  gap, not a new S15 pass.
- Published coherent eight-file package to `O:/winnt`, with prior files backed
  up under `build/M0-T423/S15/publication-backup-r1`. SHA-256 of each deployed
  file matched its candidate. Published-path logs in `O:/winnt/Logs2`:
  `t423-s15-published-console-r2-summary.json` (17/17),
  `t423-s15-published-window-r1-summary.json` (17/17), and
  `t423-s15-published-wow-r2-*` (WINMINE main window, SOL original memory
  modal, WRITE original memory modal). These preserve prior depth, not full
  SOL/WRITE application acceptance.
- Initial published-path invocations used an incompatible observer for the
  Console matrix and a desktop-incompatible reader/observer pairing for WOW;
  neither produced a valid product verdict. Both were rerun using the
  established observers above. The focused modern EDIT VT mouse test passed
  in `t423-s15-modern-edit-r1.txt`.

After P1, the published `O:/winnt` package ran `dos-native-typeahead` twice
under private-desktop Window mode with zero line delay and a 60-second timeout.
`t423-s15-published-typeahead-r1` passed: two MEM outputs, intact `exit`,
original COMMAND exit 1. The identical `r2` timed out: the final command was
`eexit`, matching the prior S14/candidate failure. This is an unstable
DOS/native input-order path, not a reliable pass. The owner explicitly directed
S15 closure and transfer of this issue to S16 to distinguish test timing from
production behavior. No test assertion was weakened.

Physical RDP pointer behavior was not visually verified by the agent; the
recorded T423 exception permits private-desktop probes plus owner side-test.
S15 is closed by owner direction with that waiver and the typeahead transfer;
T423 remains open.
