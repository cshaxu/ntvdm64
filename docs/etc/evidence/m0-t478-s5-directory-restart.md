# T478 S5 — bounded directory restart disposition

## Owner decision and scope

Baseline is S4 commit 31b809168. The owner explicitly rejects adding capability
beyond original OpenNT. An unavailable modern-Windows connection to the original
kernel indexed path may remain a registered gap. The owner additionally permits
finishing substantially implemented existing restart behavior; otherwise the
preferred result is explicit failure and removal of unnecessary scaffolding.
This S applies that conditional decision, not a new mutation-tolerance target.

## Current implementation and disposition

The implementation is already substantial and verified. Original FileFindReset
first attempts its optional indexed query and then its original index/name scan.
DIV-360 preserves reopening when reclamation discarded unconsumed entries despite
an underlying end status. Previously approved DIV-362 retains only one bounded
unconsumed successor name/index, preserves it across repeated close, and leaves
a matched successor unconsumed for FileFindNext. The old T430 deleted-anchor
failure predates those changes and cannot describe current behavior unchanged.

| Case | Current disposition |
| --- | --- |
| Normal first/next/end and close/reopen | Original scan and existing reclamation correction work; retain. |
| Remembered anchor deleted, saved successor survives and matches | Existing approved bounded hint works; retain. No new algorithm is needed. |
| Anchor and successor absent, hint absent or index mismatch | Existing explicit STATUS_NO_MORE_FILES; do not guess the next entry or add broader recovery. |
| NT4 kernel VdmQueryDirectoryFile indexed restart | No production binder exists. The original IRP/index interface cannot simply be called as a user-mode function; retain STATUS_NOT_IMPLEMENTED and original fallback. Accepted registered gap, not mandatory work. |
| Arbitrary rename, index reuse, concurrent directory mutation, indexed speed equivalence | No new guarantee or implementation is admitted. |

This selects the owner's retain/finish branch: the bounded implementation was
already complete for its accepted behavior. No production defect requiring an
additional change was reproduced. There is no basis to discard functioning
original fallback or the verified existing hint under the alternative instruction
to remove a merely fragmentary replacement. No new production code, kernel
provider, guest modification or original-mirror change is made.

Source references: [original boundary audit](../../../artifacts/mvdm-audit/external-kernel-vdm-boundary.md),
[T430 historical result](m0-t430-s5-dem-directory-reset.md), and
[previously approved successor recovery](m0-t448-s11-window-startup.md).
The unchanged source roles are demsrch.c, dem_restart_hint.h and monitor
vdm_control.c. This is not a claim to have implemented the missing indexed path.

## Current verification

- `verify-dem-directory-reset.ps1` compiles the actual selected demsrch.c under
  the production x86 CCPU40 contract and links real NT filesystem and monitor
  functions. Final provider-current run: 115 assertions, zero failures. It covers
  reopen, unrelated rename/create, deleted anchor, deleted successor, missing and
  mismatched hints, repeated reclamation, FCB formatting, errors and handle/buffer
  cleanup. Existing host fixture overrides are not actual guest evidence.
- `verify-dem-pressure.ps1`: NoDelete, NoPressure and Deleted all pass against the
  unchanged S4 product package. The probe uses 140 searches to exceed the original
  128-buffer limit and verifies continued DOS enumeration, including the existing
  deleted-anchor case. Final runs use runtime/tests for probes and temporary files.
- `verify-dem-fcb-guest.ps1`: actual INT21 FCB first/next/end/missing passes, with
  three unique 8.3 names, the 40-byte guest-owned witness and direct receipt 37.
  The runtime copy and temporary Z: mapping are cleaned. This verifies ordinary
  FCB entry, not an eight-minute timer observation.

Only test infrastructure changed. The provider runner was stale relative to the
current archive split; missing fixture objects were built, its link inputs now
include the existing thread/handle/process libraries, and an unrelated VM-frontend
teardown reference has a fail-fast test seam. Entering that seam aborts rather
than simulating a successful lifecycle; none of the directory assertions enters
it. The test map continues to pin the original directory functions.

The old FCB runner timed out waiting for an interactive COMMAND prompt, before
its witness could be accepted. The updated runner launches the same COM directly
with --wait and checks its existing distinctive exit 37, with a bounded package
PATH. This is a harness change, not proof or repair of a product prompt defect.
Initial cache/link failures and that timeout are not counted as passing tests.

## Delivery

Product source and images remain the verified S4 package
8f9f583a-a682-4175-b82e-1f926c9dbba4. Its Full 16/16 result remains S4 evidence,
not a newly executed S5 Full run. S5 compiles the current provider fixture and
checks the existing product build targets; focused tests exercise the unchanged
release. Worktree assets/release is hash-verified against the same ten-file
manifest; no new package identity or runtime behavior is claimed.

Finding 09 closes with verified existing recovery and an explicitly accepted
indexed-path gap. Document governance and diff checks gate local P1. No O:/winnt
write or push. S6/finding 04 is proposed next, not admitted.

## Unchanged mirror verification

Current demsrch.c SHA-256: `cb60c06af4075c049d66639ee42c6ea9c57d7019c5651f3f3f97fc8bb5890241`.
Pinned original SHA-256: `2fb56aad4d35a02dec38e9202d41b94a25076428c3d9eb08211d89f890c896ee`.
Byte equality: False; newline-normalized equality: False.
The retained differences include the already registered source selection,
DIV-360 and DIV-362; no mirror bytes or format-only changes were introduced
in S5. The existing original indexed provider remains outside product binding.
