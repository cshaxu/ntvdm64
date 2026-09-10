# T236 S2 Nonclosure Transfer Ledger

## Question

Which remaining parts of the COMMAND `54:08`/`54:0A`/`54:0B` and `54:01`
child, event, console and re-entry lifecycle cannot close inside the active
M0 T236 S2 packet, and which declared owner package must take each part?

## Inputs

- Active packet: `docs/STATUS.md`, M0 T236 S2.
- T236 boundary and S sequence:
  `docs/etc/operations/m0-t236-dem-command-minimum-source-recovery-plan.md`.
- Current source/recovery record:
  `docs/etc/evidence/t236-s2-command-worker-host-broker-recovery-001.md`.
- Candidate owner proposals for Redirector, WOW16 and machine/BIOS completion.
- Current COMMAND sources:
  `src/bx-vdm/bop/opennt/command/cmdexec.c`, `cmdmisc.c`, and `cmdredir.c`,
  with `src/bx-vdm/bop/shim/command_misc_shim.{c,h}`.

## Procedure

Read the active packet's objective, non-goals and exit criteria; compare its
declared composition boundary with the remaining imported OpenNT call sites;
then cross-reference each excluded owner with the authoritative candidate
queue and proposal boundaries.  This is a read-only audit.  It makes no claim
that an unqueued item is impossible, only that it is not admitted to T236 S2.

## Observations

T236 S2 admits one session-owned, fixed-width `PENDING` continuation around
the imported `cmdCreateProcess` worker and imported `cmdExec32` control flow.
It explicitly excludes CCPU, BaseSrv/CSR/CSRSS private protocols, multiple
sessions, Redirector remote/named-pipe protocol, WOW composition and machine
semantics.  Its exit criteria still include its own worker lifecycle,
completion/cancellation/double-completion, controlled resume and native
`54:08` witness.  Those are current-S work, not transfers.

| Remaining capability | Why it cannot close in T236 S2 | Disposition / receiving owner |
| --- | --- | --- |
| Redirector remote and named-pipe protocol, including remote `54:06`, `50:47/48` and `57:xx` completion | It requires the original `VDMREDIR`/`VrDispatch` family and IPC/network lifecycle. T236 permits only local anonymous-pipe endpoints and must not create a second child broker. | Queued **OpenNT Redirector owner-package completion**. Its proposal expressly owns the future extension of session redirection state using opaque IDs only. |
| `VDMForWOW`, `GetWowKernelCmdLine`, `GetWOWShortCutInfo`, WOWEXEC/WOW32 broker and NE lifecycle | T236 explicitly excludes WOW composition; the local Direct child record is not WOW support. | Queued **OpenNT WOW16 owner-package completion**. Its proposal requires a distinct source map and bounded WOW guest/host profile. |
| Floppy/FDC/DMA/CMOS/timer and `host_simulate` paths reached from DEM DASD/IOCTL | These are SoftPC/Bochs machine mechanics, not COMMAND or bx-vdm service policy. | Queued **bx machine, BIOS and selector owner-package completion**. |
| DOS `EXEC` child setup, PSP/arena/JFN/environment, guest child run, parent restoration and ordinary `54:0B` parent return | This is guest DOS/NTDOS ownership and full DOS continuity, beyond the host-child worker's local completion record. The current terminal/register result is not evidence of an ordinary guest child return. | **No queued candidate currently names this complete owner package.** It needs a new owner-approved NTDOS/COMMAND guest-EXEC-parent-return proposal before work is admitted. |
| CSRSS/BaseSrv private broker, external command producer and multiple VDM sessions | The required NT4 private protocol is unavailable; reproducing it would violate T236's single-session and no-private-protocol stop conditions. | **Explicitly unavailable, with no current proposal.** A future package would require a separately approved public replacement contract; it must not be introduced as an ad hoc local queue. |
| Actual console-input injection behind `cmdPushExitInConsoleBuffer` and historical event-thread behavior | T236 records session-visible notification and event-block state only. It does not compose keyboard/console input injection or fabricate CSRSS's event thread. | **Not expressly assigned by a current proposal.** It is a possible console/input/VDD-top-level owner decision, but the queued debugger/VDD/top-level proposal must explicitly admit it before implementation. |

## Current-S Work That Must Not Be Misreported As A Transfer

The following were executable closure work inside T236 S2 and are now
verified by its focused formal fixture set:

- verify the detached worker, `PENDING` continuation, completion, cancellation
  and double-completion paths;
- verify re-entry counting and the original call ordering that the local
  `GetNextVDMCommand` seam can support in one session;
- verify local child stream isolation and failure handling; and
- execute the affected formal Ninja targets rather than only dry-run them.

The last item is verification infrastructure, not a COMMAND, Redirector, WOW
or machine capability transfer. It now has an actual formal execution pass.

## Interpretation And Confidence

Three external owner transfers are already explicit and correctly queued:
Redirector, WOW16 and machine/BIOS.  Two significant items have no approved
future task yet: complete DOS guest `EXEC`/parent return, and console-input
injection.  BaseSrv/CSR multi-session brokering is an intentional unavailable
boundary rather than an implied backlog item. T236 S2's own single-session
worker evidence is complete; doing any transferred work inside it would change
the active packet boundary and require re-admission.

## Follow-up

When a product profile requires guest DOS child continuity, admit a dedicated
NTDOS/COMMAND guest-EXEC-parent-return owner package.  If console injection
is required, first select and amend the appropriate console/input/VDD owner
proposal with its original-source and host-boundary audit.
