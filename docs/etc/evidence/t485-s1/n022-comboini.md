# S15 P37 N022 combo initialization — delivered

Base commit: 0768da2d8f1380200e2c2dc7e8148ef32770d094.
XP commit: a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
XP path: Source/XPSP1/NT/windows/core/ntuser/client/comboini.c.
XP blob: fd0f1083974c4a740973985aa9265026e5fec7f2.
OpenNT blob: 63cf6725db28f9eaa6da5fda46ef9c0c8f2235c7.

## Whole-file disposition

The complete641-line XP original is restored byte-for-byte. This includes
original fInDestroy protection against reentrant destruction and the missing
child-EDIT checks during font/layout changes. P36 already paired the creation,
RTL geometry and CBOX layout; this completes the original file without changing
those contracts. No conditional branch is excluded and no #line is required.

The existing29-line wow_original_combo_info provider remains after the original.
Its consumer is the existing wow_control_get_combo_info callback boundary and
the XP CB_GETCOMBOBOXINFO dispatch. Modern GetComboBoxInfo cannot read the CBOX
owned by the restored control implementation. The provider validates borrowed
original objects and copies the public info; it owns no lifecycle, geometry
algorithm or secondary state. The unowned-native case remains with native USER.

The paired compatibility change is KPVOID_TO_PVOID's original non-WOW64 identity
expansion from w32inc/w32wow64.h line92, in the existing x86 control header.
Original freeing, lookaside ownership and synchronous callbacks remain.
No original guest or winnt.zip changes.

## Census and delivery

File: OpenNT1point/1275lines -> XP1point/29lines; zero point delta/-1246lines.
Global:1329files,781byte-identical,792nonerror-identical/537different;
7853points/117785lines. Delta0files/0exact/0same/0different/0points/-1246lines.
Baselines1274XP/55OpenNT. Byte-sensitive counting retains newline and EOF.

Cached full build, runtime42 dual13-image26hash publication, control39/39,
window-borrow including real combo creation/message/dropdown/cleanup, and
actual SOL Options passed. The whole XP641-line prefix is byte-identical;
only the already-existing29-line read-only info provider follows it. No extra
mirror conditional, #error or #line was added. Final build input matches the
published WOW32 manifest after fixture builds. P36 runtime41 Full24/24 is the
last Full checkpoint; this P has one changed mirror file and targeted tests.
No runtime42 Full execution is claimed.

## Sequence correction

P36's next-file pointer mistakenly omitted combodir.c between combo.c and
comboini.c. The inventory and census confirm combodir.c remains OpenNT-based.
P37 completes the already-started comboini.c; combodir.c is explicitly next,
before dlgbegin.c. N022 is8/39 delivered, not a claim that the first8 DIR entries
are complete. No missing file is counted as migrated.

[Receipt](n022-comboini-result.json), [census](n022-comboini-census.json).
