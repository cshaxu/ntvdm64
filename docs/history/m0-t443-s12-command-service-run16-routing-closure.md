# M0 T443 S12 closure — COMMAND service Run16 routing

S12 is owner-accepted on 2026-10-11.

The BOP 54:08 adapter now derives `system32\\run16.exe` from the loaded
NTVDM product root and preserves the original COMMAND service tail.  Its two
admitted entry forms are `run16.exe <original-tail>` for the execute service
and `run16.exe command` for the interactive service.  It does not write
`COMSPEC`, alter guest `COMMAND.COM`, or introduce a host `cmd.exe /c command`
layer.

Commit `a43ad2b0ab420159b2d235025c7d3795e6b5ae57` is pushed.  The corrected
ten-image package was published identically to `assets/release` and
`O:\\winnt\\system32`; the changed `ntvdm.exe` and `nthook32.dll` hashes and
the build/runtime limits are recorded in the [S12 evidence]
(../etc/evidence/m0-t443-s12-command-service-run16-routing.md).

The owner accepted the interactive result.  The retained headless-observer
limitation is not counted as a passing guest witness.  The unrelated borrowed
visible-Console-close residual remains a separately queued candidate.
