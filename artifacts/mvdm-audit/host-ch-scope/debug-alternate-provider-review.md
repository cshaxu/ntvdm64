# SoftPC alternate debugger providers and production symbol owner

This bounded source/object check covers pending original host files `base/debug/yoda.c`, `base/debug/dasm.c`, `base/debug/ega_dump.c`, `base/debug/pigyoda.c`, and `host/src/nt_yoda.c`. It performs no product or guest execution and does not edit the shared ledger.

| Source | Observed current role | Recommendation |
| --- | --- | --- |
| `base/debug/yoda.c` | Original non-PROD branch contains the debugger. Its PROD branch defines empty `force_yoda` and `delta_check_I`; the cached `yoda.obj` indeed defines those symbols. However, the worker map attributes `_force_yoda` to **`original-ccpu386:stubs.obj`**, not `yoda.obj`; identical empty bodies may be folded and archive presence cannot prove a production call or extraction. | **Pending:** establish original interactive YODA behavior and profile scope; no-op function compilation is not active debugging or full source recovery. |
| `base/debug/dasm.c` | Original disassembler used by tracing/YODA is guarded by `#ifndef PROD`. | **Pending:** resolve original disassembly/debugger consumer and intended host capability; current production compilation does not prove it. |
| `base/debug/ega_dump.c` | Original EGA state dumping is under `EGG && !PROD`. | **Pending:** preserve diagnostic profile and distinguish actual selected EGA/VGA register behavior. |
| `base/debug/pigyoda.c` | Original PIG/YODA command bridge is under `PIG`; cached object has no executable body with the current profile. | **Pending:** compare/test PIG profile independently from production CPU state. |
| `host/src/nt_yoda.c` | Original host extensions are gated under `YODA`; the remaining `host_set_yoda_ints` production stub is empty. Existing host-tail review says the selected production startup has no call, and the captured map has no named object. | **Pending:** the original host debugger provider and usage are not proven restored. An inactive role in this profile must not erase debugger/exception obligations. |

`base/inc/yoda.h` provides a separate production macro path and `ccpu386/stubs.c` provides the selected no-op/data route; their partial source integration is not a YODA guest/debugger acceptance. No new runtime evidence is claimed.
