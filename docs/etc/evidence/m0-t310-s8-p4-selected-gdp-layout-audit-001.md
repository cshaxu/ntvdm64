# M0 T310 S8 P4 — selected GDP layout audit

## Scope

This is a package-level x86/x64 ABI audit for the selected CCPU/CVIDC global
data pointer (GDP) carrier.  It covers only `mvdm-host` and `mvdm-support`
source inputs and their already-admitted SoftPC patch input.  It does not
enable any new video path.

## Selected source identity

The formal `New-T310OriginalSoftpcNinja.ps1` graph puts
`src/mvdm-softpc-patch/x86/prod` before `ccpu386` on the include path.  The
selected `gdpvar.h` is therefore:

`src/mvdm-softpc-patch/x86/prod/gdpvar.h`

It has 692 direct `GLOBAL_*` lvalue/pointer macros and 692 distinct historic
32-bit offsets.  The local `ccpu386/gdpvar.h` has a different, unselected
generated layout and is not an x64 repair input for the formal profile.
The selected header is in the same layout family as CVIDC's generated
`gdpvar.h`; their small source-version differences remain a separate
source-selection review item.

The generated, reproducible inventory is
[`m0-t310-s8-p4-gdp-generated-layout-inventory.tsv`](../operations/m0-t310-s8-p4-gdp-generated-layout-inventory.tsv),
emitted by
[`Export-T310GdpMacroInventory.mjs`](../../../tools/audit/Export-T310GdpMacroInventory.mjs).

## Source finding

The original macros use fixed offsets such as:

```c
#define GLOBAL_SubrRingPtr (*(IUH* *)((IUH)Gdp + 8))
```

and store native pointers and callback values beside fixed-width CPU values.
On x86, `IUH`, the native pointer width, and the generated slot width were
all 32 bits.  On x64, merely widening `IUH` changes the storage width while
leaving the later fixed offsets unchanged; it would overlap fields.  Merely
retaining 32-bit `IUH` instead truncates native pointers.  Neither option is
an ABI repair.

This is private SoftPC controller state, not a guest address or externally
visible host identity.  The session mapping manager must therefore not be
used to tokenise GDP fields.

## Required recovery shape

The permitted recovery direction is a generated private
`mvdm-host-overlay` GDP-state carrier:

1. retain the selected original header as source evidence;
2. derive all slot names, original offsets and declared lvalue types from it;
3. allocate native-width private storage per original slot without changing
   the original macro names, field types, initialisation order or selected
   CCPU/CVIDC call sequence;
4. make only minimal registered include/binding changes in mirror sources;
5. separately audit the `j_Gdp` / `jccc_gdp` generated-code consumers before
   enabling a runtime C-video path.

No per-warning wrapper and no low-address allocation workaround is admitted:
both would hide the fixed-layout issue instead of preserving the original
typed state contract.

## Current disposition

`GDP-GENERATED-STATE` is **overlay-required; build-closed, not
runtime-enabled**.  The formal MSVC `/MT` candidate graph compiles and archives
the same generated overlay, original CCPU, CVIDC and reached `nt_cpu` host
carrier on x86 and x64.  This resolves the generated GDP base truncation as a
package rather than suppressing C4311/C4312.  It does not yet claim that the
CCPU/CVIDC video path has an enabled runtime test; `j_Gdp` / `jccc_gdp`
generated-code consumers remain an explicit follow-on P4 audit item.
