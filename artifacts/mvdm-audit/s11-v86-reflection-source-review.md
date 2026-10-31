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

For further recovery, compare the original NT4 `PushException`/V86 interrupt
owner with these specific cases: virtual-IF policy and saved frame flags;
cross-page stack and IVT access with first/second access failures; and the
observable guest and host state when delivery fails after a partial write.
Only reuse the supervisor accessors for a broader admission once those
permissions, ordering and failure ownership are shown equivalent. A direct
call to `ReflectV86Int` is not an equivalent shortcut for wrapped mappings.
