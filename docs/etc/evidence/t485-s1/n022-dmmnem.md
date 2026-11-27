# S15 P44 N022 dialog mnemonics — delivered

Cached full build, runtime49 dual publication,40control cases, real CCPU
window-borrow lifecycle and SOL Options passed.
[Receipt](n022-dmmnem-result.json), [census](n022-dmmnem-census.json).
N02215/39 delivered; S15 9/14 and overall57/159 directories. No S/T closure.

## Original inputs and decisions

Fixed XP revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/client/dmmnem.c, blob
96342c274a7edbce5d9c391b3c8719c61be7c3d9. Registered OpenNT blob
4bbd8e3ed54824c995c2c0103a0fe8a681521542. Raw objects and complete comparison
products are under build/t485-s15/n022-dmmnem. Original XP has394lines.

Restore the entire XP FindMnemChar implementation, including DBCS virtual-key
matching and CH_ENGLISHPREFIX. Reuse native GetSystemMetrics/VkKeyScanW and
the original 0x1E constant; no duplicate parser or national-language branch.
XP's original disabled NT4 #if0 body and LATER delay arm retain their conditions
and #error exclusions. The obsolete OpenNT KANJI branch selection is not imposed
on the new original algorithm.

Restore XP INFINITE_LOOP_CURE1024 at all original search loops, pointer-shaped
casts and PTR_TO_ID, the explicit final NULL result, and class-atom IS_BUTTON
selection. Reuse the same retained class-identity binding established by P42;
move its macro to the shared dialog-manager composition scope rather than
duplicating it or retaining the old FNID predicate.

Keep the existing button borrow scope, finally-based state restoration/release,
and post-focus retirement check. A guest focus callback can retire the BUTN
storage while the original caller still clears BST_DONTCLICK. The existing
fixture exercises normal focus, destruction, exception and double-notification
prevention; this lifetime boundary remains necessary. No new queue or state.

## Verification and metrics

The mnemonic fixture now supplies the same explicit dialog/class context as
the keyboard fixture. Its exhaustion graph has1025nodes and expects1024search
iterations, matching XP's original bound rather than the old NT4 limit512.
Existing matching, escaped-prefix, first-character and callback cases remain.

Line mapping checked305matching original lines with no mismatches.
Nonerror22points/37lines against OpenNT becomes7points/22lines against XP:
-15points/-15lines. Two excluded original arms remain. Residual functional
differences are only the already retained callback lifetime protections.

Final full census:1329files,782byte-identical,793nonerror-identical,
536nonerror-different,7785points/116983lines. Before1329/782/793/536/7800/116998;
delta0/0/0/0/-15/-15. Baselines1281XP/48OpenNT. Final source/package hashes were rechecked and the census updated. P42 runtime47 Full24/24 is the last checkpoint;
this is the second changed mirror since that checkpoint. No guest changes.
