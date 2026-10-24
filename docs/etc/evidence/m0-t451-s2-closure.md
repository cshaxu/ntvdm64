# M0 T451 S2 — USER publication closure

## Delivered boundary

S2 closes the bounded publication rows R01, R03, R04, R06, R07 and R08. The
selected original USER16/WOW32 readers and algorithms remain in place. The
project-owned adapter contributes only a copied original-layout view and its
transaction/lifetime boundary; it adds neither a USER server, native-pointer
projection nor guest modification.

The sole production correction is the missing first-task publication edge:
after original `xxxInitTask` succeeds and before original `W32Thread` returns
to guest execution, the existing `publish_guest_return` path publishes the
desktop/task-base view. Failed publication remains a failed original task
handoff; retry resumes the incomplete publication and never repeats a completed
one. The full source ownership audit is retained in
[the S2 publication audit](m0-t451-s2-user-publication-audit.md).

The candidate also restores the previous CUI `wWinMain` Run16 entry pair,
removing the later `wmain`/manual command-line-tail splitter. This is a direct
reversion of that entry-normalization change, not an invented launch parser.

## Evidence

| Gate | Result |
| --- | --- |
| Focused original-owner/task/class/initial-publication cases | Pass; exact markers and paths are recorded in the S2 audit. |
| Full x86/x64 ten-image build | Pass; `r25-full-component`, 10 images and 1,476 intermediates; second increment compiled no intermediates. |
| Candidate staging | Pass; each selected image's machine and SHA-256 verified before copying into the immutable baseline package. |
| Actual immutable WOW guest | Pass; candidate `WINMINE.EXE` flag, reset, open-cell, WM_CLOSE and Run16-completion flow completed. The guest input SHA-256 is `633C1328922B4092351E0019B1BA1C6D56C0F0FEA83824CC8B7E2E731B15E80B`. |
| Publication | Pass; all ten candidate hashes agree between the staged manifest, `assets/release` and `O:\\winnt\\system32`. |

The tested final package manifest declares `run16.exe`
`C482F54B93C3EC3F1F0DEC1D2D979ED7ACA7DBC1C22A6E7CF2F4343F8DD38C99`,
`ntvdm.exe` `8333B6D92AAC64A753998DA10359FEB5AA7FC5729760599A25A6F931A34F2224`,
and `WOW32.DLL`
`475275C6B1D9F4976CC5371860D0EF19F072D07CE11D8884FCBA92B34AF62BF9`;
the complete authoritative list is
[`assets/release/release-manifest.json`](../../../assets/release/release-manifest.json).

No guest file, guest configuration or guest archive changed. The prior release
set was snapshotted before replacement; the committed prior repository revision
remains its recoverable source/asset baseline.

## Handoff

R05, R09 and R10 are deliberately not folded into S2. S3 owns host-originated
WND scalar/geometry updates, stale handle reuse, nested destruction, rollback
and the selected `SERVERINFO`/`CLIENTINFO` timing paths. It must reuse this S2
view owner rather than construct another object table.
