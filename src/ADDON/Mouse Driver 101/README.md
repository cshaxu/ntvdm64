# Windows 1.01 INT33 Mouse Driver

Owner-authorized independent guest add-on for T435. This directory is not
an OpenNT mirror or a host-runtime component. It will contain the source of
a Windows 1.01 mouse driver using NTVDM's existing INT33 provider instead
of installing another InPort/PIC interrupt handler.

## Boundary

- Keep original Windows core, media and Microsoft mouse-driver files intact.
- Build a separate 16-bit guest driver; never link its objects into host EXEs.
- Preserve the existing NTVDM INT33 implementation and execution semantics.
- Translate movement/button callbacks to the actual Windows 1.01 driver ABI.
- Let Windows draw its arrow; do not request a second DOS software cursor.
- Restore prior callback and mouse state on disable/exit as the audited
  lifecycle contract requires.
- No new host process, helper, component, IPC channel or hardware-detection
  bypass. Build/intermediate/test outputs belong below build/M0-T435/.

## Current State

S1 is ABI/source/design research. No executable driver, runtime capability or
installation is claimed by this directory admission. INT33 supports absolute
position, motion counters and callbacks, but callback units, Windows event
flags, resident-code/stack requirements and state restoration remain to be
verified before implementation.

Original driver binaries/disassemblies are read-only contract evidence, not
copied implementation. The WOW mouse-driver mirror is not a Win1.01 substitute.
Source provenance, reproducible build and real guest acceptance are required.

## Implementation Contract Under Review

The original Mouse6.24 Win1.01 driver has four fixed ordinal entries. Its
first three perform Inquire, Enable and Disable; the fourth is its private
serial detector, not Windows3's MouseGetIntVect. The new driver needs no
serial detector and must not copy that later fourth-entry behavior.

Use NASM as an isolated 16-bit guest build island, with an explicitly emitted
NE header/segment/entry table, following the existing independently authored
NE fixture's packaging technique. This does not reuse original guest bytes.
Keep callback code and its state resident/non-discardable; do not import WOW
thunks or rely on a native DLL loader. Verify real Win1.01 Setup/loader
acceptance before declaring the generated image usable.

The intended Enable transaction saves the Windows far event procedure. On
first enable it exchanges the current INT33 callback for an empty callback,
saves the provider's supported state using functions21/22, hides the DOS
pointer, clears old motion using function11, and installs the bridge callback.
A repeated Enable replaces the Windows procedure without taking a second
snapshot or hiding the pointer again. Failure must restore the prior callback;
no-provider or oversized-state results cannot be reported as success.
Numeric function numbers in this paragraph are decimal.

The bridge callback preserves registers/segments/flags, consumes interval
motion through function11, translates button transitions rather than current
button state into Windows event flags, and calls the current Windows far
event procedure. It returns with RETF to the provider's existing callback
continuation, not IRET, and sends no PIC EOI. Provider IRQ/callback ordering
remains unchanged. A callback must not call Windows allocation/file/UI APIs.

Disable first prevents new bridge delivery, removes the bridge callback,
restores provider state with function23, and reinstalls the old callback.
Repeated Disable is harmless. The state buffer is private guest storage,
bounded by the actual function21 size; never a host pointer or raw guessed
structure. Exact host-side position/clip caches are not necessarily included
in that original provider snapshot, so restoration must be tested, not assumed.

### Selected Absolute-coordinate Contract

Read-only cga-old-memory.bin contains an enabled original mouse driver at
linear11DD0, with its Windows event pointer1B57:5BC2. The actual Windows
callback tests movement bit0, and tests AX's sign at5C25: bit15 selects
normalized unsigned absolute coordinates in BX/CX; the other branch consumes
relative displacement. Button transitions are bits1/2 for left down/up and
bits3/4 for right down/up, verified at5CFF and the shift at5C06. This differs
from the later DDK description and is the selected Win1.01 contract.

The bridge therefore selects the absolute event path: retain provider event
bits0..4, set bit15 for a movement event, transform INT33 CX/DX to the
unsigned normalized0..65535 coordinate domain using the current provider
maximum coordinates, and set DX=2 buttons. Windows maps normalized positions
to its actual display and draws its own pointer. Query current geometry,
check division bounds and clamp legitimate endpoints. No function4 recenter,
range expansion, provider clip modification or fabricated movement is needed.
The function11 interval-counter design above is superseded for movement;
counter behavior remains audit evidence, not the selected implementation.

### Rejected Relative-motion Prototype

The current NTVDM call mask reports movement when its absolute virtual
position changes. At a clipped edge, raw movement can accumulate without a
movement callback. Consequently merely consuming function11 in a callback is
not yet a complete relative-device solution. Investigate the normal guest
function4 recenter mechanism, without inventing host events or changing
NTVDM's driver. Test continued motion at all edges, zero-motion callbacks,
large displacement and disable restoring the prior DOS position. Do not
publish a prototype that silently stalls at the edges or gains speed through
cumulative-counter replay.

### Required Proof

- NE exports, load/init and actual Enable/Disable calls from Win1.01.
- Event flag/register interpretation from the actual Win1.01 caller, not
  solely a later DDK; movement units/speed and button press/release.
- Callback stack/RETF continuation and provider event ordering.
- Repeated Enable/Disable, missing provider, bounded state and rollback.
- Relative movement at boundaries; idle does not generate endless callbacks.
- Windows draws one arrow; real menu selection and dragging work.
- Windows exit restores DOS position, callback, pointer and input; repeat
  startup works. Existing Invalid handle failure remains separately open.

## Implementation Contract Under Review

The original Mouse6.24 Win1.01 driver has four fixed ordinal entries. Its
first three perform Inquire, Enable and Disable; the fourth is its private
serial detector, not Windows3's MouseGetIntVect. The new driver needs no
serial detector and must not copy that later fourth-entry behavior.

Use NASM as an isolated 16-bit guest build island, with an explicitly emitted
NE header/segment/entry table, following the existing independently authored
NE fixture's packaging technique. This does not reuse original guest bytes.
Keep callback code and its state resident/non-discardable; do not import WOW
thunks or rely on a native DLL loader. Verify real Win1.01 Setup/loader
acceptance before declaring the generated image usable.

The intended Enable transaction saves the Windows far event procedure. On
first enable it exchanges the current INT33 callback for an empty callback,
saves the provider's supported state using functions21/22, hides the DOS
pointer, clears old motion using function11, and installs the bridge callback.
A repeated Enable replaces the Windows procedure without taking a second
snapshot or hiding the pointer again. Failure must restore the prior callback;
no-provider or oversized-state results cannot be reported as success.
Numeric function numbers in this paragraph are decimal.

The bridge callback preserves registers/segments/flags, consumes interval
motion through function11, translates button transitions rather than current
button state into Windows event flags, and calls the current Windows far
event procedure. It returns with RETF to the provider's existing callback
continuation, not IRET, and sends no PIC EOI. Provider IRQ/callback ordering
remains unchanged. A callback must not call Windows allocation/file/UI APIs.

Disable first prevents new bridge delivery, removes the bridge callback,
restores provider state with function23, and reinstalls the old callback.
Repeated Disable is harmless. The state buffer is private guest storage,
bounded by the actual function21 size; never a host pointer or raw guessed
structure. Exact host-side position/clip caches are not necessarily included
in that original provider snapshot, so restoration must be tested, not assumed.

### Selected Absolute-coordinate Contract

Read-only cga-old-memory.bin contains an enabled original mouse driver at
linear11DD0, with its Windows event pointer1B57:5BC2. The actual Windows
callback tests movement bit0, and tests AX's sign at5C25: bit15 selects
normalized unsigned absolute coordinates in BX/CX; the other branch consumes
relative displacement. Button transitions are bits1/2 for left down/up and
bits3/4 for right down/up, verified at5CFF and the shift at5C06. This differs
from the later DDK description and is the selected Win1.01 contract.

The bridge therefore selects the absolute event path: retain provider event
bits0..4, set bit15 for a movement event, transform INT33 CX/DX to the
unsigned normalized0..65535 coordinate domain using the current provider
maximum coordinates, and set DX=2 buttons. Windows maps normalized positions
to its actual display and draws its own pointer. Query current geometry,
check division bounds and clamp legitimate endpoints. No function4 recenter,
range expansion, provider clip modification or fabricated movement is needed.
The function11 interval-counter design above is superseded for movement;
counter behavior remains audit evidence, not the selected implementation.

### Rejected Relative-motion Prototype

The current NTVDM call mask reports movement when its absolute virtual
position changes. At a clipped edge, raw movement can accumulate without a
movement callback. Consequently merely consuming function11 in a callback is
not yet a complete relative-device solution. Investigate the normal guest
function4 recenter mechanism, without inventing host events or changing
NTVDM's driver. Test continued motion at all edges, zero-motion callbacks,
large displacement and disable restoring the prior DOS position. Do not
publish a prototype that silently stalls at the edges or gains speed through
cumulative-counter replay.

### Required Proof

- NE exports, load/init and actual Enable/Disable calls from Win1.01.
- Event flag/register interpretation from the actual Win1.01 caller, not
  solely a later DDK; movement units/speed and button press/release.
- Callback stack/RETF continuation and provider event ordering.
- Repeated Enable/Disable, missing provider, bounded state and rollback.
- Relative movement at boundaries; idle does not generate endless callbacks.
- Windows draws one arrow; real menu selection and dragging work.
- Windows exit restores DOS position, callback, pointer and input; repeat
  startup works. Existing Invalid handle failure remains separately open.
