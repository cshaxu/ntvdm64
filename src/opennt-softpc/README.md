# opennt-softpc

Original OpenNT SoftPC firmware, ROM and machine-contract source/input
packages. The source-identical initial admission is the 27-file
`base/mvdm/softpc.new/bios/` and 8-file `base/mvdm/softpc.new/roms/` families.
Their source identity and per-package boundary are recorded in
`docs/etc/evidence/m0-t263-s3-opennt-softpc-firmware-rom-admission-001.md`.

This component is not a second emulator.  Bochs mechanics remain in `bochs-core`
and `adapter-bochs`; same-shaped reached SoftPC/CCPU calls are provided only by
`adapter-softpc`.  `app` may select admitted firmware/ROM bytes and pass them
through a declared opaque mechanical loading contract.  Neither `adapter-bochs`
nor `bochs-core` imports this component or learns OpenNT terminology.

## Local intrusion register

Every imported source/input modification is registered below with original
path, source identity, exception identifier, necessity and verification. The
expected initial count is zero. No production body differs from OpenNT.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
