# XMS block request source boundary

`xms.486/xmsblock.c` remains partial because of its separately registered
guest-memory move adaptation. The unchanged original allocation/free/realloc
functions call the shared `ExtMemSA` suballocator. A failed `SAAllocate` or
`SAFree` returns AX=0 to the guest; failed reallocation returns CX=0. The
separate [suballocator audit](../shard-2/suballoc-xms-callback-failure-review.md)
shows injected failure bookkeeping observations and selected callback owners.

Two original-source branch invariants need direct validation. In
`xmsAllocBlock`, a zero DX bypasses `SAAllocate` yet the function reads local
`BaseAddress` for its return. In `xmsReallocBlock`, equal old/new sizes bypass
`SAReallocate` yet the function reads local `NewAddress`. These are
source-level uninitialized-read candidates, not established guest defects.
The original guest HIMEM source `dos/v86/dev/himem/himem4.asm` checks equal
reallocation lengths and returns before its XMS_REALLOCBLOCK service, but its
shown allocation path calls XMS_ALLOCBLOCK without an adjacent zero-size
check. Whether earlier caller validation, the actual immutable installed
HIMEM binary or another reachable service prevents or permits either input
has not been established. Source-level absence of a check in one function
is not proof of an end-to-end invocation.

Trace actual selected `xmsdisp.c` dispatch, immutable HIMEM provenance and
all guest callers. Test zero size and unchanged-size request on an independent
guest probe plus controlled host entry, with exact return and allocation
state, comparing original contract before selecting any host correction.
The `ALLOC` zero path and the decommit-failure bookkeeping are separate
questions. No original guest, product code or runtime image was changed.

## Independent guest observation

An independent DOS COM built from `xms-zero-probe.asm` called INT 2Fh 4300/4310
to obtain the installed XMS entry, then invoked XMS function 09h with DX=0.
On isolated package-c, the guest printed `ZERO AX=0001 BX=A500 DX=A513` and
`XMS ZERO PROBE DONE`; the observer reported normal exit 0. The probe attempts
to free a returned success handle but does not inspect that free's result or
the underlying base. The guest COM SHA-256 was
`E1BB58D3785E95D18051D1886189FF3D974BFF9B415A75EB19632F37A3AA98A0`;
the source SHA-256 was
`5CA49626F8EF6BAE67D07FB82C5E321852321DDD016FD073E0C8D0791CA4C0DA`.
The isolated PIF config loads package HIMEM.SYS, whose package SHA-256 is
`08AA2C47D835460ED3067FA7D6F8A3B37EDECA524AD102B0588FDD1BF389CE08`;
NTVDM SHA-256 is `01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6`.
The stage was owned and cleaned by the existing isolated package machinery;
no installed or original guest image was modified.

This proves a zero-size request returned success in this one profile, not
that the host source candidate was reached, that the returned handle/base is
valid, or that the observed result complies with the intended XMS contract.
HIMEM's selected binary is not yet tied to the retained source. Verify the
actual host entry, returned base, handle information, free result and repeat
behavior before deciding whether there is a runtime defect. Run r1 accidentally
assembled the EMS probe instead and failed the XMS marker check; it is a
harness failure, not an XMS observation. The r2 source and COM identities
above distinguish the valid run.

## Guest lifecycle and existing host-entry witness

A second independent COM, `xms-zero-lifecycle-probe.asm`, repeated the zero
allocation and then requested handle information, a same-size (zero) resize
and free. On the same isolated package identity, the guest output was
`ZERO AX=0001 BX=A500 DX=A513`, `INFO AX=0001 BX=001F DX=0000`,
`REALLOC AX=0001` and `FREE AX=0001`; the observer reported exit 0.
The zero-size handle reports zero size and all three later guest calls report
success. The COM SHA-256 is
`91CA42B4FC98EB5D9DF9147A77DB9B20471FA5C3C46ADDB6C8F35DA82285D60F`;
source SHA-256 is `DB4082F457E498F0998B8A376C32C37689BAB5AF852AC2DF969DDF7372E4F7DF`.

The run explicitly enabled the existing `MVDM_WIN31_SETUP_XMS_TRACE_PATH`
entry witness. Its recorded host BOP row is `xms service=02 ax=0000 bx=a513
cx=001f dx=0000`, establishing that the actual host XMS allocation service
received DX=0 for the independently initiated request. The witness records
inputs before `XMSDispatch`, not the host local value or return. The guest
lifecycle does not inspect the handle's underlying base and cannot rule out
an erroneous base or later effects. Same-size resize's success is consistent
with the retained guest-source early return, but does not prove that source
was used to build the installed binary. No original guest or product image
was changed. Further assessment requires the original XMS contract, host
return and allocation-state observation, including repeated requests and
subsequent nonzero allocations.

## Zero-size lock address in two isolated workers

A third independent guest probe additionally locked and unlocked the
zero-size handle before querying, resizing and freeing it. Two isolated
package-c runs used exactly the same COM SHA-256
`7D0B9C7B3B76254E377314A35665C16DBAF8665A761915FE3518DEDC71D9FC80`
and source `xms-zero-lock-probe.asm` SHA-256
`E55BF58E8AAE9EFC3392B33B96DDB5CB3501FCE57603E5413207E9D5D59E5273`.
Both exited successfully and their existing host entry witness recorded XMS
service 02 with DX=0. Allocation, lock, info, equal-size resize and free
reported success; the info size was zero. The first lock returned DX:BX
`020E:2800` and the second `0287:0C00`.

These are observed guest-visible, process-dependent addresses for a zero-size
block, consistent with but not proof of the uninitialized local's exact
influence. The XMS contract for locking a zero-size block, actual pool bounds,
source-to-binary equivalence of the guest and the host return value remain
unverified. No read or write through those addresses was attempted. Do not
claim data corruption or decide a fix from address variation alone. The
retained `himem4.asm` source makes zero-length free bypass the host-free
service; the measured free return does not prove the installed binary's exact
source or a host deallocation call.
