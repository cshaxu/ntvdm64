# H033 Win16 WRITE startup

Runtime53 with its initial guest set exited with 0x42b. The worker registered the four WRITE classes and entered WM_CREATE. Its WU32GetMenu returned a nonzero menu, and the trace advanced to device queries and module loading before module cleanup reported error 1412. The original WRITE.EXE was unchanged in all runs.

Each comparison used a fresh physical stage of runtime53 and the same private-desktop observer. The table lists replacements relative to that stage, all from the selected XP Professional SP3 x86 media.

| Additional SYSTEM32 files | Result |
| --- | --- |
| WINSPOOL.EXE | No visible WRITE main window |
| SHELL.DLL and WIN87EM.DLL | Exit 0x42b |
| WINSPOOL.EXE and WIN87EM.DLL | No visible WRITE main window |
| WINSPOOL.EXE and SHELL.DLL | Visible MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO |

The paired run shows the title `Write - (Untitled)` in successive samples. This verifies startup only. Its observer timeout is not a normal application exit. Evidence is in `build/t485-s4-h033/guest46/write-swshell54.log`; controls use `write-spool54.log`, `write-math54.log` and `write-sw8754.log`. The dispatch trace is `write53-dispatch54.txt`.

| Guest file | Media SHA256 |
| --- | --- |
| SHELL.DLL | 988E406AD8DFA8EBAFAE9FF86C9631CEF97045892813FAD570F85F9BBEB44F71 |
| WINSPOOL.EXE | B54AF935980E4FC146DEABDC34CFE62AEB931D4571F35F9DCFCF4EA3D332973B |
