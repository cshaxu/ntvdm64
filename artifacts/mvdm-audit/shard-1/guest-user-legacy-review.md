# Shard 1: pending Win16 USER historical sources

The observer checks that all six paths below were `待核验` in the current shard ledger, and that each matches both frozen OpenNT source editions. Per-file hashes, catalog IDs and four checked host graph/dependency identities are in the JSON. Original `makefile`, `layer.asm` and `wowcomm.c` match the primary OpenNT reference; the makefile differs from OpenNT-4.5, so the selection discussion below is tied to the primary recipe and retained `k/daytona/userk.lrf` only.

| File | Original role and bounded selection evidence |
| --- | --- |
| `comdev.c` | Historical Win16 communications interface; it directly includes `comdevi.h`. Its `comdev.obj` is absent from the retained USER link response. The checked primary recipe and response instead include `wowcomm.obj`; this does not establish all communication behavior or feature equivalence. |
| `comdevi.h` | Internal device definitions directly included by `comdev.c`; this is a source include edge, not proof the including file enters the selected USER image. |
| `edmlonce.c` | Original multiline EDIT routines called infrequently; `edmlonce.obj` is absent from the retained USER link response. This does not show EDIT is absent or replaced correctly: its other implementations and alternate configurations are separate. |
| `wmsyserr.c` | Original system-error and drawing source; its own source marks portions `#ifndef WOW`. `wmsyserr.obj` is absent from the retained response; the status of the error UI feature cannot be inferred from that omission. |
| `usermenu.inc` | USER menu declarations; the obvious `layer.asm` include is semicolon-commented. No selected include or active alternate-profile behavior is established here. |
| `readme.txt` | Historical USER16 build guidance explaining the `NONX86=1` choice for a specific build scenario; documentary source, not evidence that the currently retained binary used those flags. |

None of the six exact paths appears in the four checked x86 worker/WOW32 and x64 frontend/service Ninja graphs or dependency logs. They are candidate historical guest-source/declaration/documentation roles **only relative to the checked host targets and retained link response**. Do not remove USER, EDIT, COMM or error-handling feature scope based on a missing object.

`src/mvdm/wow16/user/k/daytona/USER.EXE`, exact installed `O:/winnt/system32/USER.EXE` and `WINNT/SYSTEM32/USER.EXE` inside `assets/winnt.zip` are byte-identical, SHA-256 `6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C`. The retained response and this binary equality do not prove the binary was produced by that response or which variant implemented a given feature. Existing `parallel-review-next.md` separately addresses `wowk.asm`/`wowrtl.asm`; they are not repeated here.

Replay `python artifacts/mvdm-audit/shard-1/guest-user-legacy-observer.py`. No guest or product ran or changed; no build tree or shared ledger was modified. Actual guest entry, source-to-image lineage, alternate USER profiles and functional behavior remain unverified.
