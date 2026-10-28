# Shard 1: original WOW button message owners

Propose **partial** for the pending `wow32/wmsgbm.c` source. The
[observer](wow-button-image-observer.py) and [receipt](wow-button-image-observations.json)
independently verify both original editions and both source catalogs, the
selected WOW32 compile/link edge and valid dependency, and original object
bodies in the current released image. The two original forward functions and
two reverse control functions match their respective objects with relocation
words masked. The selected cached DLL and released image have identical code
sections, not necessarily identical full-file identity. Source retention for
`wmsgbm.c` is 100%; a complete behavior-restoration percentage is not measured.

Eight actual initial image slots match the corresponding object relocations:
the Win16 forward and unthunk class arrays at `WOWCLASS_BUTTON=2`; and native
messages 0xF0–0xF4 to `WM32BMControl`, 0xF5 to `WM32BMClick`. This establishes
initial linked ownership, not invocation or later table mutation. The
original forward code translates Win16 `WM_USER+0..4` into the five Win32
button messages, and Win3.1 `WM_USER+99` into `BM_CLICK`; other inputs pass
through. The reverse original control bodies translate only when `fThunk` is
set. Class selection, call-context identity and host window lifetime belong to
their respective dispatcher and host boundaries, not to this file alone.

The prior [source fixture](wow-button-message-audit.md) tested a separately
extracted forward body and a bounded input matrix; it is not an actual guest
message round trip and was not rerun in this read-only checkpoint. Outstanding
are real Win16-to-host-to-Win16 routing, button class recognition, unknown
application messages, unthunk behavior, parameter and return propagation,
disabled/selected states, reentrancy, destroyed/reused HWNDs and teardown.
No guest, product, installed file, build tree or shared ledger was changed.
Coordinator review is required before applying the proposed disposition.
