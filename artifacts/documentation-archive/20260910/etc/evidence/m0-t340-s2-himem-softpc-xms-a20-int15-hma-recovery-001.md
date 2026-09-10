# M0 T340 S2 — Original HIMEM / SoftPC XMS cohort recovery

## Result

The complete selected `HIMEM.SYS` initialization cohort is already present in
the formal CPU40 product as original guest and host source bodies.  This S
does not add a device substitute, BOP provider, configuration exception or
host-memory shortcut.

The source-shaped route is:

```
original CONFIG DEVICE=HIMEM.SYS
  -> HIMEM InitInterrupt / InitDriver
  -> XMSSVC XMS_A20, XMS_EXTMEM, XMS_INITUMB
  -> original xms.486 service table and bodies
  -> original SoftPC SAS/UMB/INT15 contracts
  -> existing session mapping-manager only at old native-pointer seams
```

`nt_msscs.c::scs_init` also retains its original `XMSInit` call before the
debugger initialization.  The formal x86 graph selects all six original XMS
translation units and `mvdm-host/suballoc`; it links the small A20-state
overlay and same-shaped `mvdm_xms_memory` callback backend only where the
historical process-address model would otherwise cross into guest memory.

## Source facts

- `himem2.asm::InitInterrupt` accepts only request function 00 for initial
  load, calls `InitDriver`, and writes the original done/status result.
- `InitDriver` queries XMS A20, obtains memory through `XMS_EXTMEM`, publishes
  its `A20State` address through `XMS_INITUMB`, and hooks INT 2F only after
  the original failure checks.
- `xmsdisp.c` retains the original twelve-entry `apfnXMSSvc` dispatch table.
- `xms.c::XMSInit` retains `ReserveUMB` then `SAInitialize` with original
  allocation ordering and failure result.
- `xmsmisc.c::xmsNotifyHookI15` retains the original `UpdateKbdInt15(CS:AX)`
  notification and `CX=xmsMemorySize` result.

The complete row-level source, binding and result map is the accompanying
[S2 ledger](../operations/m0-t340-s2-himem-softpc-xms-a20-int15-hma-contract-ledger.tsv).

## Narrow divergences retained

Only four already-registered MVDM host divergences are reached by this cohort:

- `MVDM-HOST-DIV-132`: select one session-backed XMS callback branch on both
  host widths rather than expose a native process address;
- `MVDM-HOST-DIV-010`: retain the original AX:BX A20-state location as guest
  numeric data and write it through a fresh lease;
- `MVDM-HOST-DIV-077`: resolve the original XMS move descriptor through
  bounded session leases while preserving descriptor order and AX result;
- `MVDM-HOST-DIV-078`: treat the historical UMB `PVOID` carrier as an Intel
  physical numeric value, never as a host pointer.

No divergence changes HIMEM's guest request packet, XMS table numbering,
register result, device order or original failure path.

## Formal build result

The fixed formal CPU40/x86 build was invoked through its parallel Ninja
container for `original-softpc-process.exe`; Ninja reported `no work to do`.
The existing formal graph explicitly contains all six original XMS objects in
`original-mvdm-xms.lib`, plus the two required bounded bindings.  This is
build-closure evidence, not an assertion that guest execution reached HIMEM.

## Recovery disposition

This package is source and formal-build closed.  It is not necessary or
correct to invent a new XMS BOP handler or rewrite `config.nt` to bypass it.
The only remaining runtime question is whether the fixed product reaches the
original configuration-device phase at all; that question belongs to the
preceding CPU/host startup continuation, not to the recovered HIMEM owner
package.
