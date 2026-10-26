# M0 T460 S3 — Keyboard early-EOI output-slot repair

## Scope and independent evidence

The owner added this independent host-only repair to S3 on 2026-10-25.  The
read-only owner-supplied experiment root (outside this repository)
establishes a reproducible order difference: a normal client reads port 60
before completing IRQ1, while Win95 VKD/VPICD can complete IRQ1 first.  The
frozen baseline loses `1C` and later reads an old `FA` followed by `9C`; the
experiment's controller/PIC fixture reports 44 passing assertions after its
candidate change.  That material is evidence only: neither its source nor its
binary is a project input.

## Selected production contract

`keyba.c` owns the single 8042 scan-output slot (`output_full`,
`output_contents`, `KbdData`) and `KbdEOIHook` is invoked by the selected
production PIC after a normal IRQ1 acknowledgement.  The fix keeps two
controller-local facts for the currently latched scan byte:

1. `kbd_output_data_read` becomes true only after an actual port-60 read.
   Port 64 remains status observation and cannot consume the byte.
2. `kbd_output_irq_acknowledged` becomes true only in the normal EOI hook.

Only their conjunction retires `output_full`, clears the history projection,
and calls the existing `continue_output` path with its original delayed-IRQ
discipline.  A new queued byte, direct controller reply, 6805/8042 reset,
initialization, and `KbdResume` clear both facts.  This prevents an early EOI
from overwriting unread output, while preserving the original read-then-EOI
flow.  It neither fabricates an EOI nor reinjects a scan code.

PIC reset cancellation (`host_EOI_hook(irq, -1)`) is a different contract:
the registered keyboard hook receives that negative count, explicitly retires
the old pre-reset slot using the original cancellation behavior, and never
marks it as a normal acknowledgement.  The focused production fixture proves
that distinction in addition to the two normal ordering cases.

## Authority and bounded verification

The original `keyba.c` remains the controller and queue provider; the PIC
remains the IRQ/EOI authority.  The two added flags are not a second queue or
host policy, only the missing state needed to preserve one existing hardware
slot across its two observable acknowledgements.  The focused fixture links
the selected production `keyba.obj` and `ica.obj`, rather than copied bodies.
It must prove normal read-then-EOI, early EOI-then-read, repeated status reads,
single advancement, delayed next IRQ, direct replies, and final retirement.

Remaining publication evidence is explicitly broader: noninteractive DOS
keyboard/timing and protected-mode transition, WOW and Win3.x start/run/exit
and restart.  The fixture does not claim BIOS ISR, all host input combinations,
or interactive Win95 acceptance.  Guest media and the Win95 image are never
read, changed, rebuilt, or packaged.

## Current candidate evidence

The final selected-object keyboard/PIC fixture passes with
`KEYBOARD_PIC_PROFILE assertions=46 failures=0`, including the normal
read-then-EOI path, EOI-before-port-60 path, repeated 64h status reads,
delayed follow-up IRQ, direct reply, and the distinct negative PIC-reset
cancellation path.  The independent origin/device fixture also passes every
existing queue, split-prefix, held-input, direct-reply and reset assertion.

The same candidate is included in the completed formal component build
`build/t460-s3/full-component-r6` (ten images; second Ninja pass rebuilt no
intermediate).  A real private-desktop Win16 two-task message witness from
the staged candidate passed normal task retirement and bidirectional native
callback traffic.  These are meaningful DOS/WOW/Win3.x non-regressions, but
they do not claim physical keyboard input, BIOS ISR behavior, or complete
Win95 acceptance.

The older `verify-dos-nonzero-continue.ps1` runner was also checked but is not
valid for this candidate: it assumes a retired flat package layout and invokes
`<root>\\run16.exe`, whereas the formal package is `<root>\\system32\\run16.exe`.
The current root-carrier runner accepts the system32 package, but its retained
`command-stream-observer.exe` no longer produces the report protocol the
script requires for its first independent-Console row.  Neither attempt
reached a product assertion, so neither is evidence of a DOS regression or a
pass.  They remain finite runner-maintenance gaps before a default publication
claim, rather than grounds to alter the PIC or keyboard behavior.

## 2026-10-25 owner-directed package publication

The owner directed publication of the shared PIC-reset and early-EOI keyboard
repairs before S3 resumed its independent Clipboard analysis.  The coherent
ten-image `package-r20-shared-clipboard` package was installed to both
`assets/release` and `O:\winnt\system32`; SHA-256 comparison of candidate,
release and installed manifests confirmed all ten component identities.
Recoverable pre-publication copies are retained beneath
`build/t460-s3/publish-r21-pic-keyboard/recovery`.

The publication is limited to the already proven controller contracts.  It
does not claim the pending physical-input, BIOS-ISR, broader DOS or Win95
acceptance cases passed, and it does not include any Clipboard candidate.
