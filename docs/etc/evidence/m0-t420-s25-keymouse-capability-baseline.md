# M0 T420 S25 — keymouse capability closure

## Question

Can the complete selected original `softpc.new/base/keymouse` unit accept
ordinary modern Console input through the existing host boundary, preserve
the original guest controller/INT contracts, invoke a registered INT 33h
callback at its actual guest entry, and tear that callback down without
leaving a stale input route?

## Frozen source and ownership map

The original `sources` manifest selects exactly `keyba.c`, `keybd_io.c`,
`mouse.c`, `ppi.c` and `mouse_io.c`.  The formal graph compiles all five into
`original-softpc-keymouse.lib`, linked only by `ntvdm.exe`.  Source hashes in
the tested checkout are:

| File | SHA-256 | Reached responsibility |
| --- | --- | --- |
| `keyba.c` | `dbe61ba77fd716d349e0da37fb6011ef3c6731bc53da29c1b3ae85bf83cd1cd5` | 8042 admission/reset and BIOS keyboard handoff |
| `keybd_io.c` | `2edde1af6e01ba3f69d3bbcdefec89bb720c2013df824db7485fa450c756bb7e` | original INT 16h input/status/modifier path |
| `mouse.c` | `2d196d9f0c1ef860436d8eb47c01820913a1164dd1b32e3bd518226653476785` | mouse device state |
| `ppi.c` | `367cceaa88345e16b2e138faef9addd441da2cd39f95ea3ab6c359651bd8ebf5` | original port `61h` PPI provider |
| `mouse_io.c` | `7c02e72d3017801e2fd73f06a4a9029db4ca105aac006770c5180dac753f6646` | INT 33 dispatch, callback and disable teardown |

The existing `softpc.new/host/src/nt_mouse.c`
(`983294bd07faea19aa6b659f73f82316a0fbbf3c47dfa13f23b68033c8bb4aef`)
remains the finite worker-local bridge from public Console records to the
original mouse interrupt route.  It is not a second guest mouse state owner.

The only selected source differences remain the S5 source-first carriers:
`keyba.c` uses public CPU40 reset delivery rather than a private CPU bitmap;
`keybd_io.c` combines original fixed guest offsets with the selected
`Start_of_M_area` backing store; and `mouse_io.c` has the type-correct original
range endpoint declaration plus CPU40 direct callback EIP.  No product source
or adapter behavior changed in S25.  In particular, the test confirms rather
than replaces `MVDM-HOST-DIV-222`: `jump_to_user_subroutine` enters the
registered callback instruction itself, not the historical BOP-relative
address.

| S25 production change class | Mirror lines | Adapter lines | Autonomous production code |
| --- | ---: | ---: | ---: |
| Added, removed or relocated | 0 | 0 | 0 |

The only new code is the test-owned COM witness, native Console observer and
their build wrapper; none is selected by the product graph or copied into the
runtime package root.

## Procedure

`tests/observation/keymouse_capability.asm` is a test-only DOS COM fixture;
`keymouse_capability_observer.c` injects actual Windows `KEY_EVENT` and
`MOUSE_EVENT` records into the worker's own `CONIN$` handle.  It waits until
the existing original mouse transition has enabled `ENABLE_MOUSE_INPUT` and
`ENABLE_EXTENDED_FLAGS`; guest text alone is deliberately not treated as
mouse readiness.

The fixture performs, in order:

1. original PPI port `61h` write/read/restore;
2. INT 33 reset, range and absolute-position round trip;
3. INT 33 function `0Ch` registration for move, left-down and left-up;
4. an actual Ctrl-K down/up sequence, then INT 16 character and modifier
   reads;
5. actual Console move/down/up records and callback-mask verification;
6. INT 33 function `1Fh`, a late native mouse sequence, and a bounded proof
   that the guest callback count does not change; and
7. normal driver re-enable/hide, followed by `MEM` in the same COMMAND worker.

The initial r9 run exposed a real observer ordering race: it emitted mouse
records after guest text but before the existing Console mode transition was
observable.  r11 added the source-shaped mode gate above.  The final r12
fixture requires all three callback conditions, rather than accepting an
arbitrary one-event callback.

## Observations

The formal x86 graph at `build/M0-T420/S25/formal-x86-002` built all 526
selected units and these deployed artifacts:

| Artifact | SHA-256 |
| --- | --- |
| `run16.exe` | `5fca5184645342c67955b6a4040ee6e542098efeaf0529a56a2ed44c520508d1` |
| `basesrv.exe` | `ede007ee5f57cfb0f0db01f46f25445344bb22f99484925772128887009b1a50` |
| `ntvdm.exe` | `fe66c83b9ad6452b5a27cd2c1138726f4d7d3cd01d20526f6b43636b9de547cf` |
| `dtmgr.exe` | `44d32937ecae08db0843bed6264ed084e6d44081dd50807051259fa756685823` |
| `VDMREDIR.dll` | `71f028f0c29f19ee89e98e1b9490eeecf6ab462a367180868995fd149cc2101f` |

Those exact hashes were copied to `O:\winnt` and checked again.  On that
deployment, all three independent guest runs wrote the following observer
assertion after each individual marker had been read from guest Console text:

```text
S25_KEYBOARD_OK S25_MODIFIER_OK S25_PPI_OK S25_MOUSE_RESET_OK
S25_MOUSE_POSITION_OK S25_MOUSE_CALLBACK_OK S25_MOUSE_TEARDOWN_OK
S25_KEYMOUSE_OK
```

Raw records: `O:\winnt\logs\m0-t420-s25-formal-keymouse-r1-run1.raw`,
`run2.raw`, and `run3.raw`.

The same deployed package then passed transcript-gated established routes:

| Route | Result | Guest proof |
| --- | --- | --- |
| interactive `MEM` | exit 1 | conventional-memory report |
| two nested `COMMAND` then `MEM` | exit 1 | three distinct-depth MEM reports |
| direct `MEM.EXE` | exit 0 | conventional-memory report |
| `COMMAND.COM /c MEM.EXE` | exit 0 | conventional-memory report |
| `EDIT` then return to COMMAND/MEM | exit 1 | editor startup/exit and MEM report |

The exact machine-readable summary is
`O:\winnt\logs\m0-t420-s25-formal-regression-r1-summary.json`.

## Interpretation and disposition

The selected capability unit is complete for the current public-Console,
CCPU40-only product profile.  It proves end-to-end keyboard character and
modifier delivery, original PPI I/O, mouse position and every requested
callback event class, and callback removal before a subsequent native mouse
event.  No keyboard/mouse provider, guest-media mutation, adapter-owned input
queue or CCPU change was added.  CPU30/kernel monitor and a separate graphics
mouse presentation path remain outside this selected unit and are not
claimed here.
