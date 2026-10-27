# Win16 guest extension ownership: shard 3

`guest-extension-owner-review.csv` enumerates all 52 currently pending shard-3 paths in the requested inc/commdlg/shell/toolhelp/ole/ole2 families, with original OpenNT and OpenNT-4.5 hashes where frozen, frozen/current hash, concrete original edges or a bounded unresolved reason. All live hashes equal their frozen values. Eighteen are candidates **only as historical guest build source/definition files**:

- COMMDLG `commdlg.asm`, `parse.c`, `commdlg.def`: `makefile:116-126,143-155` names object compilation, link16 object list and DLL definition.
- OLE client `dib.c`, `doc.c`, `error.c`, `le.c`, `ledde.c`, `ole.asm`: original makefile explicitly compiles or uses its C suffix inference, includes the objects in LE_OBJ/PICT_OBJ and invokes `link` from the configured 16-bit tools path; do not describe that literal command as link16. OLE server `olesvr.asm` and `.def` have explicit MASM/object/definition and link16 target edges.
- SHELL `.def` is a dependency of `shell.dll`; the make-generated `shell.lrf` names the SHELL definition and link16 consumes it.
- TOOLHELP six helper/stack/walk source files are explicitly object dependencies and members of the `toolhelp.dll` link16 list. `class1.c` is explicitly commented out in the original makefile and remains pending; lack of selection is not capability exclusion.

The remaining 34 items, including general `wow16/inc` headers, bitmap/dialog resources, runtime SHELL.DLL, the binary SYM and COMMDLG.LIB, stay pending: filename or a make dependency alone did not establish the requested precise consumer chain. `dlgs.h` is a make dependency of parse.obj but a direct `parse.c` include was not found, so it is not promoted. The separate library-format audit is separate evidence, not an automatic change in this group.

Candidates do not prove source→current distributed guest image, guest behavior, or authorization to rebuild guest. Native-host versus guest is established only for these original guest target chains, and no host-runtime role was inferred for untraced shared headers. Existing ledger, product, guest and other shards were not changed; no runtime/build was run.
