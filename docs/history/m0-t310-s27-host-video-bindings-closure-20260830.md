# M0 T310 S27 — host-video bindings closure

S27 closes the selected SoftPC host-video endpoint contract for Win32/x86
CCPU40.  Original controller and host caller bodies remain source-shaped.
Public GDI palette calls link directly; retired NT4 Console Server graphics
calls retain their original ABI through a small adapter/session presenter seam.

The seam reports an explicit unavailable result without a presenter and
delivers typed host-only dirty-rectangle, palette and activity notifications
when one is bound.  It owns neither a video controller nor a UI.  Formal
forced-link evidence has no S27 unresolved edge, and the focused contract test
passes.  Guest-visible graphics and display presentation remain S49 work.

Evidence: [S27 P1](../etc/evidence/m0-t310-s27-p1-host-video-source-boundary-001.md)
and [S27 P2](../etc/evidence/m0-t310-s27-p2-host-video-formal-assembly-001.md).
