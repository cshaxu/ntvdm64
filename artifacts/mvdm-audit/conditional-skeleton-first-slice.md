# T473 mirror conditional-structure restoration — first verified slice

The owner requires original conditional structure to remain, including nested branches inside a deleted body, and `#error` for removed branch code. This is an intermediate slice, not a complete audit or closure.

- `softpc.new/base/bios/reset.c`: restore the original CPU30/CPU40 guard inside the unselected non-PROD branch.
- `softpc.new/base/support/terminat.c`: restore the original removed CPU30 conditional shell.
- `softpc.new/base/disks/gfi.c`: restore the original nested FLOPPY_B conditional inside the excluded SLAVEPC branch. The existing outer `#error` remains.

The three branch conditions and nesting were checked against `O:/repos.external/OpenNT/base/mvdm`. Retained `#line` directives preserve current selected-source diagnostic line numbers. The three x86 objects built successfully; their whole-file hashes differ from previous cached objects. The NTVDM relink `.text` and `.data` match the published package byte-for-byte; `.rdata` differs at only the two bytes of the existing PE debug-directory timestamp, within the timestamp field. This is not whole-image identity. No guest source or image changed and no runtime tests were run for this slice.

The current lexical candidate ledger has 141 files with unmatched original directives, not all of which represent missing skeletons. The three changes above do not close that ledger: the remaining original-to-current mapping, nested branches, condition selection, and line recovery must be reviewed and verified as a group before a comprehensive result is claimed.
