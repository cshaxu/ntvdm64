# M0 T378 closure — DOS-extender runtime

## Closed result

T378 closes the declared original DOS-extender workload package.

- S1 froze the byte-identical original HIMEM and DOSX artifacts and the whole
  CONFIG/COMMAND/XMS/DPMI owner route.
- S2 retained the complete original CONFIG.NT device, HIMEM, XMSInit,
  A20/UMB/INT 15 cohort through the existing mapping-manager boundary; no
  synthetic XMS service or device was added.
- S3 retained the original `MS_bop_3 -> DpmiDispatch` handoff, 25-entry DPMI
  table and original frame/stack ownership; no second dispatcher was added.
- S4 revalidated the final CPU40 product and immutable stage matrix.
- S5 made one frozen `DOSX.EXE` observation.  It opened original HIMEM and
  DOSX successfully and the process returned zero.

## Exact limit

The one observation did not emit `MS_bop_3`, `53:xx` or a protected-mode
marker.  T378 therefore does not claim a DOSX client entered protected mode,
nor general DPMI/DOS-extender compatibility.  It proves the selected original
load path and its normal return only.  Any next protected-mode-client workload
must be independently admitted and retain the package-first rule.

## Evidence

- [S1 admission](../etc/evidence/m0-t378-s1-dos-extender-workload-owner-admission-001.md)
- [S2 XMS boot closure](../etc/evidence/m0-t378-s2-original-xms-boot-hma-umb-lifecycle-closure-001.md)
- [S3 DOSX/DPMI handoff closure](../etc/evidence/m0-t378-s3-original-dosx-dpmi-handoff-closure-001.md)
- [S4 formal matrix](../etc/evidence/m0-t378-s4-formal-dos-extender-workload-matrix-001.md)
- [S5 frozen observation](../etc/evidence/m0-t378-s5-frozen-original-dosx-observation-001.md)
