# S15 P43 N022 dialog navigation — delivered

Cached full build, runtime48 dual publication,40control cases, real CCPU
window-borrow lifecycle and SOL Options passed.
[Receipt](n022-dlgmgrc-result.json), [census](n022-dlgmgrc-census.json).
N02214/39 delivered; S15 9/14 and overall57/159 directories. No S/T closure.

## Original identities and scope

Fixed XP revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/client/dlgmgrc.c, blob
e3d10f0f07f5533857524819aed3326c4d6e9c46. Registered OpenNT blob
8d12f37231ed4503ab51ea4b4335b86761aaa47c. Raw snapshots and complete three-way
diffs are under build/t485-s15/n022-dlgmgrc. Original XP file has466lines.

Restore both complete public-shaped GetNextDlgTabItem/GetNextDlgGroupItem
wrappers. They validate HWNDs and use the already retained private algorithms;
no evidence requires their deletion. Source-definition aliases in the existing
composition header keep them distinct from SDK imports. Inspection found no
existing calls to the public wrapper spellings in these selected sources, so
this restoration does not redirect an existing native USER route.

Restore XP _NextControl's finite traversal bound (original bug272874), original
FUNCLOG3 annotations and original DBG guard spelling. All three DBG arms remain
explicitly excluded under their original conditions. There is no local runtime
algorithm, extra compile condition, copied traversal or original guest change.

## Verification and difference accounting

Existing navigation tests exercise nested/disabled/invisible controls, tab/group
order, withdrawal, default buttons and the no-eligible-control sentinel. Added
wrapper cases cover valid/NULL starting controls and invalid dialog/control
handles. These use the real restored wrappers, not a duplicate implementation.

Original logical-line check covers357matching lines with no mismatches.
Nonerror3points/78lines against OpenNT becomes6points/6lines against XP:
+3points/-72lines. All six residual lines are #line mappings around the three
excluded original DBG arms. Restoring the omitted contiguous function bodies
removes functional deletions and exposes the independent mapping points; no
functional mirror difference remains. Original replacement and this mapping
cleanup are covered by the owner's standing approval.

Final full census:1329files,782byte-identical,793nonerror-identical,
536nonerror-different,7800points/116998lines. Before1329/782/793/536/7797/117070;
delta0/0/0/0/+3/-72. Baselines1280XP/49OpenNT. Final source/package hashes were rechecked and the global census updated.

P42 runtime47 Full24/24 is the last ten-file checkpoint. This batch requires
the normal targeted five-step delivery, not a repeat of unchanged Full gates.
