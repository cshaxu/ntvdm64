# S10 shard 3: WRITE alternate source owners and print-dialog selection

Four pending `wow16/write` files are refined beyond `write-source-role-review.md`. `write-alternate-owners-observer.py` / JSON captures exact current and original hashes. Read-only source audit, no guest build, actual WRITE interaction or print device test.

| Path | Historical function and concrete selected-recipe evidence |
|---|---|
| `chlook.c` | `DoChLook` implements formatting selection under `#ifdef ENABLE`; the file comments it as not implemented when guard is absent. Makefile contains an object rule, but standard `OBJ1..6` and `write.lnk` omit `chlook.obj`. Proposed alternate/conditional guest source file role; do not infer formatting unavailable because other source owns it or because this one is unselected. |
| `heapinit.c` | A custom heap implementation under `#ifdef OURHEAP`; makefile has a standalone `heapinit.obj` rule but standard object list and link response omit it. `heapmain.obj` and `heaprare.obj` are separately listed. Proposed alternate custom-heap source role, with actual active guest heap and failure semantics still open. |
| `diachgpr.c` | Legacy `DialogPrinterSetup` code, absent from inspected standard object list/link. Current source graph cannot prove shipped WRITE's exact producer. Proposed historical alternate dialog source, not omission of printing. |
| `diaprint.c` | Legacy `fnPrPrinter` / `DialogPrint`, absent from the standard object list/link; the listed `printdlg.obj` provides its own `fnPrPrinter` and calls `PrintDlg`, followed by `PrintDoc` (`printdlg.c:120,159,244,265`). The generic Win16 common-dialog and HDC printing observations in T468 are bounded related evidence, not a WRITE-specific acceptance. |

The manifest/installed WRITE.EXE is not the same bytes as the retained source-tree WRITE.EXE, as shown by `write-source-role-review.md`; neither the inspected response nor the replacement print dialog proves which exact binary producer was used. Other make profiles, actual formatting, printer setup, allocation, callback teardown and application behavior remain unverified. The four files may be classified by **historical alternate build role** for the inspected recipe only, and no guest feature should be excluded or claimed recovered solely on that basis.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/write-alternate-owners-observer.py
rg -n 'chlook\.obj|heapinit\.obj|diachgpr|diaprint|printdlg\.obj' src/mvdm/wow16/write/makefile src/mvdm/wow16/write/write.lnk
rg -n 'ENABLE|OURHEAP|DoChLook' src/mvdm/wow16/write/chlook.c src/mvdm/wow16/write/heapinit.c
rg -n 'fnPrPrinter|DialogPrinterSetup|PrintDlg|PrintDoc' src/mvdm/wow16/write/diachgpr.c src/mvdm/wow16/write/diaprint.c src/mvdm/wow16/write/printdlg.c
Get-Content artifacts/mvdm-audit/shard-3/write-source-role-review.md
```
