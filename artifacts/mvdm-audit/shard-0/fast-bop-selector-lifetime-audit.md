# CPU40 FastBOP selector lifecycle boundary

The selected `dpmi32/dpmimscr.c` differs from the original portable RISC
failure path by first asking `mvdm_softpc_fast_bop_prepare` for an endpoint.
On success it returns ES:BX, and otherwise retains zero ES:BX/DX; original
DPMI-in-use/no-longer-in-use interrupt hook calls remain. The selected x86
map attributes these entrypoints to `original-mvdm-dpmi32:dpmimscr.obj`.
DIV-310 names the CPU40 adaptation. Original x86 `DpmiGetFastBopEntry` instead
obtains the native monitor endpoint; importing that host CS:IP into a guest
is not a valid restoration.

The adapter reserves one currently non-present CPU40 LDT slot, publishes a
present descriptor via `DpmiSetWowLdtEntry`, stores its selector in a static
variable, and uses that cached numeric value plus offset for redirect. The
selected `CALLF` checks the adapter redirect before ordinary descriptor fetch.
Source-wide search found no writer that clears or changes the static selector
after initial assignment. Separately, the INT 2Ah compatibility branch passes
guest-supplied selector and descriptor words to `DpmiSetWowLdtEntry`, which
accepts an in-range not-present descriptor and writes the selected LDT slot.
These facts do not establish that an ordinary guest frees or overwrites the
specific reserved slot during the worker lifetime.

An x86 fixture derived from the existing `fast_bop_mode_fixture.c` links the
cached production adapter object SHA-256
`65B1B6B0980E3E45457C9BAB0F942C81C2AC409010393CB6E71AB1553CCF3243`.
After successful `prepare`, it clears the fixture-owned descriptor words and
calls the real `mvdm_softpc_fast_bop_redirect` with the same selector and
offset. The process exits 0 and prints `STALE_FAST_BOP_SELECTOR_STILL_REDIRECTS`.
Fixture source SHA-256
`5F06F91BBF646B8313C0349A346D82521B2EFCE9D7A6AF3CC33529BE57BD3539`;
the executable hash is
`50848F49F8975ADA8DE023F4758A5A3819A6D4762B85D6B299F9595FA2E65B7E`.
This proves adapter behavior under a controlled changed LDT; its LDT setter,
CPU and SAS are mocks. It does not execute guest INT 2Ah, CALLF, an actual
descriptor fault, or an actual task/worker reset. No guest or product was
modified. Existing compiler warnings concern unused mock parameters.

S11's identity/lifecycle recovery group should first establish whether the
guest's original selector allocator can publish, free or reuse this endpoint,
including task exit and same-worker restart. If reachable, compare original
x86 monitor ownership and required fault/callback semantics before choosing a
single authoritative reservation/lifetime design; do not merely guard the
test fixture or make the adapter silently intercept a reused guest selector.
Verify the real guest transition, failed reservation, nested callbacks and
teardown. This is a demonstrated local state-lifetime risk, not yet a proven
user-visible crash or authorization to change compatibility semantics.

## Original WOW selector-allocation source trace

The OpenNT `wow16/kernel31/ldboot.asm` calls `LDT_Init` before
`WOWFastBopInit`. `3protect.asm::LDT_Init` builds a free-selector list using
the `gdtdsc` guest table, and `get_sel` subsequently consumes that list.
For an external `AllocSelector` caller, the WOW branch initializes its
selected guest shadow entry and calls `WOW_DPMIFUNC_0C` to publish it to
the system LDT. Meanwhile the CPU40 adapter chooses its endpoint by scanning
the separate CPU40 LDT shadow and only publishes to that shadow via
`DpmiSetWowLdtEntry`; that routine explicitly does not update the DOSX source
table. Thus source-level endpoint reservation is not visibly reflected in
the already-built `gdtdsc` free list. If the chosen index is in that list,
ordinary later selector allocation can publish the same index, while the
cached FastBOP selector continues to be recognized ahead of normal CALLF
descriptor checks. This makes selector reuse a concrete conditional source
path, not just a hypothetical reset concern.

The actual endpoint index and guest table limit, the list's contents at that
index, and whether the immutable W31 runtime binary follows this exact OpenNT
guest source are not established by this trace. The host chooses from the
highest free shadow index, so ordinary workloads may not reach it. Confirm
those conditions in a real guest observation before asserting an observed
collision or choosing a guest-allocation fix. Preserve the independently
owned guest source and binaries unchanged.
