# S15 P53 N022 single-line EDIT auxiliary operations - delivered

## Scope and fixed inputs

Start HEAD9a86baf76, runtime61 delivered. Whole edslrare.c migrates from
OpenNT blob363d56604a2f5f326c04e5bcbb45e7f2bfdda6a4 to XP SP1
blob46fb00a8ca54a7f43bacc8b43080275e53892116 at pinned revision
a2f6d7c93aa4f11efc51147566d5961c59a8ef89. Before nonerror1point/383lines.
Global1329files/785byte-identical/796nonerror-identical/533different/
7834points/93545lines. No original guest modification/rebuild.

## Whole-file restoration and dependencies

Restore XP SLCreate and SLUndo complete selected algorithms: allocation and
failure order, initial text/password setup, undo buffer release, window lifetime
validation and EN_UPDATE/EN_CHANGE ordering. XP adds the existing native
NotifyWinEvent value-change notification after undo. P50 already paired the
SLCreate signature; P51 supplied the original maximum-user-address binding.
No new provider, algorithm, queue or state. Original NEVER branch remains
#error with original condition and #line; obsolete SUPPORT_LPK code is absent
from XP. Original source composes directly through existing bindings.

## Final verification and delivery

runtime62 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and genuine guest SOL Options pass.
[Receipt](n022-edslrare-result.json), [census](n022-edslrare-census.json).
N02224/39; overall57/159 and S15 directories9/14 unchanged. Two completed
mirror since P51 runtime60 Full24/24; no new Full or S/T closure claimed.

## Mirror accounting

edslrare.c OpenNT1point/383lines becomes XP2points/2lines, all diagnostic#line.
The original unselected NEVER arm retains #error. Every matching
executable original line maps correctly; selected algorithms have no changes.
Original LF restoration exposes the two separate line-mapping blocks.
Global before:1329files/785byte-identical/796nonerror-identical/533different/
7834points/93545lines. After:1329/785/796/533/7835/93164.
Delta:0/0/0/0/+1/-381. Baselines1291XP/38OpenNT. Newline/EOF counted.
