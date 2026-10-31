# S11 V86 exception reflection: source ownership and remaining boundaries

This review distinguishes code reuse from a source-derived replacement. It
does not claim that the remaining paths have been implemented or verified.

* The selected `dpmi32/dpmiint.c::ReflectV86Int` obtains a real-mode stack
  pointer via `Sim32GetVDMPointer`, writes FLAGS, CS and IP with separately
  wrapped 16-bit stack offsets, commits SP, and then reads the IVT through
  `IntelBase`. It is still called on the existing real-mode path.
* The CPU40/CCPU exception branch in `DpmiFaultHandler` is admitted only when
  VM=1, IOPL=3 and CR0.PG=0. It does **not** call `ReflectV86Int`: it
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
The latest failed-reflection state tests and coherent release are separately
recorded in [their evidence](s11-v86-fault-decline-evidence/). Neither result
proves the successful IOPL 0–2 path, paged reflection, host access failures,
reentrant mapping changes, or a particular live Win386 exception path.

For further recovery, compare the original NT4 `PushException`/V86 interrupt
owner with these specific cases: virtual-IF policy and saved frame flags;
cross-page stack and IVT access with first/second access failures; and the
observable guest and host state when delivery fails after a partial write.
Only reuse the supervisor accessors for a broader admission once those
permissions, ordering and failure ownership are shown equivalent. A direct
call to `ReflectV86Int` is not an equivalent shortcut for wrapped mappings.
