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

Follow-up against the current cached x86 PROD graph: the current dependency observer reports the same 11 CCPU-header and 23 C-VID-header object names as the earlier receipt, although the build graph hash changed. The explicit source uses of Cpu.Video->GetVideolatches and ->SetVideolatches are both inside c_getset.c's PIG-only block. Its actual selected PROD object has no c_getCpuState/c_setCpuState symbols. A fresh relocation sweep of the 11 CCPU-header consumers found no direct _Cpu, _Video or _C_Video relocations; object hashes and the PROD object witness are in [follow-up receipt](ccpu-evidgen-followup.json), and the current dependency receipt is [here](ccpu-evidgen-current.json). Thus these explicit PIG-only accesses are not evidence of a defect in the selected PROD build. This does not prove all indirect aliases, alternate PIG builds, runtime vector initialization/reset, or the C-VID consumers correct; those remain within the existing S11/S13 boundary. No source repair is justified solely by these declarations.

A separate fresh link of the existing cached selected libraries, using the Ninja fixture's own link command with output redirected to build/t473-s11, completed with exit 0. The fixture exercised 81 C-VID slots, all listed read/write/mark selectors, 256 byte and 253 unaligned word/dword simple read/write cases, guards, republish and concurrency checks. The exact object/library/executable hashes and limitations are in [fixture receipt](cvidc-selected-fixture-current.json). Its inherited /force:multiple fixture link remains a limitation; it is not a fresh rebuild, complete DOS/WOW guest validation, or evidence for the unselected PIG profile. The separate Ninja target process remained live when this receipt was recorded.

The previously pending Ninja target is no longer present in the process inventory. Its current graph reported the test object out of date. A first Ninja invocation without the MSVC environment failed to locate `cl.exe`; no runtime result is counted from that attempt. Invoking the same target through the existing x86 MSVC environment driver rebuilt and linked successfully, the resulting selected fixture passed its 81-slot/selector/read/write/guard/concurrency checks, and a subsequent Ninja dry run reported no work. Input and image hashes are in [current Ninja receipt](cvidc-ninja-current.json). The link still emits the documented `/force:multiple` duplicate-symbol warnings; this does not test guest mode transitions or the PIG profile.

After the later S11 worker changes, the dependency/relocation observer was
rerun against the current graph: it again found 11 CCPU-header and 23 C-VID
header consumers and no named `_Video`/`_C_Video` relocation in those 11
objects ([receipt](ccpu-evidgen-s11-recheck.json)). The selected C-VID fixture
was relinked from the current cached graph and passed the 81-slot, selector,
read/write, guard and concurrency checks; a following Ninja dry run was clean.
[Input hashes](cvidc-s11-recheck-hashes.json) bind the new test executable and
objects. The existing `/force:multiple` fixture-link warnings remain, and
these checks still do not prove arbitrary indirect calls, PIG behavior or
actual guest video reset/mode transitions. Those ownership boundaries remain
with S11/S13; no original or 486 source repair is justified by this recheck.
