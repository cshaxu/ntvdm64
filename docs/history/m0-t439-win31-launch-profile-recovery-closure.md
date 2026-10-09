# M0 T439 — Win3.1 launch-profile recovery closure

T439 closes the recoverable installed-tree repair for Windows 3.1 portable
launch profiles. It restores the evidence-backed Standard VGA profile,
generates the explicit Standard/386 wrappers and PIFs, and keeps every tool
mutation recoverable through adjacent backups and CMD-owned Apply/Unapply
orchestration.

The task also completes a format correction in `GRP.EXE`: Program Manager
PMCC group records retain their pre-tag boundary and checksum while both item
paths and the affected working-directory tag are relocated. The owner accepted
the actual Standard-mode desktop and Program Manager result.

No NTVDM or guest executable byte was changed. The remaining normal Windows
3.1 Setup protected-mode transition is deliberately deferred to the queued
[ordinary-Setup candidate](../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md);
T439 does not claim enhanced-mode reliability.

Detailed inputs, focused tool results, direct group-file validation, and the
owner runtime acceptance are recorded in
[M0 T439 S1 evidence](../etc/evidence/m0-t439-s1-win31-launch-profile-recovery.md).
