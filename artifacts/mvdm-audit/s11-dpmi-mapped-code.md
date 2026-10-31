# S11 DPMI instruction mapping: selected-CCPU counterexample

`DpmiFaultHandler` calls `DpmiEmulateInstruction` for #GP/#UD. The latter
uses `Sim32GetVDMPointer(CS:0)` and adds EIP to the resulting host pointer.
On selected CPU40, `Sim32pGetVDMPointer` resolves the selector, then
`c_GetLinAdd` translates only that base address; `c_GetPhyAdd` returns a
physical-storage pointer. The pointer plus EIP is not necessarily the
translation of guest linear base plus EIP. Original NT4's native VDM address
space instead supplies a linear process mapping.

The actual selected `dpmiint.obj`, `sim32.obj` and `ccpusas4.obj` are linked
to the CCPU monitor fixture. Case 93 maps CS base page F0000h normally but
remaps instruction linear F1000h to physical 6000h: the expected original
`8E E0` compatibility operation declines with EIP unchanged. Case 94 uses
the same selector and instruction at identity-mapped F1000h: it succeeds
and advances EIP. Case 95 places `8E` at F0FFFh and `E0` at the first byte
of a noncontiguously mapped next page: it declines. `IntelBase`, the LDT
shadow and DOSX code selector are initialized explicitly; the fixture uses
real selected CCPU page translation and DPMI function, but includes existing
`/force:multiple` test seams and does not show a real guest taking this
path. An early fixture attempt failed because its synthetic `IntelBase` was
not initialized; that setup failure is not treated as a product crash.

`tests/observation/verify-dpmi-mapped-code-known-gap.ps1` preserves these
non-passes as a diagnostic; success of that diagnostic means the known gap
was reproduced, not that the product passes. The normal suite does not count
these failing cases as accepted behavior.

A correct integration must fetch each instruction byte using the current
linear mapping, preserve original descriptor/default-size and instruction
read order, handle a page boundary, and distinguish translation failure from
a valid pointer to physical zero. `c_GetLinAdd` substitutes physical zero
for a failed translation, so merely moving the EIP addition before this
call would conceal a failure. The NT4 host exception owner and nested CCPU
fault/rollback contract are still unresolved. Do not substitute generic
#GP, assume contiguous mapping, or use an unchecked fallback to claim a
complete repair. The controlled mismatch is established; its scope as an
original software-CPU limitation versus an S11 connection gap requires the
source and reachability qualification below. No production code or guest was
changed by this witness.

The hook's broad installation path is not merely hypothetical: the unchanged
CODE32 guest run recorded `dpmi-bop index=0c` in the trace described in
[the CODE32 witness](s11-foundation-evidence/code32-prebuilt-search/review.md).
The [retained trace excerpt](s11-dpmi-mapped-code-evidence/code32-hook-entry.txt)
also records the full trace hash.
The selected dispatch table maps index 0c to `DpmiDpmiInUse`, which calls
`EnableIntHooks`; `fDpmiHookInts` is initially TRUE and the selected CPU40
branch installs `DpmiFaultHandler`. The original non-i386 path has the same
installation sequence. This establishes a real guest's entry into the hook
installation chain, not that it subsequently executes a #GP/#UD under a
noncontiguous instruction mapping or that the optional report is complete.

The same controls were extended through the actual `DpmiFaultHandler(13,0)`
(cases 96–98) and through `host_exint_hook(13,0)` with that handler installed
(cases 99–101). Both produce the same identity pass and remapped/split-page
non-passes. The latter proves the selected monitor's actual fallback reaches
the affected function in this controlled setup; it does not show that a
released guest reaches this configuration. The fixture links selected
objects with its existing `/force:multiple` boundaries. Outputs are retained
in [actual-chain-result](s11-dpmi-mapped-code-evidence/actual-chain-result.txt);
the normal 91-case monitor suite also passed after relinking. No runtime
implementation was changed.

## False-positive controls

The reverse controls expose a stronger failure than missed emulation. Case
104 places a nonmatching byte in the actual remapped instruction page but
`8E E0` at the old physical-pointer alias; the current decoder incorrectly
reports success and advances EIP from `1000` to `1002`. Case 106 puts a
nonmatching second byte on the actual noncontiguous page, with `E0` at the
stale alias; it incorrectly advances from `0FFF` to `1001`. Identity control
105 declines the nonmatching opcode with EIP unchanged. These selected-object
fixtures are deliberately synthetic and do not establish real guest frequency.
The expanded 12-case known-gap diagnostic and existing 93-case monitor suite
were built with the cached x86 worker tree and passed as diagnostics on
2026-10-30; diagnostic success includes the expected product non-passes.

## Exception-owner constraint on a direct CPU read

An isolated **host-only experiment**, not a product candidate, replaces the
pointer fetch with `sim32_effective_addr` plus the existing CCPU
`spr_read_byte`, fetching only the opcode bytes used by the existing decoder.
All 12 direct, handler and hook cases above then returned the expected
results, including both false-positive controls; the same experimental image
also passed the existing 93 monitor controls. The linker map identifies
the experimental `DpmiEmulateInstruction`, original SIM32 resolver and
original CCPU page reader; the inherited `/force:multiple` fixture seams
remain. [Patch](s11-dpmi-mapped-code-evidence/linear-read-experiment.patch)
and [results](s11-dpmi-mapped-code-evidence/linear-read-experiment.txt) record
the experiment. It does not establish video-backed reads, selector changes,
page-failure delivery, reentry, CR0 forms, all operand lengths, or complete
original semantics; in particular it can fault through CCPU while handling
an existing exception. It must not be released as a finished fix.

Case 107 now exercises this concern under `c_cpu_simulate`: it loads a CS
descriptor, changes its table entry after loading the hidden CS cache, and
executes an invalid opcode from the old mapped code page. The new descriptor
selects a nonpresent page for DPMI's decode. A test-only exception wrapper
calls the real `DpmiFaultHandler` for #UD and records #PF, then advances EIP
to a sentinel so the controlled fixture terminates. With the original
selected DPMI object the wrapper observed no #PF; with the experimental
`spr_read_byte` object it observed nested vector 14 at EIP `1000`. Both then
reached the sentinel. [Results](s11-dpmi-mapped-code-evidence/nested-fault-experiment.txt).
This proves the proposed implementation changes nested-fault behavior in
this fixture; the wrapper is deliberately synthetic and does not determine
the correct NT4 user-exception delivery or real guest reachability. An
initial baseline fixture crashed before its LDT pointer was initialized; that
setup error was corrected and is not counted as a product failure.

Case 108 repeats the changed-descriptor/nonpresent-page setup with a trapped
port instruction producing #GP instead of #UD. In the same synthetic wrapper,
the original object returns to the sentinel with `doing_contributory=0`;
the experimental direct read produces a nested #PF. A second trapped port
instruction then reaches the synthetic wrapper as vector 8 (#DF); the
original object instead completes both traps without #DF. The wrapper skips
that second trap so the fixture can finish, at which point the experimental
image still reports `doing_contributory=1`. This demonstrates the bad
nonlocal-exit consequence in the controlled CPU path, not a real guest double
fault or the exception that the original native monitor should deliver. The
retained CPU read experiment must not be integrated without an explicit
owner and recovery for that state.

A second isolated experiment brackets the attempted linear fetch by saving
`doing_contributory`, temporarily clearing it, and restoring it on normal
return, analogous to the existing monitor descriptor lookup. In case 108
the test wrapper now sees two #PF callbacks for the two traps, no #DF, and
`doing_contributory=0` at the sentinel. [Patch](s11-dpmi-mapped-code-evidence/linear-read-guard-experiment.patch),
[three-way results](s11-dpmi-mapped-code-evidence/nested-fault-experiment.txt).
That removes the demonstrated stale-state consequence in this synthetic
setup, but it does **not** resolve the change from no #PF on the existing
path to #PF dispatch in the experiment, nor establish that clearing the
fault state reproduces NT4's host exception owner. This remains an experiment
outside the runtime and does not satisfy the requested exception contract.

The existing CPU40 `spr_read_byte` is a real supervisor linear read: it
calls `lin2phy` when paging is enabled and then `phy_read_byte`. Unlike
`xtrn2phy`, it can raise a guest page fault and update paging state. It is
therefore a better behavioral reference than an unchecked host pointer, but
it is not a drop-in replacement for the host DPMI decoder. The selected
`contributory_exception` sets `doing_contributory` before calling
`host_exint_hook`; that hook can enter `DpmiFaultHandler`, which has already
cleared guest IF and TF before calling `DpmiEmulateInstruction`. A nested
`spr_read_byte` page fault enters the CPU exception machinery while the
original guest exception is still in progress, and `c_cpu_continue` can leave
that machinery without returning to the DPMI caller. Consequently neither a
plain CPU read nor a temporary save/restore around it proves the native NT4
host-monitor exception contract. This is a source-level conclusion, not a
tested reproduction of every nested-fault outcome.

The implementation must separately establish ownership of the instruction
fetch failure, its paging side effects, and restoration of the original fault
context. A successful byte-by-byte translation test alone cannot establish
those failure semantics. In particular, retaining the old pointer as an
unchecked failure fallback would preserve the demonstrated wrong-page access;
it is not an acceptable complete repair of this boundary.

The original non-i386 `dpmiint.c` also contains the base-pointer-plus-offset
operation, but its host address mapping cannot be assumed identical to the
selected CPU40 SAS mapping. The native x86 `Kt0e8` kernel-monitor route is
not automatically the exception owner for that separate user-mode DPMI
routine. `DpmiUnhandledExceptionHandler` handles a guest DPMI fault frame;
its name alone does not make it the owner of an exception in the host decoder.
The selected `ThreadStartupRoutine` has a host unhandled-exception filter,
but its presence likewise does not prove a recoverable guest exception
handoff. These are distinct paths and must not be conflated.

Further original-source reconciliation narrows the earlier attribution:
the OpenNT software-CPU build's original `base/inc/sas.h` maps
`NtGetPtrToLinAddrByte` to `c_GetLinAdd` under `NTVDM && CCPU`; its original
`host/src/sim32.c` translates the selector base and calls that macro; and
its original `ccpu386/ccpusas4.c` translates that one linear address and
returns a physical-storage pointer, substituting physical zero on a failed
translation. The original non-i386 `DpmiEmulateInstruction` then adds EIP
to that pointer. These are original source operations, not a pointer-addition
introduced by the project's monitor adapter. For ordinary linear physical
storage this source has the same structural assumption as the selected
fixture. Current `c_GetPhyAdd` additionally supports project-owned physical
bindings, so source similarity alone is not a complete controlled run of
the historical product or proof of all device mappings.

Accordingly the counterexample must not be presented as an established new
regression caused by S11 or automatically repaired as an unrelated original
component defect. The user-directed scope distinguishes original internal
defects from a genuinely missing adaptation needed to connect a required
behavior. Actual supported-guest reachability and whether the NT4 x86
monitor contract, rather than the original software-CPU contract, must own
this specific DPMI operation remain to be established. Until then the
experiment is evidence, not an authorized assertion of a source-level repair
or an excuse to claim complete semantic parity.

A source sweep also found segment-base-plus-stack-offset accesses in
`DpmiUnhandledExceptionHandler`, `DpmiFaultHandlerIret16` and
`DpmiFaultHandlerIret32`. Their potentially noncontiguous frame accesses are
related mapping concerns, not evidence that the instruction-fetch fixture
reproduces them. They need separate read/write, frame-order and lifetime
analysis before a general-purpose fix or a completeness claim.
