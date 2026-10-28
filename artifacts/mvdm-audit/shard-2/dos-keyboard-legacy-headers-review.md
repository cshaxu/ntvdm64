# Shard 2: legacy keyboard mapping headers

Two pending headers match both original trees. The companion observer captures their identities, current controller build/map and normalized dependency results. No product or guest was run.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `softpc.new/base/inc/keycaps.h` | Defines a keycap name array under `UIF_TEST`. The captured worker `cflags` do not directly define that macro, and the exact header is absent from `keyba.obj`'s normalized Ninja dependency list. Its full consumer/profile remains unknown; retain **pending**. |
| `softpc.new/base/inc/m_keybdp.h` | Contains an X `KeySym` to AT matrix table. No exact selected consumer was established; it is absent from the one captured controller dependency list. Retain **pending**, without asserting no other platform/profile uses it. |

Current graph and map select `keyba.obj`, including `KbdEOIHook` and `kbd_inb`. That controller is a separate path from a historical X key-symbol mapping table. Its selected source contains previously implemented project changes, so matching these headers to original files does not prove current keyboard behavior, EOI ordering, reset, scan-code correctness or the actual input pipeline. Neither missing dependency from one object nor a macro absent from one flag line proves global non-use. The outstanding work is to identify the exact legacy consumer and its profile before any exclusion decision, while verifying controller/input semantics separately.

Replay `python artifacts/mvdm-audit/shard-2/dos-keyboard-legacy-headers-observer.py` and compare its JSON. No guest, product, installation, shared ledger or build tree was modified.
