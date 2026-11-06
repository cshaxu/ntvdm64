# Minimal NIDDB companion (candidate)

Independently authored T479 S6 addon, admitted by the owner on 2026-11-06.
It is not the original INSIGNIA.386 binary and does not implement its full ABI.
Output: assets/release/insignia.386 after acceptance; never host system32.
Static Win3.1 loading, real child VM retirement and normal exit are verified
in controls053--055 and063. Win95 desktop mouse input is exercised in064;
its exit1 matches companion-disabled control060, not a zero-exit acceptance.

## Ownership and source evidence

The existing original `src/mvdm-host/softpc.new/base/bios/virtual.c` owns
NIDDB allocation, snapshot copying, instance selection, retirement and reset.
The original mouse provider puts callback addresses/masks, cursor state and
saved interrupt registers in MM_INSTANCE_DATA. This addon contains none of
that state or algorithm. It supplies missing VMM lifecycle notifications over
the existing BOP B1, not a new transport or task/I/O provider.

The BIOS_VIRTUALISING_BYTE contract is at linear04AC in original bios.h.
Before VMM takes its instance snapshot, _AddInstanceItem registers that byte
as ALWAYS_Field. Sys_VM_Init3 and VM_Init9 ask original NIDDB for an identity,
then write it through that VM's CB_High_Linear mapping. Existing device BOP
wrappers select the original instance. Create_VM clears the per-VM registration flag before fallible VM_Init.
VM_Not_Executeable0B retires only a successfully created instance; it cannot
inherit the system VM registration flag when another device aborts creation.

Original Device_Init1 is deliberately not forwarded: its selector registration
is a separate unsupported SoftWindows display contract, not needed to select
mouse state. Normal exit and reboot both need the original NIDDB reset. The
candidate invokes the existing Device_Reboot_Notify17 NIDDB-only reset body
(deallocate instances, restore snapshot, reset version). Original System_Exit5
also invokes ClearInstanceDataMarking, whose project adapter is an explicit
unsupported VDD path. This companion registers no VDD instance-page marks and
does not pretend to recover that separate product. This bounded reuse must be
verified by normal exit/restart and reviewed before publication.

VMM ABI evidence, read-only (not build dependencies):
- O:/repos.external/ms-dos-6/dev/smartdrv/sdvxd/vmm.inc, SHA256
  28BD6EDABA10434DFCAD26BFCDAB8DBD1A3CC28100CD6910C67397D8C05F92E9:
  control messages, CB_High_Linear, instance descriptor and service ordinals.
- The same tree's dev/emm386/lhvxd/loadhi.asm demonstrates the C-call
  _AddInstanceItem descriptor/reserved-zero argument order.

No original guest is rebuilt or patched. Build with build.ps1 in an owned
background runner and a fresh repository build directory, using the pinned
NASM/Open Watcom tools. Compilation does not establish runtime correctness.
Real Win3.1 identity0->1->0, USER callbacks, child retirement, restart and
normal exit pass, including CLS/VER/DIR with NTVDM64.386. VMM allocation
failure injection and full Win95 VM lifecycle remain unverified. See the
T479 S6 integration evidence; successful compilation is not acceptance.