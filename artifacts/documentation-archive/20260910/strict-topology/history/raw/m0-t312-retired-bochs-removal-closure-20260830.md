# M0 T312 closure — retired Bochs source/component removal

T312 removed the retired Bochs product route. The selected machine route is now
the original SoftPC/CCPU40/SAS composition only.

- S1 identified every live retired-machine dependency and its original owner.
- S2 moved source-derived register, SAS, frame, and WOW recovery experiments
  out of production, and transferred live bindings to the selected original
  CCPU40/SAS interfaces.
- S3 removed `bochs-core`, its overlay, `adapter-bochs`, the former #UD ingress,
  their tests, and retired build graph material. All retained material is
  evidence-only below `docs/etc/legacy_code/retired-bochs/`.
- S4 generated a fresh x86 CCPU40 formal graph, completed its 399-step
  candidate target, and completed the forced-link closure target.

This is a source/build-closure result, not a claim that the complete MVDM
runtime, BOP surface, or guest workload now runs end to end. The next package
must continue from original SoftPC capability and mirror-purity acceptance.

Supporting records:

- `docs/etc/operations/m0-t312-s1-live-bochs-dependency-audit-001.md`
- `docs/etc/operations/m0-t312-s2-original-softpc-binding-transfer-001.md`
- `docs/etc/operations/m0-t312-s3-retired-bochs-tree-removal-001.md`
- `docs/etc/operations/m0-t312-s4-softpc-formal-closure-001.md`
