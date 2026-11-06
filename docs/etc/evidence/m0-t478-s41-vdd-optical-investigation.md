# T478 S41 VDD/optical investigation

Status: S41 P2 restored and locally delivered under explicit owner approval.
Build, mounted/empty virtual-drive tests, VDD lifecycle, Full16/16 and Product
verification pass. Eleven images are published to this worktree only.

## Existing VDD host path

Original nt_bop.c still selects ISV_RegisterModule, ISV_DispatchCall and
ISV_DeRegisterModule. Registration resolves guest strings, calls SafeLoadLibrary,
resolves init/dispatch exports, invokes init and assigns an original table slot.
Deregistration calls FreeLibrary and clears the slot. Original missing-library,
missing-export and exhausted-slot error returns remain in this owner.
No replacement loader is indicated by source review.

The current worker graph builds the existing S44 controlled provider against
its own ntvdm.lib. `build/t478-s41/build-vdd.exit` is0; only its fixture object
and DLL were built. The current exact S38 product package is unchanged.
The maintained isolated runner now accepts a COM/VDD case and stages that
controlled DLL in its disposable tests directory. The original S44 fixture
uses a conditional test-only bare DLL name for this directory; its existing
nonisolated behavior is unchanged. No original guest is modified.

`build/t478-s41/vdd-r1` passes through real DOS BOP58 registration, dispatch
and deregistration, normal launcher exit, and these provider markers:
ATTACH, HOOK_INSTALL_OK, HOOK_INVALID_OK, IO_INSTALL_OK, BOP_INIT,
BOP_DISPATCH, IO_INB, IO_OUTB, DETACH, HOOK_REMOVE_OK. The owned test port is
handled by the fixture's registered callbacks. No physical device is accessed.
This proves the actual current original loader and public hook boundary,
not operation of the optical provider or every VDD API.

## P1 independent optical provider gap (historical snapshot)

The historical audit correctly distinguishes the guest MSCDEXNT TSR from
native VCDEX.DLL. VCDEX is a separately linked host VDD; it is not guest code.
The current source-tree sample retains a historical DLL/resource, but no
selected product VCDEX DLL. Commit c32d28b749e69c06a539c0b49911703924a6b63d
removed the unselected provider's C/header/definition/build inputs during the
first-depth exclusion audit. Its original source remains in both pinned
external inventories; it is not part of the current host build graph.

Original VDDRegisterInit enumerates DRIVE_CDROM roots, opens physical drive
handles, allocates drive state and installs its own lifetime hooks. With no
usable drives it sets carry. The existing source audit identifies original
partial-init/heap-failure limits; no new rollback policy is admitted here.
The historical DLL is not staged or run to bypass source/ABI review.

The initial restricted CIM inventory query was denied, not a no-drive result.
An explicit successful desktop-token retry records zero Win32_CDROMDrive
instances in `build/t478-s41/optical-inventory.json`. That query did not establish absence of virtual drives. P2 below supersedes
the initial environment assessment; zero CIM rows are not a no-device proof.

## P1 scope decision and P2 authorization

The owner explicitly approves restoring and delivering VCDEX and identifies
local virtual optical drives and ISO files as test assets. P2 therefore adds
this independent host DLL; no guest code/media modification is admitted.
No O:/winnt write or push is authorized.

## P2 original recovery and finite boundary

Seven source/build files are recovered. The [source receipt](../../../artifacts/mvdm-audit/shard-3/vcdex-s41-source-provenance.json)
records pinned original paths, original/current SHA-256 and exact differences.
The six MSCDEX inputs match both OpenNT and OpenNT-4.5 inventories before
adaptation. Original vcdex.rc was already retained. No autonomous optical
implementation is added or moved into a mirror.

MVDM-HOST-DIV-886 changes only VDDInitialize's declaration to WINAPI: compiling
the original implicit-cdecl DLL entry produced LNK4086; the selected link uses
VDDInitialize@12. Init/dispatch exports and original algorithms are unchanged.
The unselected DEBUG header body is replaced with#error/#line. OPENNT-HOST-298
recovers original vddsvc.h declarations, excluding non-i386 bodies with#error
and original line positions. Modern public SDK supplies the optical structures;
existing ntvdm.lib supplies the VDD ABI. The resulting x86 PE imports12 worker
ordinals plus KERNEL32; it exports VDDDispatch, VDDInitialize, VDDRegisterInit.
The PE report is build/t478-s41/vcdex-pe.txt. No private kernel provider is added.

The worker target now builds VCDEX.DLL from the original sources manifest.
Packaging requires and identity-stamps this newly built DLL, producing11 host
images. The isolated package helper accepts complete legacy10 or new11 sets.
The immutable archive contains a historical *host* VCDEX binary; the guest
verification gate now binds only this host replacement to its exact product
manifest path/hash. All real guest files retain archive hash checks. Seven
contract cases reject unregistered replacement, host/guest tampering, duplicate
records and path substitution while accepting original and registered inputs.
No archive or guest binary was modified. The disposable AUTOEXEC copy omits the
historical absolute O:/winnt SETVER command to avoid executing external writes.

## Real virtual optical verification

Existing UltraISO I: is a CDRom without media, despite zero CIM drive rows.
IsoCmd mount printed Access is denied while returning0; this was treated as a
failure, and I: remained unchanged. Windows Mount-DiskImage then attached the
previously unattached owner-supplied MSDN_Premium_1994_WINNT35_WINNT_SDK.iso
read-only as F: WINNT_SDK. Its path/hash and initial attachment ownership are
recorded under build/t478-s41/iso-mount-intent.json and windows-iso-mounted.json.
Only this owned attachment is to be detached after verification.

The independently authored DOS COM probe calls the existing immutable
MSCDEXNT TSR through INT2F. That TSR loads the newly staged VCDEX via the actual
VDD loader; no rebuilt/patched TSR or alternate provider is substituted.
optical-r2 passes: two drives F:/I:, version0215, check signatureADAD, F: as a
CD drive, drive-list enumeration, volume/TOC and repeated query after unsupported
READ_LONG closes the original native handle. The next query reopens it through
the original code. Normal launcher/task exit passes with original provider
lifetime ownership. No explicit DLL deregistration is invented: the original
TSR retains its VDD and hooks until process exit.

A direct native read-only IOCTL_CDROM_READ_TOC probe returns first/last track1
and leadout00:49:23:38, exactly matching the guest report and original volume
conversion222263 frames. The first test expected ISO sectors+150; Windows' TOC
returned one frame less. The corrected oracle is the actual native TOC, not an
invented alteration to original arithmetic. The first test and failed Full
archive-host classification attempt are retained, not counted as passes.

Original ApiAbsoluteDiskRead/ReadVTOC/directory routes are unimplemented. The
host's setCF(1) does not establish guest-visible error: the original TSR IRET
restores caller flags without updating the interrupt frame. The test therefore
checks the preserved input carry0, and does not claim successful sector data
reads. Device READ_LONG explicitly returns8103 (DONE|ERROR|UNKNOWN_CMD). No new
sector reader, filesystem layer, retry, allocation rollback or tolerance is
added. Physical drives, audio-disc playback and arbitrary media changes are
not certified by an ISO test. Original memory-exhaustion and lifetime limits
remain, consistent with the owner's original-semantics-only direction.

The existing empty I: drive also passes optical-empty-r2: enumeration still
identifies a CD drive, volume/disk/repeated queries return810C with zero data,
and READ_LONG returns8103. Direct native TOC fails with ERROR_NOT_READY(21),
which original ProcessError maps through its default CDERR_GENERAL arm. No
new error remapping is introduced. The initial empty-drive test incorrectly
expected DONE alone; that failed oracle is retained and corrected against
ReadTOC/ProcessError. This is a negative-path pass, not successful media I/O.

The current11-image package additionally passes vdd-product-r1: real BOP
register/dispatch/deregister, controlled port callbacks and hook removal, with
normal exit. Full16/16 passes in382810ms (verify-r2-Full); DOS4GW remains the
existing explicitly skipped external-fixture case, not a new acceptance claim.
That Full run checked the eight base images and two hooks in its identity
summary, and separately bound VCDEX through the guest/host manifest gate and
real optical probe. The maintained product verifier now includes VCDEX in its
cache/PE/final identity loop; a subsequent Product run checks that11-image path.
No production source changes follow the successful Full run.

The owned Windows ISO attachment was detached, I: remains empty, and the ISO
SHA-256 still matches its pre-test value. Both superseded failed-test stages
were removed with the existing identity/process-aware cleanup helper; their
logs and binary reports remain under build/t478-s41. No unrelated drive,
installation or guest medium was modified.

## Local delivery

The [result receipt](../../../artifacts/mvdm-audit/shard-3/vcdex-s41-results.json)
binds all results and11 published image hashes to package
d7ad54fc-88b2-4a5e-8c56-140a2d4a4ca3. Product verification passes its
complete selected gates and reports an identical11-file package; Full16/16
remains the unchanged production-code regression result. Worktree assets/release
is hash-verified, with the prior release backed up under build/t478-s41/release-before.
This closes finding61's approved host provider recovery. Physical optical/audio
coverage and original unsupported APIs remain explicit limits, not new work.
S41 P1 was8dea73067; this P2 is a local commit only, with no O:/winnt or push.
T478's larger S1 inventory/handoff reconciliation is not claimed complete here.
