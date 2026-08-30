# M0 T310 S21 closure — storage controller callbacks

S21 closes the original SoftPC storage-controller source contract for the
selected Win32/x86 CCPU40 profile.

- The original INT 13, WD2010, FDC, remote-floppy-client and EMS controller
  bodies remain selected mirror sources; no controller logic was rewritten.
- Callback tables and their declarations, initializers and indirect calls were
  reviewed together. No source ABI repair was necessary.
- One earlier mirror guard incorrectly suppressed original `host_delays` under
  CCPU. S21 restores that independent timing global while retaining the
  registered guard only for duplicate SAS-memory globals.
- The fresh 358-edge forced-link candidate contains no unresolved
  `original-softpc-disks.lib` entry. The remaining unresolved entries are
  named later owner packages, including S22 host-storage endpoints.

This is source and formal-assembly closure only. It does not claim that a
guest disk, floppy or EMS workload has executed. S22 owns host-media binding;
S49 remains the mandatory integrated machine verification.

Evidence: [source boundary](../etc/evidence/m0-t310-s21-p1-storage-controller-source-boundary-001.md),
[call contract](../etc/evidence/m0-t310-s21-p2-storage-controller-call-contract-001.md), and
[formal assembly](../etc/evidence/m0-t310-s21-p3-storage-controller-formal-assembly-001.md).
