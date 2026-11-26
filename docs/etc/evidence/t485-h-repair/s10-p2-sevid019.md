# S10 P2 sevid019.c whole-file review

Ninth file, three frozen points HXP-R-00763--00765. Fixed original is
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid019.c`, revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89. The original hash was checked.
The [three-point record](s10-p2-sevid019-points.json) retains exact bodies.
All source bytes unchanged; non-error differences remain3 points/6 lines.
Original LF/EOF retained. No #error or #line exists.

00763 changes the S_2696 byte writer's shifted half-address from2 to3.
00764 changes S_2699 word writer's aligned full address shift1 to2.
00765 changes its odd-lane advance3 to7. Together these implement the existing
physical plane formula4*(a&~1)+(a&1): even-to-odd step1, odd-to-next-even step7.
Restoring any one violates scalar byte/word correspondence. Original
write_bios_byte/write_bios_word call chain2_evid[4], whose table names these
providers. CCPU SAS scalar stores also use this selection. Independent byte,
word and dword plane-oracle tests are in cvidc_vector_binding_fixture.c and
passed with P1. This is actual provider/layout evidence, not a claim that
all historical compact-layout profiles were wrong.

## Historical block-copy boundary

The historical record artifacts/mvdm-audit/t483-remaining-reaudit/
r0642-r0647-vram-copy-review.json reports28/56 odd-target full-copy failures
and correctly does not treat its passing prefix oracle as full acceptance.
The current S_2698 VRAM branch still uses compact pointer stepping and the
odd-target decrement. That original generated loop is not declared repaired.
It is separate from the three remaining scalar differences above.

Current selected ccpusas4.c::c_sas_move_bytes_forward loops through
c_sas_hw_at/c_sas_store; word and dword moves likewise use scalar loads/stores.
They do not invoke this generated block-copy loop. Existing P1 SAS move cases
therefore cannot prove S_2698 correct. ega_writ.c retains the historical glue
handler table, so this review does not claim the generated symbol or table
was deleted or universally unreachable. Its later ordered file review must
retain this distinction; changing an unused original loop merely to satisfy
an old direct-call experiment is not justified here.

Three residuals remain pending, zero restorations or new approvals. Source
review9/30 files,126/1060 points,934 unreviewed; current total271/62154,
cumulative+70/-40709 from201/102863. One file delivered; six format changes
and two unchanged reviews await P2. Next base/inc/gvi.h. No new runtime pass.
