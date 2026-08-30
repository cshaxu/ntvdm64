# M0 T310 S28 — host-platform bindings closure

S28 closes the selected SoftPC host-platform bindings for Win32/x86 CCPU40.
It selects the original SoftPC trace provider, retains the documented patch
idle-link body without claiming scheduler recovery, binds the original Base
client exit route, and exposes Console Server registration as explicit
unavailable rather than a fake transaction.

All remaining forced-link edges have a non-S28 owner.  The closure does not
claim integrated execution, private Console Server recovery, or an idle-policy
implementation.

Evidence: [P1](../etc/evidence/m0-t310-s28-p1-host-platform-source-boundary-001.md),
[P2](../etc/evidence/m0-t310-s28-p2-host-platform-trace-and-idle-assembly-001.md),
and [P3](../etc/evidence/m0-t310-s28-p3-host-platform-owner-disposition-001.md).
