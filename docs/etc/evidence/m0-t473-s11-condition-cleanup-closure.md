# T473 S11 — authored compilation-condition cleanup

The owner narrowed S11 to non-MVDM authored compilation conditions. The
unfinished original execution and later recovery scopes are handed to the
first queued original-source recovery proposal; they are not claimed complete.
S12 retains the separately approved NTCON input-source handoff scope.

## Disposition

The [condition audit](../../../artifacts/mvdm-audit/condition-inventory/README.md)
links the source-site inventory, selected WOW binding ledger, adapter
preprocessing comparisons and final build/package checks. Original-source,
third-party and generated carriers are identified separately from authored
code. WOW selectors select specific original declarations/bindings; SDK and
architecture conditions retain their documented API, layout or width roles.
The original C-VID accessor and CPU40 use different branches of the same
provenance-registered SAS carrier, as demonstrated by actual Ninja dependencies.

The delivered work removes inherited CCPU/CPU40/video/NTVDM/ANSI/WIN32
definitions from the selected authored build commands, an unused graphics
flush probe, seven redundant `#ifdef` wrappers around `#undef`, and a
`UNICODE` selection between identical dialog resource pointer aliases. The
x64 builds no longer inherit the unused thread-alert adapter selection.
Original-source choices and the original guest remain intact. Earlier scoped
mirror changes are separately recorded as MVDM-HOST-DIV-479, rather than
being attributed to this final non-MVDM cleanup.

## Verification and boundary

The final cached x86 and x64 builds and 428-command authored-profile check
passed. After applying the released identity, all ten rebuilt images matched
the published images exactly or differed only in PE link/debug-directory
timestamps; both release destinations matched the published ten-image
manifest. The unchanged package's Full 16-gate suite, Win3.1 Standard/386 by
DOSONLY matrix, CCPU and input-focused evidence are linked from the audit.
No repeated product suite or redeployment was needed for timestamp-only
relinks. Those gates do not establish physical mouse acceptance or resolve
transferred recovery issues. The original guest binaries were not changed.

## Historical setup checkpoint

The prior S10 add-on/setup checkpoint and its then-pending investigations
remain in the [add-on review](../../../artifacts/mvdm-audit/addon-layout-review.md#current-p-setup-correction-and-closure-gates)
and [Video7 pointer evidence](../../../artifacts/mvdm-audit/video7-pointer-review.md).
These are historical evidence, not an active S11 packet.
