# S15 P73 N023 obsolete diagnostic header

Question: does the old winerrp.h still belong to the XP mirror or any selected
product dependency? Start HEAD2d81ad5b2/runtime86. Original OpenNT object
f936cd534aa6c13eef4336fb34de2f20e245103d; current SHA256
a04d4685c2a5fef4c68614161a616eac40b4fd97efbcb5c0c9b52f2169e3ceff.
Before retirement it accounted for1nonerror point/284lines.

## Three-way disposition

Fixed XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89 has no
windows/core/ntuser/inc/winerrp.h. Git tree search of windows/public found no
same-name replacement. Original diagnostic macros instead reside in
windows/core/w32inc/w32err.h, blob a6910a79b13d80f2b0c6d34099a5aaec2e6809cd;
RIPERR retail line741 retains UserSetLastError and omits formatting operands.
Repository src/tools/test search found no include of the old header, only a
compat/precomp explanatory comment. That comment now names the XP owner.

Remove the obsolete mirror file, as required when XP removes an existing
directory member. No functions, exports, state or error behavior are changed.
No new diagnostic tool or replacement header is imported: existing selected
retail bindings already provide the finite required behavior. The old original
object and removed-file census remain in the [receipt](n023-winerrp-result.json).
This is removal, not a claim of original-byte equality or restored error code.
No #error or #line remains to count in the removed file.

## Verification and delivery

Cached full build passes without the file. runtime87 dual publication verifies
all13images at both destinations (26hashes). Real original Win16 controls pass
all9reported phases with zero errors and normal exit. No guest changed or
rebuilt. Earlier P72 controls40/40 and nine owner fixtures remain supporting
evidence for unchanged algorithms, not a claim of a new Full run.
Documentation governance and relative links are required before commit.

Non-guest mirror census:1332files,792byte-identical,803nonerror-identical,
529different,7936points/64711lines. Delta-1file/0exact/0same/-1different/
-1point/-284lines.1313XP/19OpenNT. N0234/4 reconciled; S15 11/14,
overall59/159. Seven completed files since P69 Full24/24. No S/T closure.
Next N024 kernel in DIR order, retaining the paired XP changes already delivered.
