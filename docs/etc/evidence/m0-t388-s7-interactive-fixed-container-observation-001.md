# M0 T388 S7 — fixed-container interactive COMMAND observation

## Scope

This record preserves the fixed formal CPU40/x86 product observations after
the source-shaped Console and local Base VDM wait-boundary repairs:

- BOP diagnostics are written only to an explicitly supplied host report, not
  to the guest-facing Console.
- The local BaseSrv counterpart now uses one `hWaitForVDM`-shaped event for
  both record publication and a completed re-entry transition.  The earlier
  two-event seam made an ordinary no-command wait observe an initially
  signalled re-entry event.

Neither change modifies guest media or creates an app-owned command parser.
The final row uses only ordinary `KEY_EVENT` records written to the real
Console input handle after a recorded original COMMAND boundary; it does not
write guest RAM, alter a BOP record, or parse a COMMAND buffer.

## Formal product and container

- Product: `build/M0-T386/S3/base-env-x86/original-softpc-process.exe` after
  the formal incremental relink.
- Fixed root: `O:\ntvdm64`.
- Container: `build/tools/console-startup-observer.exe`, owning real
  `CONIN$`/`CONOUT$`, non-debug, eight-second bound.

## Results

| Host environment | `54:0F` required paragraphs | Result | Interpretation |
| --- | ---: | --- | --- |
| inherited modern environment | `024D` | bounded timeout; Console contains no project diagnostics | The original COMMAND path is reached, but its inherited environment remains too large for the reached transient/arena layout. |
| six-variable control environment | `0047` | superseded | The observer invocation omitted `--interactive`; its compatibility default explicitly supplied `-f -o --command EXIT`.  It is valid one-shot evidence only, not an interactive-lifecycle result. |

The control run is diagnostic only: it does not define a product environment
policy or authorize truncating the user environment.  `BaseCreateVDMEnvironment`
is already imported and called unchanged; its original purpose is path
normalization/copying, not an environment-size cap.

## Current inherited-environment interactive row

The current formal product was run once in the same eight-second, non-debug
Console-owning container with the normal inherited environment.  The observer
waited for the first original `54:04` return before queuing `ver` and `exit`
as ordinary Console key events.  It recorded:

```text
result=timeout
scripted-console-input=delivered
scripted-console-input-trigger=54:04-return
scripted-console-input-ready=yes
scripted-console-input-remaining=54
```

The source-owned BOP record reaches the two original `54:0F` calls, but does
not reach `54:01` or `54:0E` in the bounded interval.  The queued events are
therefore not yet consumed.  This rules out the former fixed-delay injection
race as an explanation and does not support a keyboard, PIC EOI, or Console
transport repair.

A second fixed-container observation enabled only the already scrubbed
`MVDM_COMMAND_CONTINUATION_REPORT_PATH` diagnostic.  It measured the original
two-pass contract directly: the first `54:0F` returned `BX=0251` paragraphs
(9,552 bytes), and the second invocation accepted that same buffer.  The
bounded stop then observed `CS:IP=03F4:1F9A`, whose bytes were `63 69 65 73
5C` (environment text), rather than COMMAND instructions.  The evidence is
therefore an overwrite of still-required original transient bytes after the
otherwise successful retry, not a missing input event or a failed BOP return.

The selected `BaseCreateVDMEnvironment` body retains the original copy and
normalization flow in the accepted `opennt-host` subset: it copies the
complete parent block while normalizing `PATH`, `WINDIR`, and `SYSTEMROOT`;
it deliberately does not reduce the environment.  The original
`rdata.asm::EndInit` two-pass allocation then
places its enlarged environment at the first free arena segment before it
moves the COMMAND transient.  On this modern inherited block that first free
range overlaps still-live `EndInit` bytes.  The old `BX=0` fallback avoided the
overlap by silently retaining the initial DOS environment; it remains rejected
for this task because it is not a complete host-environment recovery.

## Current owner conclusions

- The oversized-environment issue belongs to the original COMMAND/NTDOS
  arena-and-environment contract plus the host launch environment.  It is not
  a Console, BOP routing, PIC/EOI, or SoftPC instruction-fetch failure.
- No unchanged original function supplies a later allocation slot for that
  first full environment.  A valid recovery must preserve the whole projected
  block and the original two-pass ABI while changing neither `COMMAND.COM` nor
  guest allocator semantics.  The reached original code leaves no such
  placement seam: a product policy must either supply an explicitly admissible
  session environment before `BaseCreateVDMEnvironment`, or authorize a new
  source-derived placement extension.  This is an unresolved source-design
  admission, not a reason to reinstate the rejected fallback.
- S7 is **not closed**.  No prompt, `ver`, `dir`, editing, Ctrl+C, or `exit`
  row is claimed by this evidence.

## Corrected no-argument interactive observations

The observer has two deliberate launch forms.  With no extra argument it
supplies the historical one-shot test command `-f -o --command EXIT`; only
the explicit `--interactive` form launches the product with `-f` and no
declared command.  The former was mistakenly used by the earlier control
row, so it cannot be used to infer an interactive exit.

The formal CPU40/x86 product was relinked with the source-audited first DOS
record unchanged: `AppName=...\\COMMAND.COM`, `CmdLine=CR/LF/NUL`, and no
terminal-on-command-exhaustion disposition.  Two corrected fixed-container
observations are retained:

- `m0-t388-s7-console-r44-interactive-original-record.txt` used no injected
  keys.  It remained live through the ten-second bound; its captured
  BaseVDM record shows the first original request consumed and no product
  exit.  This replaces the invalid clean-environment early-exit conclusion.
- `m0-t388-s7-console-r45-interactive-original-record-script.txt` queued only
  ordinary `CONIN$` key events after the existing `54:04` return.  The
  observer reports `scripted-console-input=delivered` and
  `scripted-console-input-remaining=0`; the child-only trace records the
  original SoftPC scan-code offer, keyboard controller handling and IRQ1
  service.  This proves the Console-to-SoftPC keyboard route consumes the
  keys without an app prompt parser, a BOP record, or a guest-memory write.

The console snapshot remains blank and neither run yet proves a visible
prompt or execution of `ver`/`exit`.  The remaining S7 problem is therefore
the original COMMAND startup/display progression after the verified keyboard
delivery, not a spontaneous termination, missing Console input pump, or a
reason to change `COMMAND.COM`.

## Corrected observation: Base VDM wait is not a Console-line producer

The preceding diagnosis is superseded for the current formal product. After
the source-owned event-map atomicity repair and a formal incremental relink,
the observer trigger was moved from the too-early `54:0E` console-initializer
return to the original SoftPC `keyboard_io()` `AH=2` / `IDLE_waitio()` edge.
That marker is default-off, sits immediately before the existing original idle
call, and cannot create input, alter guest RAM, or alter a BOP result.

Two fixed-root, non-debug, minimal-environment rows used the same staged
`O:\ntvdm64` media and the same formal x86 product:

- `m0-t388-s7-console-r80-keyboard-waitio-ver-only.txt`: bounded ten-second
  row. It reached original COMMAND `54:01`, `54:0E`, and `54:04`, but never
  reached the `keyboard_io(AH=2)` trigger; no Console keys were queued.
- `m0-t388-s7-console-r81-keyboard-waitio-ver-only-30s.txt`: the permitted
  thirty-second bounded row. It reproduced the same condition. Its companion
  `*.bop-return.txt` records repeated original timer IRQ0 service through the
  point where COMMAND's `54:01` request has returned; it never records the
  original keyboard-wait BOP.

This is not a keyboard/PIC defect, nor does it prove that a BaseSrv-side
Console-line producer is required. Original
`dos/command/cmdmisc.c::cmdGetNextCmd` makes `GetNextVDMCommand` its next
source-shaped request, and the imported
`base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand` confirms the historical
separation: it manages launch/re-entry DOS records and wait handles but never
reads interactive Console input.

The original guest source distinguishes two cases that must not be conflated.
The normal `COMMAND.COM` `do_again` loop has its direct DOSKey/buffered-console
read path behind `if 0`; the enabled source path always calls
`CMDSVC SVC_CMDGETNEXTCMD`. Thus `Do16BitPrompt` is a real original DOS-CON
path, but only for the shell-out/TSR/re-entry cases selected through
`SCS_CMDPROMPT` and `SCS_PROMPT16`; it is not evidence that a first no-argument
`COMMAND.COM` launch normally reads a prompt directly.

Ordinary interactive DOS applications such as `EDIT.COM` still use the
separate original SoftPC Console event-thread -> 8042 -> IRQ1 -> BIOS/DOS CON
route once they have been launched. A generic Console-line-to-`VDMINFO`
producer would steal those application keys and remains rejected. The normal
initial shell, however, is historically coordinated outside that guest loop:
an original Base/Console command producer creates `VDM_TO_TAKE_A_COMMAND`
records, while BaseSrv only brokers those records and wait handles.

The immediate unresolved owner is therefore the source-shaped initial shell
composition, not the keyboard controller: determine the original external
producer/record lifecycle used for a direct shell and how this product should
provide an explicit direct-`ntvdm.exe` UX without pretending it is BaseSrv or
changing guest media. S7 records this as a source audit finding only. No
prompt parser, synthetic BOP record, guest-memory write, or keyboard repair is
admitted by this evidence; no prompt or `ver` success is claimed.

The selected OpenNT source trees confirm the boundary. `BaseClient`'s
`base/win32/client/process.c` calls `BaseCheckVDM` while processing a normal
`CreateProcess` request; `base/win32/client/vdm.c` packages that request for
`BaseSrvCheckVDM`; and `srvvdm.c` stores/delivers the resulting DOS record.
There is no independently importable original interactive `cmd.exe` package in
the selected OpenNT source sets that can be composed as this product's direct
shell. It is an external historical client of the BaseClient process API, not
a missing MVDM or BaseSrv source file. Consequently any direct executable
shell is an explicit app/session composition extension, with the original
BaseCheckVDM/VDMINFO record shape as its only admissible historical contract.

## T388 closure disposition

T388/S7 is closed as a **source-owned correction of its original matrix
premise**, not as an interactive-shell success. The fixed rows remain valid
evidence that Console diagnostics, real Console key delivery, SoftPC keyboard
handling, IRQ1 and timer service are independently observable. They do not
prove a first permanent `COMMAND.COM` prompt, `ver`, `dir`, line editing,
Ctrl+C, `exit`, or `EDIT.COM` execution.

The retained original source establishes why those omitted rows are not
further attempts within this packet: the enabled first-shell `do_again` path
uses `CMDSVC SVC_CMDGETNEXTCMD` / `54:01`; its direct DOSKey input branch is
compiled out. The original guest DOS-CON prompt route instead belongs to a
declared second, non-first `COMMAND.COM`: `SCS_FIRSTCOM == 0` enters
`DoReEnter`, and the selected `SCS_CMDPROMPT` disposition can reach
`Do16BitPrompt -> PRINT_PROMPT -> INT 21h/AH=0Ah`.

Accordingly the old S7 rows are closed as **not applicable to the first-shell
no-argument premise**, with no guest-media change, synthetic BOP record,
host prompt reader, or direct guest-input injection. The separate candidate
[`proposal-explicit-interactive-command-profile-001.md`](../operations/proposal-explicit-interactive-command-profile-001.md)
owns the explicit `ntvdm.exe command.com` second-shell profile, its
copyright/banner and DOS-CON proof, child/parent return, PIF/presentation
disposition and fixed-container interactive workload matrix.
