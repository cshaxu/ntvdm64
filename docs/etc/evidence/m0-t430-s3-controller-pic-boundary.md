# T430 S3 controller/PIC applicability

## Question and inputs

Does SoftPC ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7's C0 response and
port60 IRQ-clear correction apply to the selected x86 /MT NTVDM CCPU40
controller? Baseline 065ba3b3f/S2, formal cache M0-T427/S2/r001.
Original and sibling sources are read-only evidence, not build dependencies.

## Initial source review

The formal graph defines NTVDM. kbd_inb's corrected sibling IRQ-clear site
is in its non-NTVDM branch. The selected NTVDM branch clears status, retains
output_full and uses the original KbdEOIHook to retire the slot and refill.
The shared C0 case omits code_to_send, but the selected NTVDM response branch
also does not copy code_to_send into output_contents: it only invalidates
KbdData/output_full. Thus the sibling one-line assignment alone cannot repair
the selected response path. These are source conclusions pending focused
actual-provider proof; no production change or runtime improvement is claimed.

## Verification and disposition

`tests/mvdm-host/keyboard_controller_pic_test.c` links the actual production
keyba.obj and ica.obj. The fixture initializes original SAS/CCPU, PIC and
keyboard, starts at the explicit guest-keyboard-ready boundary (DelayIrqLine
cleared), then calls the original port/INTACK/EOI entrypoints. Test-only host
callbacks control hardware mutex, BIOS buffer-space, delayed IRQ and EOI
dispatch; they do not replace controller, translation, PIC or queue logic.
The delay callback records the requested IRQ; the test delivers that request
explicitly. This proves neither timer accuracy, multithreaded locking nor a
real guest BIOS ISR. Immediate device responses are prepended, not FIFO
appended; the fixture follows that original ordering.

Reproduce: build `keyboard-controller-pic-test.exe` with the formal Ninja
graph using MSVC14.43/SDK22621 x86 /MT NTVDM CCPU40; then run
`powershell -NoProfile -ExecutionPolicy Bypass -File
tests/observation/verify-keyboard-controller-pic.ps1 -OutputRoot
build/M0-T430/S3/<fresh-run>`. The runner verifies production symbol owners
and selected compile profile, rejects non-build/sealed output, bounds only
its own fixture to ten seconds and retains input hashes and complete output.

| Boundary | Expected/observed evidence | Disposition |
| --- | --- | --- |
| Initialized C0 response | input_port_val=BF; two distinct old output sentinels are returned instead of BF. | Confirmed retained original NTVDM response defect, not a functional pass. |
| Masked IRQ1, port60 read | Byte is read, status changes, output slot and IRR remain; no EOI callback occurs. | Original NTVDM EOI-owned protocol, not the non-NTVDM sibling's read-owned protocol. |
| Normal unmask/INTACK | vector9, IRR cleared and ISR asserted. | Actual production PIC assertion passes. |
| EOI and subsequent byte | EOI clears ISR, original KbdEOIHook retires slot and queues next delayed IRQ; explicit delivery returns the next byte. | Controller/PIC ordering passes with controlled host boundaries. |
| Final EOI | output_full/bKbdEoiPending/pending_8042 cleared; no residual IRQ1 IRR/ISR. | Final device-resource assertion passes. |

r002 and r003 record 18 assertions, zero failures, including the explicit
RETAINED_LIMIT marker. r002 elapsed88ms. Earlier r001 iterations failed due
to fixture setup: omitted PIC owner initialization, guest-ready IRQ gating
and reversed immediate-response order. Those failures remain saved; no
production fix is attributed to correcting test setup. No assertion was
weakened to manufacture a controller repair.

Pinned OpenNT keyba.c SHA256
D3326210BE154866924CA99F1E44E0A76E9A912E15975305804F6B59C4CF7953;
current keyba.c BA5C8D2A1E437C24F5C208BD048F93F6466C69AE0F8487652DF831190937B235;
current ica.c 3874E59A27410E56C2D8C28B576C528D82ACD2F04E64C50664D1ECDA9A846075.
The original NTVDM response branch invalidates KbdData/output_full without
installing code_to_send; current project metadata hooks do not change that
response. Original NTVDM kbd_inb and KbdEOIHook already have the read/EOI
ownership distinction. The sibling patch's IRQ-clear site is excluded under
the actual NTVDM compile flag. Its shared C0 assignment is insufficient for
this selected response path. Reviewed sibling keyba history, including
6586091d (scan-to-key adapter only); no matching selected-NTVDM response repair
was established. This is not a claim to have proved no fix exists anywhere.

Recovery ladder: original source is directly composed and contains the
response defect; existing local host reset hook addresses user-key provenance,
not controller replies; the verified sibling correction has a different
selected-branch contract; inventing another original repair is prohibited by
the owner gate. Therefore no production/mirror change is adopted. Record the
C0/response limitation in TODO; remove the misleading proposed IRQ repair.
Scalar/RMW/string/stack/control CPU profiles are inapplicable to this
controller-only proof: no CPU implementation changes or CPU equivalence claim.

r003/published-identity.json confirms all eight O:/winnt/system32 files still
equal the accepted S2 manifest. Only test source, its Ninja fixture selection
and governance/evidence change. No production rebuild, full product rerun or
publication is claimed or required for this test-only delivery. No product
process was stopped, no Z: mapping was made and no guest media changed.
S3 concludes applicability/disposition, not a successful C0 repair. S4–S7 and
final owner acceptance remain outstanding; T430 stays open.
