# opennt-softpc

Original OpenNT SoftPC firmware, ROM and machine-contract source/input
packages.  Initial admission is limited to the provenance-reviewed
`base/mvdm/softpc.new/bios/` and `base/mvdm/softpc.new/roms/` families.

This component is not a second emulator.  Bochs mechanics remain in `bx-core`
and `bx-mantle`; same-shaped reached SoftPC/CCPU calls are provided only by
`adapter-softpc`.  `app` may select admitted firmware/ROM bytes and pass them
through a declared opaque mechanical loading contract.  Neither `bx-mantle`
nor `bx-core` imports this component or learns OpenNT terminology.

## Local intrusion register

Every imported source/input modification is registered below with original
path, source identity, exception identifier, necessity and verification. The
expected initial count is zero.  No production body is admitted by this
skeleton alone.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |

