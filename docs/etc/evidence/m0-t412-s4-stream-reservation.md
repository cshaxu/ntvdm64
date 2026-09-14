# T412 S4 stream reservation binding

## Question

Can the standalone broker preserve the original `BaseSrvUpdateVDMEntry`
standard-stream copy, although that copy happens before the new worker has
connected to BaseSrv?

## Inputs

- Original `BaseSrvDupStandardHandles` / `BaseSrvCloseStandardHandles` in
  `srvvdm.c` retain the source order and stdout/stderr alias rule.
- Original BaseClient `BaseCheckVDM` supplies the three standard handles;
  the standalone copied Check record previously omitted them.
- The S4 three-program product and guest `COMMAND.COM` from `O:\ntvdm64`.

## Procedure

1. Preserve the original HANDLE-shaped fields only as broker-local receipt
   identifiers in the copied Check, Update and Get records.
2. Retain the typed file/pipe object in the authenticated launch reservation
   during `UpdateVDMEntry`; the reservation is released with its worker and
   drains retained objects.
3. On authenticated worker `GetNextVDMCommand`, resolve those identifiers and
   deliver the three objects through MIDL typed-handle output parameters.
4. Rebuild all three x86 products and run `COMMAND.COM /c run16.exe
   --internal-console-probe` with redirected parent standard streams.

## Observations

The first attempt deferred delivery until worker Get and failed at original
Update with `ERROR_INVALID_PARAMETER` (87).  A reservation-only first-command
variant then regressed the initial worker Get.  The currently executable path
therefore retains the original Update-time duplicate into the already
registered suspended worker.  The Get client accepts those already-local
handles without requiring a second RPC attachment.  A fresh real probe
returned 0 from the parent and recorded `Check`, `reserve`, `prepare`, worker
`Get`, reentry increment/decrement and parent exit-code with zero broker
status.  Its guest native-child trace recorded the copied payload,
standard-handle fields, guest capture locations and child exit value 5.
Logs are retained at `O:\ntvdm64\logs\m0-t412-s4-probe-worker-local`.

A second run using `COMMAND.COM /c run16.exe MEM.EXE` initially returned 31.
The cause was the typed Check attachment wrapper treating a `GetFileType`
unknown result with stale last-error state as an attachable resource.  It now
clears last error before the query and treats an unknown type with a set error
as absent.  The trace-free rerun reached a second original BaseSrv
Check/reserve/prepare/worker-Get sequence and the native child returned 0.
The redirected output files remain empty, so this does not prove
byte-producing guest redirection.

## First interactive COMMAND record repair

The three-program first-shell control initially reached original
`cmdGetNextCmd` but repeatedly reopened `COMMAND.COM` before the DOS Console
wait.  This was not a guest-media, PIF-parser, SoftPC, or CCPU failure: the
same current x86 graph reached the banner, prompt and `ver` input through the
monolithic control.  A default-off shape witness at the existing original
`GetNextVDMCommand` call showed the decisive difference: the three-program
reply exposed `std=111`, while the monolithic record exposed `std=000`.

The Check request itself had no standard-stream fields (`present=0`).  The
loss occurred later because `broker_vdm_get_values` omitted the three result
fields from the Get reply.  The worker therefore retained pre-RPC stack
residue in `BASE_GET_NEXT_VDM_COMMAND_MSG.StdIn/StdOut/StdErr`, which original
`cmdCheckStandardHandles` correctly interpreted as redirection.

The repaired scalar wire carries only a three-bit standard-stream presence
mask, never a native HANDLE.  Typed attachments still replace present streams
when the server returns them; a no-attachment reply now explicitly clears an
absent stream or selects the worker's already-inherited counterpart.  The
adapter also recognizes both the retained NT4 Console pseudo-handle signature
and a modern Console object before attempting a stream attachment.  It leaves
file and pipe behavior untouched.

After the full three-program rebuild, fixed-console observation
`O:\\ntvdm64\\logs\\m0-t412-s4-stream-presence-reply-r21` recorded the
original first-command record as `std=000`, reached the original DOS
buffered-console-input marker, and captured the DOS banner and
`O:\\NTVDM64>` prompt.  A follow-up input run at
`O:\\ntvdm64\\logs\\m0-t412-s4-stream-presence-reply-r22` echoed `ver` and
returned to the prompt.  Both runs used a temporary exact copy of the
immutable pure-DOS profile PIF as `O:\\ntvdm64\\COMMAND.PIF`, removed in
`finally`; neither changes guest media nor proves `exit`/worker-return,
byte-producing redirection, or general PIF compatibility.

The direct CLI regression initially exposed a second boundary error: after
the original Update-time duplication had made the three standard handles local
to the suspended worker, the broker's subsequent Get tried to resolve their
numeric values as reservation receipts and returned `ERROR_INVALID_HANDLE`.
The reservation now records only the finite fact that a value was made local
by that same Update duplication. Get leaves those values untouched and sends
no second typed attachment. The x86 reservation test covers absent, marked and
duplicate local values. An isolated x86 relink staged `basesrv.exe` and
`ntvdm.exe`; `run16.exe` was unchanged. The live
`O:\ntvdm64\run16.exe O:\ntvdm64\MEM.EXE` direct-redirection observation
then recorded three successful Get calls, parent exit-code and disconnect at
`O:\ntvdm64\logs\m0-t412-s4-direct-mem-r2`. `MEM` writes its display through
the DOS Console path, so its empty redirected files are expected and do not
claim byte-producing output acceptance.

## Direct versus guest redirection

The staged guest case was separately run as `COMMAND.COM /c MEM.EXE >
O:\ntvdm64\logs\m0-t412-s4-command-flow-r1\guest.out`. COMMAND consumed
and returned from its initial command (the original continuation trace records
the service sequence), but no guest output file was created. That is the
current Redirector/guest-redirection unavailable boundary, not evidence that
the direct CLI file handles failed to transfer. The direct and guest cases are
therefore explicitly distinguished.

The third comparison passed literal `>` as an argument to `run16.exe MEM.EXE
> literal-target.txt`, without a host shell. It reached normal
Check/reserve/prepare/Get and exit at
`O:\ntvdm64\logs\m0-t412-s4-literal-redir-r1`; no host output target was
created. Thus run16 did not invent shell parsing for a literal operator.

## Interactive task-return observation

The preceding `r22` Console script treated each public Console `KEY_EVENT`
pair as if the original SoftPC worker promised a matching Scan-1 break byte
before the next pair. That is not an OpenNT contract. `KeyMsgToKeyCode` first
maps the Console Scan-1 value to its internal key number (Enter is
`1Ch -> 2Bh`), and the original BIOS/device path may defer an Enter release
when its typeahead buffer is full. The observation harness was corrected to
queue its small ordinary `ver\r`/`exit\r` sequence at the already-observed
original DOS line-input boundary, with normal human inter-key cadence. It does
not write guest memory, synthesize a BOP, or duplicate a keyboard map. Its
acceptance boundaries are source-owned command output and final process exit.

Fresh fixed-container run
`O:\ntvdm64\logs\m0-t412-s4-console-exit-r27` used the rebuilt three x86
programs and a temporary byte-identical `O:\ntvdm64\COMMAND.PIF` sidecar
removed in `finally`. It recorded `scripted-console-input=delivered`,
`scripted-console-input-remaining=0`, and `run16.exe` exit `0`. The captured
Console shows the original banner/prompt, `ver`, a returned prompt, and
`exit`; the child trace records corresponding stream output and retained
original `54:08` dispatch. Thus the first interactive task now returns
through the three-program topology.

The run intentionally leaves `basesrv.exe` and `ntvdm.exe` resident after
the launcher returns. Their exact PIDs were recorded and explicitly cleaned
up after observation. This is not accepted as cleanup behavior: idle worker
retirement and empty-broker exit remain T412 S5 work. The PIF remains an
immutable test input in S4; moving or deleting guest media is outside this
packet.

## Direct MEM and EDIT boundary observations

Fresh direct positional run `run16.exe MEM.EXE`, recorded at
`O:\ntvdm64\logs\m0-t412-s4-direct-mem-r3`, deliberately had no same-name
PIF sidecar. It exited `0`, opened the package-root `MEM.EXE`, and displayed
its memory report in the fixed Console. Its original default configuration
also opened `DOSX.EXE`; this is a valid direct-CLI configuration branch, not
the pure-DOS first-shell profile used by the separate interactive COMMAND
test. The retained broker and worker PIDs were explicitly cleaned after the
observation.

`EDIT.COM` is the retained 413-byte QBASIC launcher. Three direct three-program
observations prove `EDIT.COM` and package-root `QBASIC.EXE` open successfully:
the inherited-environment run is
`m0-t412-s4-direct-edit-r1`, the package-root/system-directory PATH control is
`m0-t412-s4-direct-edit-minimal-env-r2`, and the temporary same-name PIF A/B
is `m0-t412-s4-direct-edit-pif-r3`. The minimal PATH reduces `QBASIC.INI`
searches to the expected package and system locations, but neither it nor the
PIF changes the fixed non-visible Console outcome: QBASIC remains alive past
the bounded observation without a text Console frame or mouse-input mode.

This is not a missing-QBASIC or PIF-selection diagnosis, nor an EDIT failure
claim. The prior selected presentation evidence already establishes that this
observer cannot replace visible EDIT mouse acceptance. It is an honest
three-program startup boundary: EDIT/QBASIC execution is reached; its visible
editor, keyboard and mouse behavior still require the dedicated real-Console
acceptance path specified by S4.

## Interpretation

Confidence is high that the original Update-before-Connect ordering and
worker-local standard-handle consumption execute, and that the first
interactive COMMAND task returns to the launcher. The receipt-only form is
not accepted: it remains an open design issue rather than a completed
transport claim. Nested DOS execution, byte-producing stdout/stderr, EOF
and guest redirection remain required acceptance cases.

## Follow-up

Design a receipt-only first-command transfer that preserves the same ordering,
then exercise a byte-producing COMMAND redirection case before closing S4.
