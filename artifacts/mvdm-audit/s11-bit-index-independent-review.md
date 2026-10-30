# S11 independent CCPU memory-bit-index review

This is a source and arithmetic review of the uncommitted
`MVDM-HOST-DIV-469` candidate in `softpc.new/base/ccpu386/c_oprnd.h`. It is
not a product build, native-instruction or guest execution result. No guest
or product source was modified by this review.

For a register bit index interpreted as signed `s`, the expected element
displacement is `floor(s / width) * (width / 8)`. The candidate expressions
are `((u16 >> 4) * 2) - (sign16 ? 8192 : 0)` and
`((u32 >> 5) * 4) - (sign32 ? 536870912 : 0)`. Their intermediate values
fit signed 32-bit arithmetic. A separate Python integer-model check compared
each expression with that floor-division formula for the boundary values 0,
1, width-1, width, width+1, signed maximum, signed minimum, unsigned maximum
minus width and its neighbours, plus 10,000 reproducibly generated values
per width using `random.Random(9473+i).getrandbits(width)`. All 20,020 cases
matched. For each case, the check also compared both expressions after adding
to base 0 and maximum base at 16- and 32-bit modular address widths. This is
an independent mathematical model of the expression, not execution of the
compiled candidate macro or proof of real-CPU segmentation behavior.

The unchanged `address_add` in `c_addr.c` returns unsigned addition in the
32-bit address mode and applies `WORD_MASK` after addition in the 16-bit
mode. The candidate calls it after `d_mem` and before the existing `TYPE`,
`limit_check`, `usr_chk_word`/`usr_chk_dword` and operand fetch, preserving
the source order of permission, limit, page and fetch checks. `BT` then updates
CF after operand fetch; the modifying forms perform their existing writeback
after the instruction helper. This observation does not prove actual fault
delivery, atomicity, correct segment-limit semantics at a wrap, or that an
unexpected writeback failure cannot follow the CF update.

`c_main.c` selects these macros for register-index BT/BTS/BTR/BTC and keeps
register-destination handling unchanged. The immediate forms use their
existing `D_Ew`/`D_Ed` path and are not changed by this patch. The helpers
reduce the index modulo operand width. Assembly encoding rules and actual CPU
semantics of high immediate values still require an independent execution
witness, rather than a conclusion from the unchanged code alone.

The original BT-family helper code uses `1 << op2` with `op2` as large as 31.
In C, shifting signed `int` into its sign bit is undefined behavior; this is
an existing source-level portability risk, not a newly demonstrated runtime
failure and not fixed by the candidate. Validate actual compiled bit-31
behavior independently before choosing a separate minimum-diff correction.

Current-state recheck: the initially cached oracle binary was stale and did
not emit the boundary markers required by the current runner; the first
recheck consequently failed before running the case matrix. Rebuilt both
selected fixture executables with the existing x86 Ninja graph, then ran the
current formal runner in a fresh directory. All 118 selected-object cases and
its hardware-oracle marker checks passed; exact hashes and case evidence are
in `build/t473-s11/bit-family-current-audit-r2/result.json`. The selected
matrix includes one 16-bit register destination, an immediate-FF memory case,
and bit-31 values in the dword family. These runs do not prove absence of C
undefined behavior under other compilers, nor actual guest fault delivery or
all segment/fault-order cases. The build linked fixture seams with existing
duplicate-symbol `/FORCE` warnings, so its validated link-map owners remain
part of the evidence boundary.

Minimum remaining acceptance cases are word register indices -17, -16, -15,
-1, 0, 15, 16, 32767 and -32768; dword indices -33, -32, -31, -1, 0, 31,
32, `INT_MAX` and `INT_MIN`; 16-bit address arithmetic around 0 and FFFF and
32-bit arithmetic around 0 and FFFFFFFF; and register destinations. Verify
BT read-only access, the modifying forms' write permission, segment/page
faults before any visible CF or memory change, operand span crossing a segment
end, and immediate index FF without a register-style displacement. Compare to
a real processor or another independent architectural oracle. The existing
hardware and guest probes only witness a word operand with 32-bit addressing,
a positive control and register index -1; they do not cover this matrix.
