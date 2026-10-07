# Windows 3.1 NTVDM mouse bridge

T436 S2 independent guest add-on. This is not an OpenNT mirror, host library,
new process or replacement CPU/device implementation. Implementation and
runtime verification are in progress. The release copy is assets/release/MOUSE31.DRV,
with exact source/binary identity and candidate acceptance in addon-manifest.json.
Provider tests and /S /3 desktop input plus /S normal return are verified;
normal reentry and enhanced cleanup remain open. Use tests/observation/build-win31-mouse-driver.ps1 with a
fresh absolute BuildRoot below build/ and an explicit Python interpreter.

## Selected boundary

- Preserve NTVDM's real-mode INT33 provider and its existing IRQ/input owner.
  Windows draws the Windows arrow; this driver does not draw a DOS pointer.
- Win3.1 Inquire/Enable/Disable use protected-mode selectors. Do not treat a
  selector as a real segment or write mutable state through a code selector.
- Use the existing DPMI simulated real interrupt and callback services for
  raw INT33 calls and a protected callback. Save/restore the prior real-mode
  callback with INT33 exchange; balance only the bridge's own pointer hide.
- Real-mode ES values are numeric SI/packet fields. Never load a returned
  real segment into protected ES. Only the protected register-image pointer
  uses ES; a real DPMI test caught and verified this correction.
- Do not rely on the OpenNT DOSX function20 reflector: dxintr.asm explicitly
  leaves exchange translation unimplemented. The retail DOSX/VMM contract
  still requires actual mode-specific runtime tests.
- Keep callbacks resident/fixed, preserve register/stack return semantics,
  restore the previous provider callback before freeing its DPMI thunk, and
  handle repeated Enable/Disable without accumulating resources.
- Reuse the independently authored Win101 normalization/button principles,
  not its NE version or real-mode callback address. Verify Win3.1 event masks
  and actual USER consumption before claiming clicks work.

The retail Mouse8.20 ABI has Inquire1, Enable2, Disable3, MouseGetIntVect4,
optional control-panel/data/power exports5–7 and named WEP8. Whether optional
exports are required for this bridge is audited, not guessed. MouseGetIntVect
must describe the bridge's actual interrupt ownership, not falsely claim the
PS/2 hardware interrupt that the existing DOS provider already owns.

## Evidence and tests

Read-only retail NE/export inspection:
`tests/observation/audit-win31-mouse.py --driver <MOUSE.DRV> --output <build-json>`.
Generated driver/probes belong below build/M0-T436/S2. Original drivers, guest
binaries/default profiles remain unchanged; live tests select the new driver
only on recoverable installations. S3 owns final installation packaging.

References: original source-shaped ABI comments in
`src/mvdm/wow16/drivers/mouse/mouse.asm` and `mouse.def` (WOW thunks, not reused
as a DOS driver); original DPMI reflection in `src/mvdm/dpmi/dxintr.asm` and
`dxint31.asm`; Microsoft DDK adaptation guide mouse contract and Microsoft
article Q74572. These are ABI evidence, not imported implementation.
