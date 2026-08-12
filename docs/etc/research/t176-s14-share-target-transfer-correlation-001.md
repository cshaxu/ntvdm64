# T176 S14 — SHARE Target Transfer Correlation 001

| Order | Trace/source fact | Meaning |
| --- | --- | --- |
| 1 | S13 accepts `54:01` at `95AB:03C1`, commits 12 writes, and resumes at `03C5`. | The declared target-launch envelope completed. |
| 2 | COMMAND `tcode.asm` calls `SVC_CMDGETNEXTCMD`; `tmisc1.asm` consumes the returned `EXECPATH_SIZE` and `EXECEXT_TYPE`. | `54:01` is the original COMMAND launch boundary. |
| 3 | S13 reaches original `50:36 / SVC_DEMENTRYDOSAPP`; `msproc.asm` issues it immediately before `DOCLI`. | Native execution entered the guest program handoff. |
| 4 | The next INT 21 trace has old `CS:IP=0D10:0106`. `share.asm` has `org 100h`, two-byte `mov ah,4ch`, two-byte `xor al,al`, then `int 21h`; the exact interrupt return IP is `0106`. | `SHARE.EXE` ran its full stub and requested normal termination, exit code zero. |
| 5 | The next unaccepted service is `50:3C`. `dossvc.h` names it `SVC_PDBTERMINATE`; `demdisp.c` maps it to `demTerminatePDB`; `msctrlc.asm` calls it while removing a process before `arena_free_process`. | The first unmet boundary is original DEM PDB termination, not target execution or `54:11`. |

`54:11` remains unobserved because `50:3C` precedes it. `demTerminatePDB`
belongs to the original DEM search/VDD cleanup module (`demsrch.c`), not
Bochs or guest DOS. The next action is a whole-provider dependency audit;
no one-off `50:3C` success result is justified.
