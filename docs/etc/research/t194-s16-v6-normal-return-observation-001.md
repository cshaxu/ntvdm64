# T194 S16 v6 normal-return observation 001

Date: 2026-08-12  
Packet: M0 T194 S16  
Disposition: one bounded observation complete; normal COMMAND return not
observed.

## Exact invocation

`Invoke-T194S10V6BoundedObservation.ps1` made exactly one invocation with a
15-second watchdog and `retryPolicy: none`. Its immutable input record is
`artifacts/analysis/t194-s16-v6-normal-return-observation-001/inputs.json`:

| Input | SHA-256 |
| --- | --- |
| Current S15 runner | `706882B0EE453CB5E4A64A81A9E357375766DB4B99BDBAD74932577C1570CB37` |
| Current S15 shim | `57AD850F46782FFF798DBEA39EFE6C7EB69FAD3B192DDF2DA5850E696F3F6A41` |
| S8 r2 bundle engine | `D9060D2201BE0DD8F650D44ADFD1F503D9549C1699952F6ACE2F84AC623E758C` |
| S13 v6 profile | `F91B27EF63FEEEB64A7DCB18F25FCBF2B626DE05CFC1961F4D96CF21B4262548` |
| S13 `TARGET.EXE` | `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC` |

The watchdog terminated the wrapper process tree successfully after 15,374 ms
(`treeKillExit: 0`); the retained child exit code is `1`. This is a watchdog
classification, not a guest completion result.

## Observed machine facts

The complete stderr log is 13,322,416 bytes with SHA-256
`65823EC33D941AAC9108E750DB22DF0D353920DA0A83AB070A79BCFA8AC287B3`.
Stdout is empty (SHA-256
`E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855`).

The only four observed BOP forms, in order, are:

1. `50:11`, followed by the existing committed transaction
   `address=8b00 bytes=6cd2`;
2. selector `12` (no host-service family);
3. `50:3B`;
4. `50:0F` at `CS:EIP 9346:7439`, followed by pass-through.

No `54:11` marker occurs. After `50:0F`, the retained log first records the
unchanged pass-through instruction window `c4 c4 50 0f`, then records
`cs=0000 eip=00000a84 bytes=ffffffff` 126,818 times (126,819 pass-through
records total) until the watchdog. The record proves neither a COMMAND return
nor the cause of that subsequent control state.

## Interpretation and follow-up

The current v6 runner, shim, r2 bundle and root compose through real native
startup and the first transaction. The normal-return target is not reached.
The next admissible question is source-only: determine the original OpenNT
owner, contract and continuation requirements of the reached `50:3B` and
`50:0F` services before considering another run, provider change, or Bochs
change. No absence in this trace licenses a new adapter handler.
