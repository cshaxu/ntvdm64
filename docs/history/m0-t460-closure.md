# M0 T460 closure — WOW32 Clipboard and device-lifetime recovery

T460 closes the former first Queue candidate, WOW32 Clipboard. It retains the
original OpenNT Clipboard owners and their public USER boundary; it does not
modify, rebuild, or package guest media.

## Delivered scope

- S1 established the original-owner and callback-ABI matrix.
- S2 restored and accepted the selected `CF_TEXT` guest/native transfer,
  rejection and original-cache ownership path.
- S3 completed the original deferred-render, replacement, callback retirement
  and task-loss boundary. It also delivered the independently scoped PIC ICW1
  device-EOI cancellation and keyboard early-EOI/read-order repairs.
- S4 accepted the same original `HGLOBAL` route for `CF_TEXT` and `CF_DIB`,
  including normal transfer, rejected data, replacement, deferred rendering,
  callback return and owner task loss.

The detailed evidence is [S1](../etc/evidence/m0-t460-s1-clipboard-owner-matrix.md),
[S2](../etc/evidence/m0-t460-s2-clipboard-transfer-acceptance.md),
[S3](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md),
[PIC](../etc/evidence/m0-t460-s3-pic-eoi-reset-plan.md),
[keyboard](../etc/evidence/m0-t460-s3-keyboard-early-eoi-plan.md) and
[S4](../etc/evidence/m0-t460-s4-clipboard-selected-format-acceptance.md).

## Final package and verification

The coherent ten-image S3 package was published at `5e2e8bfaf`; the later
audit/package collection at `c1729f5a9` was checked against that delivery.
For the three rebuilt images whose file hashes differ (`ntvdm.exe`, `ntsrv.exe`
and `nthook32.dll`), code/data/import section comparison found no runtime
logic change; the difference is linker metadata only. The current package's
sealed, current-source `CF_DIB` full-transfer witness passes all 18
checkpoints. Current-package `CF_TEXT` transfer, callback-return and task-exit
witnesses also pass.

The closure also retains the completed build and device evidence: two cached
full component builds (second build reused all 1509 intermediates), 24
PIC/RTC/mouse assertions, 46 keyboard assertions, VGA source contracts and
the Win95 launch-tool recovery/safety fixtures. The release manifest remains
the authority for the deployed `assets/release` and `O:\\winnt\\system32`
ten-image set.

Freshly recompiling the shared-public-Clipboard DIB peer was observed to be
environment-sensitive at the guest rejection checkpoint, while its sealed
input set (whose source hashes equal the checked-in S4 sources) passes on the
same current package. This is retained as a test-host limitation, not claimed
as a product regression: public Clipboard arbitration is outside the fixture's
ownership. It does not alter the recorded accepted S4 matrix; a future test
infrastructure task may make that fresh-build witness deterministic without
changing Clipboard behavior.

## Boundary handed forward

Clipboard DDE remains unimplemented and is explicitly still the later Queue
candidate. The next unadmitted capability is shell and layered Registry. No
T460 result asserts DDE conversation, reply, cancellation or DDE-data
ownership semantics.
