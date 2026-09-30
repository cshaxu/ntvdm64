# M0 T423 S19 P2 — PID-first runtime validation

## Scope

This record validates the protocol-17 PID-first management implementation in
the coherent x86 runtime package.  It does not close S19: owner verification
remains its final exit gate.

## Formal build and published identity

The serial formal x86 graph completed all 573 nodes in
`build/M0-T423/S19/formal-final`.  The following files were verified as PE
machine `0x014c` and copied byte-for-byte to `O:/winnt`:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `8b5b0d2d715bc14932fe31a637566c06db5197fb760b47f0e0d4890f1d791ba0` |
| `ntsrv.exe` | `328160517deb9fca64444a0d4c6b717301ef5cddf992b9c20c06c4f23292a5b3` |
| `ntvdm.exe` | `df6ddef9c54c5fd8aaccac76b85dc6c310bd2eb6f4afbf820c026983e64ef120` |
| `ntkvm.exe` | `9a37864b9b83c6938dc4fc25db852bda63258327ec64ce745d92729f99d939ea` |
| `ntcon.exe` | `fc2a31ce09f1e927e487fbf5f576fb14ff42eb94d7529087d0c324472e5a422f` |
| `ntmon.exe` | `01cbc12a3bde8f9a0f295b474a8945e479afca454eb879d845685276bc45eeb6` |
| `VDMREDIR.dll` | `c55dc99b5507e03eb59c009f25397f946316a50a068fbff6f393ddfc914bda82` |

## Runtime evidence

All runtime logs are under `O:/winnt/Logs2`; generated fixtures are confined
to `O:/winnt/tests` and removed by their verifier.

| Check | Result |
| --- | --- |
| PID-only contract | `monitor-rpc-test.exe --empty` passed protocol v17 snapshot, version rejection and absent-PID termination rejection. |
| Authenticated native registration | `basesrv-service-reservation-test.exe --native-backend` passed unique authenticated root, actual target identity, membership report, no fake close and rundown. |
| NTCON session close | `ntcon-close-test.exe` passed its normal Console close and active-target identity checks. |
| Real PID termination | `verify-ntcon-management.ps1 -TwoSessions` terminated only the selected NTCON/CMD session; its directly attached CMD closed, while the independent NTCON/CMD session accepted input and returned 23. |
| Existing text regression | `Verify-CommandExitStatus.ps1` passed all 17 default output/exit routes: direct and interactive COMMAND, MEM, nested COMMAND, streams/EOF, real `G7.COM` exit 7, `COMMAND /c`, and EDIT return. |
| NTMON presentation | `verify-monitor-layout.ps1` passed frame, arrows, colours, 25 rows, overflow selection, horizontal extent, shrink, empty state and confirmation. |

The earlier host-specific named-pipe fixture access denial remains deliberately
separated from this result; it is not relabelled as a management pass.

## Remaining S19 gate

The runtime package is now coherent and published.  S19 remains active only
for owner-side acceptance of the product package; S20 owns removal of the
separate 100 ms creator/retirement polling mechanism.
