# T478 S41 VDD/optical investigation

Status: active; independent optical provider scope requires owner disposition.
No product code change or S41 closure is claimed.

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

## Independent optical provider gap

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
instances in `build/t478-s41/optical-inventory.json`. Thus real medium reads
cannot currently be acceptance evidence. No virtual device or medium is added.

## Pending scope decision

The owner has been asked whether to recover/deliver this original standalone
host provider, with real-device verification unavailable, or retain the
independent-provider exclusion and explicitly register the unprovided capability.
This is a real product/component boundary, not an excuse to replace the working
RegisterVDD loader. Active S41 stop conditions require disposition of a material
new provider. Tests/evidence may continue; provider implementation and S41
closure wait for the answer. No O:/winnt write or push.
