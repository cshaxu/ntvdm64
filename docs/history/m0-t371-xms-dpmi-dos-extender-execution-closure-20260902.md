# M0 T371 closure — XMS, DPMI and DOS-extender execution

## Closed scope

T371 closes its selected original XMS/DPMI/DOSX **source, focused-cohort and
formal-product** work.

- Original SoftPC startup ordering establishes `XMSInit` as the earliest whole
  XMS cohort, rather than a trace-selected `52:xx` leaf.
- Original `XMSInit` and original `suballoc` were exercised through both
  source allocator choices.  The sole imported ABI correction is registered
  as `MVDM-HOST-DIV-198`.
- The selected original DPMI/DPMI32 archive, complete 25-entry dispatch table
  and immutable DOSX guest-carrier boundary remain current in the fresh
  CPU40/x86 formal final link.
- No second XMS/DPMI dispatcher, private mapping manager, host DOSX library,
  raw host pointer carrier or synthetic success route was introduced.

## Runtime disposition

The one admitted fixed observation timed out before producing a positive XMS,
DPMI or DOSX marker.  This is not a claim of XMS/DPMI/DOSX guest-runtime
completion and is not classified as a defect in those source cohorts.  The
remaining gate is earlier COMMAND/DOS execution continuity; future work must
recover that owner package rather than add a DPMI leaf workaround.

## Evidence

- [S1 original execution topology](../etc/evidence/m0-t371-s1-original-xms-dpmi-execution-topology-001.md)
- [S2 original XMS startup cohort](../etc/evidence/m0-t371-s2-original-xms-startup-cohort-001.md)
- [S3 original DPMI/DOSX current closure](../etc/evidence/m0-t371-s3-original-dpmi-dosx-current-closure-001.md)
- [S4 fixed integration observation](../etc/evidence/m0-t371-s4-fixed-xms-dpmi-integration-observation-001.md)
