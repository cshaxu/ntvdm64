# Shard 2: Win16 WRITE inputs and image identity

Thirteen entries were selected from the shard-2 pending ledger. The companion observer records frozen/current/original hashes and the historical build references; neither build nor guest execution was performed. Individual files are evaluated below, not inferred from directory membership.

| Entries | Evidence and bounded disposition |
| --- | --- |
| `dtabbtn.bms`, `dtabmark.bms`, `just.bms`, `sp15.bms` | Match both originals. Exact selected resource consumers/configuration were not established; all remain pending. |
| `editsect.c`, `form1.c` | Match both originals. Individual object rules exist, but the observed normal `write.lnk` does not name those exact objects; conditional alternatives must be traced before classification. Both remain pending. |
| `help.c` | Matches both originals; the makefile has a DBCS-conditioned `help.obj` prerequisite, but the supplied `write.lnk` does not itself name that object. Preserve as pending until the actual DBCS link response is resolved. |
| `looks.h` | Matches both originals; no exact selected consumer was established. Remains pending. |
| `kmake.bat` | Matches both originals and is a historical build/setup script. Candidate exclusion for script role only; it was not executed and does not establish the producer of any image. |
| `state.rst` | Matches both originals and contains editor/search/window state pointing to WRITE source files. Candidate exclusion as historical editor state only, not the referenced source. |
| `WRITE.RES` | Matches both originals; the historical normal makefile applies `$(APP).res` to the linked executable. Candidate historical guest build artifact only, not proof these bytes produced the delivered image. |
| `WRITE.EXE`, `write16.exe` | Current source-tree files match each other and OpenNT 4.5; WRITE.EXE also matches OpenNT while write16.exe is absent there. Both are distinct from the ZIP/installed WRITE.EXE; retain both pending as historical guest images, not equivalent to the deployed image. |

The source-tree executables hash to `08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`; ZIP `WINNT/WRITE.EXE` and installed `O:/winnt/WRITE.EXE` match each other at `EB44B5D5F5A405B6A8CD018B9F790BCA2E7E895DAD4F7E1F5462EC14CCDAA87A`. The original makefile describes copying `write.exe` to `write16.exe`, but that description does not establish provenance of either retained file. It selects `d_form1.c` for `format.obj` under DBCS and `format.asm` otherwise; do not substitute the similarly named `form1.c` by assumption. Source review does not prove runtime document, edit, OLE, dialog, resource or lifecycle behavior.

Replay `python artifacts/mvdm-audit/shard-2/win16-write-guest-observer.py` and compare the JSON. No guest, product, installed files, shared ledger or build tree was modified.
