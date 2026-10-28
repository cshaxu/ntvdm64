# S10 shard 1: historical PIFEDIT and FORCEDOS family

Seven pending files were compared with original trees, four current Ninja VALID dependency sets and bounded delivery locations. `pifedit-forcedos-family-evidence.json` has each SHA-256, three reference identities, exact dependencies, ZIP and system32 basename observations; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/pifedit-forcedos-family-observer.py`. No item is an exact VALID dep in the four observed host graphs; each basename was absent from the ZIP and exact installed system32 location. That does not exclude alternate configurations or external files.

| Pending file | Role and evidence; limits |
| --- | --- |
| forcedos/obj/i386/forcedos.exe | Retained original executable absent at both primary reference paths but byte-identical to third opennt-src-2 mirror. Historic FORCEDOS source uses CreateProcess with CREATE_FORCEDOS and waits for the process (`forcedos.c:244-264`). Its introductory comment describes creating a PIF; that is not the observed current function body. No executable runtime/host support tested. |
| forcedos/pif.h | PIF file layout, matches both primary references. No direct include named pif.h in forcedos source found by bounded textual search; cannot infer historical alternate build never used it. |
| pifedit/pifedit.ico | PIFEDIT icon, matches both references; `pifedit.rc:21` names it as resource. No compiled resource or installed PIFEDIT identity proven. |
| pifedit/pifedit.rc | PIF editor dialog/menu/string/icon resource, matches both references; original sources file selects it. Dialog behavior not tested. |
| pifedit/pifedit.rcv | Version resource source, matches both references; included at pifedit.rc:3. Not an independently executable component. |
| pifedit/pifhelp.h | Help declarations/constants, matches both references; included through pifedit.h:3. Help target availability not tested. |
| pifedit/pifsubs.c | Original dialog-bound PIF update/validation/save logic, matches both references; original sources selects it. `standalone-tool-chain-audit.md` distinguishes it from project `src/addon/pif`, a format-oriented PIF.EXE utility, not an automatic interactive-editor replacement. No GUI, PIF corruption, file ownership or save failure tested. |

The original PIFEDIT `sources` target is an independent Windows application; lack of these exact input files in current host graph is not proof PIF format support is missing. Conversely, PIF.EXE existence is not proof historical PIFEDIT UI/field semantics. No product, guest, build tree, system config or shared ledger was modified.
