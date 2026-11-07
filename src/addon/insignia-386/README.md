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
as ALWAYS_Field. Sys_VM_Init3 and Create_VM7 ask original NIDDB for an identity,
then write it through that VM's CB_High_Linear mapping. Existing device BOP
wrappers select the original instance. Create_VM clears the per-VM registration flag before fallible VM_Init.
Create_VM invokes the original NIDDB VM_Init9 operation before the guest's
Init_VM/BIOS phase can mutate device state inherited from the system VM.
The later VM_Init notification does not allocate a second instance.
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
## S3 VMD callback candidate

The original retail Win3.1 VMD queries INT 2F AX=1607/BX=000C:
CX=0 asks whether mouse state is already instanced; CX=1 obtains AX=0,
DS:SI pointing to the real-mode callback. This companion supplies that
contract through Hook_V86_Int_Chain. A four-byte shared V86 allocation holds
only the original BOP BF followed by RETF. The original BIOS BOP wrapper
selects NIDDB state before mouse_EM_callback; this addon adds no mouse queue,
position, button or callback state. Other INT 2F requests chain unchanged.

The allocation is paragraph aligned, not instanced, and lives for the static
VMM lifetime. Allocation/hook failure fails initialization. Addresses outside
the representable low-memory segment range are rejected, not truncated.
The callback is installed at Sys_Critical_Init before VMD Device_Init queries.
VMD must actually be loaded; the isolated acceptance profile explicitly uses
`device=*vmd`. Merely having `mouse=*vmd` was insufficient in the tested profile.

ABI evidence: Microsoft Windows 3.0 DDK Virtual Device Adaptation Guide,
chapters 19 and 21 (PCjs transcription); local original vmm.inc above confirms
service ordinals A8 and 41 and Client_DS/Client_SI offsets 60/4. Retail
WIN386.EXE SHA256 c67e667e25e91c65efb5acddec437da8b586037da7de502228ca8fbd806cf3f8,
VMD initialization object offsets EE/F5 confirm the returned register pair.
Only ABI facts are used; no third-party implementation is imported.
Editor087 proves actual DOSONLY1 guest EDIT mouse File-menu delivery and normal
exit with this addon and no diagnostic TSR. The host package is instrumented;
mouse Exit, native payload mouse and full delivery gates remain open.
