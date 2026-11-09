# T481 S7 owner-directed closure

The owner tested the deployed Win3.1 configuration and reported that basic
interaction works and the first DOS window no longer goes black after opening
the second. The tested O:/w31 configuration removes `mouse=*vmd` while
retaining `device=*vmd` and both authored drivers. The investigation and its
limits are recorded in [S7 investigation](m0-t481-s7-investigation.md).

This is an owner-directed bounded closure, not a claim that the original VMD
binding failure was root-caused. The win-multitask installer candidate and
test changes remain uncommitted and unpublished; qualification, publication,
review and delivery remain T481 obligations. Both x86 and x64 non-desktop
CTest sets pass 46/46 after correction of stale CPL test expectations. Full
product and desktop gates are not established by those results.

The subsequently reported invisible mouse pointer in full-screen Win3.1 DOS
sessions is admitted as S8. The owner subsequently inserted the SoftPC import
audit as S9 and moved Win95 research and validation to S10. Neither is waived
by this closure.

Pending installer review: `tools/win-multitask/APPLY.CMD` and `UNAPPLY.CMD`
use CMD built-ins plus the released `HASH.EXE`; their writes target the
selected installation's `SYSTEM.INI` and owned PATCH files. They do not patch
original guest executables. The checked-in candidate package test exercises
Win3.1/Win95 detection, VMD entry handling, refusal and restore cases. This
is a source-scope review, not new runtime qualification; the installer has not
been published. Its reversible-install records are part of the separate
installer workflow and were not added to the owner's current O:/w31 setup.
