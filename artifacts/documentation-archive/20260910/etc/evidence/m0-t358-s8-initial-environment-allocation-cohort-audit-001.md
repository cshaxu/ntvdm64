# M0 T358 S8 — original initial-environment allocation cohort audit

## Question

Does the original `54:0F` initial-environment cohort itself require a
recovery before the selected CPU40/x86 COMMAND workload can resume, and what
does the earlier `Alloc_error` observation actually prove?

## Inputs

- Selected original host source:
  `base/mvdm/dos/command/{cmdenv.c,cmddata.c,cmdmisc.c,cmddisp.c}`.
- Selected original guest source:
  `dos/v86/cmd/command/{rdata.asm,init.asm}` and
  `dos/v86/doskrnl/dos/alloc.asm`.
- Current mirrors, including the registered `MVDM-HOST-DIV-118` width-only
  overlay for `cmdenv.c`.
- Earlier fixed report:
  `m0-t358-s7-fixed-exit-lifecycle-observation-001.md`.
- Later, frozen root-layout report:
  `m0-t360-s7-owner-root-original-layout-observation-001.md` and its retained
  `O:\\ntvdm64\\observation-t360-s7.txt.bop-return.txt` record.

## Original cohort

`cmddisp.c` maps COMMAND service `0F` to the original
`cmdenv.c::cmdGetInitEnvironment` body.  On the first request it derives one
OEM environment block, strips the source-defined `COMSPEC`/`windir` cases,
adds the source-defined COMSPEC string, and returns a paragraph count in
`BX`.  The guest buffer is `ES:0000`; the caller supplies its capacity in
paragraphs in `BX`.

The permanent-command path in `rdata.asm` first allocates an environment
block through INT 21h `AH=48h`, calls `54:0F`, and branches as follows:

1. `BX=0`: retain/copy the old environment.
2. `BX <= EnvSiz`: retain the allocated segment and continue.
3. `BX > EnvSiz`: free the first block, allocate exactly the returned count,
   call `54:0F` again, and require `BX == EnvSiz`.

The second inequality transfers to `init_env_err -> nomem_err ->
Alloc_error`.  The first or replacement INT 21h allocation also transfers to
the same non-returning terminal on carry.  `init.asm::Alloc_error` is an
unconditional transfer to `RESGROUP:BadMemErr`; it is not a normal COMMAND
completion marker.  The reached NTDOS `$alloc` implementation either returns
the allocated segment with the source success return or its original
not-enough-memory/arena failure result.

## Current binding and mirror review

The current product compiles the original `cmdenv.c` provider and
`cmddisp.c` route.  The only relevant source divergence is
`MVDM-HOST-DIV-118`: host-side `size_t` calculations are checked before the
existing `USHORT`/`DWORD` COMMAND boundaries.  It preserves the source
environment filtering, allocation sequence, `ES:0000` destination and `BX`
paragraph contract; it neither supplies a replacement environment policy nor
changes guest allocation behavior.

The selected CPU40/x86 path keeps the original SoftPC `GetVDMAddr`/Sim32
address mechanism for this synchronous host-to-guest copy.  No host pointer,
handle, or environment object becomes guest state or an asynchronous record;
the copy remains within the original immediate BOP call.  No new mapping
manager or independent environment provider is introduced by this cohort.

## Evidence reconciliation

The earlier S7 report observed two `54:0F` returns followed by an
`Alloc_error`-mapped frame, but recorded only `AX` and carry—not the required
`BX`, `ES`, INT 21h return, or resident-memory witness.  It therefore could
not distinguish the three source branches above.

The later frozen original-root observation uses the valid `O:\\ntvdm64`
SystemRoot layout and a separately identified formal product.  Its durable
BOP record reaches both `54:0F` returns and then the first `54:01`, followed
by `54:0E`, `54:04`, and a later `54:01`.  That proves the selected current
product passed the guest's second-call check and did not take the observed
`Alloc_error` terminal.  It does **not** establish which difference between
the old and new frozen products/inputs removed the older terminal; S8 makes
no such causal claim.

## Disposition

The current valid package has no active `54:0F`/NTDOS-allocation blocker and
requires no environment-provider, allocator, guest-media, mapping, or CPU
change.  The earlier `Alloc_error` report is retained as historical evidence
but is superseded as a current predecessor by the later source-owned
`54:01` reachability result.

T358 must return to its actual objective: establish a source-owned completion
or return marker for the delivered `/C EXIT` path.  The next packet must map
the exact guest/host control flow from the first delivered command through the
later `54:01` request before another workload observation.  It must not
re-open the resolved `54:0F` cohort or repair a BOP leaf.

