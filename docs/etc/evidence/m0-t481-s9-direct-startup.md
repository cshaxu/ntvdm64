# T481 S9: owner-reported direct Win3.1 startup failure

The owner reported `run16 O:/w31/patch/win386.cmd` terminating before the
desktop with illegal instruction bytes `63 6f 64 65 78`. S8 had not received
owner acceptance. The selected matrix previously launched the batch from an
existing CMD and replaced PATH, so it did not cover this direct inherited-environment
case. `win31_runtime_matrix_test.ps1 -DirectRun16` now provides that route;
`-DirectShortPath` is its isolated short-PATH control.

On independent Win3.1 copies, the current published S9 candidate and the S8
package both timed out at the same illegal-instruction dialog, CS:04D3,
IP:2032 and bytes `63 6f 64 65 78`. Thus this reproduction predates S9. The
same S9 image and direct invocation passed desktop, mode and normal exit
when only its isolated test process PATH was shortened. The first attempted
long-name test copy instead produced a `config.nt` error and is not evidence
of the owner's instruction failure. The original W31 installation was checked
unchanged; neither environment control changes the user's system environment.

A separately linked diagnostic host wraps the original environment copy and
only reads guest state. In the matching inherited-PATH failure, the COMMAND
environment block (ES=057E, 018C paragraphs) overlapped DS:203C, the released
INIT `EnvSiz` location. Its value changed from 018C before copying to 7261
afterward; the run then produced the same illegal bytes. In the short-PATH
control the allocated 010A-paragraph block did not contain DS:203C, the value
remained 010A, and Windows started and exited normally. These observations
match the previously retained original guest defect in TODO, without proving
the guest state of the owner's already-closed process.

The owner subsequently kept a matching failure open for a read-only snapshot.
The live process displayed `CS:04D3 IP:1F91 OP:63 6f 64 65 78`; the probe
confirmed those instruction bytes and found PSP environment segment 057E in
an owner-04D3 memory block of 0181 paragraphs. Both the fault address and
the known DS:203C `EnvSiz` location lie inside that block. The first probe
read no environment variable values beyond the instruction bytes already
displayed. At the owner's explicit subsequent approval, a second read-only
probe scanned the live environment block in memory, without saving or printing
values. It identified both the displayed instruction location and DS:203C as
inside `PSMODULEPATH`, whose value is 295 bytes long. The displayed `codex` bytes
therefore do not require an entry in PATH. The owner reports that their PATH
has no `codex` entry; this finding is consistent with that report. The block
observation and controlled copy trace establish the current overlap, but the
live failure is a snapshot after the copy, not a live before/after trace.

In a further isolated control, invoking the enhanced-mode PIF directly through
Run16 with the agent's inherited environment also failed before the desktop,
at illegal bytes `63 68 61 69 6e`. This rules out the batch wrapper as a
sufficient explanation for that reproduction; it does not claim every direct
PIF launch fails. The retained matrix runner exposes this control through
`-DirectRun16 -DirectPif`.

No product fix has been made for the guest defect: current authority explicitly
prohibits modifying the original guest, automatic environment truncation,
injected `/E:` or allocator special cases. Shortening PATH is a per-invocation
manual workaround, not a fix or a general safe environment-size threshold.
Do not claim the reported acceptance passed. The S9 image's graphic fixes and
the direct-start environment failure have distinct evidence and disposition.

## Owner's S7 success and additional controls

The owner states the same command worked during S7 and earlier. That report is
not contradicted by reproducing a failure under the current agent environment:
the earlier interactive process environment was not captured. The current
`O:/w31/SYSTEM.INI` SHA-256 `A83950B9BE72EAFADCDE89254258D811918C298749798941B02897F086EAFF66`
matches the configuration recorded at S7 acceptance. The actual S7 host
NTVDM SHA-256 `6DA99B438AB888577A2F6EAADFED3A57A8D19567A3413CAE217A33CE43153C25`
and then-released `ntvdm64.386` SHA-256
`0E4DA9B0B88FE89B20C30CBD7751EF1C4299316D6317BA40C98116B137A5712B` were
used together in an independent copy; its INSIGNIA driver matches the S7
release. With today's agent-inherited environment this S7 pair also failed,
with `CS:04D3 IP:2038 OP:63 6f 64 65 78`.

An additional isolated control constructed PATH from the current machine and
user registration plus the two test-tool directories. With the other agent
variables retained, the S9 pair still failed, at bytes `63 68 61 69 6e`.
With that PATH and `CODEX_`, `BROWSER_USE_` and `NODE_REPL_` variables removed
from the test process, both the S7 pair and S9 pair completed the same direct
startup and normal exit. These are controlled test environments, not a copy
of the owner's current or past terminal environment. They establish that
environment composition can change the outcome with either build; they do not
establish that the owner's PATH changed or exclude all other possible causes
of their particular screenshot. No environment values were retained in this
record.

The owner then reported that both `winstd.cmd` and `win386.cmd` failed. The
same inherited agent environment reproduced the direct standard-mode failure
before Windows startup, with illegal instruction bytes `63 68 61 69 6e`.
The read-only diagnostic for that standard-mode path observed the environment
block overlapping the same `DS:203C` location: `EnvSiz` changed from 018D to
646F during the environment copy. This establishes the same underlying
mechanism for both isolated direct reproductions. The standard-mode test's
source-hash finalizer also encountered a transient file deleted from the live
owner installation's PATCH/TEMP during the run; that is a concurrent-source
observation, not evidence that the test changed it. Subsequent diagnostic
checks excluded that volatile TEMP subtree while still verifying the other
source files. These probes do not recreate the vanished original process or
prove its exact inherited environment.

The matching standard-mode direct control also started and exited normally
with the reconstructed machine/user PATH and agent-only variables removed in
the isolated test process. Together with the enhanced-mode controls, this
supports one environment-sensitive startup mechanism in both modes; it does
not prove when or how the owner's interactive environment changed. The S7
manual success remains valid historical evidence, and the current owner
report remains a failed acceptance requiring an actual solution.

## Startup environment ownership and S7 comparison

The owner subsequently accepted S8, and reported a broader comparison:
Win+R direct `run16 command` also fails, while CMD → `run16 command` →
`win386` can start. The precise direct-command failure has not yet been
independently captured. It must not be assumed to share the PIF failure's
mechanism merely because both go through DOS. Compare direct and existing-CMD
launches with matched environments and both COMMAND and other DOS programs.

A preliminary hidden-window probe used one isolated package and inherited
agent environment to compare a newly created Console with a CMD parent, for
`command /c ver` and `MEM.EXE`. All four processes timed out after 30 seconds.
The supervisor initially returned zero despite the per-case timeouts; that is
a probe defect, not a test pass. Its disposable script was corrected to return
failure when any case fails. It did not record console output or dialog state,
and it reused one service after each timed-out case. It therefore neither
reproduces the owner's successful CMD-parent comparison nor attributes any
timeout to product behavior; a valid comparison requires isolated cases and
the existing observer's captured console/dialog evidence. The first probe and
its captured statuses remain under `build/t481-s9/direct-dos-matrix.*`.

An independent follow-up (`build/t481-s9/direct-dos-isolated.ps1`, supervisor
PID 10728, start 2026-11-09T05:06:38Z) created and retired a fresh package and
service for each case. It ran `command /c ver` and `MEM.EXE`, each directly in
a new hidden Console and via a new `cmd /d /c` parent. With the recorded
machine/user PATH and agent-only environment variable prefixes removed, all
four exited zero. With the agent's inherited environment, all four hit the
30-second diagnostic deadline. The runner returned failure (1), not success.
The output is `build/t481-s9/direct-dos-isolated.txt`; values of environment
variables were neither printed nor saved by the probe. Each run included a
test-stage PATH prefix and uses a hidden new Console, so it is not an exact
Win+R invocation. No output, dialog text or runtime stack was captured; exit
zero is not a proof of correct displayed content. This matrix establishes a
repeatable environment-sensitive difference in the test setup but does not
reproduce or refute the owner's CMD-parent success, identify which variable
changed between owner launches, or attribute every timeout to the known guest
overlap. The running owner's original guest and installation were not used as
the test target.

An additional shell-launch parent probe called `Shell.Application.ShellExecute`
from a supervised PowerShell process and recorded only process identities. Its
new PowerShell child's live parent was that PowerShell launcher, not Explorer.
It therefore cannot stand in for the owner's Win+R launch or establish the
environment Explorer supplied. No process environment values were recorded.

The desktop ShellWindows dispatch was subsequently used for a separate
test-owned hidden process; its recorded parent was `explorer.exe` PID 22072,
not the PowerShell client. A separate Python child of that same desktop
dispatch recorded only `PSMODULEPATH` and length 295, matching the length in
the owner's faulting NTVDM. Equal lengths do not prove equal values, full
environment identity, or when this variable acquired its current content.

Explorer-launched PowerShell PID 30660 then ran the independently staged DOS
matrix at 2026-11-09T05:16:53Z. Its four cleaned-environment cases exited zero;
its four inherited-environment cases timed out. `failures=4` is a failed
diagnostic, not a passing run. The PowerShell host and stage PATH prefix may
change the environment relative to a direct Win+R Run16; no console output,
dialog or exception capture was made. The result therefore strengthens the
environment comparison without establishing the owner's CMD-parent contrast
or the exact reason the owner's S7 session passed. Outputs are under
`build/t481-s9/direct-dos-explorer*` and
`build/t481-s9/explorer-env-metadata.txt`; no variable values were saved.

To remove the PowerShell environment from the observed target launches, a
separate test-owned Python process was launched by desktop Explorer PID
22072; process metadata confirmed that actual parent and its PSMODULEPATH
length was 295. Without changing its inherited environment it ran absolute
staged COMMAND `/c ver` and MEM paths, directly in new Consoles and with
`cmd /d /s /c` as parent. Every case had a separate staged package identity.
Both direct launches timed out at 30 seconds; the two CMD-parent cases exited
1. No output or dialog was captured, so the CMD results could be command-line
or product failures and cannot establish the owner's successful parent path;
the timeouts cannot by themselves attribute a crash to the environment copy.
The supervisor initially returned zero merely because the Python loop finished;
this is not a passing test. The disposable Python probe was changed to report
`completed-with-failures` when any case fails, making the supervisor fail on a
repeat. The actual completed run is retained as
`build/t481-s9/explorer-native-dos.jsonl`; no environment values were saved.

The owner later reported an existing CMD → `run16 command` → Win3.1 success,
and a direct Win+R `run16 command` failure. A closer control used desktop
Explorer's own ShellWindows dispatch to launch the isolated stage's actual
`run16.exe command`, then separately launched a new CMD from that Explorer to
run the same command. The read-only dialog probe checked only whether a
stage-owned NTVDM had visible child text containing `illegal instruction`;
it did not record the text or environment values. For both the current
`package-rotate` and S7 `package001`, the direct case had parent
`explorer.exe` and the fresh-CMD case had parent `cmd.exe`; all four reached
NTVDM and displayed that error type. Runs were supervised under PIDs 16156
and 24544 respectively. Each case had an independent staged package identity
and cleanup scope. Results: `build/t481-s9/explorer-exact-compare.jsonl` and
`build/t481-s9/explorer-exact-s7.jsonl`.

This reproduces the direct interactive failure with the real desktop launch
parent and shows that today's Explorer-derived environment also triggers it
with the S7 host package. It does not establish the failed instruction bytes,
the complete cause of these four new failures, or the environment of the
owner's already-running successful CMD. A freshly launched CMD inheriting
today's Explorer environment is not evidence about that earlier CMD session.

The desktop Explorer that parented the earlier direct probe (PID 22072) has
since exited. The current desktop ShellWindows dispatch launched a metadata
probe from Explorer PID 28636, started at 2026-11-08 21:36:25 local time; its
PSMODULEPATH value length was again 295, with no value recorded. A fresh
current-package direct/ CMD-parent interactive comparison under that new
Explorer (supervisor PID 5284, started 2026-11-09T06:13:26Z) again found the
stage-owned NTVDM `illegal instruction` dialog in both cases. Both owned
stages were retired by the runner. This confirms the failure remains after
the Explorer change; equal variable lengths do not prove equal environments
and this does not reconstruct the owner's prior successful CMD session.

An actual interactive CMD control was then launched by current desktop
Explorer PID 28636 in a fresh isolated stage. A test-owned process attached to
that CMD's Console, waited for its prompt and typed `run16 command`; it saw
no DOS banner within 20 seconds while the stage-owned NTVDM displayed an
illegal-instruction dialog. The test-owned CMD, worker scope and stage were
retired; no existing user terminal was touched. In an independent cleaned-
environment control using the same input probe, the CMD prompt and DOS
banner appeared and no illegal-instruction dialog was observed. The probe's
later `ver`, `exit` and return marker were not observed, so neither complete
interaction nor normal exit is claimed. The cleaned control was PowerShell-
launched and therefore does not reconstruct the owner's actual CMD environment.
These results support environment-sensitive initial COMMAND startup but do
not explain what changed in the owner's earlier CMD or S7 sessions.

With the owner's explicit authorization, a read-only process-memory probe
recorded variable names/lengths and totals, never values. Current desktop
Explorer PID 28636 contained 91 environment entries totaling 12,922 UTF-16
bytes; PATH length was 2,423 characters, PSMODULEPATH length was 295, and 21
`CODEX_`/`BROWSER_USE_`/`NODE_REPL_`-prefixed entries occupied 1,457 UTF-16
characters including entry terminators. Eight still-running older CMD processes
with no identified live parent each had 17 entries totaling 850 UTF-16 bytes,
PATH length 109 and no PSMODULEPATH or those prefixes. Their relationship to
the owner's successful CMD is unproved. The presence of agent-prefixed names
does not establish who started or changed Explorer, and no values were read
out or persisted.

An Explorer-dispatched native Python child then ran two fresh, independently
staged product cases. Both inherited the same PATH and PSMODULEPATH unchanged.
The first kept the complete inherited environment and timed out running
`COMMAND.COM /c ver`; the second removed only the three agent-name prefixes
from its own child environment and exited 0. No user's process or system
environment was changed; both owned test stages were removed. This supports
the additional environment entries as a trigger in this controlled setup,
not a user-approved product filter or proof of correct displayed output. It
does not prove the owner's earlier S7 or existing-CMD environment. The
metadata and result are retained under `build/t481-s9/process-env-lengths2.out`
and `explorer-env-factor.jsonl`; no variable values were saved.

The same authorized length-only inspection found two other Explorer processes
in the session, PIDs 14508 and 8544, started at 09:56 and 09:57 local time on
2026-11-08, before the S7 closure record. Each had 45 environment entries,
6,674 UTF-16 bytes, PATH length 1,507, PSMODULEPATH length 91 and no entries
with the three agent prefixes. The current desktop Explorer PID 28636, started
at 21:36, had 91 entries and the larger lengths above. This establishes that
older and current Explorer processes coexist with materially different
environments. It does not establish which process launched the owner's S7
test, or how the current Explorer acquired its environment. The inspection
recorded no environment values.

An additional isolated test used Windows `CreateEnvironmentBlock` with the
current user token and `inherit=FALSE`, without inspecting or saving its
values and without changing the user's running processes or permanent
environment. Fresh separately staged COMMAND `/c ver` and MEM processes
both exited 0. Supervisor PID 40972 started at 2026-11-09T06:26:33Z; owned
worker scopes and stages were retired. This establishes that the Windows-
generated user environment is a usable startup control on this machine. It
does not prove correct visible output or that every DOS program works, nor
does it alter the underlying original COMMAND environment-size defect.

The installed launch scripts set TEMP and TMP in their local CMD environment
and call the matching PIF through a nested Run16. The native launch request
carries the outer Run16 environment to its CMD child. For the PIF,
`run16-exe/main.c::begin_worker_win16_directory` builds a child copy, replaces
WIN16DIR and removes named internal frontend/stream entries.
`BaseCreateVDMEnvironment` produces the projected worker environment. Run16
passes its Unicode form to `OpenNtBaseClientStartVdmWorker`; NTSRV's
`OpenNtBaseServiceStartVdmWorker` passes that request environment to worker
creation. It does not simply substitute NTSRV's own environment. The original
`cmdGetInitEnvironment` later reads the worker environment, constructs the OEM
guest environment and copies it into the allocation.

In the owner's live fault the previously displayed instruction bytes and the
INIT field both fell inside PSMODULEPATH; only the name and its 295-byte value
length were emitted by the explicitly approved in-memory probe. This does not
identify PSMODULEPATH as the sole cause: block size and layout, not one
specific variable, govern the overlap. Neither the owner observation nor the
agent-only PATH controls establish that the owner's PATH contained `codex` or
changed between S7 and S9.

From S7 baseline 6831b9dd7 through the investigation revision, there is no
source diff under Run16, NTSRV, NTVWM, the COMMAND host service, or the
original COMMAND guest source. Release input manifests identify the same
unstamped input bytes for the other ten host images; NTVDM changed. The host
emulator changes and authored addon remain separate comparisons, not a
reconstruction of the earlier live environment. The owner's S7 process
environment was not captured, so these facts cannot identify what changed in
that process between the two manual runs.

With explicit owner approval, the active desktop Explorer (PID 28636) was
restarted in session 1 using `CreateEnvironmentBlock` for the normal user
token. Its replacement, PID 40828, owned the desktop; its environment had
7,214 UTF-16 bytes, 55 entries, PATH length 1,507, PSMODULEPATH length 91,
and no agent-prefixed entries. Only names and lengths were inspected. No
permanent environment or guest file was changed.

From the replacement Explorer, an isolated `run16 command` launch reached
an NTVDM worker and a console observer read the Windows NT DOS banner. The
separate direct and CMD-parent comparison observed no illegal-instruction
dialog during its 25-second observation windows; this alone is not an exit
or full application test. Owned test stages were removed. The guest's large
environment limitation remains; this desktop repair is not a product fix.

An additional test launched the installed `O:/winnt/system32/run16.exe
command` directly from the replacement desktop Explorer, with no pre-existing
project process. After NTVDM started, a Console observer saw the DOS banner
and submitted `exit`; the launcher exited within 20 seconds and the project
processes were subsequently absent. The process exit code was unavailable to
this observer, so this establishes return, not an exit-code assertion. This
run used the deployed package rather than a fresh-identity test package.
An independent repeat submitted `ver` before `exit`, observed the native
Microsoft Windows version line in the test Console, and saw the launcher exit.
It likewise did not retrieve the launcher's exit code.

After that restart, a desktop-launched production `run16
O:\w31\patch\win386.cmd` reached the `Windows 3.1 386 Enhanced` frontend
after dismissing the previously authorized COM-port warning. A title alone
does not prove desktop readiness. A separate isolated direct-Run16 matrix,
using a normal-user-environment control and the existing startup witnesses,
passed both standard and enhanced mode with DOSONLY=0, including desktop/mode
and normal-exit assertions. Both owned installation copies were retired.
The first matrix attempt used `O:\tmp\t481s9cle` and failed both modes with
the NTVDM `config.nt` unsuitable dialog; the successful retry used the shorter
`O:\tmp\q8c`. This is a test-input difference, not a proven root cause for
the longer-path failure; it does not establish production support for that
path or the owner's complete editor interaction.

The same desktop-launched check of installed `O:/w31/patch/winstd.cmd`
subsequently observed the `Windows 3.1 Standard` frontend without an illegal
instruction dialog. Its title alone does not prove desktop readiness or normal
exit; the separate isolated direct-Run16 matrix supplied those assertions.
