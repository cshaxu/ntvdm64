# T420 S34 COMMAND stream recovery

## Question

Can the selected original DOS COMMAND unit retain ownership of its own
redirection grammar while the three-program product transfers inherited host
streams separately from Console identity, including BOP `54:08` re-entry at
second and third COMMAND depth?

## Inputs

| Input | SHA-256 / role |
| --- | --- |
| `src/mvdm/dos/command/cmdexec.c` | `fe475cafa8f8b667ad93722e401e84a511e9279ad2b21c8eb9b60bb0a1dfc849`; selected original COMMAND owner of `cmdExec32` and `cmdCreateProcess`. |
| `src/ntvdm-exe/win32/command_process_compat.c` | `d8a058f42f909a2bd61c13c2a7c1aa035842d3a5e89150b8b8fa93391c2246f7`; finite standalone replacement for the unavailable system VDM spawn. |
| `src/run16-exe/main.c` | `6fa762c670b2b899586c39ed673df67e5df4a705b1dde89211639a4faa8269bd`; parent-side inherited-stream capture only. |
| `src/basesrv-exe/opennt/source/base_service.c` | `a2e859cb2473a4ba94f39ddd79df680a8b7efd13c9d2e2eeda248ef492da044e`; original BaseSrv record policy plus typed attachment delivery. |
| `S34N.EXE`, `S34E.EXE`, ConPTY observers | Test-only x86 fixtures built below `build/M0-T420/S34/` and copied only to `O:\winnt\tests`. |

`cmdexec.c::cmdExec32` selects `COMSPEC /c` before its asynchronous original
`cmdCreateProcess` route.  The product cannot invoke the historical system
VDM creator.  The adapter therefore delegates a **composite** already-selected
tail to `run16.exe COMMAND.COM /c` as one Windows argv item.  It does not
interpret DOS quotes, `<`, `>`, `>>`, `|`, or DOS handles.  Simple non-image
tails keep their prior `run16 <tail>` route so native host built-ins such as
`ver` retain their existing behavior.

## Procedure and observations

1. Rebuilt `run16.exe`, `basesrv.exe` and `ntvdm.exe` in the formal MSVC x86
   graph `build/M0-T420/S31/formal-x86-r1`.  The link and
   `Verify-VdmTibStorage.mjs` passed.
2. Deployed only the three package executables to `O:\winnt`.
3. Ran `Verify-CommandExitStatus.ps1` with the current console observer and
   guest `G7.COM`.  All 17 established routes passed and every textual
   marker was checked from captured guest Console output, not merely launcher
   exit codes.  This includes direct/interactive COMMAND, MEM, nested
   COMMAND, native streams/EOF, guest exit seven and EDIT.
4. Ran direct real-guest stream cases using a ConPTY host that keeps Console
   identity while COMMAND receives typed streams:

   | Case | Observed payload |
   | --- | --- |
   | `S34N.EXE \| S34E.EXE > P4.TXT` | `S34_NATIVE_CHILD_STDIN=S34_NATIVE_CHILD_STDOUT_OK` |
   | `S34E.EXE < IN5.TXT > I5.TXT` | `S34_NATIVE_CHILD_STDIN=PIPE-IN-FINAL` |
   | `S34N.EXE > A5.TXT`, then `>> A5.TXT` | exactly two `S34_NATIVE_CHILD_STDOUT_OK` lines |
   | host `cmd.exe` pipe: `run16 COMMAND.COM /c VER \| findstr /c:"MS-DOS"` | `MS-DOS Version 5.00.500` |
5. Ran an interactive resident COMMAND with `command /c S34N.EXE | S34E.EXE
   > P5.TXT`, then the same command nested one further level into `P6.TXT`.
   Both files contain exactly
   `S34_NATIVE_CHILD_STDIN=S34_NATIVE_CHILD_STDOUT_OK`.

Logs are under `O:\winnt\logs\s34-*-final*`; disposable runtime fixtures and
outputs are under `O:\winnt\tests`.

## Interpretation

The original COMMAND/DOS code owns every guest redirection and pipe decision.
The former failure was at the standalone BOP spawn
seam: the raw composite tail was passed to `run16` as if its left token were a
program image.  Re-entering `COMMAND.COM /c` preserves the original parser and
works through the third tested depth.

A `COMMAND /c` run launched from a still-open host Console can leave the
original console-associated worker resident after the launcher returns.  This
is not treated as a pipe-handle leak or replaced with a timer/reaper: the
owner explicitly requires original worker lifecycle semantics.  Test harnesses
end only the verified `O:\winnt` processes between isolated rows.

## Follow-up

The current tested first-worker route still has an architecture blocker: its
pre-Connect BaseSrv branch creates a worker-local standard handle and the
reply preserves that numeric local value.  An attempted replacement that kept
the receipt until `GetNextVDMCommand` and returned a system-handle attachment
compiled but hung the real guest pipe; it was reverted and never committed or
left deployed.  This evidence therefore proves the COMMAND grammar repair,
but **does not close S34** under the no-raw-handle broker rule.  The next S34
step is to trace and repair the attachment delivery/lifetime without a second
parser or a timer/reaper.  The separate VDMREDIR package owns network
redirector operations, not COMMAND grammar.

## Typed first-worker repair (active, not yet accepted)

The current candidate removes the direct-to-suspended-worker branch. That
branch bypassed the reservation delivery receipt and left the worker-local
numeric result of `BaseSrvDupStandardHandles` in the copied Get reply. The
candidate instead keeps the retained original `BaseSrvDupStandardHandles`
ordering: its source receipt is copied to a reservation receipt during Update;
the source carrier is released; and Get resolves the reservation receipt only
for the typed RPC stream output. The fixed-size Get scalar remains the
existing three-bit presence mask, never a handle value. The worker marks each
received typed standard stream inheritable before original COMMAND can create
a child.

On 2026-09-19 the four affected x86 translation units (`base_service.c`,
`base_values.c`, `base_reservation.c`, and `base_rpc_client.c`) compiled with
the exact scoped flags extracted from the existing formal x86 graph. The
existing graph's Ninja lock was stale, and a fresh S34 graph could not yet be
generated because its required Node 22 executable is not visible to the
non-interactive environment. This is compile evidence only: no product package
was deployed and no guest pipe claim is made until an x86 link and real
first-worker pipe transcript pass.

On 2026-09-19, Git-Bash Node 22.22.1 generated a fresh formal graph at
`build/M0-T420/S34/formal-x86-r1`; it linked `run16.exe`, `basesrv.exe`,
`ntvdm.exe`, and `VDMREDIR.dll`. The rebuilt service lifecycle fixture passes
with receipt-backed stdin/stdout: the first worker receives all three stream
attachments, writes `S34\n` through stdout, and the retained pipe reader gets
the exact bytes. The temporarily published package also passes host-redirected
`COMMAND.COM /c ver` and direct native-child stdout. A fresh ConPTY worker,
however, times out for `COMMAND.COM /c "ver > file"` and for the guest pipe
row. Thus this is compile and bounded service evidence only; S34 stays open
until the first-worker Console/guest-redirection lifetime defect is repaired.

## 2026-09-19 filename and matrix reconciliation

The prior transcript used long DOS output basenames such as
`S34DIRECTPIPE.OUT`.  DOS correctly created the 8.3 spelling
`S34DIREC.OUT`; treating the nonexistent long spelling as the result file
produced a false zero-byte failure.  The corresponding nested result is
`S34NESTE.OUT`.  Both files contain:

```
S34_NATIVE_CHILD_STDIN=S34_NATIVE_CHILD_STDOUT_OK
```

The tracked `tests/observation/s34_host_pipe_matrix.cmd` now uses only
`G1.OUT`, `G2.OUT` and `G3.OUT` as guest-owned output names and fails on the
first nonzero row.  On the current published x86 package it proved:

| Row | Result |
| --- | --- |
| Direct native child to a host stdout pipe | marker observed; exit 0 |
| Host stdin pipe to native child | `HOST-PIPE-IN` marker observed; exit 0 |
| Guest `S34N.EXE | S34E.EXE > G1.OUT` | payload, exit 0 |
| Same at second nested COMMAND depth to `G2.OUT` | payload, exit 0 |
| Same at third nested COMMAND depth to `G3.OUT` | payload, exit 0 |

The combination in which a guest pipe's final command writes to an **outer**
host redirected file/pipe is still a separate negative observation: `H1.OUT`
remains empty while the explicit final guest file is correct.  It is retained
as a native-child/redirection interaction to repair or source-prove before
S34 closure; it is not used to claim a passing pipe row.

## Rejected capture-queue experiment

The standalone `mvdm_command_native_child` adapter was briefly changed from
one session capture slot to a FIFO of copied command/environment/standard
triples.  It compiled and linked in the formal x86 graph, but a clean worker
still passed the first matrix and failed the second matrix's third-depth row
with `Cannot execute O:\WINNT\COMMAND.COM`.  Its trace again showed only the
first native create at that row.  The experiment therefore did not explain or
repair the failure and was restored before the product artifact was retained.
No capture-queue code remains in the worktree or deployed `ntvdm.exe`.

## 2026-09-19 final closure

The previously recorded host-final-sink observation was re-run after the
receipt-owner repair and proved non-reproducible. Its earlier transcript mixed
a stale deployed executable with a test matrix whose guest long result names
were silently shortened by DOS. It is therefore not a retained product defect.

The final source identities are:

| Source / boundary | Git object |
| --- | --- |
| Original `cmdexec.c` mirror | `9b4fc990ea776ff3d3e9e1d5061753de3964ffa2` (byte-identical) |
| worker system-VDM spawn seam | `f4b43e6f15e7a6d9719b4d813caba90af8a6246d` |
| launcher inherited-stream capture | `d5cf2acb8387f797599db7b722771e04fa690cf0` |
| BaseSrv typed receipt delivery | `d9564e87a78008fcaadfd6332ca9e82139b93300` |

`build/M0-T420/S34/formal-x86-r1` completed the formal MSVC x86 link for
`run16.exe`, `basesrv.exe`, `ntvdm.exe` and `VDMREDIR.dll`. The matching
published `O:\winnt` files were used to rebuild `S34N.EXE` and `S34E.EXE`.

`tests/observation/s34_host_pipe_matrix.cmd` then completed three fresh runs
with error level zero for every row. It proves host stdout pipe, host file
redirection and host stdin pipe separately from guest parsing; it proves guest
`>`, `>>`, `<` and `|`; and it proves the exact pipe marker in `G1.OUT`,
`G2.OUT` and `G3.OUT` at first, second and third `COMMAND.COM` depth.

Finally, `Verify-CommandExitStatus.ps1` completed all 17 established,
text-gated product routes against the same publication. The result includes
direct and resident COMMAND, native stdout/stderr/EOF, direct and three-depth
nested MEM, direct `COMMAND.COM /c`, guest and native exit paths, and the
EDIT return route. It checks the captured guest Console witness rather than
equating a launcher exit code with a guest pass.

The original COMMAND source owns guest syntax and DOS stream mutation. The
three finite standalone seams only transport inherited stream attachments or
replace the unavailable historical system VDM creation call; none parse DOS
redirection grammar. The broker record carries typed receipts, never raw
process handles, guest pointers or Console pseudo-handles. S34 is closed.
