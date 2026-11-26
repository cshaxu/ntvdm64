# S7 P10: original sound CLI requests and their ABI

## Whole-file conclusion

`softpc.new/host/src/nt_sb.c` has four additions to pinned XP blob
`af611f75f3a0702c46d3b66a554c4983176a23b7`: two `#ifndef CCPU` guards and their
closing directives around the CLI enable/disable requests in InitDevices and
SbCloseDevices. All four are removed. The complete file is byte-exact XP,
with zero non-error differences and original diagnostic lines. There are no
excluded bodies in this file requiring replacement.

The frozen classification HXP-R-03954 through HXP-R-03957 incorrectly treated
lack of kernel VDM support as sufficient evidence to exclude these calls.
The actual provider is compat/vdm_control.c: NtVdmControl returns
STATUS_NOT_IMPLEMENTED without reading/writing the request or altering last
error. Both original callers ignore this status and continue normal device
initialization/cleanup. No CPU interrupt state is changed by this binding.
Restoring the calls neither enables kernel monitoring nor adds emulation.

## Necessary declaration counterpart

The canonical non-mvdm/public/internal/base/inc/vdm.h lacks the XP request
records. Restore the complete contiguous original declaration unit between
VDMSET_INT21_HANDLER_DATA and the following _NTDEF_ branch, from XP blob
`10880392902878fffd1335b371459799b682f564`. This includes LDT, ADLIB, CLI and
initialization request records and constants. No duplicate definitions were
found in selected sources. Existing nt_sbfm.c exclusions remain unchanged;
declaring ADLIB requests does not enable its behavior.

Restoring only the CLI fragment split the missing declaration block and
increased XP diff points; the final whole-unit restoration avoids that.
The unchanged header exclusions are not counted as new restoration. Relative
to XP, the header delta is minus 3 points and minus 45 diff lines; nt_sb.c
adds minus 4 points/minus 4 lines, giving minus 7/minus 49 for this batch
against XP. These are differences, not source-size reductions.

The header still has its historical OpenNT baseline and other existing ABI
adaptations, including inline interrupt tables and FAMILY_TABLE. This limited
required dependency is not complete N016 migration: N016 must reconcile its
whole XP ABI rather than blindly replacing those live layouts. A #line 324
anchor resumes the historical OpenNT mapping after the inserted unit. The
addition is one contiguous declaration point under the owner's standing
approval for one point below 100 lines. No runtime algorithm or layer boundary
is added. N016 and the 39/159 migration counter remain unchanged.

## Final verification and delivery

Runtime13 cached full build and both 13-image publication destinations passed.
The original NtVdmControl ABI fixture passed all service values and the five
CLI controls with untouched request/last-error assertions. Actual WINMINE
window passed. Win3.1 enhanced reached desktop and exited normally; its strict
roundtrip witness is `window-input=1 fullscreen-text=1 returned-window-input=1
second-fullscreen=1 same-window=1`. NESticle produced the real 640x400 menu,
hid it with Escape and restored the identical menu mask on the second Escape.
Evidence is under build/t485-s7/p10/final13. Full was not rerun for this batch.

Whole-file self-review confirms byte-exact nt_sb.c, original declaration names
and layouts, unchanged unavailable-service provider, unchanged header exclusion
arms, preserved historical diagnostic mapping after the restored unit, and no
new adapter/callback/state. All four original audit IDs are fully restored.
The header's historical-baseline increment remains registered separately.

S7 final-verified primary progress is 15/20 (five remaining). Primary-file
cumulative reduction is 856 points/7592 lines; including this batch's required
header XP-relative reduction, cumulative S7 reduction is 859 points/7637 lines.
Including S6 gives 860 points/7638 lines. These counters do not claim complete
N016 migration. Commit/push closes this delivery, not S7 or T485.
