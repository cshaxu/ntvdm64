# Windows 1.01 INT33 Mouse Driver

Independent T435 guest add-on, not an OpenNT mirror or host component.
Windows draws its own arrow; the unchanged NTVDM INT33 driver owns mouse
position, geometry, interrupts and callback delivery.

## Contract

NASM emits a fixed/resident 16-bit NE library with Inquire, Enable and Disable
at ordinals1–3. There are no imports, hardware vectors, PIC operations or WOW
thunks. Original guest driver bytes are ABI evidence, not implementation.

Enable saves the Windows far event procedure, exchanges the old INT33 callback
for an empty callback, hides the DOS pointer once, then installs the bridge.
Repeated Enable replaces only the event procedure. Disable prevents delivery,
removes the bridge, balances its own hide and restores the old callback.
Missing-provider refusal does not mutate that state. No position, range,
ratio, motion-counter or full-state snapshot reset is performed.

The actual Win1.01 event consumer accepts movement bit0 with bit15 selecting
unsigned absolute BX/CX fractions0..65535. Button transition bits1/2 are left
down/up; bits3/4 are right down/up. DX is2 buttons. The bridge normalizes
INT33 CX/DX using the provider's current maximum coordinates, clamps endpoints
and avoids division by zero. It preserves registers, segments and flags and
returns with RETF to the existing provider continuation, not IRET. The callback
does not call file, allocation or UI APIs.

The rejected relative-counter/snapshot prototypes and original INT33 snapshot
checksum defect are retained in the linked evidence, not in this driver.

## Build And Installation

Run `tests/observation/build-win101-mouse-driver.ps1 -BuildRoot <fresh-build-root>`.
Outputs MOUSE101.DRV and independent mock/real-provider COM probes. Guest
objects never enter the native EXE build graph.

The verified compiled copy lives in assets/release/MOUSE101.DRV; the separate
addon-manifest.json records source/binary identity. Setup's apply-setup.cmd
consumes that copy without requiring an assembler or build path. Original
Setup embeds it in WIN100.BIN and installed PATCH also keeps the driver copy.

Windows1.01 embeds its mouse module in WIN100.BIN; replacing a loose MOUSE.DRV
does not update that installed image. The owner-authorized
`tools/win101-setup/install-mouse101.ps1 -OriginalImage <WIN100.BIN> -Driver <MOUSE101.DRV>
-OutputRoot <fresh-absolute-build-root>` installs only the MOUSE slot in a
recoverable copy. It preserves every other byte, module location/link and total
image length. It refuses unsupported imports/relocations and malformed images.
Original installation/media remain untouched; no runtime hot patch is used.

## Evidence And Remaining Gates

[S2 evidence](../../../docs/etc/evidence/m0-t435-s2-int33-driver.md) records
actual source/artifact hashes and reproducible entrypoints. The mock guest has
36 checks and actual INT33 registration/state-return probe has11 checks.
Private-desktop real Windows evidence shows arrow movement, menu down/up and
End Session. The minimal non-mirror host palette repair reuses the original
VGA colour resolver; strict return acceptance requires zero launcher exit and
ordered Windows-return, actual DOS MEM output and batch-continuation markers.

`tests/observation/observe-win101-image.ps1 -BatchReturn -RequireNormalReturn`
provides that strict gate, with the prepared image, mouse probe and selected
worker roots supplied explicitly. FIFO input acknowledgement alone is not
interaction proof. Fixed captures are diagnostic observations, not assertions.
Physical Raw Input, RDP clipping and general Windows application compatibility
are not claimed by the private input-sink probe. Same-worker repeated startup
and normal exit are verified. Dragging/general application compatibility is
not claimed by these mouse-driver checks. CURRENT records package delivery and
the remaining owner acceptance boundary.
