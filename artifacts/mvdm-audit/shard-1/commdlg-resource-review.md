# Shard 1: original common-dialog bitmap resources

This read-only audit covers five pending shard-1 paths. The [observer](commdlg-resource-observer.py)
and [receipt](commdlg-resource-observations.json) recheck current bytes against
both original editions and both frozen audit catalogs. The two catalogs
include one DOS record and two WOW edition records for each path. No guest,
product, installation, build tree or shared ledger was modified.

Original `messages/usa/commdlg.rc` declares bitmap `bmpDirDrive` as resource
ID 576 from `filebmps.bmp`. The original `makefile` consumes `commdlg.res`
and applies it to the 16-bit `commdlg.dll`; these recipe edges do not prove
which source built any shipped binary. The retained guest DLL, ZIP member and
currently installed DLL have identical SHA-256. Read-only NE parsing shows
that its resource 576 contains the exact DIB bytes of the original 128×16,
4-bit `filebmps.bmp`, followed by resource-alignment padding. This proves
the resource bytes in that image, not that the DLL was loaded or rendered.

| Shard-1 path | Result and proposed role | Remaining boundary |
| --- | --- | --- |
| `cddrv.bmp` | Original 16×16 indexed pixels and 16-color palette match tile 5 of the strip. Candidate historical standalone bitmap input exclusion. | Verify other original build consumers before final exclusion; do not exclude the CD-drive icon capability. |
| `smnet.bmp` | Pixels and palette match tile 6. Candidate historical standalone bitmap input exclusion. | Verify other original build consumers; do not exclude network icon or dialog behavior. |
| `smram.bmp` | Pixels and palette match tile 7. Candidate historical standalone bitmap input exclusion. | Verify other original build consumers; do not exclude RAM-drive icon or dialog behavior. |
| `smfldr1.bmp` | Palette matches but pixels do not match any of the eight selected-strip tiles. **Pending.** | Determine alternative consumers and whether the image has a separate selected resource. |
| `COMMDLG.RES` | Original recipe names this resource file as a 16-bit DLL build input. Candidate exclusion of the historical intermediate file role only. | Establish actual retained `.res` to DLL provenance if relevant; immutable shipped image and dialog behavior remain in scope. |

The tile comparison checks every palette entry and pixel index, preserving BMP
row orientation. It does not establish how the strip was authored or whether
its tiles depict identical rendering after palette changes. Source-tree text
search for the four standalone names found no references in `src`, `tools`
or `tests`; this finite observation does not prove absence from every
historical external build. Each source file retains 100% original bytes;
the behavior-restoration percentage is not established. Suggestions require
coordinator review before any shared status change.
