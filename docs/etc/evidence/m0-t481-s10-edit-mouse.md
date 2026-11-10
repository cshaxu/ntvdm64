# T481 S10 — native EDIT.EXE physical mouse investigation

The owner accepted the preceding Win95 desktop/EDIT.COM reentry repair and
reports that physical mouse clicks inside windowed native EDIT.EXE do not
respond. This is separate from the successfully repaired nested callback bug.
S9 closure and S10 admission are recorded in CURRENT and the working plan.

The installed `O:/WINNT/edit.exe` and `C:/Windows/System32/edit.exe` are the
same Microsoft Edit 1.2.1 image, SHA-256
5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0.
The prior dual-editor integration witness invokes Win95 guest USER mouse_event;
it is not a physical/RDP frontend test. Its success therefore does not refute
the owner's physical-input report.

The checked-in ntvwm_modern_edit_mouse_test.c was compiled and run on the
actual host editor in an isolated hidden console. It observed input mode
00000288; native MOUSE_EVENT records did not open the File menu, whereas the
existing ntvwm_input_write VT encoding did. This confirms the direct native
conversion in that environment, not the Win95 bridge or real hardware path.
Report: build/t481-s9/edit-mouse-console-report.txt.

Read-only source tracing identifies the boundary from the authored per-VM
INT33 callback in `src/addon/ntvdm64-386/ntvdm64.asm::mouse_event`, through
`mvdm_vm_executor_mouse_bop` and `mvdm_vm_io_mouse` to
`ntvwm_presentation_input` and `ntvwm_input_write`. No point in this chain is
claimed to be the failure until an end-to-end witness establishes it.
The identical-image hidden-console check observed mode 00000288 and confirmed
that its direct `ntvwm_input_write` VT conversion opens Edit's File menu;
it is not an end-to-end Win95 test. A test-only NTVDM diagnostic build
records the actual per-VM mouse BOP entry, descriptor and dispatch return in a
bounded memory ring, without recording keyboard text. The unchanged runtime
path was exercised on an isolated Win95 copy: the existing guest-USER-driven
dual-editor save witness passed, and the ring recorded 20 mouse calls across
two distinct VM handles, all with dispatch status zero. This proves the
instrument and synthetic comparison path, not physical RDP delivery.
The temporary guest copies and isolated runtime were removed afterward.

An independently stamped 11-image diagnostic package with that recorder was
placed in O:/tmp/w95dbg; its NTVDM SHA-256 is
FA586C3304E1B7CEF65BBFEA65A4E7DA9C2AF44DA843513EF6BCDC37BCF58991.
O:/tmp/w95dbg/START95.CMD launches it explicitly. The ordinary
O:/WINDOWS/PATCH/WIN.CMD remains pointed at the formal published package.
A read-only bounded collector was started before requesting the owner's
physical-click witness; it does not record keyboard or document contents.
A separate read-only console-mode probe was checked against a test-owned
hidden instance of the same Edit image. It reported input mode 0x288
(VT-input enabled, mouse-input bit disabled) and a 55x30 buffer/viewport;
the exact fixture process was then stopped. The probe reads mode/geometry,
not text, and is ready for a live NTVWM child once the owner witness exists.
