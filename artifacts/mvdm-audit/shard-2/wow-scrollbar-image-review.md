# Original WOW scrollbar message owners

This shard-2 checkpoint covers the pending physical `wow32/wmsgsbm.c` role.
The [observer](wow-scrollbar-image-observer.py) and [receipt](wow-scrollbar-image-observations.json)
check both original editions, both audit catalogs, selected x86 WOW32 link,
valid object dependency, and actual released WOW32 code. Two original forward
conversion bodies and three reverse `wcntl32.c` bodies match the selected
objects outside relocation words. The original source retains 100% of its
bytes; behavioral restoration percentage is not measurable from this evidence.

Eight actual image table pointers are checked against COFF relocations and
their exact bound owners: `WOWCLASS_SCROLLBAR` slot 7 in both Win16 thunk and
unthunk arrays; and native scrollbar messages 0xE0–0xE4 and 0xE6 in the
Win32 message table. This establishes initial table ownership in each
direction, not execution, subsequent table mutation or correct semantics.
`wmsg16.c` has existing project mirror changes; this checkpoint does not
reclassify that file or assume its runtime call contract is fully restored.

Original forward `ThunkSBMMsg16` translates SETPOS and SETRANGE signed 16-bit
values, maps SETRANGE redraw, and supplies per-call buffer positions for
GETRANGE. `UnThunkSBMMsg16` packs returned endpoints. In the opposite
direction the original `WM32SBMControl`, `WM32SBMGetRange`, and
`WM32SBMSetRange` own the native-to-guest mapping. The selected message
dispatcher owns the lifetime of each call context; actual guest/host window
objects and message delivery are host-managed. The call context must not be
retained beyond its call, and nested messages or target destruction need
explicit validation. This observation does not exercise that behavior.

Proposed disposition is **partial**: prove real 16-bit and native message
traffic in both directions for negative, zero, 0x7fff and 0x8000 endpoints,
GETRANGE output, SETRANGE redraw, USER messages outside the mapped subset,
reentry, target destruction and failed delivery. Do not infer a defect or full
control acceptance from static table identity. The prior [source review](wow-scrollbar-message-review.md)
records the same outstanding boundaries.

No shared ledger, product, guest or installed package was changed. The main
reviewer must replay the observer and decide whether to merge the candidate.
