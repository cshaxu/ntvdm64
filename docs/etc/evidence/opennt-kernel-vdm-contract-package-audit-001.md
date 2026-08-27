# OpenNT kernel VDM contract-package audit

The external `base/ntos/vdm` package is now tracked as a kernel-contract
package, not as `opennt-host` source. Its 26 local OpenNT files are hash-listed
with a final non-link disposition. The operation ledger records all fifteen
public `NtVdmControl` service classes, their selected MVDM call sites, original
kernel handler/dependency and one final user-mode owner or explicit exclusion.

Only exact declarations may be selected into `mvdm-platform-abi`. Kernel source
files never enter the user-mode host link closure, but they remain candidates
for a required **per-file/per-block source-derived-adapter audit**. The already
observed `vdm.c` IRP/MDL/file-object/IRQL blocks and `vdmentry.c` `Ke386*`
execution blocks are kernel-bound exclusions; the remaining package must not
be prejudged.

The operation ledger supersedes the former single monitor-family statement for
kernel VDM detail; the monitor family remains only the aggregate owner index.
