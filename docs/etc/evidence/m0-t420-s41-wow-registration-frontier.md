# M0 T420 S41 — WOW registration frontier

## Purpose

This record attributes the current immutable `WRITE.EXE` startup frontier
before admitting a replacement USER registration path.  It is not a claim
that WOW16 acceptance has completed.

## Reproduction

The S41 x86 composition (`build/M0-T420/S41/w1-composition-r1`) was copied
only to an isolated short-path test package under the governed build tree.
It was started as:

```text
run16.exe system32\WRITE.EXE
```

The existing child-only trace and termination report recorded:

```text
run16-binary 32
get-state 258
get-command-null 0
terminate-vdm return=003DB41E
host-terminate return=003F4F61
```

`32` is the product's `BINARY_TYPE_WIN16`, and `258` is the original
`ASKING_FOR_PIF | ASKING_FOR_WOW_BINARY` prefetch.  That prefetch deliberately
has no executable command payload.  It proves that BaseSrv found the WOW
record; it is not a broker delivery failure.

The x86 link map relocates the `TerminateVDM` caller to original
`nt_bop.c::MS_bop_1`.  In that source, the only matching branch calls
`TerminateVDM` after `WOWInitEntry()` (`WOW32!W32Init`) returns `FALSE`.

## Actual CPU mapping state

A default-off scalar observation was added at that already-reached original
`MS_bop_1` boundary.  The isolated run recorded:

```text
wow-bop cr0=00000011 cr3=00000000 csip=01c7:aebc
```

`CR0.PE` is set but `CR0.PG` is clear, and the active CR3 is zero.  Therefore
the reached DOSX/WOW path is protected mode without paging at the registration
frontier.  There is no active guest page directory that a standalone shared
view may extend.  The withdrawn private-CR3 model is disproved for this path;
S41 must use a source-supported linear-memory and descriptor carrier instead.

## Modern USER boundary

The existing isolated x86 probe was rerun from
`build/M0-T420/S40/user-registration-surface-r1/`:

```text
WOW_USER_REGISTRATION_SURFACE shared=C0000022 build=00000000 slots=0
```

Thus modern `USER32!UserRegisterWowHandlers` returns `STATUS_ACCESS_DENIED`
and writes neither the original shared-info pointer nor any of the twenty
WOW output callbacks.

The original OpenNT body is
`windows/core/ntuser/client/client.c::UserRegisterWowHandlers` (lines
2256–2314 in the pinned comparison tree).  Its entire registration operation
has a finite shape: retain 21 input callbacks, publish the 20 USER-owned
output entries, and return `&gSharedInfo`.  It does *not* create the shared
section; that is the earlier NT4 USER/CSRSS connection boundary.

Consequently, current `W32Init` proceeds with no valid original output table;
its original `WK32InitializeHungAppSupport` calls the absent
`pfnRegisterUserHungAppHandlers`, returns `FALSE`, and the already identified
`MS_bop_1` branch terminates the WOW VDM.

## S41 disposition

S41 must not restore the retired TLS-only `wow_user_runtime` as a success
stub.  Its required product result is instead a worker-local, original-shaped
registration/view carrier: exact `PFNWOWHANDLERSIN`/`PFNWOWHANDLERSOUT`
layout, one shared ABI view, and an explicit worker thread/domain binding.
The original client registration body is the semantic template.  The missing
NT4 USER/CSRSS producer remains a documented stopping boundary; its finite
standalone replacement must live outside the mirror and may be published only
once every output slot used by the selected WOW32 closure is valid.

## Consequence for the PMODE32 clock and shared view

The immutable profile has two separate, mandatory addressability contracts.
`wow16gpsi` is a DWORD to an original `SHAREDINFO` view (then to its
`SERVERINFO` and handle table), while `GetTickCount`/`GetCurrentTime` reads
the fixed linear `7FFE0000h` clock address.  The second requirement cannot be
met by returning a low SAS address for the first one.

The earlier CCPU fixture demonstrates that the unchanged CPU40 page walker
can translate a read-only `7FFE0000h` page when a test supplies CR3 and sets
CR0.PG.  That is a capability proof only.  The live `MS_bop_1` observation
above proves that this product's reached DOSX/WOW path has neither condition.
It follows that the old proposal to append one PTE to an assumed live DOSX
directory is invalid for the current path; its fixture must not be presented
as a product mapping implementation.

There are therefore only two source-consistent routes still worth evaluating:

1. establish a complete worker paging lifecycle at the existing DPMI
   protected/real transition owners, with an identity map of every active SAS
   page plus the clock and client-view mappings, then withdraw PG before every
   original real-mode transition; or
2. find a separate original non-paged PMODE32 carrier for the fixed clock.

The pinned source/profile already rules out a third option: passing a modern
USER32 pointer, a low zero-filled structure, or a one-off BOP mapping.  Route
2 has no identified owner so far.  Route 1 is not admitted merely because the
fixture works: it must first prove normal DOSX transitions, descriptor
continuity, stale-translation withdrawal and the immutable guest read on the
actual worker path.  Until then S41 cannot claim the clock or full USER client
view complete.

## DPMI transition confirmation

The same default-off observation was then placed in the original DPMI32
transition owners, not in an adapter.  A second isolated `WRITE.EXE` launch
recorded the following complete reached sequence (repeated transitions are
preserved intentionally):

```text
53:01-protected cr0=00000011 cr3=00000000 csip=00cf:1101
bop-fd-real     cr0=00000010 cr3=00000000 csip=d1f5:118f
53:01-protected cr0=00000011 cr3=00000000 csip=00cf:1101
bop-fd-real     cr0=00000010 cr3=00000000 csip=d1f5:118f
53:01-protected cr0=00000011 cr3=00000000 csip=00cf:1101
bop-fd-real     cr0=00000010 cr3=00000000 csip=d1f5:118f
53:01-protected cr0=00000011 cr3=00000000 csip=00cf:1101
bop-fd-real     cr0=00000010 cr3=00000000 csip=d1f5:118f
53:01-protected cr0=00000011 cr3=00000000 csip=00cf:1101
bop-fd-real     cr0=00000010 cr3=00000000 csip=d1f5:118f
53:01-protected cr0=00000011 cr3=00000000 csip=00cf:1101
```

`53:01-protected` is `DpmiCpu40SwitchToProtectedMode`; `bop-fd-real` is the
original real-mode return route.  Across every reached owner, CR0 changes
only between real mode (`10h`) and protected mode (`11h`), and CR3 remains
zero.  This eliminates the possibility that an earlier normal DPMI switch
creates then silently removes a usable DOSX page directory before WOW
registration.  It also turns route 1 above into a new, explicitly bounded
standalone paging adaptation—not a restoration of an existing OpenNT DOSX
page-table lifecycle.  No such adaptation is implemented by this evidence
change.

The transition trace additionally records the currently loaded DS cache as
`base=000ca010h, limit=0000ffffh`; the reached `MS_bop_1` boundary records
`base=0009e610h, limit=0000ffffh`.  Those are DOSX's current low, 64-KiB
descriptors, not the PMODE32 `KGDT_R3_DATA` flat descriptor which USER16 will
load later.  They therefore cannot carry a `7ffe0000h` read.  This does not
pretend that the un-reached later USER16 load was observed; instead its
required shape is independently source-proven: OpenNT
`base/ntos/ke/x86/x86init.c` initializes `KGDT_R3_DATA` as flat user data, and
the immutable USER16 code explicitly loads selector `23h` before its fixed
address read.  A segment-base wraparound would violate that original contract,
so it is not an admissible standalone substitute.

The fixed clock's original producer is also outside MVDM: OpenNT
`base/ntos/init/init.c` sets `SharedUserData->TickCountMultiplier`,
`base/ntos/ke/miscc.c` updates `TickCountLow`, and
`base/ntos/mm/mminit.c` creates the read-only user double mapping.  Those
kernel/MM dependencies cannot be recursively imported.  Their finite
observable contract—read-only tick and multiplier at the fixed guest linear
address—is the only part a future worker-domain binding may reproduce.

## Admissible worker page-domain design

The evidence leaves exactly one implementation shape compatible with the
unchanged PMODE32 consumer and the source-policy stopping boundary.  The
worker may own a finite CCPU page domain, but only as a replacement for the
kernel's *observable mapping* contract; it may not become a private kernel,
USER server, page-fault handler, or alternate DOSX executor.

1. Allocate page-directory/table storage through original
   `nt_mem.c::VdmAllocateVirtualMemory`, in the active worker session.
2. Identity-map every committed SAS page needed by the selected worker before
   setting PG.  This preserves all DOSX/CCPU physical addresses rather than
   translating only the clock call site.
3. Register one worker-owned native clock backing page through the existing
   original `VdmAddVirtualMemory` path, and map it at `7ffe0000h` as
   present/user/read-only.  The host publisher is the sole writer; guest write
   translation must fail.
4. At the existing original DPMI transition owners, set CR3 and PG only after
   PE has been established; clear PG before the original code clears PE; on a
   later protected transition restore the same domain.  Rewriting CR3 is the
   existing CCPU TLB invalidation operation.
5. On every setup failure, leave the original non-paged state unchanged.  On
   worker/session teardown, stop publishing, remove the external mapping and
   release all allocations before the session is released.

The existing bounded CCPU fixture already proves steps 2--4 mechanically:
identity fetch/descriptor reads, read-only `7ffe0000h` translation, native
backing visibility without BOP, CR3 invalidation, and PG-before-PE withdrawal.
It is not production evidence.  Production admission additionally requires
the normal DOSX transition trace, failed setup rollback, guest write refusal,
clock progression while guest code runs, re-entry after real-mode return, and
worker teardown without retained backing.  Only after those gates may the
same page domain carry the separate S41 USER client view and later S44's full
registrar.

The current S41 formal graph rebuilt and ran that fixture as
`ccpu-high-linear-page-test.exe`, exit 0.  Its legacy host stdout is not a
reliable marker channel (the run prints its setup diagnostics to stderr and
the original host I/O cohort owns stdout), so the recorded evidence is its
exit status plus its checked source assertions, not absence or presence of a
console banner.  This confirms the mechanical precondition after the current
CPU40 composition; it does not waive any of the production gates above.

### First worker binding result

The first bounded worker binding was then selected into the formal x86 graph.
It uses only original `VdmAllocateVirtualMemory`/`VdmAddVirtualMemory` and
CPU40 CR0/CR3 operations; the mirror-side call sites are the existing WOW BOP
and original DPMI real/protected transition owners.  An isolated launch of
`run16.exe system32\WRITE.EXE` recorded:

```text
wow-bop cr0=00000011 cr3=00000000 csip=01c7:aebc ...
wow-bop cr0=80000011 cr3=00900000 csip=01c7:aebc ...
```

Thus the real worker creates a nonzero CCPU page-directory and enables PG at
the intended boundary.  The requested `run16` process subsequently returns
zero; that result does not claim USER registration or USER16 acceptance.
This is a page-domain establishment result only: USER16 has not yet executed,
and page-domain teardown, write refusal, clock progression and DPMI re-entry
remain explicit S41 gates.

No guest binary was changed.  No result above constitutes WRITE acceptance.

## Descriptor-domain collision

The worker cannot install the original kernel flat selectors by overwriting
the active DOSX GDT.  The immutable WOW `fastwow.asm` uses
`KGDT_R3_DATA|RPL3` (`23h`) for DS/ES/GS and `KGDT_R3_TEB|RPL3` (`3bh`) for
FS.  In the active WOW DOSX descriptor domain, however, `pmdefs.inc` assigns
the corresponding GDT entries to `SEL_PSP=20h` and `SEL_DXDATA=38h`.  The
current CPU40 `DpmiCpu40SwitchToProtectedMode` installs that DOSX shadow as
the active GDT immediately before `53:01` makes its selectors live.

Therefore directly writing flat descriptors at indices 4 and 7 would corrupt
the original DOSX PSP and data selectors.  This rules out the apparently
small "add two descriptors" patch as both incorrect and non-minimal.

The source-consistent standalone boundary is a separate, worker-owned WOW
descriptor view: copy the source-published DOSX table for continuity, overlay
only the kernel-visible flat data and TEB entries in that view, select it only
while an original WOW callback/worker transition requires those selectors,
then restore the untouched DOSX shadow before returning to DOSX execution.
That is the finite observable part of the original monitor context switch;
it is not a rewrite of DOSX or a modification of immutable guest media.
S41 must prove entry, return, stale-view withdrawal and cleanup before this
view can carry the later USER shared view.  No descriptor implementation is
claimed by this observation alone.

The generic kernel GDT initializer creates `KGDT_R3_TEB` with base zero and a
one-page limit (`ke/x86/x86init.c`), but the original x86 context-switch owner
subsequently rewrites that descriptor's base to the current TEB
(`ke/x86/ctxswap.asm`).  Thus a worker-domain view must use a one-page,
current-worker guest TEB projection as the active `3bh` base, whose `+18h`
self field is the guest-linear address of that projection.  It must not use
the template's initial zero base as a permanent runtime value.

The current formal x86 composition does not define the historical `i386` C
preprocessor symbol.  `wow32/fastwow.h` consequently selects
`FASTBOPPING=0`; the native `fastwow.asm` path is not the selected worker
path.  This matters because that assembly intentionally assumes an NT4 native
flat FS can dereference a real host TEB and its `WOW32Reserved` field.  A
standalone CCPU worker must not expose a modern host TEB to guest code.  The
selected non-fast original C callback path instead returns through CCPU.  S41
therefore needs a guest-linear `KGDT_R3_DATA` carrier for the immutable USER
clock/client view, while host-private `NtCurrentTeb()->WOW32Reserved` remains
an adapter TLS carrier for original WOW32 C code.  It does not need to invent
a guest dereferenceable modern TEB merely to revive the unselected fast path.

The pinned USER profile and the original `ntuser/inc/user.h` agree that
`TEB+44h` is the start of an in-line `CLIENTINFO`, not a pointer. Its
task-version, desktop, client-delta and cached-window fields are at
`+50h/+5ch/+60h/+6ch/+70h`. S41 establishes only the exact empty carrier and
TEB `Self` pointer at `+18h`. It must not publish a made-up task version,
desktop, handle count or window graph: those fields have original owners in
S42/S43. The all-zero object domain is a valid pre-publication state, not a
claim of usable USER-session state.

### Bounded implementation staged for verification

The admitted S41 implementation follows the evidence rather than the
withdrawn private-CR3 experiment.  It uses the existing original
`VdmAllocateVirtualMemory` carrier for its tables, identity-maps the current
SAS range, maps one `VdmAddVirtualMemory` clock backing page read-only at
`7ffe0000h`, and clears/restores PG only at reached original DPMI mode owners.
It copies the active DOSX GDT, overlays only selectors `20h` and `38h` in that
copy, and restores the untouched DOSX GDT before real-mode withdrawal.  Its
guest view contains only source-proven pre-publication data: `SHAREDINFO.psi`,
`SHAREDINFO.aheList`, and `TEB+18h` self; client and object-producer fields
remain zero.

`tests/mvdm-host/wow_page_domain_fixture.c` now passes in the formal x86
composition (`wow-page-domain-test.exe`, exit 0).  It proves rejection before
the session/protected-mode boundary, rejection while A20 wrapping is enabled,
read-only translation at `7ffe0000h`, clock publication progress, DOSX GDT
selection before WOW activation, correct WOW descriptor encoding, PG
withdrawal/re-entry, and session teardown with no retained view.  The clock
test deliberately observes the selected CCPU translated read carrier rather
than introducing another guest executor: the existing
`ccpu-high-linear-page-test.exe` supplies the independent real instruction
fetch/read/return proof and also exits 0 on the same formal composition.

The A20 result is material: `VdmAddVirtualMemory` assigns the native clock
backing above one megabyte.  With the original SAS A20 wrap enabled, that
physical address would fold below one megabyte and silently read the wrong
page.  The page-domain implementation therefore rejects creation while A20
is enabled; it does not alter A20 itself.  DOSX/XMS remains the original owner
of that transition.  This closes the focused S41 domain fixture gate, not the
remaining product `WRITE.EXE` registration, teardown, and 17-route acceptance
gates.

## Short-path isolated worker confirmation

The earlier `87` result from a package under the deep build directory was not
a product failure: original `BaseCheckVDM` rejects a DOS current directory
longer than its 64-character contract.  The same formal binaries were copied
unchanged to a short, isolated test package under `build/M0-T420/S41/`.
`run16.exe command.com /c
ver` returned `0` and printed `MS-DOS Version 5.00.500`; this proves the
three-program baseline before the WOW observation.

From that same package, `run16.exe system32\\WRITE.EXE` classified the image as
`BINARY_TYPE_WIN16`, created a worker and reached the original WOW BOP.  The
default-off trace records the exact before/after state:

```text
wow-bop            cr0=00000011 cr3=00000000 csip=01c7:aebc
wow-domain-entered cr0=80000011 cr3=00900000 csip=0000:0000
wow-bop            cr0=80000011 cr3=00900000 csip=01c7:aebc
```

This establishes the selected architecture precisely: normal DOSX does not
provide paging; S41 creates its worker-owned domain only at the reached WOW
boundary, after the original owner has disabled A20 wrapping.  It is neither
a pre-DOSX private-CR3 experiment nor a guest modification.  The isolated
processes are test-only and are removed after the remaining S41 regression
run; `O:/winnt` and all guest media remain untouched.

## S41 formal regression

The formal `run16.exe`, `basesrv.exe` and `ntvdm.exe` were deployed together
to `O:/winnt` only after that package was idle; all three deployed SHA-256
values matched `build/M0-T420/S41/w1-composition-r1`.  The current
`console-startup-observer.exe` was rebuilt from its selected source before
the run: the prior deployed observer predated its
`--observe-console-line-delay-ms` parser and incorrectly forwarded that
observer-only switch to COMMAND.  This was a test-binary deployment defect,
not a guest or product failure.

`Verify-CommandExitStatus.ps1` with prefix
`s41-domain-published-r2` then passed every one of its 17 text-gated routes:
interactive and nested COMMAND, repeated/direct MEM, native streams and EOF,
direct and nested `/c`, guest exit status 7, and EDIT return.  The summary is
`O:/winnt/logs/s41-domain-published-r2-summary.json`.  This regression
gate does not promote WRITE to a passing application workload; it establishes
that the S41 worker-domain binding does not regress the previously accepted
DOS product surface.
