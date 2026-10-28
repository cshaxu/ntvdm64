# Shard 2: shared WOW interface headers and guest includes

Five pending files match both original editions. The observer and JSON record source identities, original guest include/build references and a current Ninja dependency snapshot. `ninja -t deps obj/wowtbl.obj` emits relative names; the observer resolves each against the actual build directory before comparing exact paths.

| Entry | Evidence and bounded recommendation |
| --- | --- |
| `inc/wowcomm.h` | Included by current `wucomm.h`; exact path is in `wowtbl.obj` dependencies. Candidate partial host-interface declaration. Serial behavior, packed ABI, handle lifetime and real COM tests remain separate. |
| `inc/wowgdi.h` | Included via `precomp.h`, exact path in `wowtbl.obj` dependencies. Candidate partial host-interface declaration. A compile dependency does not prove guest/host layout equivalence or GDI operation. |
| `inc/wowkbd.h` | Included via `precomp.h`, exact path in `wowtbl.obj` dependencies. Candidate partial host-interface declaration. Keyboard state, callbacks and layout behavior are not established. |
| `inc/WOWGDI.INC` | Original GDI guest assembly includes it, with the original GDI makefile listing the include; original Winspool assembly also includes it. Original include makefile gives a `wowgdi.h`/`wow.inc` generation dependency. Candidate historical guest build input only, subject to the coordinator checking all current consumers and build profiles. Do not infer that these exact `.INC` bytes were generated from the present header. |
| `inc/WOWCMDLG.INC` | Original COMMDLG guest assembly includes it and the makefile lists the dependency; include makefile describes its header dependency. Candidate historical guest build input only, with the same generation and profile boundary. |

The two assembly includes were not present in the captured `wowtbl.obj` dependency list; that is a single-object observation, not proof of absence from every host or guest build. Source identity and matching dependency paths do not prove structure layout, ordinal agreement, actual guest loading, error paths or object lifetimes. No guest was rebuilt and no product, installed files, shared ledger or build tree was modified.

Replay `python artifacts/mvdm-audit/shard-2/wow-shared-abi-header-observer.py` and compare its JSON. Ninja graph and dependency observations describe the captured current configuration, not all build configurations.
