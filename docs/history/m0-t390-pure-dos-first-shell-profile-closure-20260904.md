# M0 T390 — pure-DOS first-shell profile closure

## Closed outcome

T390 closed the prerequisite pure-DOS first-shell profile for the explicit
interactive `COMMAND.COM` product form.

* Original PIF/configuration fields carry the selected `config.nt` /
  `autoexec.nt` pair; the local Base VDM binding now preserves that original
  record field rather than inventing a parser.
* The fixed profile avoids the otherwise reached `AUTOEXEC.NT → DOSX.EXE`
  predecessor and delivers one source-shaped normal child `COMMAND.COM`
  record.
* The original second `COMMAND.COM` reaches its banner, prompt and DOS `CON`
  input wait.  A normal Console `ver` line traverses the source-owned Console
  worker, 8042, ICA, IRQ1, BIOS and DOS route, displays the result, and returns
  to the original prompt.

The final runtime proof is
[S5 P2 original second-COMMAND Console proof](../etc/evidence/m0-t390-s5-original-second-command-console-proof-002.md).

## Corrections retained

* `MVDM-HOST-DIV-221` prevents a stale original PIC acknowledgement rejection
  from becoming fabricated `INT FFh` in the modern host-thread arrangement.
* `ADAPTER-WIN32-032` preserves the original caller-visible Console text-buffer
  lifetime across `doNullRegister()`.

Both retain the reached original contracts; neither changes guest media,
`COMMAND.COM`, a BOP selection, direct guest memory, nor CPU flags.

## Explicitly not claimed

This closure does not prove `exit`, EOF, Ctrl+C, line editing, arbitrary DOS
program execution, DOSX/DPMI, native-child parent return, PIF fullscreen,
graphics, Redirector or Win16.  Those are separate workload/lifecycle
packages in the queue and may reopen only their first proven original owner.
