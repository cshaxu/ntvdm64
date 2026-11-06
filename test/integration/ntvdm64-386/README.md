# Win3.1 guest bridge integration witnesses

`verify-win31-identity.ps1` builds independently authored DOS/Win16 probes,
copies the installation, installs the candidate add-on into that copy and
uses the existing private-desktop runtime matrix. The original installation
is inventoried with hidden/system files and checked unchanged afterward.
The probe checks actual VMM identities, callback registers/stack/flags,
counted waits, blocked-state wake, notification serials and normal retirement.
It is not a replacement guest component or a complete command/I/O test.

`host-wake.c` is an integration-only link seam for independent-thread wake.
Build the production `mvdm_vm_notification.c` with its BOP symbol renamed
to `mvdm_vm_bridge_bop_direct`, `RegisterEOIHook` to `test_register_eoi`, and
`RemoveEOIHook` to `test_remove_eoi`. Link that object and `host-wake.c` before the
normal worker archives in a separate test executable. The archive's ordinary
notification object must not be extracted; verify both entry-point owners in
the link map. The wake queue, ICA/PIC and CCPU objects stay unchanged.

Only the witness's one operation-9 publication is moved to a native thread.
All other BOP operations use the real implementation. The producer also invokes
the original ICA cancellation under the lock before CCPU can consume its
publication. The EOI observation wrapper delegates unchanged and requires an
actual `count == -1` callback during that cancellation; a successful wake alone
does not prove the cancellation branch ran. This tests the cancellation path,
not a complete guest PIC ICW1 reinitialization. There is no host sleep,
timer, guest-memory access or CPU-thread join. At guest shutdown the witness
checks that the producer returned successfully; a live/failed producer fails
the process. This zero-time handle check is a test assertion, not production
polling or lifecycle policy. The source is not selected by product builds.

During an observed VM-retire BOP, the seam queues a wake under the ICA lock
before delegating to the real retirement implementation. It then requires
that the queue be empty and a late publication for that generation be rejected.
This witness assumes no other queued request at that point. During successful
bridge close it reacquires/releases the exact IRQ and installs/removes an EOI
hook to check that both resources were returned. These are test assertions,
not a production resource-stealing or retry path.

Stage the test executable as a separately identified coherent package and
run the same guest witness. Preserve the test package and source hashes in
the stage evidence. A successful test proves independent host-thread IRQ
delivery to a real blocked guest VM, not NTSRV command completion, frontend
registration, original BOP continuation restoration or production acceptance.
Those contracts still need their actual owners connected and verified.

`retire-vm.asm` is a separate test-only VxD (isolated device ID 7f81), never
part of the product driver or release assets. Its API rejects the current and
system VM, enumerates actual VMM handles, and calls Nuke_VM only when the target
has the original blocked flag. `dos-retirement.asm` writes A's identity before
waiting; B destroys A and verifies that a late wake for A is rejected. A must
never return from its wait. The normal Win16 controller waits for all ordinary
VMs to disappear before requesting Windows exit. Supply the assembled helper
through `-RetirementDriver` to select this witness; retain both raw results.

The Nuke_VM ABI is described by the original Microsoft Windows DDK
[Virtual Device Adaptation Guide](https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/).
Its service ordinal and VM_Not_Executeable control ordering are checked against
the read-only Microsoft `ms-dos-6/dev/smartdrv/sdvxd/vmm.inc`. These are interface
research sources only; no source body is imported. Tests exercise Win3.1 itself
and do not infer its behavior merely from the older guide.

`-Reuse` selects a subsequent `dos-reuse.asm` VM after the two ordinary
identity witnesses have disappeared from the driver's actual VMM inventory.
The controller launches its PIF through Win16 WinExec. It requires the new
VM handle to equal one of the earlier handles, with a newer generation;
absence of actual reuse is not counted as success. Old-generation direct and
queued wake API calls must fail, while a new-generation counted completion
must succeed. The 36-byte `VRUS` report is cross-checked against the original
two VM reports, and Windows exits only after the new VM has also retired.
This uses the normal candidate package, not the host-thread test seam.

`-InitializationFailure` uses a separate `host-init-failure.c` link seam:
rename only the notification BOP entry to `mvdm_vm_bridge_bop_direct`, link
the test entry first, and retain the real notification/IRQ implementation.
It fails arm after host reservation and VPICD ownership. The real guest driver
must close the bridge; the seam then reacquires/releases the exact IRQ and
EOI hook and writes a one-shot marker to the runner-owned report path.
The controller requires the failed driver to expose no successful identity,
and the normal Windows startup/exit checks still apply. No DOS witnesses are
launched in this negative case. Marker, matrix and API result are all required;
absence of a functioning driver alone is not a passing failure test.
