# Original multimedia alias object: controlled lifecycle witness

The [test fixture](old-media-alias-fixture.c) links the cached x86 `wmmalias.obj` itself with a real process critical section and test-owned `malloc_w`/TEB task seam, without `/FORCE` or product/guest edits. The fake TEB/PTD offsets (`0x34`, `0x3c`) are taken from this exact object's disassembly and are not claimed as the real host ABI. Handle values are synthetic; no WINMM or guest is used. Reproduce in a fresh output directory:

```cmd
artifacts\mvdm-audit\host-ch-scope\old-media-alias-build.cmd build\t473-s10\old-media-alias-r2
build\t473-s10\old-media-alias-r2\fixture.exe
```

Build and process exit 0. Output: `a=0100 b=0100 before=12345000 after=00000000 c=0100 new=12346000`. Two lookups of the same synthetic 32-bit handle produce one alias; reverse lookup matches; freeing clears the old reverse value; creating a different same-hash input reuses the same synthetic alias and resolves to the new value. The fixture asserts every part. Hashes: original/current `wmmalias.c` D4949206F261D22F80F96E769388AEE94037B27092C69C68EC058619F843F4FB; selected cached object 01683BB93E144685F971D3E34FD6720EB25307AF95DBD94662BAC9BE9320A68E; fixture 76D9C67041F0555FAEECE7CA584D19D77006D9519A086BF014394C240EFCF229; build recipe E351B949E606E908C8D412FD37CB55DB762C69F231341FFFC4E09CDD465C81CB; fixture exe A9BB9243CBC1164F3A4CBA4FEC6C44D0437B440349117536064EEB666C07E38D.

This establishes real original-object behavior with controlled seams, **not** that these functions survive the linked production image or have a real guest caller. Existing [consumer audit](../shard-3/wow-multimedia-alias-consumer-followup.md) found only disabled legacy consumers and no mapped Get/FreeMMedia function. Consequently it cannot justify a product partial/feature-restored classification; keep the active guest/WINMM bridge and S16 valid-device identity, callbacks, cross-task ownership and teardown requirements. No lifecycle behavior beyond the synthetic single-threaded sequence was observed, and cached object identity is not a fresh source-bound build.
