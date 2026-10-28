# S10 shard 1: WOW multimedia header dependency audit

Scope: pending `wow32/wmmalias.h`, `wmmstruc.h`, `wmmtbl.h`. Current SHA-256 matches both OpenNT reference trees, respectively `48bcee7abcca92bafd6dcea4bcfd9437bae10c170bb9bfee4db587e00e134cc0`, `8c8c764f500c47689f5d97c485e8d81c6361130c6ce744a3dbe8d2d26c1eaf5b`, `c35252b376c0f6132df8439f19ef66c780a2cf3bb39720b62c0d1d8b6f2069e4`.

Normalized VALID deps from `ninja -C build/.ninja/msvc-x86-release/wow32 -t deps` show wmmalias.h is included by obj/wmmalias.obj, wmmstruc.h by 88 objects (via precomp.h:110), and wmmtbl.h by obj/wow32.obj. The Ninja graph links those objects into WOW32.DLL. Dependency inclusion or object linking is not proof every macro or function survives optimization or is used.

* `wmmalias.h` declares 16-bit multimedia alias structures and bits; `wmmalias.c:41-69` has lookup, reverse lookup and free entry points. `walias.h:260-282` defines driver, wave and MIDI alias macros. Consult `shard-3/wow-multimedia-alias-audit.md`: it found selected object but no mapped GetMMedia16/32/FreeMMedia16 symbols and left actual consumers, reuse and teardown unresolved. No assertion that all macro users execute.
* `wmmstruc.h` describes 16/32 multimedia structure conversion and header flags. Its COPY_WAVEOUTHDR16_FLAGS use occurs in wmmedia2.c, whose relevant converter bodies are globally disabled (`#if 0` per shard-3 audit). Header graph dependency must not be mistaken for active wave conversion or callback correctness.
* `wmmtbl.h:18` declares aw32MMED; `wowtbl.c:66` includes wmmtbl2.h, which registers WMM32CallProc32 at entry 2. `wow32.c:2196` handles MOD_MMEDIA. As prior `wow-mci-image-review.md` records, an alternate Win16 mmsystem → host WINMM thunk route exists; it is not evidence all multimedia operations or callback lifecycle work.

Reproduce via Get-FileHash, direct include search, Ninja deps normalized relative to its build directory, and the cited source paths. Prior image-bound and runtime evidence is limited to referenced reports; this slice did not run a guest, verify device behavior, resource lifetimes, reuse or every included macro. No guest, product, shared ledger or build tree modified.
