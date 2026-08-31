# M0 T318 S2 P37 — XMS Session-Backend Selection and Fixed Run

Date: 2026-08-31  
Status: source/build closure plus one attributable negative startup observation

## Change

The formal CPU40 common compilation flags now define
`MVDM_XMS_SESSION_BACKEND`.  This selects the already linked, same-shaped
`xmsCommitBlock`, `xmsDecommitBlock`, and `xmsMoveMemory` implementation in
`adapter-mvdm-host-out/softpc/mvdm_xms_memory.c` for both host-width rows.
Original `xms.c` keeps its allocator, error handling, and public function
contracts.  No BOP, guest loader, or new XMS provider was added.

## Build result

Fresh formal Ninja CPU40 product builds and final links completed for:

- Win32/x86: `build/M0-T318/S2/formal-x86/original-softpc-process.exe`
- x64: `build/M0-T318/S2/formal-x64/original-softpc-process.exe`

The compilation retains visible historical source diagnostics; this change did
not suppress or reclassify them.

## Fixed observation control

`runtime-r31` was staged from the newly linked x86 product.  Its DOS media and
SoftPC firmware manifest entries are byte-identical to `runtime-r30`:

```text
non-product-assets-identical=true
r30 product = 94a70b3997c7cb3e04bdc4b586881ee981a3263dd0e951978d5b1c954b24994f
r31 product = b84ea2555efae0b1e7eb03a912e3604397d39bcf90432f98878992b36e6dc01e
```

The unchanged non-debug, console-owning launcher, arguments and eight-second
budget reported:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

## Conclusion

The previous XMS selection gap is repaired at the product-build boundary, but
the fixed startup failure is unchanged.  This eliminates that selected XMS
branch as an explanation for the present baseline; it proves neither entry to
NTIO nor any NTDOS, EXEC, or parent-return transition.  The next audit remains
the full original `reset -> ConsoleInit -> heartbeat` host-owner cohort, with
the observation container held fixed until a concrete source change results.
