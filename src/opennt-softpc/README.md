# opennt-softpc

Original OpenNT SoftPC firmware, ROM and machine-contract source/input
packages. The source-identical admission is 37 production inputs: the 27-file
`base/mvdm/softpc.new/bios/` family, its selected `base/bios/bios.c` and
`base/inc/bios.h` inputs re-rooted here, and the 8-file
`base/mvdm/softpc.new/roms/` family. The current complete identity ledger is
`docs/etc/evidence/m0-t270-s5-p1-opennt-softpc-source-identity-ledger-001.md`;
the earlier T263 admission record remains historical supporting evidence.

This component is not a second emulator.  Bochs mechanics remain in `bochs-core`
and `adapter-bochs`; same-shaped reached SoftPC/CCPU calls are provided only by
`adapter-softpc`.  `app` may select admitted firmware/ROM bytes and pass them
through a declared opaque mechanical loading contract.  Neither `adapter-bochs`
nor `bochs-core` imports this component or learns OpenNT terminology.

## Local intrusion register

Every imported source/input modification is registered below with original
path, source identity, exception identifier, necessity and verification. The
expected initial count is zero. No production body differs from OpenNT.

The exact `bios/bios.{c,h}` source pair is also a current BOP inventory input; T266/S3 records its source identity and local use.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
