# M0 T435 Windows 1.01 add-on closure

## Owner acceptance

Owner confirms “好，t任务完美完成。” This closes T435 after installation,
profile recovery and asset synchronization delivery. Owner then directly admits
the former queue-tail Windows 3.x non-WOW candidate, starting with Windows 3.1.
CURRENT holds that new packet; other [Queue](../states/QUEUE.md) candidates retain
their relative order.

## Delivered work

- Independent Windows 1.01 INT33 mouse bridge, authored source now in
  src/addon/win101-mouse-drv; original guest core remains unchanged.
- Installer and launch implementation in src/addon/win101-setup: original
  media preserved, additions under PATCH, no hardcoded machine paths or drive
  mappings, no-argument interactive Setup, installed-directory confirmation,
  temporary WORK cleanup, self-contained installed PIF/configuration, win.cmd
  only and final pause with actual exit-code preservation.
- Owner-provided assets committed; assets/release subsequently synchronized
  to the actual verified published ten-image package with its hash manifest.
  EXECUTION requires same-commit release synchronization for product changes.

Implementation/publication S2 commits263118b33 and7d273e11c are followed by
S3 installer/assets delivery dd7cddd7d and verified asset/rule follow-up
9e2b16170, all pushed to main before this closure.

## Evidence and limits

[S1 audit](../etc/evidence/m0-t435-s1-win101-mouse-audit.md),
[S2 implementation](../etc/evidence/m0-t435-s2-int33-driver.md) and
[S3 installation/delivery](../etc/evidence/m0-t435-s3-setup-package.md) retain
source/ABI, failures, recovery, tests and publication identities. S2 includes
real Windows mouse/menu interaction, repeated same-worker startup/normal exit,
guest checks, full14 gates and deployed smoke. S3 final focused fixtures cover
profiles, temporary-copy repair, success/failure/recovery and cleanup. Owner
confirms installation and Notepad execution inside Windows 1.01.

Published APP0.0.435/RPC45/I/O25 stays unchanged. Ten assets match the tested
S2 manifest and live O:/winnt/system32 hashes. No new runtime publication,
process interruption or full matrix is performed for this documentation closure.

Direct run16 of Windows 1.01 Notepad remains rejected by the retained original
classification rule: SEC_IMAGE returns C000011B, mapped to OS/2 by the original
classifier, and run16 returns50 before WOW submission. This is recorded, not
repaired or claimed compatible. Physical/RDP mouse capture, dragging and broad
application compatibility are not inferred from the bounded acceptance.

Documentation governance/link checks, diff review and synchronized clean Git
state are the closure gates; no original guest or mirror change is introduced.
