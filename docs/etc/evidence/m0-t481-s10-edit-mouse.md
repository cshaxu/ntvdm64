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

For comparison with a physical-input capture, the saved synthetic run contains
20 callback events, each with an entry, descriptor and return record. Its File
menu clicks include movement, left-down and left-up at cell (5,0), with masks
1, 3 and 5 respectively, in both VM handles. This is a known-good synthetic
sequence, not evidence that RDP input produces the same sequence. The bridge
derives cells from guest INT33 coordinates; the VT encoder subtracts the native
Console viewport origin. Their behavior with a nonzero viewport remains an
unverified coordinate boundary and must not be assumed to explain the report.

The frontend comparison has an additional concrete boundary: the test-only
input hook invokes the KVM input sink directly, below native host capture and
Raw Input. Production `kvm-window/win32/component.c` consumes the first client
click to capture the mouse; later button transitions are sent to the sink,
with queued motion flushed first. Captured motion comes from WM_INPUT, while
button transitions come from legacy window messages. Thus the sink-injected
test does not validate RDP Raw Input, capture, or native click delivery. The
owner reported repeated clicks, so first-click capture alone does not establish
the cause of the reported failure.

On the owner's subsequent manual run of `O:/tmp/w95dbg/START95.CMD`, the owner
reported the tested version completely normal. The first attempt used an
incompatible COMMAND.COM and reported Incorrect DOS Version; the owner then
used `O:/WINNT/system32/COMMAND.COM` successfully. A snapshot from the Win95
worker contains 74 recorded mouse callback sequences across two VM handles,
including seven left-button press/release pairs; all 74 dispatch returns were
zero. SHA-256 of the snapshot was
803F275A8FFAF622B437E46DA9612FF101BD664C96C5DAF9A59860FB0AA3DF2F.
It contains no proof of which executable owned each event or that a particular
menu opened. The owner observation and callback records are separate evidence.

Two collectors were inadvertently attached to the same output; one was
stopped and the retained snapshot was readable. The diagnostic package's 11
host-image hashes differ from the formal package, so its manual acceptance
cannot by itself establish binary identity with the formal package. No product
source was changed to address EDIT.EXE during S10; the original negative
observation is not attributed to a newly identified defect.

An on-disk PE section comparison against `O:/WINNT/system32` narrows that
difference: ten images have identical raw bytes in every section except
`.rsrc`; the instrumented NTVDM also differs in `.text`, `.rdata`, `.data`
and `.reloc`. Section equality is stronger than a timestamp comparison for
the ten images, but does not establish equivalence of the instrumented NTVDM
or prove which executable owned the recorded callbacks.

## S10 full delivery

After the owner required a fresh complete delivery, the x86 and x64 cached
configure/builds passed and the actual nested product link targets were built.
A new 11-image package `e09695f6-40f3-453d-8b96-e732d1050f4d` was staged
from those inputs. Its selected Full run passed 16 gates with zero failures;
the external DOS4GW/Nesticle fixture was unavailable and explicitly skipped.
The first Full invocation stopped before any gate because unstamped build
inputs did not match the newly stamped package; the consistent-input rerun
passed. The complete tested package was published to `assets/release` and
`O:/winnt/system32`, with all 11 destination hashes verified. Both installed
W31 addon hashes matched their released files and its SYSTEM.INI was unchanged.
Win95 normal shutdown and the separately observed console-close error remain
subsequent work; the diagnostic owner's success does not prove why the earlier
EDIT.EXE observation differed.
