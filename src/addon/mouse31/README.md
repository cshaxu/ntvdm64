# Windows 3.1 NTVDM mouse bridge

Independent guest addon, originally admitted in T436 and repaired in T479 S6.
This is not an OpenNT mirror or a replacement CPU/device implementation.
Release: assets/release/MOUSE31.DRV; addon-manifest.json records exact identities.
Win3.1 enhanced mouse, real DOS child commands, child retirement and normal
Windows exit are verified with the insignia-386 companion. Win95 desktop mouse
is exercised; shutdown returns1 as in the companion-disabled baseline, and
full Win95 VM lifecycle is not accepted. See the T479 S6 integration evidence.
Build with tests/observation/build-win31-mouse-driver.ps1, a fresh absolute
BuildRoot below build/ and an explicit Python interpreter.

## Selected boundary

- Preserve NTVDM's real-mode INT33 provider and its existing IRQ/input owner.
  Windows draws the Windows arrow; this driver does not draw a DOS pointer.
- Win3.1 Inquire/Enable/Disable use protected-mode selectors. Do not treat a
  selector as a real segment or write mutable state through a code selector.
- Use DPMI simulated real interrupts for raw INT33 queries and preservation of
  the prior real-mode callback. DPMI0303 supplies the real-to-protected callback;
  register its real address through simulated INT33, not a protected selector
  through INT33/0Ch. Actual enhanced-mode reflection of the latter registered
  an invalid C000 address in the S6 failure witness.
- Enhanced mode requires the independent insignia-386 companion to activate
  original host NIDDB device-state instances. Real callback pointers belong to
  their VM; no driver-allocated real-mode gate is assumed to be mapped in another
  VM. Preserve only the bridge's own pointer-hide contribution.
- Real-mode ES values are numeric SI/packet fields. Never load a returned
  real segment into protected ES. Only the protected register-image pointer
  uses ES; a real DPMI test caught and verified this correction.
- Do not rely on the OpenNT DOSX function20 reflector: dxintr.asm explicitly
  leaves exchange translation unimplemented. The retail DOSX/VMM contract
  still requires actual mode-specific runtime tests.
- Keep callbacks resident/fixed and preserve registers and the interrupted
  simulated-interrupt image. Cleanup restores the saved real-mode provider
  callback before freeing the DPMI callback, retaining failed releases
  for retry. Repeated Enable/Disable, cross-VM dispatch and normal cleanup remain
  explicit S6 acceptance obligations; a desktop movement alone is insufficient.
- Reuse the independently authored Win101 normalization/button principles,
  not its NE version or real-mode callback address. Verify Win3.1 event masks
  and actual USER consumption before claiming clicks work.

The retail Mouse8.20 ABI has Inquire1, Enable2, Disable3, MouseGetIntVect4,
optional control-panel/data/power exports5–7 and named WEP8. Whether optional
exports are required for this bridge is audited, not guessed. MouseGetIntVect
reports the selected NTVDM provider's INT71 (IRQ9 / legacy IRQ2), or -1
when absent. It does not install an IRQ handler or program the PIC. Returning
-1 merely because this bridge does not own the PIC hides the provider from
Windows enhanced-mode mouse virtualization. The WOW thunk is not a suitable
reference implementation for this enhanced-mode contract.

## Evidence and tests

Read-only retail NE/export inspection:
`tests/observation/audit-win31-mouse.py --driver <MOUSE.DRV> --output <build-json>`.
Generated driver/probes belong below the active task build/ directory. Original
guest binaries remain unchanged; tests select authored addons on recoverable
installation copies. Enhanced mode loads the companion through SYSTEM.INI.

References: original source-shaped ABI comments in
`src/mvdm-guest/wow16/drivers/mouse/mouse.asm` and `mouse.def` (WOW thunks, not reused
as a DOS driver); original DPMI reflection in `src/mvdm-guest/dpmi/dxintr.asm` and
`dxint31.asm`; Microsoft DDK adaptation guide mouse contract and Microsoft
article Q74572. These are ABI evidence, not imported implementation.
