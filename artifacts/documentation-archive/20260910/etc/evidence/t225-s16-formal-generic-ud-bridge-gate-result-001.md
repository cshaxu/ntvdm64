# T225 S16 Formal Generic-UD Bridge Gate Result

## Question

Does projecting the existing `BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE` gate into the
formal CPU5 Ninja configuration cause primary native `#UD` events to reach
the S15 adapter observer?

## Change and identity

The pinned CPU5 projection now has one explicit absent-source addition:

```
#define BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE 1
```

It is the existing `BX-MANTLE-067` selector-blind bridge gate, previously
used by targeted CPU5 witnesses.  No `src/bx-core`, Bochs, mantle, adapter
bridge ABI, provider, guest image, device, firmware or profile source changed.

A fresh graph at `build/M0-T225-S16/ninja-r1` records projected-config
SHA-256 `23B523EF4B3D7E8EC6D84ADB24EC5D0EADD888DCEF71934FA715CE33410C413F`
and builds all 308 formal edges.  The S15 focused fixture passes and Ninja
subsequently reports no work.

## Primary result

The equivalent Direct and Readonly 10,000- and 100,000-tick runs retain
normal budget exit 4 and zero BOP/generic-UD records.  At 100,000 ticks, both
raw logs still show the known real-mode `SLDT` diagnostics.  Thus the macro
is present and the core gate is compiled, but the adapter observer still does
not receive the event.

## Static-link explanation

The result is a composition-link closure defect.  The formal archives both
export the core-required symbol:

| Archive | Definition |
| --- | --- |
| `lib/bx-mantle.lib` | default finite-fixture `bx_ntvdm_mantle_generic_ud_bridge_v1` |
| `lib/bx-vdm.lib` | adapter composition `bx_ntvdm_mantle_generic_ud_bridge_v1` |

`src/bx-mantle/bx_ntvdm_generic_ud_bridge.cc` explicitly guards the former
with `#ifndef BX_NTVDM_EXTERNAL_GENERIC_UD_BRIDGE` and states that an external
adapter bridge must compile out only this default decline definition.  The
formal projection did not set that macro, and the native target's current
static-library order allows the mantle definition to satisfy the core
reference.  S15's adapter fixture remains valid because it links `bx-vdm`
alone; it does not prove the native composition route.

## Follow-up

S17 must make the existing external-bridge composition explicit in the formal
projection and native target link order, then re-run the same bounded
observation.  This is not a BOP, CPU semantics, CCPU, device or provider
repair.  It is a selector-blind static-composition correction required to
make the already registered bridge reachable.