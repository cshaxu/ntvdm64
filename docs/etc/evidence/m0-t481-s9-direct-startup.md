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
