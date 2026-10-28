# Shard 1: pending Win16 historical declarations

The observer confirms the following three files were `待核验` in the current shard ledger and match both frozen OpenNT editions. Per-file identities and four checked host graph/dependency hashes are recorded in the companion JSON. None of the three exact paths appears in those x86 worker/WOW32 and x64 frontend/service inputs; that observation alone is not a complete guest-role disposition.

| File | Established reference and boundary |
| --- | --- |
| `wow16/inc/gdidefs.inc` | Original assembly-style GDI/device declarations. `src/mvdm/inc/wowfax.h` contains an absolute historical include of this path under `#if defined(_WOWFAX16_)`; `wow16/test/shell/wowexfax.c` defines that macro before including `wowfax.h`. The latter is a source anchor, not proof that its include resolves to this exact header or that the conditional is preprocessed in a current guest build. The historical `src/mvdm/build.log` also reports an include-not-found event, but is not current build evidence. |
| `wow16/inc/paswindw.inc` | Pascal-style Windows type/constant declarations. No literal reference was found in the inspected `src` text search; generated, nonliteral or external build references were not ruled out. |
| `wow16/inc/win3deb.inc` | Historical debug macros and constants. `wow16/kernel31/kernel.inc:56` has a semicolon-commented include under `if KDEBUG`; it is not an active include in that text. Other selected consumer or alternate preprocessed profile remains unproved. |

All three remain pending: neither a conditional or commented reference nor absence in four host graphs establishes the full original guest consumer closure. In particular, retain the unresolved WOWFAX guest/include search path rather than claiming a full fax build or discarding GDI declarations.

Replay `python artifacts/mvdm-audit/shard-1/guest-win16-declarations-observer.py`. This read-only audit ran no guest, compiler, product or provider and modified no guest, product, build tree or shared ledger. It does not prove an installed guest image, alternative build configuration or runtime feature.
