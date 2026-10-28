# Part 1 MVDM: DOS command and device family

This covers 46 assigned DOS command/device paths outside the separately audited GRAPHICS and keyboard families. Each source hash, three reference hashes, literal original makefile mentions, four normalized VALID host-dependency results, ZIP entry hashes and exact system32 installed hash is captured in `dos-utilities-evidence.json`; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/dos-utilities-observer.py`. All 46 exact source paths have zero VALID dependencies in the scanned host graphs; that says nothing about 16-bit guest build selection. The third comparison edition and full paths are in JSON when a primary path is missing. No guest execution or source build was performed.

Original recipes distinguish the COMMAND/KB16 guest images, `.exc`/`.exs` and HIMEM.BIN intermediates, skeleton/localization sources, and notes. Original `make.inc` has `.exc.com` and `.exs.sys` conversions; HIMEM additionally uses stripdd. Recipe text is evidence of intent, not proof of any binary's provenance. Existing `shard-1/dos-utility-family-review.md`, `dos-locale-family-review.md` and `guest-debug-variant-review.md` contain narrower prior traces; consult actual file names in evidence before merging. In particular ZIP QBASIC.EXE differs from this original source-tree binary, and installed SETVER.EXE differs from the retained original. Do not silently identify variants, modify them, or claim runtime behavior from their names.

| Assigned path | Original role and source identity | Bounded packaging observations |
| --- | --- | --- |
| `dos/v86/cmd/append/append.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/backup/backup.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/command/COMMAND.COM` | DOS image/utility; both primary match. | ZIP source-equal; system32 source-equal. |
| `dos/v86/cmd/command/command.exc` | historical linker intermediate; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/command/help.skl` | historical message skeleton; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/command/readme.txt` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/debug/DEBUG.EXE` | DOS image/utility; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/debug/debug.tag` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/debug/dosdebug/debug.arf` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/edlin/EDLIN.EXE` | DOS image/utility; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/exe2bin/EXE2BIN.EXE` | DOS image/utility; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/exe2bin/exe2bin.skl` | historical message skeleton; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/exe2bin/exe2bin.tag` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/keyb/KB16.COM` | DOS image/utility; both primary match. | ZIP source-equal; system32 source-equal. |
| `dos/v86/cmd/keyb/kb16.exc` | historical linker intermediate; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/keyb/keyb.tag` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/loadfix/loadfix.exc` | historical linker intermediate; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/mem/mem.skl` | historical message skeleton; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/mem/mem.tag` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/mem/value` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/nlsfunc/nlsfunc.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/nlsfunc/nlsfunc.skl` | historical message skeleton; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/qbasic/gorilla.bas` | DOS sample source; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/qbasic/money.bas` | DOS sample source; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/qbasic/msherc.com` | DOS image/utility; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/qbasic/nibbles.bas` | DOS sample source; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/qbasic/qbasic.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP different; system32 basename absent. |
| `dos/v86/cmd/qbasic/qbasic.hlp` | historical data; both primary match. | ZIP source-equal; system32 basename absent. |
| `dos/v86/cmd/qbasic/remline.bas` | DOS sample source; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/setver/setver.doc` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/setver/setver.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP basename absent; system32 differs. |
| `dos/v86/cmd/setver/setver.tag` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/cmd/share/share.EXE` | DOS image/utility; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/ansi/ANSI.EXS` | historical linker intermediate; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/ansi/ansi.skl` | historical message skeleton; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/ansi/ANSI.SYS` | DOS image/utility; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/ansi/ansi.tag` | historical change/notes data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/ansi/ansivid.inc` | assembly source/include; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/country/alclbjfk` | historical data; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/country/country.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/country/COUNTRY.SYS` | DOS image/utility; both primary match. | ZIP source-equal; system32 source-equal. |
| `dos/v86/dev/himem/HIMEM.BIN` | historical linker intermediate; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/himem/himem.exe` | DOS image/utility; primary variant/missing; see JSON. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/himem/HIMEM.SYS` | DOS image/utility; primary variant/missing; see JSON. | ZIP source-equal; system32 source-equal. |
| `dos/v86/dev/himem/himem1.asm` | assembly source/include; both primary match. | ZIP basename absent; system32 basename absent. |
| `dos/v86/dev/himem/usa-ms/messages.asm` | assembly source/include; both primary match. | ZIP basename absent; system32 basename absent. |

Each image and source retains its specific execution/feature obligation: command parsing, memory and environment, codepages, utilities, graphical device interfaces and error/exit behavior were not tested. System32 basename checks miss root-level files such as `WINNT/QBASIC.EXE`, but ZIP paths are recorded precisely. No product, guest, build tree, configuration or shared ledger modified.
