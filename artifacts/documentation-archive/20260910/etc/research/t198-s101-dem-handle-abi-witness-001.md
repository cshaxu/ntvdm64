# T198 S101 DEM handle ABI witness 001

## Change

The shared contained DEM codec now matches OpenNT `GETHANDLE(AX, BP)`:
`AX` carries the high 16 bits and `BP` the low 16 bits.  `50:12` publishes
that order, while seek, normal read, fast read and close reconstruct the same
token in that order.

## Verification

`tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` source-built, linked and
ran the current composition at
`artifacts/build/t198-s101-dem-handle-abi-r1` as one MSVC x64 `/MT` process.
The fixture exits zero and covers:

- open-derived AX:BP token consumption by seek and close;
- normal `50:16` read;
- `50:42` ZF-clear seek plus checked namespace bulk write;
- invalid-token failure behavior; and
- continued `50:43` pass-through.

No Bochs, mantle, BOP ingress, host capability or fast-write source changed.

## Next

S102 may make one exact-input rebuilt native guest observation.  It must
classify the first unresolved boundary from logs and may not call a process
exit a DOS result.
