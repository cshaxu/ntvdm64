# M0 T452 S2 — module and alias delivery receipt

S2 delivered the two source-shaped W03 repairs at `0f8487f04`:

- `WK32GetModuleFileName` bounds output and flush by the caller's declared
  capacity, preserving real-module versus HTASK-alias selection.
- `GetHtaskAliasProcessName` retains original process enumeration and ANSI
  conversion while replacing the fixed stack snapshot and unbounded copy with
  a single required-size query, one transient allocation and one populated
  query.

The selected CCPU40 external-memory fixture passed the direct
`VdmAllocateVirtualMemory`/`VdmFreeVirtualMemory` pair.  The production-command
alias fixture passed its normal, exact, truncated, one-byte and unavailable
cases.  The coherent ten-image candidate was built, Userenv-isolated WinMine
interaction passed, and the matching package was published to both
`assets/release` and `O:\winnt\system32`.  Full commands, hashes and staging
records remain in [the T452 plan](../operations/t452-modules-memory-aliases-plan.md#implementation-and-evidence-rule).

This receipt does not extend the S2 repairs into alias-table reuse or
WinOldAp representative lifetime; S3 owns that audit.

