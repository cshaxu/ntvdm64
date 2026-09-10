# T225 S17 External Generic-UD Bridge Composition Result

## Question

Can the formal native executable resolve the already registered generic `#UD`
bridge from bx-vdm rather than mantle's fixture-default, without a Bochs source
or BOP/provider change?

## Build closure

The pinned CPU5 projection now explicitly enables both existing gates:

```
BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1
BX_NTVDM_EXTERNAL_GENERIC_UD_BRIDGE=1
```

The first retains the registered selector-blind core event copy.  The second
uses the existing guard in `bx_ntvdm_generic_ud_bridge.cc` to compile out only
mantle's fixture-default bridge definition; its stop-observation helper
symbols remain in mantle.  The native target library order is now
`cli -> bx-mantle -> bx-core -> bx-vdm`, so bx-vdm resolves the core's bridge
reference after the core archive introduces it.

No Bochs/bx-core/mantle/bx-vdm source, bridge ABI, BOP routing, provider,
guest image, device, firmware, CCPU, or profile behavior changed.

A fresh formal Ninja/MSVC x64 `/MT` graph at
`build/M0-T225-S17/ninja-r1` projected config SHA-256
`DDB0D13E35C2FD9E7EC29896E6296F83ECE7F6BF5D2AD474E8937D389F97872F`,
built 308 edges, passed the S15 accepted/declined fixture, and subsequently
reported no work.

## Native result

The immutable S10 primary input was run at 100,000 internal ticks with both
observers explicitly enabled.

| Mode | Exit / terminal | BOP records | Generic-UD records | First record |
| --- | --- | ---: | ---: | --- |
| Direct | 4 / budget | 33, no overflow | 33, no overflow | `0070:0475`, `C4 C4 50 11`, RESUME |
| Readonly | 4 / budget | 33, no overflow | 33, no overflow | identical |

The copied first generic window is:

```
C4 C4 50 11 EA F9 02 0D 01 98 8B D0 B4 01 CD
```

`0070:0475` translates to physical `0x0b75`, exactly matching S14's
read-only static NTIO location.  This is positive proof that the formal
machine writes and enters the intended immutable NTIO path and that the
adapter, rather than mantle's fixture bridge, now handles the generic event.

The complete bounded sequence contains existing accepted endpoints from DEM,
COMMAND and machine/top-level selector families.  Their presence is not an
individual implementation admission.  S18 must group the reached sequence by
whole original owner package and compare it against the existing BOP ledger
before any provider change.

## Boundary

This resolves a composition/linkage defect, not a guest behavior defect.  The
next action is package-level source/ABI disposition review; no trace hit may
be converted directly into a service patch.