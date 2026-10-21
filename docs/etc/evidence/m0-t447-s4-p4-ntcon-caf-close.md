# M0 T447 S4 P4 — NTCON CAF management-close handoff

## Final disposition

The [owner-directed transfer closure](../../history/m0-t447-transfer-closure.md)
supersedes any completion claim below. This is partial hardening, not universal
CAF defect resolution: launcher timeout/error exits and direct component waits
still violate the newly approved service-only contract. The full source-audit
gap list belongs to the lifecycle successor. Latest closure-build (13 images,
1376 unchanged intermediates), seven closure-tests, service33, three WINMINE
modes and actual staged CMD/NTMON self-delete echo tests passed. The final
closure-publication.json confirms all 13 hashes at both authorized destinations.
Installed and guest failures documented below remain failures, not passes.

## Service-owned completion repair, 2026-10-21

The repair below supersedes the historical publication attempt at the end of
this record. Source baseline is `a9bc337fd` plus the owner's pending changes;
those changes were reviewed and retained or corrected, not reset. Single
implementer/reviewer, no guest modification or guest compilation.

### Actual responsibility and normal versus management completion

- NTSRV owns the authenticated Console lease and request-completion barrier.
  `OpenNtBaseServiceWaitFrontendConsoleRestored` classifies no-lease/nested
  requests and the current lease owner. Run16 does not choose a root-only
  Console operation. Final acknowledgement is retained even when it arrives
  before the launcher waits or the frontend connection is removed.
- Run16 invokes the same scope completion on normal, task-failure and partial
  setup-failure exits. It preserves failure identity/exit status; an aborted
  worker is not falsely marked `target_completed`.
- Normal task completion parks NTCON and `FrontendLeaseReady` returns the
  borrowed Console. NTCON and an idle worker may survive for reuse; Run16
  does **not** wait for NTCON process death.
- Management Console deletion uses the same NTSRV transaction for internal
  and external NTMON. NTCON prepares return and freezes presentation; NTSRV
  sends Console-close, observes worker exit, and permits final NTCON teardown.
  Only the final restoration acknowledgement releases the lease owner.
  A preparation acknowledgement is not final return.
- NTCON owns the actual active-buffer/input/cursor/palette restoration. A
  visual restoration failure does not suppress independent input restoration.
  A failed channel join retains live callback storage and reports failure,
  never a false successful return.

### Seven-point audit and adjacent self-review

| Finding | Disposition and evidence boundary |
| --- | --- |
| Background scan bypasses drain | Draining remains an owned close transaction, never unattached admission. Scan sends Console-close only after preparation, or frontend death. |
| Worker-error skips launcher wait | Uniform scope completion, including setup failure; fixture retains original failure and false target-completed. |
| Timeout strands drain | Late acknowledgement and actual worker exit advance the retained transaction. Real ten-second timeout fixtures exercise both stages. |
| Cleanup error suppresses mode restore | Independent restoration attempts retain first error; live storage is not freed after failed joins. Arbitrary Win32 API failure injection is not claimed. |
| Late graphics reopens CAF | Terminal presentation freeze rejects new acquisition/video and ignores queued display changes after successful preparation. |
| Partial captured-handle leak | Close the current duplicated process handle if its event duplication fails. Source-path review; exhaustive DuplicateHandle fault injection is not claimed. |
| Broken repeat-round assertion | Keep the original reusable channel rounds; separate management-return fixture owns its terminal frozen session. |

Self-review also found that a frontend dying during drain could receive a
second ordinary worker-close from the orphan scan. The scan now leaves that
case with the Console-close transaction; the timeout fixture explicitly kills
the frontend before its worker and checks that shutdown remains unsignalled.
The prepared NTCON pump excludes stale manual notification/retire, creator and
channel-exit events; it retains the service-state wake and a pinned broker
process wait, so broker death cannot leave a frozen frontend waiting forever.
No polling sleep, injected Enter, input flush or caller-location heuristic is
part of the runtime fix.

### Build and verification ledger

Owned scratch is `build/t447-s4-p4/`; test-only G7 witness generation uses
`build/M0-T447/S4/p4-fixture/`. Stable caches remain under `build/.ninja/`
and `build/.ccache/`. No cache clear or prune. Raw dumps and coherent rollback
images remain local while this reopened defect awaits acceptance.

Initial full-build r1 failed because an MSVC tree retained GNU linker/archive
paths and the caller selected a different CMake executable. Reconfigured the
same trees with the actual MSVC linker/lib and Kitware CMake; no cache deletion.
Full-build r2 and r3 passed all13 and second-pass identity of1376 intermediates.
The final r4 run includes the later self-review fixes; its result, not r2/r3,
governs publication. The first r4 configure correctly rejected the unregistered
window-library hash change; registration below preceded its retry.

The service33 suite found two stale fixture assumptions, not relaxed product
checks: Console association alone still cannot obtain the native release
event, and an authenticated prepared root loss selects Console-close rather
than ordinary shutdown. Fixtures now establish native frontend registration
and assert the actual close kind. The added dead-frontend test initially tried
an authenticated RPC from that dead peer; corrected it to inspect service-owned
terminal state, retaining the shutdown-negative assertion.

Reproducible actual-UI entrypoint is
`tests/observation/verify-console-caf-self-delete.ps1`, with the compiled
`console_startup_observer.c` and `console_handoff_pty_probe.c`, an exact complete
package and fresh build LogRoot. It launches the system conhost, uses real
in-worker NTMON Delete/Y, validates the selected frontend identity, and checks
both saved and current-active buffer echo **before Enter**. The harness owns
its process Job; it never sends input to the preserved owner fault window.
Earlier CMD and direct NTMON tests passed. COMMAND initially failed the prompt
gate before CAF; those attempts are not passes. Final evidence distinguishes
test prompt spelling from actual product startup failure.

### Imported-source exception

No OpenNT mirror or original guest diff is added. One existing imported nxvm
header changes its finite component destruction budget from5000 to10000ms,
per the owner's explicit all-bounded-shutdown-ten-seconds instruction. A local
DIVERGENCE marker, `src/ntcon-exe/README.md`, and `nxvm-import.json` retain the
upstream identity and pin local bytes. Only that constant changes behavior;
ABI, joined disposal and timeout failure rules remain. The library stays
frontend-only and does not gain a protocol or worker dependency.

## Reopened investigation: self-hosted request, 2026-10-20

The historical "Root cause" section below is the rationale for the published
attempt, **not a proved explanation of the still-reproducible owner symptom**.
The initial owner report implicated nested CMD or COMMAND plus CAF plus an
internal NTMON deleting its own frontend. The later preserved conhost scene
also fails with direct `run16 ntmon`; nesting is therefore NOT a necessary
condition. Earlier successful direct/external observations remain individual
results, not universal exclusions. See the actual-scene evidence below.
Do not report this investigation or the staged transaction repair as closed.

### Preserved owner conhost scene: distinct echo buffer

Question: does the real failing cooked read retain a different screen buffer,
or is the symptom merely inferred from an independently constructed probe?

The owner preserved the failing `conhost -> run16 ntmon -> CAF -> delete
NTCON` window. Its outer CMD is PID20492, parent conhost25728, created
2026-10-20T22:33:52Z. No nthook/ntw16 module was present in that outer CMD.
The read-only capture attached briefly and detached without writing input,
changing mode/buffers or pressing Enter. Do not confuse the earlier
`owner-conhost-20492-before-enter.txt` with this scene: despite its filename,
that earlier capture already contained the previous `test` command error.

Actual-scene observations under `build/t447-s4-p4`:

- `owner-live-attrs-r1.txt`, 22:39:29Z: input mode0x1f7, output0x7,
  current attributes0x7, visible cursor(20,1), viewport(0,0)-(38,29).
  LINE_INPUT and ECHO_INPUT are on. The screen remains at the outer prompt.
- `owner-input-queue-r1.txt`, 22:41:08Z: zero queued input records, using
  GetNumberOfConsoleInputEvents/PeekConsoleInput, not an input-consuming read.
- `owner-cmd-wait-r1.txt`, 22:43:09Z: the sole CMD thread5544 is waiting in
  NtDeviceIoControlFile; its immediate return address is in ReadConsoleW.
  Other listed stack candidates are raw addresses, not an unwound call stack.
- Full local dumps at22:44:16/17Z preserve the state without terminating the
  processes. CMD dump is28,567,102 bytes, SHA256
  E179FC06165F65C6144BA419C4306AA3195C6ECDC5FC03E5D906FDA699DD9505;
  conhost dump is76,486,332 bytes, SHA256
  AED4675C8FB28CD257E6A668DAAA0C6BD537FBB07DD14F08D1BBBCB6F783B2F8.
  MiniDumpWriteDump returned TRUE; the logged LastError6 is stale, not a
  failing result. Dumps stay local and are not release inputs.

Offline analysis uses the actual system conhost image and its matching
Microsoft symbol-server PDB, not a substituted OpenConsole host or assumed
NT4 layout. RSDS key is05003BCECD057B162B08FF52B1C0D7F71; PDB size2,134,016,
SHA256 CCC414431D553FD98A90587CE9771F76CFC7205BEEB4B0A965A09FAE3825B2B1.
Dump module base is0x7ff66b1c0000. PDB enumeration used a synthetic load base
0x180000000; subtract it to obtain RVAs before applying the dump module base.

The exact binary's cooked-read constructor RVA0x6792c stores its screen
reference at object+0x20. Its write implementation RVA0x69f90 uses this
reference, then screen+0x30 for the text buffer. The dump contains a cooked
read at0x2bc75602d60 (vtable RVA0xb4860): application CMD.EXE, inline UTF-16
text `cddd`, length4; screen reference0x2bc755bac30, text buffer0x2bc7770ce60,
cursor(4,0). These are actual received characters, not injected by this
read-only snapshot procedure.

The exact CONSOLE_INFORMATION::GetActiveOutputBuffer getter RVA0x31110 reads
this+0x6e8. Globals construction places that Console-information object at
image RVA0xe06c8. Therefore the dump's current active-screen slot at
RVA0xe0db0 contains0x2bc7558c460, NOT0x2bc755bac30. Its text buffer is
0x2bc755af720 and cursor(20,1), agreeing with the live public-API snapshot.
The binary-derived offsets are offline diagnostic evidence only: they must
not enter product code or become a dependency on a private conhost layout.
Supporting disassembly/symbol reports are `owner-cooked-constructor.asm.txt`,
`owner-cooked-write.asm.txt`, `owner-active-getter.txt`,
`owner-globals-constructor.txt`, `owner-globals-constructor-2.txt` and
`owner-global-symbols.txt`.

The final admission check removes the alternative that the cooked read simply
reused a stale output HANDLE supplied by CMD: at RVA0x6403e--0x64055 the actual
conhost calls Console-information vtable slot1 and passes its returned screen
reference to the cooked-read factory at0x640d4. The saved vtable slot points
to RVA0x31110, the active-output getter above. The factory and constructor
preserve that reference. Thus this read captured the THEN-active screen,
which is different from the NOW-active screen in the dump. Reports:
`owner-cooked-admission.txt` and `owner-cooked-factory.txt`. The read began
before the final selection of the currently visible screen; merely restoring
mode bits or changing the active screen later cannot rebind it. Identifying
which premature-close scheduling branch won requires a pre-event trace, but
does not change this established handoff-order defect.

Conclusion: the actual failed CMD read has received characters and holds a
different echo screen than the currently visible screen. This is stronger
than mode bits or the independent late-switch experiment. It explains why
ending that read with Enter can allow a fresh read to echo correctly; it is
not evidence that FlushConsoleInputBuffer is required.

Source review of published HEAD a9bc337fd (not the staged repair) identifies
two cooperating ordering holes: management marks the frontend draining while
waiting for return, but service_retire_expired_frontends_at treats draining
as unattached and can send shutdown first; worker rundown signals the native
receipt with ERROR_PROCESS_ABORTED, while FinishNativeRequest returns that
error before setting target_completed. Run16's native path conditions its
root return/wait on target_completed, and scope_end has no compensating wait.
Thus a failed worker can release outer CMD while the CAF buffer is still
active. window_route explicitly activates a newly created temporary buffer
for CAF and restores console_output later. No NTMON direct authority over the
visible Console is needed for this race.

Confidence boundary: distinct active/read buffer identity is established in
the real failed scene; the exact event chronology selecting the premature
worker-exit branch was not recorded before this dump. Do not claim the scan
branch was observed executing, that it is the only possible ordering hole,
or that staged fixes have passed the owner's scenario. No new product build,
publication or repair acceptance follows from this offline diagnosis.
Documentation validation after recording the scene: git diff --check passes;
background governance check18696 exits1 because CURRENT exceeds32KiB. This is
an outstanding control-record size issue, not a passing delivery gate. No
commit, publication, S4 closure or owner repair acceptance is claimed.

The existing handoff observer issued its management RPC from an external
process, so it did not exercise the reported self-hosted edge. The new opt-in
`MVDM_OBSERVER_CAF_SELF_NTMON` runs the actual published NTMON inside the
nested shell, waits for its title, presses Delete, verifies the exact
`End Console <test frontend PID> [Y/N]?` text, then sends Y. It does not select
an arbitrary frontend or call a substitute close RPC. Tests use the installed
package, not staged production changes. Its NTCON and NTSRV hashes match the
release identities below; Run16 is
`4B847D0467ADF26648547F2FD4ABFC8CCE2F64093F7C4DF5595631203C54A709`.

A second definite observer error was found: its `q` followed by Enter executes
the host's PATH-resolved `q.cmd`. That script calls its
arguments and exits CMD. This explains the observer's failed *post-Enter*
character check, not the owner's product defect. The corrected probe clears
the diagnostic character with Backspace before submitting an empty line, and
switches further input away from the now-dead CAF HWND. Pre-Enter snapshots
are now taken before sending Enter, not immediately after sending it.

`tests/observation/console_handoff_pty_probe.c` supplies an owned ConPTY/Job
container and captures its actual terminal output. The observer's opt-in
owned-PTY mode keeps this Console instead of detaching into AllocConsole.
The test-only inherited input-pipe handle is made non-inheritable before
launching product children. After CAF closes, test keys travel through the
ConPTY input pipe rather than bypassing its parser with WriteConsoleInput.
No user terminal, global Console setting or product binary is modified.

Evidence under `build/t447-s4-p4`:

| Diagnostic | Result and limit |
| --- | --- |
| `self-ntmon-installed-r1.txt.caf.txt` | Real internal NTMON confirmed and target exited; first character present before Enter. Post-Enter result invalidated by q.cmd. |
| `self-ntmon-pty-r1` | Input/Console path ran, but terminal byte capture was empty; not output evidence. |
| `self-ntmon-pty-r2` | Terminal output capture works after isolating inherited standard handles; still invalid q.cmd post-Enter check. |
| `self-ntmon-pty-r3` | Fixed blank-line test: immediate q and post-empty-line r both present; not a reproduction. |
| `self-ntmon-pty-r4` | Same result with actual ConPTY input pipe; mode 0x1e7; output stream contains q, erased q, next prompt and r. |
| `self-ntmon-pty-r5` | Same result with 1.5-second diagnostic dwell at CAF, NTMON and confirmation; no production delay. |
| `self-ntmon-pty-r6` | Adding mouse-input mode before launching outer CMD still returns 0x1e7; immediate and post-empty-line echo work. This did not establish a 0x1f7 matched control. |
| `self-ntmon-pty-r7` | ConPTY Win32-input key encoding retains immediate q and post-empty-line r; no reproduction. |
| `self-ntmon-pty-r8` | Observer calls FreeConsole before confirming Y. Raw stream byte 9248 is q at tick65585562, before erase/Enter at65586328; r is emitted at65586593. Extra observer membership is not sufficient to explain the missing reproduction. |

### Pending cooked-read buffer experiment

An independently authored host-only diagnostic,
`tests/observation/console_cooked_buffer_probe.c`, tests a concrete possible
mechanism without modifying or launching product/guest images. Public Microsoft
[Console host source](https://github.com/microsoft/terminal/blob/main/src/host/readDataCooked.cpp)
retains a SCREEN_INFORMATION reference for each cooked read and uses that
reference for echo. This is research evidence, not imported code or proof of
the exact installed host implementation.

The local experiment creates two owned Console buffers and starts ReadConsoleW
in a thread. In the late-switch case it selects the second buffer after that
read starts; in the early-switch control it selects it before starting the read.
Both inject q, inspect both buffers before Enter, finish the line, then start
a fresh read and inject r. A 250ms call-site dwell is diagnostic pacing, not a
kernel-admission acknowledgement. Actual buffer contents are the observation.

Background runner21764 built MSVC x64 /MT and ran owned children28616/7184,
each bounded10s; both exited0 at2026-10-20T22:11Z. Captured evidence is
`build/t447-s4-p4/cooked-buffer-r2.stdout.log` and the two `-late/-early.txt`
reports. Results:

| Switch relative to first read | Old buffer before Enter | Active buffer before Enter | Fresh read after Enter |
| --- | --- | --- | --- |
| After read starts | q (113) | space (32) | r (114) visible |
| Before read starts | space (32) | q (113) | r (114) visible |

Source SHA256 E03FC2CF3AB6814562D2B03791CA592D45AD51689F0D1E530F5869EDF75A8F84;
probe image DACF45874FE6348F460CC19BD1A4785BEB2E19BE9DF61EF41A8B25B8DA14C30E.
This proves a local mechanism that produces no visible echo until a new line,
without incorrect cooked-mode bits. **It does not yet prove the product enters
this ordering**, or explain the internal/external caller difference. The next
causal evidence must establish outer CMD's read admission relative to NTCON's
last buffer selection, including error-return and late-presentation paths.
No additional production repair or publication was made for this experiment.

These results do not disprove the owner's report. Remaining unproved edges
are the actual Terminal UI/focus/physical keyboard sequence and the original
failure's 0x1f7 input mode versus the probe's 0x1e7. The attempted mode-matched
control did not retain the requested difference through CMD startup. COMMAND
has not been rerun with this new internal-caller observer.

Source inspection independently found that console_return_frozen guards the
presentation loop but not frontend_session_enter; channel SET_MODE and text
projection remain reachable after return acknowledgement. This is a staged
barrier coverage defect, not yet causally tied to the owner's symptom.

Temporary ownership: single S4/P4 investigator; `build/t447-s4-p4` holds the
diagnostic runners, logs and captures. Each owned observer is bounded at 45s,
with a 55s runner guard; builds are background and serialized in the existing
x64 Ninja tree. Retain at most 1 GiB/one investigation phase, with no-progress
review after each bounded run. Before phase cleanup, preserve this evidence,
exact hashes and any failing witness, join owned processes, then use the
repository's reviewed cleanup path. No cache clearing or unrelated process
termination is authorized by the probe.

## Scope and ownership

This is a project-owned host lifecycle correction.  It changes NTSRV and
NTCON only; it does not alter or rebuild guest source/media, OpenNT mirror
files, guest configuration, or public RPC layout.  The existing scalar
workerless-frontend reply acquires two named values in the shared host header:
final close and pre-close return-to-Console.

## Root cause

The reported sequence is `run16 command` (or `run16 cmd`), Ctrl+Alt+F, NTMON
Console deletion, then return to the outer CMD.  A failing trace already
showed restored cooked mode (`0x1f7`), so a mode-bit or input-flush patch would
have addressed the wrong symptom.  The management path ordered DOS worker
shutdown before NTCON left Window mode.  Worker shutdown broke the owned
NTCON channel (`ERROR_PIPE_NOT_CONNECTED`); NTCON could not synchronously
select the borrowed Console before teardown.  The parent CMD's next read then
did not render one-character input until Enter.

Natural CAF return and CAF followed by normal Window close did not exhibit
this route, isolating it to management close while the Window route was live.

## Correction

1. NTSRV puts the selected frontend into its existing draining state and
   signals a pre-close return request while the authenticated channel lives.
2. NTCON's presentation thread selects the Console synchronously and reports
   the existing frontend-restored acknowledgement.
3. NTSRV waits, bounded at ten seconds, for that acknowledgement, then sends
   the unchanged authenticated worker-close request.  A failed acknowledgement
   still attempts requested worker close but is reported as failure.
4. Final frontend close is published only after that close route completes.
5. A second, unrelated lifecycle writer in `worker_registry.c` that made the
   final worker exit close an ordinary root immediately was removed.  The
   existing lifecycle deadline remains the single owner for ordinary
   workerless-root retirement.

This is not a delay/retry policy and does not change original DOS final
`ResetConsoleState` behavior.

## Verification

| Evidence | Result |
| --- | --- |
| `basesrv-service-reservation-test.exe` | Pass: reservation kinds, CUI/WOW worker reuse, authenticated close/lifecycle and original Check/Update/Get/ExitVDM path. |
| `Test-FullComponentBuild.ps1` final r3 | Pass: 13 images plus `wow_message_observer`; second build rebuilt no output or intermediate; 1,344 intermediate records unchanged. |
| Staging | Fresh `assets/winnt.zip` baseline; every selected image PE-machine and SHA-256 checked. |
| Publication | Exact 13 files in `assets/release` and `O:\winnt\system32` match the final manifest. |
| Runtime observer | Its command line quoting and hidden-window defects were corrected.  In this automation desktop the actual `LibKvmWindow` remains undiscoverable (`find-kvm`) after the outer CMD and inner command prompt reach readiness.  This is not counted as a product pass. |

The final manual acceptance should use the actual owner route: `run16 cmd` or
`run16 command`, Ctrl+Alt+F, NTMON Console delete, then type a character in
the original terminal before Enter.  It must echo immediately.

## Release identities

The final deployment's changed host images include:

- `ntcon.exe`: `0C7F46E000FEBF5B24F95391FD5B8220E97AF360A918C34A1A13AB040BB05845`
- `ntsrv.exe`: `FC191F97FB0EE1BEB4190B98B5BC37D197CC90E880DB48A6200EDFECD59E4D8E`
- `ntvdm.exe`: `691EE63CA50020E161403F035B6FD29D68CCC20117EE8356774F62AAE1BF3FFF`
- `nthook32.dll`: `6C48BBB2AEF67ECD2E066D3F214ACD111BB615A30D22192271C2104D4FA9C9BE`

All other selected image identities are listed in `assets/release/release-manifest.json`.
