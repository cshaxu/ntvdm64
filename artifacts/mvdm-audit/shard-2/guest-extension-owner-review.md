# Win16 guest extension owner review

This is the exact pending subset of shard-2 for wow16/inc, commdlg, shell, toolhelp, ole and ole2; per-path CSV maintains the disjoint list. Source/resource candidate requires an original/current byte match and actual selected object list + tool/link path or an include path from such a source. Existing DLL/OBJ/LIB and unresolved files remain pending; library-role review is separate.

55 rows; 28 candidates; 27 pending; 0 frozen/current mismatches.

COMMDLG resources are identified by default USA selection; other LANG/DBCS paths are not asserted. OLE server retail/debug selection and include preprocessor branches remain conditional. A role does not prove current installed guest binary lineage or feature behavior; no host-runtime admission or guest rebuild.

Original/current recipe identity:
- `wow16/commdlg/makefile` original `118C94655FAA068FD0027DE8BBFEF0BEBEC53A8D26B213268D80BC86981A5D36`, current `118C94655FAA068FD0027DE8BBFEF0BEBEC53A8D26B213268D80BC86981A5D36`
- `wow16/ole/client/makefile` original `42A19F54364CF6FEE49047DA225A702FADFF759541E37143B298F3A2EC7C0B58`, current `42A19F54364CF6FEE49047DA225A702FADFF759541E37143B298F3A2EC7C0B58`
- `wow16/ole/server/makefile` original `040ECF946AE5F0FFBBC946C70637801210FDBFE01533E5CC66D0C85133665009`, current `040ECF946AE5F0FFBBC946C70637801210FDBFE01533E5CC66D0C85133665009`
- `wow16/toolhelp/makefile` original `015B38918E45C957B4E29720E9C183090BED2BA30671105E50877DC679466941`, current `015B38918E45C957B4E29720E9C183090BED2BA30671105E50877DC679466941`
