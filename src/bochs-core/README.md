# bochs-core

Pinned Bochs 2.6 original mirror. Only `adapter-bochs` may call this
component. Any later divergence follows the mirror audit rules and is
registered here.

## M0 T273 S1 recovery register

The old tree is comparison-only until its individual files are copied into
this root by later admitted packets.  Its 205 upstream-relative candidates
compare to the pinned Bochs 2.6 baseline as 177 exact and 28 registered
differences; none lacks an upstream counterpart.  The supporting
[recovery audit](../../docs/etc/operations/m0-t273-s1-bochs-recovery-audit-001.md)
records the complete changed-file set and routes all new executable bodies to
a future private `bochs-core-overlay` implementation boundary.

The 28 difference candidates are not authorized merely by this register.
Their prior local divergence identifiers and detailed evidence must be
revalidated at copy time.  `bochs-core` has no service source yet, and no
formal build may read `src.old/bochs-core`.
