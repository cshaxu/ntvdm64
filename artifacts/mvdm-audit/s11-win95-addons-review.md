# T473 S11 owner-requested Win95 add-on review

Owner requested inclusion of current compat95, patch95, win95-launch and their
related tests. These independently authored add-ons do not alter the host ten
image set. The release artifacts are published only under assets/release.

The command-only launcher now prepares identity-checked VMM32 and KRNL386
copies, a private SETVER copy, and the selected mouse driver with backups and
a schema-3 journal. PATCH95 requires supported source hashes, creates fresh
outputs, preserves unchanged DS blocks and verifies final hashes. COMPAT95's
keyboard interrupt trampoline preserves its published return offset while
restoring the interrupt flag expected by the DOS7 IRET-style chain.

Verified: both add-ons built from current source (MSVC /W4 /WX and NASM);
release patcher passed original-input SHA identity, idempotence, unsupported-input
and no-input-mutation checks for VMM32, kernel and SETVER. Three command tests
passed after release staging: occupied profile refusal, failed preparation and
schema-1 unapply. The manifest binds the new binaries and sources; compat95 is
marked candidate. Schema-3 successful install/unapply and actual Win95 keyboard
runtime behavior were not tested in this review; do not call the driver verified
or claim end-to-end Win95 acceptance. No original guest or media was modified.
Prior release add-on files are retained in build/t473-s11/win95-owner-review.
The unrelated S11 WOW candidate and concurrent changes remain uncommitted.
