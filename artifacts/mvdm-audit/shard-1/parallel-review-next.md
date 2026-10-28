# S10 shard 1: WOW USER generated assembly provenance

Scope: two still-pending physical paths in the original shard-1 partition,
`wow16/user/k/daytona/wowk.asm` and `wowrtl.asm`. Both are current-only in the
frozen reconciliation: neither occurs in the two original MVDM catalogs, and
there is no recorded original file hash. Do not use a 100% original-source
retention ratio for either file. No guest code or image was changed or rebuilt.

The [replayable observer](parallel-review-next-observer.py) and
[recorded hashes and results](parallel-review-next-evidence.json) establish:

1. The current USER makefile, conversion script and sed rules are byte-identical
   to primary OpenNT. For x86, the recipe selects `wowk.obj` and `wowkr.obj`
   and links them into `USER.EXE`; the retained `userk.lrf` lists both objects.
2. Applying the pinned conversion rules to the retained `wowasmk.asm` gives
   every line of retained `wowk.asm`. Applying them to `wowrtl.asm` gives
   `wowkr1.asm` with exactly three additional removals of `SHORT` at lines
   239, 242 and 506. The original batch file has a jump-range filtering step
   that can perform such removals, but no original build log establishes the
   cause of these three particular removals. A second conversion reproduces
   `wowkr.asm` line by line.
3. The retained USER map attributes line-number sections to `wowk.obj` and
   `wowkr.obj`, and contains representative `ValidateHwnd`, `GetClassNameA`
   and `HMValidateHandle` symbols. The retained `USER.EXE` and installed
   `O:\winnt\system32\USER.EXE` have the same SHA-256. The recipe points to
   original `windows/core/ntuser/client/wow.c` and `rtl/wow.c`; matching names
   occur there, but symbol-name similarity is not binary provenance.

| Path | Recommendation | Remaining proof |
| --- | --- | --- |
| `wowk.asm` | Pending; probable generated guest USER build input | Establish provenance of retained `wowasmk.asm` from the corresponding source revision and attribution of the retained object/image without rebuilding guest; do not treat the generated file as a modern host implementation. |
| `wowrtl.asm` | Pending; probable generated guest USER intermediate | Establish provenance from the corresponding RTL source revision and retained object/image; verify original toolchain transformation beyond textual equivalence. |

The two paths should not be marked restored or excluded solely because their
recipes describe guest assembly. Their corresponding USER behavior remains in
scope irrespective of how the generated source paths are eventually classified.
In particular, historical assembly contains private USER-layout assumptions;
this does not authorize interpreting modern host pointers as NT4 structures.
