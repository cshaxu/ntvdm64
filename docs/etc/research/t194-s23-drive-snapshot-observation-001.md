# T194 S23 drive-snapshot observation 001

Date: 2026-08-12  
Packet: M0 T194 S23  
Disposition: one bounded observation complete; `54:11` not observed.

Using the S15 runner/shim, S22 bundle and S13 v6 root, the existing controller
made exactly one 15-second, no-retry observation. The watchdog terminated the
process tree successfully after 15,415 ms (`treeKillExit: 0`, child exit `1`).
The full stderr log is 1,078,146 bytes, SHA-256
`3913169020FC543E9F011E164B05C640DED46110C4C7F111A023B47F483BE619`.

The observed BOP order is `50:11`, `12`, `50:3B`, `50:0F`, `50:1B`, `50:32`,
`54:05`, then `50:46`. The first transaction still commits `6cd2` bytes at
`8b00`. Unlike S16, `50:0F` is followed by later BOP progress, proving its
snapshot-backed result resumed in the native derivative. `50:46` at
`9346:7588` passes through; subsequent `0000:0A84`/`0000:0815` windows repeat
until watchdog. No `54:11` marker occurs.

This is neither normal COMMAND return nor evidence for a Bochs change. The
next question is the original `DemGetDPBList` contract and current multi-write
provider disposition.
