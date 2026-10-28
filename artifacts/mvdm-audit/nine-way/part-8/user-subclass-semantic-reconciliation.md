# Part 8: Win16 USER standard-class subclass and message declarations

This review addresses five owned guest USER paths, not the separate `wow32/wsubcls.c` or modern NT USER sources. Exact original hashes are in `guest-family-inventory-observations.json`.

Original `wow16/user/wsubcls.c` defines C wrappers such as ButtonWndProc, each passing its own 16-bit procedure address through CallWindowProc. But original `wow16/user/makefile:88-89,193` lists and builds `wclass.obj` from `wclass.asm`, not `wsubcls.obj`. `wclass.asm` declares the same standard control wrappers as explicit assembly and embeds `SCLS` magic and class IDs before them. Thus the C file's semantic description is valuable, but naming it as the selected guest compiled provider from these recipes is unsupported. Historical `user.def:122,303` provides the CallWindowProc and standard-class ordinal interface. Exact bytes and linkage of a live packaged USER.EXE are not established by this recipe.

Original host `wow32/wsubcls.c` is a different, selected original WOW32 source: GetStdClassThunkWindowProc maps standard class identity to the guest ordinal and preserves a callable guest proc for original subclass chaining; `wow32/wumsg.c::WU32CallWindowProc` handles its call. Guest address, WOW mapping and real host window/procedure identity must remain distinct; destruction and callback reentry cannot safely treat a guest procedure as a native pointer.

| Owned path | Bounded role |
| --- | --- |
| `wow16/user/wsubcls.c` | Original C standard-class wrapper source; observed original makefile instead names assembly wclass.obj. Preserve alternate source and guest capability, do not claim active C body. |
| `wow16/user/iuser.inc` | Original USER alias declarations included by `user.inc`; exact conditional expansion remains an ABI input, not another execution owner. |
| `wow16/user/msglayer.inc` | Original message-validation constants/types; `msglayer.asm` and recipe include it. Presence does not establish actual runtime validation of every message. |
| `wow16/user/usermenu.inc` | Original menu structures/flags; observed `layer.asm:9` include is commented, other profiles/consumers pending. Do not infer that menu behavior is absent. |
| `wow16/user/winutil.asm` | Original helper source referenced by `user.inc`; profile and active code resolution need separate proof. |

Historical `m0-t420-s40-wow-user-profile.md:3912-3936` records eight focused original WU32SetWindowLong policy cases using controlled seams and explicitly says they are not a real guest/native USER integration. Its later records discuss additional bindings but must be tied to their actual candidate/image; no current run is claimed. S15 should verify immutable guest standard-class old procedure roundtrip, direct call and CallWindowProc, nested subclass replacement, target failure, window destruction, stale proc rejection and correct value/error recovery; S11 owns reentrant execution, S19 release identity. Avoid adding a parallel subclass registry or activating a historical C file solely based on names.

Replay `rg -n "wclass.obj|wsubcls.obj|msglayer.obj" src/mvdm/wow16/user/makefile`, inspect wclass.asm/wsubcls.c and `wow32/wsubcls.c`, plus part-8 inventory observer. No guest, product, shared ledger or runtime changed.
