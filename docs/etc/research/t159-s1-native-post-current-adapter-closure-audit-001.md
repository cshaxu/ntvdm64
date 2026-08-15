# T159 S1 native POST/current-adapter closure audit 001

## Question

Can the current T154 adapter composition use native Bochs POST plus the opaque
deferred-startup rendezvous without a source change, archive rebuild, device
expansion, firmware synthesis, or BOP change?

## Inputs and procedure

This is a static-only audit of T103, T104, T133, `main.cc`, `exception.cc`,
the current adapter runtime/profile parser, and the current derivative and
observer scripts. No compiler, linker, executable, firmware image, or source
file was invoked or changed.

## Observations

| Concern | Current closure fact |
| --- | --- |
| Pre-CPU lifecycle | Deferred `main.o` has `BX_NTVDM_ENABLE_EXECUTION_PLAN=0` and `BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1`. After `bx_init_hardware()` it arms the adapter session then enters the normal CPU loop; it does not apply a plan before POST. |
| POST rendezvous | The deferred observer emits a 512-byte option ROM at `CA800`, whose active trigger is `UD2` at offset three. Its retained hash is `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB`. |
| Profile ABI | The parser accepts the handoff only with an enabled `ntio-v0` plan and exact `#UD`, `0f0b`, `CA80:0003`. The adapter checks the same vector, real-mode CPU state, CS:IP, and instruction window before returning a plan. |
| Deferred consumer | `cpu/exception.o`, with the current generic intercept/listener settings and `BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1`, consumes that matching `#UD` once. It preflights/copies ordinary RAM and performs the existing checked entry transition; it contains no BOP-service classification. |
| Object boundary | `New-T98S1CurrentAdapterEngineDerivative.ps1 -DeferredStartupPlan` copies current adapter/CLI sources, rebuilds only `main.o` and `cpu\\exception.o`, and links those before retained `cpu/libcpu.a`. No Bochs archive is rebuilt/repacked or made a prerequisite. |
| Current provider set | The copied adapter list includes T154 `bx_ntvdm_dem_fastio_provider_v1.c`. Deferred startup neither selects nor alters it; it remains available only after normal guest execution reaches `50:42`. |

The direct and deferred consumers are mutually exclusive at compiled `main.o`.
A profile handoff cannot turn a direct binary into a deferred one. An unmatched
`#UD` remains on the ordinary exception path, not a generic BOP handler.

## Interpretation

The no-source-change closure is **proven statically**. This is the only form
aligned with T158: native BIOS/VGA POST initializes IVT state before OpenNT's
pseudo-ROM saves and uses INT10. It neither repairs the direct entry image,
synthesizes IVT state, adds a device, nor moves DOS/OpenNT behavior into the
adapter.

T104 built and observed the same two-object shape with an earlier current
adapter set. T159 proves the present generator preserves that shape while
copying T154's provider closure. It does not claim a fresh binary links or
executes.

## Reproducible successor boundary

T160 must use a fresh MSVC **x86** build root and only:

```text
tools/build/New-T98S1CurrentAdapterEngineDerivative.ps1 -DeferredStartupPlan
nmake /f ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe
```

It must verify the generated manifest/makefile, retain the two-object limit,
record binary hash/map, and make at most one bounded deferred POST observation
through the existing option-ROM/profile generator. A missing object, ABI
conflict, or archive/device request stops work rather than broadening Bochs.

## Confidence and follow-up

Confidence is high for static closure because the macro pair, rendezvous
bytes, adapter checks, generator constraints, and T104 predecessor are
retained. T160 S1 is separately admitted for build and bounded observation;
it must not change source or provider behavior.
