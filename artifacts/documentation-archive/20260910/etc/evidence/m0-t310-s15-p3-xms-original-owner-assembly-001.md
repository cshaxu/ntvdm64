# M0 T310 S15 P3 — XMS original-owner assembly

## Question

Did the selected CCPU40 formal SoftPC graph omit original XMS dependencies and
therefore misclassify them as adapter work?

## Inputs

- `src/mvdm-host/xms.486/sources`
- `src/mvdm-support/suballoc/sources`
- `src/mvdm-host-overlay/xms.486/xms_a20_state.c`
- `src/adapter-mvdm-host-out/softpc/mvdm_a20.c`
- `src/session/{mapping_manager,guest_memory_lease,session}.c`
- `tools/build/New-T310OriginalSoftpcNinja.ps1`

## Procedure

The complete original `suballoc.c` manifest member was added as its own
`original-mvdm-support-suballoc.lib`, rather than replacing `SAInitialize`,
`SAAllocate`, `SAFree`, `SAReallocate` or `SAQueryFree` with adapter bodies.
The matching XMS private overlay was built into `original-mvdm-xms.lib`, as
required by mirror-private overlay ownership.  The existing same-shaped A20
facade was added to the SoftPC binding library; it merely routes historical
`sas_*20_bit_wrapping` spellings to the original selected `c_sas_*` provider.
Finally, the already-owned session implementation was linked as its own
library because the selected XMS memory backend and private overlay require
the session's bounded guest-memory leases.

The formal graph was generated and built with:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
  -BuildRoot build/M0-T310/S15/original-softpc-ccpu40-s15p3 \
  -NodeExecutable O:\\.nvm\\versions\\node\\v22.22.1\\bin\\node.exe
ninja -C build/M0-T310/S15/original-softpc-ccpu40-s15p3 \
  original-softpc-candidate
ninja -C build/M0-T310/S15/original-softpc-ccpu40-s15p3 \
  original-softpc-forced-closure.dll
```

## Observations

- The x86 CCPU40 graph completed all 369 requested build edges, including
  `original-mvdm-support-suballoc.lib`, the XMS overlay object, `session.lib`,
  and the augmented `softpc-bindings.lib`.
- The deliberate whole-archive forced-link audit generated its DLL and log.
  The log contains no unresolved `SA*` allocator, XMS A20-state, historical
  `sas_*20_bit_wrapping`, XMS memory-backend, or session-lease symbol.
- Remaining forced-link diagnostics belong to other original owner packages;
  this P does not treat `/force:unresolved` as executable closure.
- Original compiler warnings remain emitted.  No warning was hidden and no
  original XMS or SubAlloc body was replaced.

## Conclusion

The selected XMS package's previously missing allocator/A20/session edges are
now assigned to their actual owners: original `mvdm-support/suballoc`, private
`mvdm-host` overlay, selected CCPU SAS, and the existing session boundary.
This closes the source-assembly gap, not the S15 runtime closure: a reachable
SoftPC machine lifecycle still has to exercise XMS initialization, dispatch,
block allocation and A20/UMB dispositions without a Bochs or MONITOR path.

## Follow-up

Use the assembled original owners to inspect the reachable SoftPC startup
route.  Do not add a parallel XMS allocator, raw guest pointer backend, or a
Bochs-based fixture.
