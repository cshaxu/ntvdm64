# Shard 2: DPMI and virtual interrupt declarations

Three pending entries match both original trees. The companion observer captures their identities, representative guest/host source references and normalized Ninja dependencies for the selected DPMI interrupt object. No build or runtime test was performed.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `dpmi32/precomp.h` | Exact normalized dependency of current `obj/dpmi/dpmiint.obj`; the object is in the selected DPMI static library and worker map attributes `DpmiSetProtectedmodeInterrupt` to it. Candidate **partial** for its selected declaration role. |
| `inc/intapi.h` | Exact normalized dependency of the same object; its source includes it and references install/uninstall interrupt-handler interfaces. Candidate **partial**; a declaration and map entry do not prove those calls are executed or matched to the expected provider. |
| `inc/VINT.INC` | Original HIMEM guest assembly includes it; original WOW32 `i386/fastwow.asm` also references it, and original include makefile describes generation from `vint.h`. No full current/profile consumer audit is established; retain **pending**, not guest-only by assumption. |

The observed `dpmiint.c` may contain registered project adaptations, even though these headers match originals. Interrupt-vector ownership, nesting, protected/real-mode transition, reset, failure and teardown behavior are separate from source inclusion. `VINT.INC` was not a dependency of this one captured host C object; that says nothing about other objects or build profiles, and does not prove the deployed guest's exact ABI.

Replay `python artifacts/mvdm-audit/shard-2/dpmi-interrupt-interface-observer.py` and compare its JSON. Graph and map hashes are a point-in-time snapshot. No guest, product, installed file, shared ledger or build tree was modified.
