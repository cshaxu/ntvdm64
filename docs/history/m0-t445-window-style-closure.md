# T445 Win3.x appearance closure

## Owner disposition

The owner directs closure after S9, requests one final commit restoring the
W16Style demo's20px proportions and removing the temporary generated caption
prefix, and explicitly permits the historical unstyled native-menu animation
investigation to remain in TODO. This is owner-directed closure, not a claim
of additional manual testing. No subsequent task is admitted automatically.

## Delivered ownership

- NTW16S32/64 own the reusable presentation engine; W16Style.exe is its thin
  released demo. Title/menu/standard-control System bitmap type is20px.
- Run16's --win16-style and --win16-style-all retain their scopes and child
  propagation. Native Hooks attach at their native width. WOW32 attaches the
  x86 style DLL for Win16; ntvdm.exe and Windows WOW64 are not style owners.
- NTSRV client core is separated from launch-style state. NTCON links core
  only. Shared/separate WOW propagation retains existing creation contracts.
- The temporary title-marker module and two diagnostic exports are removed.
  Application captions are never prefixed or stripped. Real attachment,
  frame state and rendering now supply the tests' independent witness.
- Demo geometry returns from24/16 to20/16 of its authored grid. This is not
  automatic reflow of arbitrary third-party applications.

## Stage record

| Stage | Disposition |
| --- | --- |
| S1 | Source-backed Win3.1 design; [evidence](../etc/evidence/m0-t445-s1-installation-design.md). |
| S2 | Owner-accepted palette repair; [evidence](../etc/evidence/m0-t445-s2-palette-repair.md). |
| S3 | Standalone visual experiment; historical native animation concern explicitly deferred; [evidence](../etc/evidence/m0-t445-s3-style-experiment.md). |
| S4 | Menus, MDI and standard controls, including later background/scroll corrections; [review](../etc/evidence/m0-t445-s4-menu-mdi-review.md). |
| S5 | Modular style DLLs and released demo, owner-closed without manual acceptance; [evidence](../etc/evidence/m0-t445-s5-style-dll.md). |
| S6 | CLI and propagation work, retained ingress gaps completed in S7; [evidence](../etc/evidence/m0-t445-s6-real-chains.md). |
| S7 | RPC core/style split, separate-WOW propagation and registered DIV-331 environment repair; [evidence](../etc/evidence/m0-t445-s7-base-rpc-client-split.md). |
| S8 | Hook/WOW32 integration, coherent13-image release; [evidence](../etc/evidence/m0-t445-s8-style-integration.md). |
| S9 |20px font, actual menu hover/About corrections and final demo/caption cleanup; [evidence](../etc/evidence/m0-t445-s9-integrated-acceptance.md). |

S9 P1 is266067264cd7e26079ec67550f0368f6e5f7ab79. The commit containing this
closure delivers P2 and its verified publication. No original mirror/guest
change is introduced by S9/P2; the T as a whole retains the separately
registered S7 DIV-331, so it must not be described as entirely mirror-diff-free.

## Verification and limits

The S9 evidence records exact candidate, background runs, positive/negative
checks, full Product/Control regressions and three-way publication hashes.
The coherent13-image release manifest is the current artifact authority.
Earlier stages closed without manual testing are not retrospectively called
manual passes. P2 off-screen captures are not new physical-pointer evidence.

The [debt ledger](../states/TODO.md) retains native-menu animation diagnosis,
bounded CP1252/custom-control coverage and fixed-layout host About clipping.
SOL/WRITE's known initialization failures remain with the existing basic-WOW
queue packages; error dialogs are not usable application acceptance.
No global font/theme changes, automatic application-layout engine, guest
patch, new CLI parameter or unrelated WOW recovery is claimed.

[Archived status](m0-t445-pre-closure-status.md) preserves the former detailed
status/history. [Current status](../states/CURRENT.md) alone governs admission;
[Queue](../states/QUEUE.md) order is unchanged.
