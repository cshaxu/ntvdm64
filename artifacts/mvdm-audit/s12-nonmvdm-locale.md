# S12: unselected non-MVDM locale and menu branches

The observed x86/x64 product commands do not select SUPPORT_LPK, FE_IME,
DBCS, FE_SB, KANJI, KOREA, WASWIN31 or MEMPHIS_MENUS. No definitions or
undefinitions of these labels were found in project/generated C/header sources
or the installed Windows SDK and Visual Studio C headers. Seventy-five
original-source positive branches in the observed closure were replaced by
markers with their conditionals retained. The companion JSONL records original
locations and removed-body hashes. This does not claim that other build
profiles or language configurations were assessed.

Independent reconstruction from `1ae50915e` verified all 75 hashes and 21
complete files. Cached x86/x64 product builds and documentation governance
passed. All ten product images matched the previous release exactly or
differed only by four PE linker timestamp bytes; runtime tests and publication
were not repeated for equivalent images.
