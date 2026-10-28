# Part 8 semantic reconciliation: TOOLHELP class, task and module ownership

## Findings tied to released code

The immutable guest TOOLHELP.DLL's NE table exports TaskFirst @63, TaskNext @64, ClassFirst @69 and ClassNext @70. In the original WOW guest build, task1.c provides guest task enumeration, while the original makefile excludes class1.c/class2.asm and links ththunks.asm, which emits ClassFirst and ClassNext thunks. The selected original WOW32 `wowtbl.c` incorporates `wthtbl2.h`, whose three entries (dummy/class-first/class-next) all choose `UNIMPLEMENTEDAPI`. Original `WOW32UnimplementedAPI` returns FALSE in the release branch.

The companion observer verifies this is not merely an unlinked source table: in the published retail WOW32.DLL, `_aw32WOW`'s seven preceding module separators identify TOOLHELP start at index 1760; its dummy, ClassFirst and ClassNext entries each point to `_WOW32UnimplementedAPI@4`, followed by the next table separator. That target's `wow32.obj` body binds to the published DLL with relocations masked. This is exact observed image/table linkage; the observer does not run a 16-bit caller or verify arbitrary function pointers after relocation.

The prior authority `docs/etc/evidence/m0-t470-s1-toolhelp-debugger-owner-matrix.md` explicitly records this class failure as original WOW behavior, not an approved host class-enumeration substitute. `m0-t470-s2-toolhelp-query-disposition.md` records an earlier executable negative witness with a particular T467 package and original guest image: Win3.1 version/mode were reported, TaskFirst=0, valid ModuleFirst=0, ClassFirst=0 and the guest retired normally. The record attributes this to inability to establish the private guest KERNEL/TOOLHELP query provider, without proving a specific wrong field. Those historical results are **not** a fresh test of the currently released DLL or of every DLL pair. The current independent probe exists at `tests/observation/win16_toolhelp_probe.asm` and `verify-win16-toolhelp-probe.ps1`; it checks normal/negative query and size paths. Existence is not a new test run.

## Per-file disposition and outstanding validation

| Owned key | Original behavior/owner, failure and retirement | Evidence-based status / work |
| --- | --- | --- |
| `class1.c` | Non-WOW class entry validates initialization, pointer and size; original class head lives in private USER layout. | Original WOW build excludes it; classify this specific build role as unselected, preserve class capability boundary. Do not reinterpret modern USER private structures. |
| `class2.asm` | Non-WOW ClassInfo verifies selector and walks original USER class layout. | Same excluded WOW file role; no authorized host replacement inferred. |
| `ththunks.asm` | Selected guest thunk forwards class calls to WOW32. | Guest build input retained; released WOW32 table demonstrably maps calls to FALSE target. Maintain original negative regression. |
| `task1.c` | Guest TaskFirst/Next validate `wLibInstalled`, entry size, and next handle before original TaskInfo; no NTSRV state substitute. | Partial guest owner evidence; prior incompatible-pair negative witness, actual compatible guest query/handle retirement not proven. |
| `module.c` | Guest module enumeration and validity checks use original guest state. | Partial guest build/owner evidence; prior valid and bad-size negative witness; no general success or module retirement proof. |
| `dllentry.asm` | Guest DLL entry and initialization participate in original owner state. | Historical selected guest build; initialization failure and compatible image-pair lifecycle remain bounded by T470. |
| `notify2.asm` | Guest notification machinery, distinct from NTSRV process management. | Historical selected guest build; callback nesting, unregister and task death remain unverified. |
| `timer.asm` | Guest ToolHelp timer service, distinct from SYSTEM/TIMER.DRV. | Historical selected guest build; timeout callback and teardown remain unverified. |
| `usergdi2.asm` | Guest USER/GDI-related helper in original recipe. | Historical selected guest build; resource lifetime/selector paths remain unverified. |
| `string.h` | Historical guest declarations used by task/module sources. | Historical build input; exact per-caller behavior follows owning source, not header identity. |
| `toolhelp.h` | Guest public structures and API declarations. | Historical build input; exact packing must match actual caller and selected guest DLL; prior probe uses packed structures. |
| `TOOLHELP.RES` | Historical compiled guest resource input. | Build artifact; source-to-shipped-image provenance remains unproved. |
| `TOOLHELP.DLL` | Retained original guest binary equals ZIP copy; exports parsed. | Partial image/interface evidence; loader search and compatible image-pair behavior are not established solely by hash. |

## Finite follow-up and acceptance

1. S10 may classify the 13 owned files using the scoped decisions above, preserving T470's accepted original class failure and original guest ownership; no guest or host production modification is justified by this evidence.
2. Under S14's WOW task/cross-boundary scope, only if ToolHelp query recovery is admitted for a **compatible immutable guest image pair**, bind all tested image hashes, rerun the independent TaskFirst/ModuleFirst good-size and bad-size probe, and add task/module destruction and handle-reuse witnesses. If no compatible pair exists, retain the documented negative contract rather than fabricate host class/task state. Current T470 evidence remains valid for its exact prior configuration, not a universal proof.
3. Under S19, run the existing negative probe against the tested release package where its inputs are available; confirm ClassFirst FALSE, expected query/initialization result for the chosen pair, normal retirement, and matching image hashes. A source table binding alone cannot replace that guest test. Callback, timer and notification nested/exit/failure cases stay as separate guest-owner obligations where required.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-8/toolhelp-table-release-observer.py` and compare its JSON; `toolhelp-family-observer.py` verifies original guest hashes and NE exports. No tests or product builds were run for this reconciliation; no guest, product, shared ledger or build tree changed.
