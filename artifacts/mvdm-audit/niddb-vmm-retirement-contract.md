# NIDDB retirement: original VMM and unproved provider ordering

Scope update: the internal NIDDB candidate is held outside this T. The
selected-object fixture and three-VM marker below are diagnostic evidence for
that held candidate, not a required passing gate for the restored original
object. Their historical passing result must not be attributed to the current
candidate. Actual VxD provider connection remains a separate in-scope question.

This is source and retained-evidence research, not a guest execution test or a
guest change. The original VMM control-call declarations inspected below are
read-only comparison material; they are not a build, link or runtime dependency.
Their local source is
`O:/repos.hobby/ms-dos-archaeology/v6.0/install/include/vmm.inc`, SHA-256
`062898D351565889FA7520CBC4881DA74CA35C24E66AA89B1FF261F11C9CB608`.

## Original VMM control-call evidence

* Lines 1193–1215 define `Sys_VM_Init`, `Sys_VM_Terminate` and `System_Exit`.
  The normal system-VM termination call is not made for a crash exit;
  `System_Exit` is described for normal or crash Windows exit and says the
  instance snapshot has already been restored. These declarations do not
  establish the INSIGNIA.386-specific byte update or that a failed
  `Sys_VM_Init` necessarily reaches `System_Exit`.
* Lines 1222–1242 say `VM_Critical_Init` and `VM_Init` can fail through carry,
  after which the VM becomes non-executable and is destroyed. The normal
  `VM_Terminate` call is made while the target VM is still active and permits
  `Simulate_Int` and `Exec_Int` activity.
* Lines 1243–1271 define `VM_Not_Executeable`: `EBX` identifies the target VM,
  `EDX` carries flags, and `Simulate_Int`/`Exec_Int` activity is not allowed.
  `VNE_Nuked` explicitly denotes a VM destroyed while active; normal
  `VM_Terminate` need not precede that path. `Destroy_VM` can occur later.

These are VMM-level contracts, not evidence of a particular Insignia VxD's
implementation, BIOS data writes, or post-notification device-BOP execution.

## Host path and evidence limits

`src/mvdm/softpc.new/base/bios/virtual.c` receives the control notification
through `virtual_device_trap`, matches the target by `EBX`, invokes the
termination callback with its instance active, and retires that instance.
`virtual_swap_instance` subsequently reads `BIOS_VIRTUALISING_BYTE` for
virtualized device BOPs. `src/mvdm/softpc.new/base/bios/bios.c` routes BOP B1
directly to `virtual_device_trap`, while the mouse/device wrappers call
`virtual_swap_instance` before their actual handler. Thus the VMM target
handle, the last host instance and the BIOS byte cannot be assumed identical.

The unpublished DIV-467 candidate restores a surviving instance after
retirement. If a later device BOP is hypothetically dispatched with the ID of
the retired slot, the original `virtual_swap_instance` contract dereferences
that empty slot. The VMM declarations show active-VM destruction is a valid
control-call case, but do **not** prove that the retired byte remains visible
to a subsequently executing device BOP. That stale-byte sequence is a modeled
condition, not a reproduced guest bug. The current object fixture exercises a
surviving system instance and callbacks, but does not establish VxD-side byte
publication or run an actual device BOP under that condition.

The [Mouse31 acceptance review](mouse31-acceptance-review.md) records an
earlier guard that silently skipped a stale-byte swap. It was withdrawn because
the void swap API returns to the BOP wrapper, which would then execute the
device operation using another VM's state. It must not be reinstated as a
successful recovery without proving the intended provider and failure contract.

The prior [external-provider search](../../docs/etc/evidence/m0-t422-s2-user-client-lifecycle-ledger.md)
found neither original `INSIGNIA.386`/`SPCMSWD.DRV` binaries nor their source
in the pinned original trees, local SoftPC comparison tree, selected guest or
available local assets. A new filename scan of the current repository,
`O:/w31/SYSTEM`, `O:/winnt` and `O:/assets` likewise found no matching
INSIGNIA file. Absence by filename does not prove absence in unsearched
archives or rule out a different provider. The user-owned `O:/w31/SYSTEM.INI`
previously showed no explicit `INSIGNIA.386` entry; that observation does not
prove no driver can send BOP B1.

To establish the missing ordering, a provenance-verified original provider or
an actual VxD control/byte witness must show the precise VM handle, current
virtual byte, callback and device-BOP sequence around `VM_Not_Executeable`.
Until then, the existing failure condition and possible alternative repairs are
design questions; neither a real guest failure nor a safe fallback is proven.

A selected-object multi-VM teardown control now creates a system VM and two child VM instances before either System_Exit or NIDDB_System_Reboot. Each path produces exactly three termination callbacks, each with its own distinct instance pointer, then restores the initial snapshot and unlocks allocations. The actual cached candidate object and a source-derived unmodified baseline object both pass (`s11-foundation-evidence/niddb-multi-vm-exit`); this is a compatibility control for ordinary all-instance teardown, not a regression witness or a guest VxD ordering test. Registers and provider notifications are supplied by the fixture.

A further selected-object control creates three distinct VM instances, selects the second, retires the first inactive instance, then creates a replacement in the freed slot and verifies switches between the surviving and replacement generations before all-instance cleanup. The cached candidate passes; the source-derived committed baseline fails immediately because the master pointer is left at the retired first instance (`s11-foundation-evidence/niddb-three-vm-reuse`). The test compares pointer values without dereferencing the retired allocation. This establishes a three-instance/slot-reuse host counterexample under controlled notifications; it does not establish how INSIGNIA.386 updates its byte during a natural guest lifecycle or authorize a stale-ID fallback.

The three-VM inactive-retirement and reused-slot sequence is now part of the actual `tests/mvdm-host/niddb_object_contract_fixture.c` and the existing `verify-softpc-object-contract.ps1 -Contract niddb` gate requires its explicit marker. The formal gate passed with the cached candidate object, including the existing wrong-pointer-width ABI negative; an explicit source-derived baseline object failed the earlier inactive-retirement assertion as expected. Formal receipts and output are in `s11-foundation-evidence/niddb-three-vm-reuse`. This is still a controlled selected-object test, not installed-guest VxD sequencing or full delivery acceptance.
