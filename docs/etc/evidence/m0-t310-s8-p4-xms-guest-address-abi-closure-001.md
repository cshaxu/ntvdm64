# M0 T310 S8 P4 — XMS guest-address ABI closure

## Original contract

`xms.486/xmsblock.c:xmsMoveBlock` reads a 12-byte descriptor immediately
below the original `SS:BP` location: destination linear address, source linear
address, then a word count. It doubles the count, performs a forward copy,
and only then returns AX=1. The original body assumes that `GetVDMAddr` and
both XMS linear values are valid host pointers; that is an NT4 x86 process
layout assumption rather than an Intel/XMS ABI.

`xms.486/xmsumb.c` uses the historical `ReserveUMB`/release `PVOID` argument
as an Intel physical address before converting it to a paragraph segment. In
the selected user-mode path it is a numeric carrier, not a native pointer.

## Recovery rung

The original XMS files remain the owner and retain their public entrypoints,
descriptor layout, ordering, linked-list behavior and success direction.
`MVDM-HOST-DIV-077` adds only the smallest same-shaped adapter boundary:
`mvdm_xms_move_block` takes the original 16-bit stack values and acquires the
session's existing bounded guest-memory lease for the descriptor and each
forward-ordered copy chunk. No native pointer, split pointer value or new
family-specific mapping manager is introduced. An invalid descriptor or span
has no original XMS guest-error branch, so the adapter requests the existing
controlled session cancellation and leaves AX unmodified rather than
fabricating success or an undocumented BL code.

`MVDM-HOST-DIV-078` uses the already registered UMB numeric codec. It does
not resolve a host pointer or allocate an identity token.

## Warning closure

The following baseline rows are closed by source contract, not suppressed:

- `T310-S8-P4-CLASS-000269` through `...000281`: `xmsblock.c` raw pointer
  l-value and truncation conversions.
- `T310-S8-P4-CLASS-000282` and `...000283`: `xmsumb.c` raw `PVOID` to
  `DWORD` conversions.

`T310-S8-P4-CLASS-000267` (`DbgPrint`) and the remaining XMS initialization
conditional/callback records are outside this address closure and remain
visible in the all-tree ledger.

## Verification

The existing formal MSVC `/MT` graphs were used without warning suppression:

```text
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/xms/xmsblock.obj
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/xms/xmsblock.obj
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/xms/xmsumb.obj
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/xms/xmsumb.obj
```

Both architecture logs contain no `C4311`, `C4312`, compile error or fatal
error for either original XMS translation unit. The new adapter itself was
also compiled with the same x64 and x86 formal `/MT` flags; both builds have
zero diagnostics. The build-generator manifest now selects
`mvdm_xms_memory.c` as part of the named SoftPC binding library. Full runtime
machine composition remains S8/S9 work; this record proves only the repaired
XMS guest-address ABI boundary.
