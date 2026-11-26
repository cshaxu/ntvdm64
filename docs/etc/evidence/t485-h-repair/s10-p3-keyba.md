# S10 P3 keyboard controller review

Eleventh ordered file: keymouse/keyba.c;44 frozen points01157--01200.
Fixed XP revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89 and original
Source/XPSP1/NT/base/mvdm/softpc.new/base/keymouse/keyba.c. Original hash
verified before comparison. The [point record](s10-p3-keyba-metrics.json)
contains all44 decisions. Twenty format-only points restored verbatim.
Original LF restored, six provably redundant same-path #line anchors removed,
three missing original mappings restored at XP496/700/1338, and obsolete
terminal #line3242 removed.82 #error lines and exclusion frameworks unchanged.
Ignoring whitespace and #line, before/after tokens are identical.
Non-error byte metrics1/5452 ->220/308 (+219/-5144). Formatting approval applies.

## Retained functional groups and original alternative

01159/01165--01168/01170/01174--01175/01184--01185/01196 preserve input
origin across controller buffers, held key events, translated prefixes and
output. They carry metadata, not a second input queue. nt_event.c calls
PendingKeyboardHistory before reset to select actual unused raw events for
return; original byte counts cannot distinguish replies, partial break
prefixes, held keys or already-consumed bytes. The original ring continues
owning events. keyboard_origin_device_test.c exercises projected original
routines and the actual history carrier; its limited host stubs are not
presented as a guest integration proof.

01160/01171--01172/01186--01190/01197--01198 implement read/EOI completion.
Original EOI clears output_full unconditionally, even when the guest has not
read60h. Existing production-linked keyboard_controller_pic_test.c proves
normal read-before-EOI, early EOI-before-read, status reads not consuming data,
no repeated IRQ, and ICW1 cancellation. KbdIntDelay must also suppress another
IRQ after early EOI; do_q_int resets both per-output facts. Pending history
excludes data already consumed, even while waiting for EOI. Reset captures
whether a user-originated IRQ must be cancelled, empties queues first, then
mvdm_keyboard_reset_pending_irq uses the existing delayed-IRQ/PIC providers.
The provider clears no unrelated controller response. These two facts model
independent hardware events; replacing them with one flag loses an ordering.
No new helper/state machine or generalized queue is introduced in this review.

01191/01195 ensure controller command C0 returns input_port_val and all direct
responses publish output_contents/status. Restoring only original pending
flags leaves stale data in kbd_inb. The actual controller/PIC fixture checks
C0,20,AA,AB,D0,E0 and direct responses while scan-code output is pending.

01192 uses cpu_interrupt(CPU_HW_RESET,0). Selected CPU40 c_main.c owns a LOCAL
cpu_interrupt_map; c_cpu_interrupt raises CPU_RESET_EXCEPTION_MASK through its
canonical event operation. The original direct map mutation is not an exported
CPU40 ABI. Keep this existing call; no CPU30 monitor is admitted.

All24 functional points remain evidence-backed pending owner decisions.
The original implementation and the named tests provide concrete contracts;
new final-package test results must still be recorded. No claim of new
per-point negative-control executions or complete guest keyboard coverage.

## Progress and verification

Source-reviewed11/30 files,175/1060 points;885 remain. Twenty local format
restorations plus four previously delivered restorations;151 residuals.
Current total486/57002 versus201/102863: +285/-45861. Ten files delivered;
P3 build/publication/targeted tests/commit/push pending. Next keybd_io.c.
The entire keyba file is now handled before initiating the batch build.

Original-line check:1901 matched nonblank non-directive lines have the XP
logical positions. The canonical CPU reset adaptation is explicitly mapped
to XP2849, not mistaken for identical text in excluded CPU30 at XP2847.

## Final package verification

Owned build/publication PID1156 and targeted runner PID12580 completed.
Runtime3 cached full build and dual13-image publication passed. All26 final
destination hashes and30 census source hashes rechecked after tests.
Actual production controller/PIC90 assertions passed, including normal/early
EOI, no repeated IRQ, controller responses, PIC reset and Alt+Enter scan-code
sequence. Keyboard contracts passed all four return/BIOS join/device/unused
input cases. These are scoped host/device tests, not a new GUI roundtrip claim.
C-VID fixture, real MEM/NTVDMD exports/debug/lifecycle and NESticle rendered
menu with Escape hide/restore also passed. Evidence build/t485-s10/p3.
No original guest or winnt.zip changes. Full remains S13. Commit/push pending.
