# Shard 2: WOW32 multimedia and message table headers

Three entries currently pending match both original trees. The companion observer queries `ninja -t deps` read-only for `wowtbl.obj`, resolves the emitted relative paths against the actual build directory, and checks them against these exact files; it also records current compile edges and map symbols. All three are **partial** candidates, with bounds below.

| Entry | Observed role and limitations |
| --- | --- |
| `wmmedia.h` | Included through current `precomp.h` and declared in the normalized dependency record. It declares multimedia structures and `WMM32CallProc32`. This does not prove callback, pointer or host-device lifetime correctness. |
| `wmmtbl2.h` | Included by `wowtbl.c`; the original multimedia table names `WMM32CallProc32` and two unimplemented entries. Current map separately identifies the project wrapper and renamed original body. Source and map do not by themselves prove the linked table relocation or an actual guest invocation. |
| `wmtbl32.h` | Included through `precomp.h` and declares `aw32Msg`; the map identifies the message table in `wmtbl32.obj`. This does not prove any individual 16↔32 message conversion, reentry or handle lifetime. |

Earlier `../shard-1/wow-mci-image-review.md` contains a narrower independent image-binding review of the MCI path. Earlier multimedia audits also distinguish dormant structured audio/joystick bodies from the selected generic call path. Neither evidence set establishes all multimedia APIs, callback ordering, error handling or every message mapping. Current source selection and the project extension are distinct from unchanged original header bytes.

Replay `python artifacts/mvdm-audit/shard-2/wow32-media-message-observer.py` and compare its JSON. Build-graph and map hashes are observations at capture time and may legitimately change after a rebuild. No guest, product, installation, shared ledger or build tree was modified.
