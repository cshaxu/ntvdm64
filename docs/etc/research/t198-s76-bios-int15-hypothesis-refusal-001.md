# M0 T198 S76: BIOS INT 15 Hypothesis Refusal

## Proposed contract

S75 identified the original `cassette_io` default branch for an INT 15 call
with AH=`0Eh`: preserve AL, set AH=`86h` and CF, then continue after its
three-byte BOP. A standalone source-derived provider and focused test were
prepared as a reversible preflight only.

## Contradictory runtime evidence

The fresh source-built run at
`artifacts/build/t198-s76-bios-int15-invalid-r1` compiled and linked the
preflight provider, but did not enter it. The copied exception event is
real-mode (`mode=1`) at `C000:014A`, yet its captured instruction window is:

```text
window=15:ffffffff
```

The fixture's generic observer consequently treats it as a non-BOP #UD before
composition. The event contains no copied `C4 C4` prefix, so it cannot prove
selector `15h`, a three-byte BOP length, an INT 15 caller, or an AH=`0Eh`
`cassette_io` invocation. The standalone positive/negative provider test only
proved its hypothetical contract; it is not evidence that the native event
has that contract.

## Disposition

The uncommitted provider, manifest member, composition route and test were
removed. No BOP 15 or BIOS semantic is added to `bx-vdm`, and no Bochs change
is inferred.

The next question is machine-owned: why does guest control transfer from
`073B:0939` to `C000:014A`, and why does the native #UD capture begin at
`15h`? It requires a source map of the C000 ROM mapping, guest IVT/control
transfer and generic-UD instruction-window mechanics. Until that map proves a
well-formed BOP, the existing controlled stop remains correct.
