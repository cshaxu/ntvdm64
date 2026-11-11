# T478 S39 keyboard investigation

Status: bounded finding52 investigation closed. No product source change.

## Current source and release binding

`build/t478-s39/keyboard-current-image-bindings.json` repeats the existing
keyboard image observer with only its renamed source root corrected to
src/mvdm-host. All11 original WKB32 functions bind to the current object and
released executable code. Current wkbman.c is byte-identical to both pinned
OpenNT editions (SHA256 D5D728A2E7527814343BD2470BCE98CA9028796FF7E404C7FE7B57ABBCFE4367).
Released WOW32 SHA256 is
3A69A376EFADF3F9882F75D5BA870B0989446F53438419D138E69161E20DEBB9,
from the verified S38 package818fff8c-efa7-4570-93d6-1f9c820f40ed.
The current map resolves keyboard and ANSI/OEM operations to USER32.
This is selection/binding evidence, not a substitute for runtime assertions.

`build/t478-s39/source-review.json` records source input hashes and pointer
ownership. GETPSZPTR maps a pointer; it does not truncate binary keyboard
state at a NUL. The selected WOW32 Sim32FlushVDMPointer is the original
non-CPU30 no-copy success contract. Therefore the original strlen-sized
flush excluding the terminator does not establish a lost-terminator defect.
The guest storage is directly mapped before the native conversion call.

## First actual guest probe

Independently authored `test/observation/win16_keyboard_probe.asm` uses
original KEYBOARD ordinals, KERNEL InitTask and USER InitApp. It runs via
the existing isolated runner with Case keyboard. No original guest media,
layout setting or desktop input is modified. Evidence is retained at
`build/t478-s39/keyboard-r1`, including package/input hashes, executable,
raw guest record, parsed values, process identities and normal exit report.
The private-desktop real guest/launcher exits0; all current assertions pass.

- AnsiToOem/OemToAnsi convert Ab9 including its NUL, leaving four guard bytes.
- Both Buff variants convert four bytes A,NUL,Z,9 and retain four guard bytes.
- AnsiToOemBuff with count0 preserves all eight destination sentinel bytes.
- Space VkKeyScan returns32; GetKeyboardType returns4; MapVirtualKey returns57;
  GetKBCodePage returns437. OemKeyScan records57 (not yet separately asserted).
- GetKeyNameText returns5 and Space plus NUL; the two bytes beyond capacity32
  remain untouched. The name text is recorded, not hard-coded as an oracle.
- ToAscii receives a zeroed256-byte state and returns1 with space in the result;
  both bytes after its four-byte destination remain untouched.

The four ANSI/OEM APIs are void in the original public Win16 header; their
recorded AX values are deliberately not treated as a public success contract.
No product fix is justified by this first probe. The existing exact S38
package remains published; no new Full or publication is claimed.

## Follow-up plan after first probe (completed below)

Reconcile empty and short-buffer behavior, both zero-count conversion paths,
scalar failure results and caller-precondition limits. Current codepage437
ASCII evidence does not prove DBCS or dead-key behavior. Resolve applicable
host provider ownership and bounded evidence before closure; do not introduce
new keyboard layouts, recovery policies or guest implementation changes.

## Extended cases and original limits

`keyboard-r2` terminated normally but failed an incorrect test expectation:
GetKeyNameTextA with capacity0 wrote a NUL. Direct native USER32 comparison
at `build/t478-s39/native-name.json` confirms capacities0 and1 both return0
and write that NUL. Original OpenNT windows/core/ntuser/inc/ntcftxt.h
GetKeyNameText explicitly stores pString[retval]=0 after the kernel returns0;
kernel/xlate.c rejects cchSize<1. Thus this is original caller-buffer behavior,
not a new project overflow to mask. The first expectation failure is retained.

Final `keyboard-r3` passes the original behavior and all prior cases, plus
empty conversions, both zero-count Buff calls, invalid MapVirtualKey type,
invalid GetKeyboardType request, zero/one-capacity names and noncharacter
ToAscii. Both original thunks and the launcher exit normally. OemKeyScan's
space result now must equal the MapVirtualKey result. The noncharacter output
itself is not required to be initialized; only its outer guard is asserted.

`verify_keyboard_boundaries.py` compiles the exact two current original
ToAscii/MapVirtualKey bodies with the current packed wowkbd.h argument types.
Seven controlled cases pass, including -1/0/1/2 results, binary keyboard state
with a nonzero byte255, two output bytes82,A0, four-byte flush, and original
high-bit-to-bit15 diacritic projection. This proves transport of provider
results, not actual Japanese character conversion or dead-key composition.
The result and input/body identities are recorded in controlled-r1/receipt.json.

Original USER GetKBCodePage returns GetOEMCP. Original ToAscii delegates to
ToUnicode and converts using the host codepage, retaining the signed count.
Those are host USER/layout responsibilities, not missing keyboard thunk
algorithms to import into WOW32. The real host run has OEM codepage437.
Alternate DBCS layouts and composed dead keys are explicitly not runtime
certified. No global settings are changed to manufacture that claim.

All11 wrappers use local mapped pointers/scalars and retain no per-task
allocation, callback or keyboard layout owner. The actual probe exits normally;
no new keyboard teardown implementation is indicated. Invalid selectors,
unwritable destinations and insufficient caller storage retain original
preconditions, without adding fallback buffers or exception recovery.

## Bounded disposition and delivery

No demonstrated missing host connection or project defect remains in finding52.
This closes the original-ownership investigation, not universal layout/device
certification. [Committed results](../../../artifacts/mvdm-audit/shard-3/keyboard-s39-results.json)
retain final guest values, controlled results, native comparison, all11 image
bindings and ten verified release hashes. No source/binary change is needed.
S38's exact package and Full16/16 remain the delivery basis; no redundant
product build, Full or republication is claimed. This is a local tests/evidence
P only, without O:/winnt write or push. S40/finding54 follows.
