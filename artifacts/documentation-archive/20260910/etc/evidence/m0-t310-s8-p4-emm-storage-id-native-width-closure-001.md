# M0 T310 S8 P4 — SoftPC EMM private storage-ID native-width closure

Date: 2026-08-29

## Scope

This closure reads the complete reached EMM private-storage contract:

- `softpc.new/base/dos/emm_mngr.c` retains the original EMM handle table,
  allocation, reallocation, `USEBLOCK`, `FORGETBLOCK`, and page-map order.
- `softpc.new/base/inc/emm.h` remains the original host-service declaration
  boundary.
- `softpc.new/host/inc/host_emm.h` remains the source-shaped storage access
  abstraction.
- `softpc.new/host/src/nt_emm.c` and `x86_emm.c` are the two selected original
  host providers.

## Finding and disposition

The original provider documents `storage_ID` as a host-dependent value, and
the NT provider explicitly implements it as a pointer. NT4 could put that
pointer in `long`; a modern x64 process cannot. The manager never exposes this
value to guest code and does not perform guest-address arithmetic on it.

`MVDM-HOST-DIV-061` changes only the host-private representation to existing
`IHP` and preserves all original calls and ownership. It is intentionally not a
session mapping-manager user: mapping IDs represent external/guest-facing
identities, whereas EMM storage is native SoftPC private backing memory.

The same reading found an original conditional `<malloc.h>` include that left
the reachable `host_malloc` macro without a visible allocator declaration in
the formal non-MONITOR profile. The standard declaration is now included;
there is no allocation-policy change.

## Verification

Formal Ninja object targets passed after the change:

- `build/M0-T310/S8/p1-machine-source/x64/emm-storage-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/emm-storage-x86-build.log`

The two logs contain no `emm_mngr.c` `C4311`, `C4312`, or `C4047` diagnostic.
The remaining signed-16-bit map-table declaration diagnostics (`C4057` and
`C4028`) are source-contract items, not pointer-width conversions; they remain
in the whole-tree ledger for separate original-contract review.
