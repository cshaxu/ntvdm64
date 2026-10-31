# S11 V86 exception reflection: source ownership and remaining boundaries

This review distinguishes code reuse from a source-derived replacement. It
does not claim that the remaining paths have been implemented or verified.

* The selected `dpmi32/dpmiint.c::ReflectV86Int` obtains a real-mode stack
  pointer via `Sim32GetVDMPointer`, writes FLAGS, CS and IP with separately
  wrapped 16-bit stack offsets, commits SP, and then reads the IVT through
  `IntelBase`. It is still called on the existing real-mode path.
* The CPU40/CCPU exception branch in `DpmiFaultHandler` is admitted when
  VM=1 and CR0.PG=0. It does **not** call `ReflectV86Int`: it
  independently computes the same three wrapped word offsets and calls
  `spr_write_word`, then `spr_read_dword` for the IVT. This avoids assuming
  the single host pointer covers an A20 wrap, but remains a mirror change.
  DIV-485 records this boundary; DIV-486 covers failed-handler state rollback.
* The selected original CCPU `c_page.c` supervisor accessors use physical
  memory directly when paging is off. With paging on they use `lin2phy` with
  supervisor permissions and split crossing-page accesses. They can fault;
  their signatures do not return a success status. This source fact alone
  does not establish equivalence to NT4 monitor host-access fault handling or
  establish the correct ordering of partial writes and architectural state.
* The selected `c_bsic.c::setFLAGS` changes IF only when CPL <= IOPL;
  `host_cpu40_set_virtual_if` separately updates the shared fixed-state bit
  under the ICA lock. `inc/vint.h` identifies that bit and its software-CPU
  FCLI/FSTI path. These are distinct representations; treating either as a
  universally interchangeable NT4 VIF policy is not justified by those facts.

The real CCPU unpaged IOPL-3 controls, including wrapped SP and interrupt
versus trap gates, are recorded in [the reflection ledger](s11-swint-frame-recovery.md).
The failed-reflection state tests and preceding coherent release are separately
recorded in [their evidence](s11-v86-fault-decline-evidence/). Those results
alone do not prove successful IOPL 0–2 reflection, paging, host access failures,
reentrant mapping changes, or a particular live Win386 exception path.

The published correction extends the unpaged path to IOPL 0–2. The selected
software CPU executes guest CLI/STI directly when `MIPS_BIT_MASK` is set;
its interrupt decision uses CCPU IF, and `c_setIF` sets that state directly
even when `setFLAGS` cannot change IF at the guest CPL. For an interrupt
gate the candidate clears both CCPU IF and the existing fixed-state bit;
trap gates leave them unchanged. Real CCPU divide cases 217–228 cover each
IOPL, each gate and initial IF enabled/disabled, including the wrapped stack
in interrupt-gate cases.
The prior selected object declines the same IOPL-0 vector-7 comparison;
the correction passes it. This is a controlled CCPU result, not evidence of
actual Win386 reachability or a general equivalence with every NT4 host
virtualization policy. The 219-case CCPU suite, 16-gate Full suite, WOW
applications and all four Win3.1 mode combinations passed; 86 guest binary
hashes remained unchanged. The tested ten-component package was published to
both destinations with 20 matching hashes. [Evidence](s11-v86-low-iopl-evidence/).

The selected-object fixture now interrupts each of the three stack writes and
the IVT read before the selected access occurs. All four controls preserve
registers and fixed-state IF, while retaining exactly the writes preceding the
failure; see [result](s11-v86-fault-access-failure.txt). This compares the
source-level access order to `PushException` and exercises mocked nonlocal
exit only: it does not prove the host's actual page-fault or exception owner.
The original `instemul.asm` reviewed for this sequence has SHA-256
`6356B416E0942EB3814E4C5A14FF5ED09DB13DC1443842BA51AB9F512522D6E4`.

The adjacent low-IOPL `CD nn` path is a separate unresolved boundary. The
original `trap.asm:V86Intnn` emulates an unprefixed unhooked V86 interrupt,
but sends hooked interrupts to its pass-through owner; the slow
`emv86.asm:OpcodeINTnnV86` has distinct prefix, flag and write ordering.
The previous CCPU `c_main.c` raised #GP before decoding the immediate when CPL
exceeded IOPL; `host_exint_hook` excludes V86 from its protected opcode
monitor. Earlier diagnostic CCPU cases 235–237 installed a test-only #GP consumer and
observed #GP, no software-hook call and unchanged SP for IOPL 0–2; see
[observations](s11-v86-low-int-current.txt). This deliberately intercepted
diagnostic does not establish the real downstream guest result. It is not in
the passing compatibility suite and must not become the desired contract.
The published correction reconnects the matching unprefixed,
unpaged and unhooked DPMI fast route at IOPL 0–2. It saves an IOPL-3 frame
image and clears CCPU IF and fixed-state IF at the original fast-path boundary.
The return IP is passed as data; CPU EIP remains at the faulting instruction
until the reflector has completed the memory accesses and commits the target.
The same real CCPU fixture linked against the previous objects fails all three
selected-owner controls, while the candidate passes them and declined-owner,
hooked, prefixed and initially-clear-IF controls. Controlled selected-object
fault injection records the four fast-path access boundaries. These are
scoped tests; the force-linked fixtures and mocked access failures do not prove
actual NT4 host-memory fault delivery or guest reachability. Paged, prefixed,
hooked and non-DPMI low-IOPL cases retain their prior #GP fallback, not a claim
of full NT4 equivalence. The 240-case CCPU suite, Full 16-gate suite, WOW
application tests and four Win3.1 combinations passed; 86 tracked guest
binary hashes remained unchanged. The tested ten-component package was
published to both targets with 20 matching hashes. [Evidence](s11-v86-low-int-evidence/).

The one-byte INT3 and taken INTO dispatchers previously required IOPL 3
before calling the now IOPL-independent unpaged exception owner. Removing
that guard reconnects the owner at IOPL 0–2; `CD` software interrupts retain
their existing privilege check. A force-linked pre-change `c_main` object
fails the two low-IOPL declining-owner controls; the candidate passes those
and six real-CCPU handler controls, with 225 boundary cases passing in total.
The full 16-gate, application and four Win3.1 combination checks passed for
the released package; 86 guest binary hashes remained unchanged and both
ten-component destinations matched. The CCPU fixture uses force-linked seams;
these controls are not proof of real guest incidence or paged exception
delivery. [Results and release](s11-v86-low-trap-evidence/).

For further recovery, compare the original NT4 `PushException`/V86 interrupt
owner with these specific cases: virtual-IF policy and saved frame flags;
cross-page stack and IVT access with first/second access failures; and the
observable guest and host state when delivery fails after a partial write.
Only reuse the supervisor accessors for a broader admission once those
permissions, ordering and failure ownership are shown equivalent. A direct
call to `ReflectV86Int` is not an equivalent shortcut for wrapped mappings.
