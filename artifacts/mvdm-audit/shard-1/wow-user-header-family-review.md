# S10 shard 1: WOW USER, message and dispatch declaration family

Thirteen pending paths were examined as one related source/dispatch family. Every current file matches both reference editions byte-for-byte by SHA-256. `wow-user-header-family-evidence.json` provides each full hash, direct include locations, every normalized VALID dependency object and the observed graph hash. Replay with `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/wow-user-header-family-observer.py`. Source inclusion and selected object are not proof that every declaration, variant, cleanup or runtime path works; no new executable-image or guest test was performed.

| Pending file | Evidence and recommended classification boundary |
| --- | --- |
| wkbdtbl2.h | Directly included by wowtbl.c; one VALID object. Original keyboard dispatch rows include LOCALAPI and UNIMPLEMENTEDAPI. Classify as dispatch declaration carrier, not evidence keyboard functionality is absent or complete. |
| wmsg16.h | precomp.h inclusion; 88 VALID objects. Declares general 16-bit message thunk/unthunk; selected wmsg16.c holds implementation. Retain as active declaration carrier with per-message behavior and reentrancy still pending. |
| wmsgbm.h | precomp; 88 objects. Button message constants and conversion declarations; see `wow-button-image-review.md` and `wow-button-message-audit.md`. Retain carrier; those bounded witnesses do not cover every button path. |
| wmsgem.h | precomp; 88 objects. EDIT message conversion declarations; see shard-2 `wow-edit-tabstop-unthunk-review.md`. Retain carrier; all EDIT and callback ownership not demonstrated. |
| wowdde.h | No direct include in scanned WOW32/project-binding C/H nor VALID dependency in this graph. Contains DDE structures; classify as unselected declaration candidate only, pending generated/alternate-profile and actual DDE behavior checks. Do not infer DDE unavailable. |
| wowtable.h | No direct include or VALID dependency in bounded scan. Historical table declarations; classify as unselected carrier candidate only, not a claim that dispatch tables are absent. |
| wparam.h | precomp; 88 objects. Declares mapped thunk parameter/block structures. Selected wparam.c and `wow-param-map-review.md` cover specific ownership; retain active carrier with nested/failure lifecycle boundaries from that report. |
| wsdynmc.h | Directly included by wsdata.c, wsext.c and wsraw.c; 3 VALID objects. Defines Winsock entry indices, a declaration carrier for selected files; provider loading, socket ownership and all 49 entries remain unproved. |
| wsubcls.h | precomp; 88 objects. Defines subclass thunk structures and magic. Selected wsubcls.c consumes them; retain carrier, with subclass teardown, reentry and handle reuse requiring targeted evidence. |
| wucursor.h | precomp; 88 objects. USER cursor thunk declarations; selected wucursor.c and `wow-cursor-review.md` provide bounded source evidence. Do not conflate with actual mouse input or cursor lifecycle completion. |
| wuhook.h | precomp; 88 objects. Hook set/unhook/next declarations; selected wuhook.c. Retain active carrier; cross-thread ordering, nested callbacks and cleanup remain behavior-specific obligations. |
| wulang.h | precomp; 88 objects. ANSI/DBCS conversion declarations; selected wulang.c and `wow-language-image-review.md` bind selected paths. Do not extend that evidence to every locale, bad pointer or lead-byte case. |
| wuwind.h | precomp; 88 objects. USER window and WNDDATA declarations; selected wuwind.c. Identity and lifetime remain with actual owner and callback code, not this header or its many precomp dependencies. |

Direct includes are textual scan of `src/mvdm/wow32` and `src/wow32-dll` C/H files, and VALID dependencies cover only the current x86 WOW32 Ninja tree. Other profiles, generated sources, indirect resource use, runtime graph and complete host/guest semantics remain outside the negative observations. Zero hits is not proof of global unreachability. No shared ledger, product, guest or build tree changed.
