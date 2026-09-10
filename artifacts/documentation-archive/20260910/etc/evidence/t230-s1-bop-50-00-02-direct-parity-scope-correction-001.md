# M0 T230 S1 — BOP 50:00--50:02 Direct Source-Parity Scope

## Question

How can the initial v2 DEM providers become maintainable OpenNT mirrors without
turning them into a second profile-dispatch implementation?

## Inputs

- OpenNT `demhndl.c`: `demClose` (lines 28--90) and `demChgFilePtr`
  (lines 342--389).
- OpenNT `demfile.c`: `demChMod` (lines 437--506).
- Existing bx-vdm outer session and profile routes.

## Decision

The v2 boundary is Direct-only OpenNT source parity:

1. `50:00`, `50:01` and `50:02` retain original source-file order,
   function/control-flow shape and local-data structure where composition
   permits.
2. v2 contains no Readonly, Overlay or Virtual implementation, API, branch or
   test requirement.  Its outer caller must admit Direct before calling it.
3. v1 remains untouched as the executable compatibility baseline.  Its later
   migration or removal is an owner-package decision, never an isolated leaf
   cleanup.
4. Deviations are limited to checked guest-memory input, typed mechanical
   result ABI, host-namespace capability calls and native-width handle
   containment; every such divergence is commented in the source.

## Procedure and observations

- Reordered `bop/demhndl.c` to source order: `demClose`, then
  `demChgFilePtr`.
- Expanded `bop/demfile.c` into a visible `demChMod` mirror with the original
  get/set split, normal-attribute mapping, DOS mask and `dcerr` path.
- AX:BP is an opaque token into a session table which owns a native-width
  `HANDLE`; this keeps the guest ABI 32-bit on both x86 and x64 hosts.
- The outer `50:01` route now rejects every non-Direct view before it can call
  the v2 provider.
- `rg -n -i 'readonly|overlay|virtual' src/bx-vdm/bop` produced no match.
- Formal build and focused Direct regression passed:

```text
ninja -C build/M0-T230-S1/50-01-demfile-v2-r1 \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
bin/t225-s19-dem-package-lifecycle-fixture.exe direct  -> exit 0
```

## Interpretation

The three providers are Direct-path source-parity implementations with local
Direct regression evidence.  This is neither a complete DEM package nor a
native guest-run claim.

## Follow-up

Retain v1 and decide its fate only with the full owner package.  Do not add
non-Direct behavior to these v2 source mirrors.