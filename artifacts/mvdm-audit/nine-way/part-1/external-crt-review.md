# Part 1 external: CRT ctype, case mapping and locale provider

This report covers the 17 specifically assigned external OpenNT records under `base/crts/crtw32/` in `nine-way-ownership.csv`. The stable SRC key, source path, SHA-256 and four exact normalized VALID dependency scans for every row are in `external-crt-evidence.json`; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/external-crt-observer.py`. All 17 exact external source files exist, and none appeared as an exact VALID dep in the four current graphs. These rows are external provider/relevance obligations, not MVDM mirror files; no directory-wide exclusion or source import is proposed.

Selected WOW32 `wkernel.c:162,171,222,279,457` uses `_strlwr` and `_stricmp`; `wkman.c:2108` also uses `_stricmp`. The current built WOW32 map attributes `_stricmp` and `_strlwr` to `libucrt` objects, plus locale update/toupper helpers. This supports current build-time modern CRT ownership for those call sites, not proof current installed-image identity, any specific locale, ANSI codepage conversion, native-private USER behavior or equivalence to original CRT. Character classification and locale inputs influence other code paths and remain separate behavioral obligations.

| Assigned external path | Concrete external role and bounded recommendation |
| --- | --- |
| convert/isctype.c | Original multibyte classification implementation; selected WOW ctype consumers must be enumerated and compared under active locale before declaring equivalent. |
| convert/nt/sources | Historical NT build manifest for convert group; its relevance follows its selected function closure, not an executable feature. |
| convert/sources.nt | Alternate NT build configuration carrier; identify original selection before excluding this file role. |
| convert/tolower.c | Original character case conversion; compare relevant modern CRT behavior for ASCII and non-ASCII before semantic acceptance. |
| crt32.nt | Common original CRT build configuration; verify actual required compiler/ABI flags rather than importing an entire CRT. |
| h/ctype.h | Original public classification declarations/macros; current SDK/CRT owns modern declarations, but macro and locale differences need bounded evidence. |
| misc/ctype.c | Original classification data table; ownership belongs to actual CRT provider; selected codepage/locale equivalence untested. |
| misc/initctyp.c | Original classification initialization; current CRT owns its own lifecycle; whether legacy initialization order is observable remains untested. |
| misc/nlsdata1.c | Original NLS locale state/data; current locale provider and multibyte semantics must be established. |
| misc/nlsdata2.c | Additional original NLS locale data/state; same ownership and failure boundary. |
| misc/nt/sources | Historical misc NT build manifest; evaluate only needed source closure. |
| misc/setlocal.c | Original setlocale implementation; actual project calls and locale state ownership must be checked, no silent equivalence claim. |
| string/nt/sources | Historical NT string build manifest; selected functions, architecture and provider determine relevance. |
| string/sources.nt | Alternate NT string source list; same constrained build-role assessment. |
| string/stricmp.c | Original portable case-insensitive compare; current build map names libucrt implementation for `_stricmp`. Encoding/locale compatibility and installed build binding remain untested. |
| string/strlwr.c | Original case conversion; current build map names libucrt `_strlwr`. Locale and buffer/error behavior remain untested. |
| string/x86/stricmp.asm | Original x86 alternative implementation; exact historical selection and modern replacement equivalence are not established. |

Recommended status for these keys: retain pending specific provider/semantic boundaries, while recognizing build manifests as non-runtime roles. Any potential exception must isolate specific observable behavior and evidence, rather than wholesale importing legacy CRT or assuming modern CRT matches it. No original binary, product, guest or external source was executed or modified; shared ledgers were not changed.

## Scope of selected case-folding calls and locale

In addition to the prior wkernel citations, `wkman.c:2108,3266,3305,3660,3816` contains direct `_stricmp` calls, so this is broader than a single KnownDLL path. Original external `string/stricmp.c` explicitly uses ASCII A–Z folding in the C-locale branch and `_tolower_lk` under a non-C locale; `string/strlwr.c` likewise has an ASCII C-locale branch and an NLS mapping branch. A bounded textual scan for `setlocale`/`_configthreadlocale` in current WOW32, wow32-dll and ntvdm-exe C sources returned no call, but this does not measure the running process's actual locale: linked libraries or other code can affect it. No active-process default-locale read, non-ASCII/DBCS case comparison, codepage or concurrency test was run. Retain these distinctions and do not claim either that the default is proven C or that modern CRT differs from OpenNT.
