# Shard 3: original WRITE source, resources and documents

The [observer](write-source-observer.py) and
[observations](write-source-observations.json) verify all twelve files against
both original editions and both audit catalogs. The primary `makefile` and
`write.lnk` are checked byte-for-byte. All proposed dispositions below concern
historical file roles only; neither WRITE application behavior nor guest
compatibility is excluded. No guest or product was built or modified.

An important identity boundary: the retained mirror `WRITE.EXE` SHA-256 is
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`,
while the ZIP and direct installed `O:/winnt/WRITE.EXE` both hash to
`EB44B5D5F5A405B6A8CD018B9F790BCA2E7E895DAD4F7E1F5462EC14CCDAA87A`.
The historical build recipe therefore cannot establish that these sources
produced the shipped image. Existing WRITE startup/file-open observations do
not prove each UI feature, printing, editing, OLE or document persistence.

| File | Proposed | Original file role and unresolved boundary |
| --- | --- | --- |
| `chlook.c` | pending | `makefile` has a standalone object rule but `write.lnk` omits the object; the source has an `ENABLE`-guarded implementation. Resolve alternate recipes and application behavior. |
| `diachgpr.c` | pending | Original change-printer dialog source; absent as an object from inspected link response. Determine the selected printer-dialog owner and other profiles. |
| `diaprint.c` | pending | Original print dialog and initialization source; absent as an object from inspected link response. Prove which current guest and host providers implement printing. |
| `heapinit.c` | pending | `makefile` has a standalone object rule, while `write.lnk` omits it; `OURHEAP` conditions need alternate-profile analysis. |
| `double.bms` | pending | Original bitmap-form resource input; no direct occurrence in the inspected current recipe/resource source. Resolve conversion/legacy consumers before exclusion. |
| `indent.bms` | pending | Original bitmap-form resource input; resolve conversion/legacy consumers before exclusion. |
| `left.bms` | pending | Original bitmap-form resource input; resolve conversion/legacy consumers before exclusion. |
| `rmargin.bms` | pending | Original bitmap-form resource input; resolve conversion/legacy consumers before exclusion. |
| `write.itl` | pending | Resource script identifies itself as international and references `rbutton.bmp`/`rmark.bmp`; selected recipe takes `write.rc` from language-dependent `RES_DIR`. Resolve the intended alternate/international path. |
| `write.def` | excluded historical guest build-input role | Original link response explicitly consumes this Win16 module definition. This does not discard the shipped application's exports or their behavior. |
| `readme.txt` | excluded historical build-document role | Original instructions for historical WRITE builds, preserved as source evidence; they are not host implementation or an authorization to rebuild guest. |
| `filespec.wri` | excluded historical document role | Original WRITE-format specification document, preserved as evidence for file-format recovery; its file role is not the format feature or a runtime test. |

Every listed file retains 100% of both original source/input bytes. Complete
behavior or executable-code recovery percentages remain unmeasurable. The
original WRITE link response and the installed binary have not been tied by a
reproducible guest build, which is prohibited by current policy.
