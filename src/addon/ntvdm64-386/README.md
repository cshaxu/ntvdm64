# NTVDM64 guest VM bridge

This component owns the independently authored enhanced-mode guest bridge for
T479. It is not an imported Windows component or a hardware V86 monitor.

| Guest | Release artifact |
| --- | --- |
| Windows 3.1 enhanced mode | `assets/release/ntvdm64.386` |
| Windows 95 | `assets/release/ntvdm64.vxd` |

Both formats belong here. Shared implementation stays in this component;
format-specific link settings do not justify separate implementations.
Build products and diagnostic probes belong under the active task's `build/`
directory. Publication copies validated artifacts to `assets/release/`, not
the host `system32` directory. Original guest binaries remain unchanged.

## Current state

The S4 candidate uses NASM OMF and Open Watcom LE, with shared resident code
and data segments. Both output names currently contain the same statically
loaded image; dynamic load/unload is not supported. The Win3.1 .386 is published
as a candidate with its verified bridge scope in addon-manifest.json. Win95
.vxd publication remains deferred. Complete interactive I/O is not accepted; the S5 candidate scope is below.
Historical experimental drivers are comparison evidence, not build inputs.

The S5 candidate now registers the VMM callback with the host BOP entry
and admits independent54:08/0A requests. Win3.1 tests cover two actual native
commands in both exit orders, exact callback state, command-in-flight VM
retirement and actual VM handle reuse. The54:0A input is still test stimulus;
physical guest keyboard, full I/O and nested acceptance remain S6 obligations.
The Win3.1 candidate is published with those limits in addon-manifest.json.

The current candidate allocates VMM-owned per-VM control-block storage, assigns
a 64-bit generation at creation and invalidates execution at
`VM_Not_Executeable`. API operation zero exposes identity for isolated S4
verification only; operation one returns a VMM-allocated V86 callback address.
The callback accepts an actual guest far call and uses `Simulate_Far_Ret` to
return. It is not a post-BOP continuation, whose stack lacks that return frame.
Operation two waits on the caller VM's counted VMM semaphore; operation three
signals a live VMM handle with matching 64-bit generation. Neither operation
performs host RPC. Semaphore allocation belongs to VM creation and destruction
to `Destroy_VM`; `VM_Not_Executeable` first rejects admission. The current
Win3.1 probe proves two precompleted signals are consumed by two waits and a
stale generation is rejected. A second Win3.1 probe also blocks VM A while
VM B executes and signals A; the signal reports the original VMM blocked bit
and A's wait-entry state before delivery. Operation four counts executable
VMs using VMM's live inventory. The witness waits for the two DOS VMs to retire
before requesting Windows shutdown. This proves cross-VM wake and normal
retirement, not host IRQ notification or forced destruction. Other operations
fail in that verified candidate. The subsequent Win3.1-verified IRQ candidate adds
operation five to inspect the last physical notification and operation six to
publish an isolated-test doorbell. Its Device_Init reserves a host IRQ through
the signed BOP, registers VPICD, then arms delivery. The physical handler
drains wake identities, validates their VMM generation and signals the matching
semaphore before acknowledging the serial and physical EOI. Wake records are
not command results. System_Exit masks
and releases the reservation before restoring VPICD's default behavior.
The matching ten-component candidate passed actual PIC delivery on the
host-reserved IRQ 15: both DOS VMs observed a serial at least as new as their
publication, with zero notification errors and normal Windows exit. This
does not prove host task registration/completion or an off-CPU producer.
Device ID `7f80` is a temporary
isolated-test identity, not a claimed globally allocated device number. The
candidate is not automatically installed into general guest configurations.
Conflicting private API device IDs remain an unverified compatibility boundary
before general installation; S5 must not infer bridge identity from this ID
alone. IRQ reservation/ownership and rollback have the separate proofs below.

## Build and provenance

Run `build.ps1 -OutputRoot <fresh-repo-build-directory> -Wlink <wlink.exe>`
through the repository's owned background runner. It records source, tool and
output hashes. It neither downloads tools nor edits or installs guest files.

Code is independently authored. VMM service numbers, client-frame offsets,
DDB layout and control messages are ABI facts checked against the supplied
`ms-dos-6/dev/smartdrv/sdvxd/vmm.inc` and OpenNT-4.5
`nt/private/windows/inc16/vmm.h`; these files are read-only research inputs,
not build dependencies. `_Allocate_Device_CB_Area` uses the original C-call
stack convention and initialization-only lifetime. The existing guest VMM
owns allocation, scheduling and VM destruction; this add-on does not replace
those implementations. The Win3.1 and Win95 candidate separately passed two
concurrent DOS VM identity checks and 320 callback calls preserving stack,
EBP, EDI and the direction flag on each guest. Win3.1 normal retirement and
queued IRQ wait/wake pass, including an independent native-thread publisher
in a separate integration link. A test-only VxD also destroys a blocked DOS
VM through VMM Nuke_VM: the production retirement path removes its queued
wake and rejects a late publication for its old generation. Normal shutdown
is followed by test-only assertions that the IRQ reservation and EOI hook
can be acquired again. These assertions are not linked into the product.
The subsequent normal-package reuse witness creates a third DOS VM after
the first two retire, observes B's actual handle with generation 4 instead
of 2, rejects both old-generation wake APIs and consumes a valid new signal.
The actual initialization rollback witness rejects arm after IRQ/VPICD
acquisition; the driver closes the host resource and ceases exposing an API,
while Windows still starts and exits normally. This is one exercised failure
branch, not exhaustive VMM allocator injection. Actual command-result
integration and Win95 wait/wake remain separate obligations. The owner now
prioritizes the complete Win3.1 path; no new Win95 investigation is running.

VPICD descriptor/service ABI is checked against the read-only Microsoft
1988--1990 `OpenNT-4.5/nt/private/os2/client/thunk/include/vpicd.inc`, SHA-256
`2727EC5678B3A6085958D95542764526B25E518070473DCCD580334044DB5661`.
It is research evidence, not an imported source or build dependency.

The [stage plan](../../../docs/etc/operations/m0-t479-guest-bridge-plan.md)
owns the implementation sequence and executable acceptance requirements.

## Win3.1 mouse integration candidate (T481 S3)

The paired authored drivers have distinct owners: INSIGNIA.386 supplies original
NIDDB VM lifetime and the VMD BOP BF callback contract; NTVDM64.386 temporarily
exchanges the VM's INT33 callback while its host command token is live. It
restores the previous callback on admission failure and command completion.
The host validates VM plus token before queuing copied text-cell mouse records.
No original guest binary is modified.

The isolated verified profile loads all three entries under `[386Enh]`:

```ini
device=*vmd
device=<addon-directory>\INSIGNIA.386
device=<addon-directory>\NTVDM64.386
```

Use a single entry for each driver. `mouse=*vmd` alone did not load VMD in the
observed profile. These are configuration requirements, not permission to
replace the original VMD or install addons in host system32. Addon publication
belongs only in assets/release. Current candidate test evidence and unresolved
qualification are tracked in the T481 working plan; this section does not
qualify Win95 or supersede the last published addon manifest.