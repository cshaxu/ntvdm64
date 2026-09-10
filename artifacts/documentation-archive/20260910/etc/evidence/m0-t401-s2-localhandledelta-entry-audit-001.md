# M0 T401 S2 — WRITE `LocalHandleDelta` entry audit

## Scope

This is a read-only source and staged-media audit of the `WRITE.EXE` startup
candidate identified after the captured `Kernel31` general-protection fault.
It determines whether the application's ordinal-310 `GetProcAddress` call
enters the in-guest Kernel implementation or a WOW32 host thunk.  It does not
modify, rebuild, substitute, or execute WRITE, Kernel31 guest media, WOW32, or
the product.

## Evidence

`src/mvdm-guest/wow16/write/initmmw.c` obtains ordinal `0x136` from `KERNEL`
and calls it before its first `FCreateRgbp` allocation.  The original export
definition identifies that ordinal as `LOCALHANDLEDELTA`:

| source | fact |
| --- | --- |
| `src/mvdm-guest/wow16/kernel31/kernel.def:340` | `LOCALHANDLEDELTA @310 NODATA` |
| `src/mvdm-guest/wow16/kernel31/linterf.asm:1557-1567` | direct far guest body: obtain `pLocalHeap`, return or assign `hi_hdelta` |
| `src/mvdm-guest/wow16/kernel31/retail3/kernel.map:180` | body address `0002:301E` |

The fixed T398 runtime manifest identifies its `system32/KRNL386.EXE` as the
approved original `retail3/krnl386.exe` media after the documented
`original-kernel31-fixexe-16bit-header-recovery` transform.  The transform
means raw file hashes must differ, so this audit parses the staged NE entry
table rather than treating a differing hash as a code difference.

For the actual stage (`O:\\t398\\system32\\KRNL386.EXE`), the NE header has
an entry table at `0x489` of length `0x4ac`.  Entry ordinal 310 is a movable
entry with flags `0x01`, segment `2`, and offset `0x301e`.  Segment two at
that offset begins:

```
55 8b ec 1e 8b 1e 06 00 8b 46 06 0b c0 74 06 8b
```

The same bytes occur at `2:301e` in the immutable `retail3/krnl386.exe`.
They are the assembled prologue and first instructions of the source body in
`linterf.asm`.

## Disposition

`src/mvdm-host/wow32/wktbl2.h:261` does contain an `UNIMPLEMENTEDAPI` entry
named `LOCALHANDLEDELTA` at WOW32 function-table slot 208.  That is not this
ordinal-310 call: the actual NE entry reaches the direct guest Kernel body,
not `WOW16Call`/`W32Dispatch`.  Enabling or replacing the host-table entry
would therefore not repair this WRITE startup path and is rejected as an
unrelated semantic change.

The candidate remains useful only as a pre-fault in-guest-memory dependency:
an invalid current local heap could fault inside the genuine Kernel body.  It
is not evidence that a missing WOW32 carrier caused the observed GPF.  Any
next observation must identify an earlier source-owned boundary without
altering the original guest control path.
