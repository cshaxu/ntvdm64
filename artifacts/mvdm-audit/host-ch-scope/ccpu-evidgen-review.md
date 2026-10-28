# CCPU and C-VID VideoVector declarations

The selected x86 worker has two different original `evidgen.h` files. The
[observer](ccpu-evidgen-observe.py) and [receipt](ccpu-evidgen-evidence.json)
record each header hash, declaration order, current Ninja dependency groups,
and actual CCPU-header-consumer COFF object hashes and named relocations.
Run the observer from the repository root and compare JSON with the receipt;
the result is tied to that specific build snapshot.

The CCPU file declares five function-pointer fields; the C-VID file declares
81. Assuming four-byte x86 pointers, the same names have these offsets:

| Field | CCPU offset | C-VID offset |
| --- | ---: | ---: |
| `GetVideolatches` | 0 | 0 |
| `SetVideolatches` | 4 | 156 |
| `setWritePointers` | 8 | 312 |
| `setReadPointers` | 12 | 316 |
| `setMarkPointers` | 16 | 320 |

These are incompatible type layouts, not interchangeable same-named headers.
Under `C_VID`, `base/inc/egacpu.h` includes `evidgen.h`, declares `C_Video`
using that selected type, maps latch access through `C_Video`, and maps the
three selector operations to named functions. The current recorded deps
identify 11 objects with the CCPU header: `sasCdef`, `mouse_io`, `trace`,
`nt_ega`, `nt_vga`, `nt_graph`, `nt_mouse`, `nt_fulsc`, `nt_det`,
`mvdm_softpc_text_video`, and `v7_pointer_binding`. They identify 23 with
the C-VID header, including video owners, `ev_glue`, `vglfunc`, `accessfn`,
the adapter vector binder and its fixture; exact names appear in the receipt.
Inclusion alone does not prove that a translation unit dereferences a field.

For the 11 CCPU-header objects, `objdump -r` found no direct `_Video` or
`_C_Video` relocation in the recorded objects. That narrows one failure
hypothesis, but cannot rule out aliases, other symbols, indirect/generated
access, missing object groups, or different profiles. It does not prove that
cross-translation-unit layouts are safe in general. The prior C-VID fixture
checks selected publication and latch operations in its own C-VID profile;
it does not verify all CCPU-header consumers or cross-layout dereferences.

The CCPU header's original declaration and selected compiler dependencies
support a **partial** source-role assessment, with behavior and ABI
compatibility unresolved. Do not mark full or infer a tested defect solely
from the differing declarations. S11/S13 must resolve actual field access,
provider ownership, layout and mode/reset behavior across all selected
contexts. No product or guest source was modified for this observation.
