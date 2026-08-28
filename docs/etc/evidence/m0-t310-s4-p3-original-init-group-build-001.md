# M0 T310 S4 P3: original initialization-group build

## Scope

This probe takes the first source-ordered original SoftPC composition group:

```text
InitialiseDosEmulation
  -> reset
     -> base/bios (CMOS, ROM/BOP table, memory-size)
     -> base/keymouse (keyboard/mouse/PPI controllers)
     -> selected CCPU/SAS and host-control roots
```

It is an archive and forced-link audit only. It does not assert a runnable
SoftPC backend, enable a device, or allow a Bochs fallback.

## Exact source membership

- `base/ccpu386/sources`: 130 original translation units (already selected by
  the T310 S2 graph).
- `base/bios/sources`: 14 original translation units.
- `base/keymouse/sources`: 5 original translation units.
- Existing selected host-control roots: 10 original translation units.
- `fmstubs.c`: separately archived, evidence-only; its original immediate
  debug-break contract is retained uniformly with `__debugbreak()` on x86 and
  x64.

`New-T310OriginalSoftpcNinja.ps1` now reads the original BIOS and keymouse
`sources` manifests rather than maintaining a hand-written subset.

## Formal results

| Architecture | Original archives | Result |
| --- | --- | --- |
| MSVC `/MT` x86 | CCPU, BIOS, keymouse, selected host-control, Win32 binding and patch-evidence archives | success |
| MSVC `/MT` x64 | CCPU, BIOS, keymouse, selected host-control, Win32 binding and patch-evidence archives | success |

The corresponding deliberately non-runnable `/WHOLEARCHIVE` forced-link audit
also completed on both architectures. It reports 264 physical unresolved forms
on x86 and 263 on x64 after the BIOS/keymouse group is included. The increase
from S2 is evidence that PIC/IOS/device/CPU calls from the newly admitted
original bodies are now visible; it is not an authorization to fill them with
invented stubs.

Examples of newly visible required original forms include `ica_intack`,
`ica_hw_interrupt`, `Ios_out_adapter_table`, `Ios_outb_function`, and
`Ios_outw_function`. Their owner remains the S4 PIC/PIT, IOS/device and CCPU
families. P4 must classify every added physical form against its original
provider before any binding is implemented.

## Outcome

The executable `base/bios` and `base/keymouse` packages are now both source
and build members of the original SoftPC initialization candidate on x86 and
x64. The next work is interface classification, not a second machine and not
a broad compile-fix loop.
