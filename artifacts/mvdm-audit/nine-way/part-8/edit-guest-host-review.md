# Part 8: original Win16 EDIT sources versus selected host USER bodies

All eight owned Win16 guest files match both frozen original editions. Critically, the current WOW32 build selects **different source paths**: `src/opennt-host/windows/core/ntuser/client/*.c`, not the same-named `src/mvdm/wow16/user/*.c`. Seven selected host USER objects each have one function body bound to the published WOW32.DLL with relocation words masked (listed below). The ZIP and installed USER.EXE are byte-identical; that does not prove a given guest creates an EDIT or that the different guest and host sources are behaviorally equivalent.

| Owned key | Captured provider evidence | Bounded disposition |
| --- | --- | --- |
| `wow16/user/editec.c` | Selected distinct NT USER control-body-editec.obj: `_ECCalcChangeSelection@20` 302 bytes, 0 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/edecrare.c` | Selected distinct NT USER control-body-edecrare.obj: `_ECCreate@12` 129 bytes, 4 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/editsl.c` | Selected distinct NT USER control-body-editsl.obj: `_SLChangeSelection@16` 547 bytes, 16 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/editml.c` | Selected distinct NT USER control-body-editml.obj: `_MLBuildchLines@24` 1331 bytes, 18 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/edmlonce.c` | Selected distinct NT USER control-body-edmlonce.obj: `_MLCreate@12` 251 bytes, 6 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/edmlrare.c` | Selected distinct NT USER control-body-edmlrare.obj: `_MLGetLine@16` 108 bytes, 5 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/edslrare.c` | Selected distinct NT USER control-body-edslrare.obj: `_SLCreate@12` 160 bytes, 6 relocations masked | Guest historical source retained; migration/semantic equivalence, message conversion and lifecycle pending. |
| `wow16/user/edit.h` | Guest EDIT declaration; selected host has independent headers and body sources | Pending actual declaration consumers and semantic correspondence. |

The body binding proves sample code presence only; it does not prove calls reach it, guest and host layouts correspond, or reentrant message/error/callback behavior, line editing, clipboard, selection, destruction and failure recovery work. Treat guest and NT USER identities as distinct, preserve registered host changes/exceptions, and do not rebuild or modify guest. The task's original minimal-diff and host-resource ownership principles remain applicable.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-8/edit-guest-host-observer.py` and compare JSON. No guest, product, build tree, external source or shared ledger was modified.
