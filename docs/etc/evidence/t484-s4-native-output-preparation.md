# T484 S4 native output and EDIT preparation

## Scope and ownership

P27 moves native/guest output input aliases into the existing CMD preparation
owner. The shared physical stage accepts TestInBinary, placing inputs under
its selected binary directory's tests child. Default package-root staging is
unchanged. This preserves the real probe's system32 working directory and
relative Run16/COMMAND/tests paths; neither guest nor product behavior changes.
The existing test-subdirectory validation, source ownership and cleanup remain.

Both native output and COMMAND-to-EDIT runners retain a hash-verified package
manifest. Native output extracts gate evidence from its resolved tests path.
EDIT already uses CMD physical staging and a CMD-built native Userenv reader;
no second setup implementation is introduced. Result extraction remains test
observation, not an interpreted build/preparation dependency.

## Verification and explicit failure

Owned Userenv process25648,2026-11-24T19:31:35Z to19:33:22Z, runs
build/t484-s4-native-output/run.ps1. Cached full build passes. Package identity
and staging contracts pass, including default inputs, structured/flat binary
input placement, subdirectory validation and rejection without input files.
The real concurrent output assertion fails with missing NATIVE-DURING-DOS.

After adding package-manifest evidence, owned process31016 runs final.ps1,
19:34:19Z to19:35:22Z. The final native-output runner again fails the same
strict assertion: native exit37, DOS result0, NATIVE-DURING-DOS present in the
intermediate native snapshot but absent after DOS return. This reproduces the
[previously recorded product failure](t484-s3-native-output-isolation.md).
No assertion is weakened and this is NOT a functional acceptance pass. TODO's
separate product investigation remains open. The supervisor's exit0 means its
migration checks and independent EDIT case completed, not that both tests pass.

The independent private-desktop COMMAND -> native EDIT -> CMD -> DOS case
passes: actual EDIT menu/status bar, Ctrl+Q, CMD echo, DOS MEM and launcher exit1.
This uses current-token Userenv; inherited developer-environment behavior is
not newly claimed. Verified manifests and screenshots/console records remain
under build/t484-s4-native-output. Both final isolated stages were removed.

All11 raw host images and22 dual-published hashes remain unchanged. Reuse the
identical publication; no new Full, product repair or original guest rebuild
is claimed. Original media and assets/winnt.zip are untouched. Governance,
parser and diff checks gate the commit.

## Progress

Frozen census344/355 disposed,11 pending; supplemental preparation chains6
reduce to5. These are migration dispositions, not all-test-pass counts. S4 and
final clean S5 acceptance remain open.
