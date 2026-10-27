# WOW generic thunk x86 helper â€” preliminary boundary audit

## Identity and actual selection

`src/mvdm/wow32/i386/sources` explicitly lists both `fastwow.asm` and
`callpr32.asm`. The latter is a 32-bit host helper, not a guest binary; it
copies a dynamic argument block and restores the saved host stack pointer
after invoking the target. The source explains that the recovery supports
applications which specify a different calling convention from the callee.

Current cached WOW32 graph `build/.ninja/msvc-x86-release/wow32/build.ninja`
compiles `wkgthunk.c` but does not list `callpr32.asm`; the current provider
map does not show `WK32ICallProc32MakeCall`. An isolated `/P` preprocessing
run used the graph's actual cflags and x86 MSVC environment against unchanged
`wkgthunk.c`: it contains the 32-argument portable C call and case-32 branch,
and does not contain the x86 `WK32ICallProc32MakeCall` call. The preprocessed
file SHA-256 is `C29B83B9342A7ED92F53248D2C5E80FBCB7382788B060863AD4E0D40A8234F76` and the run is
`build/t473-s9/callpr-preprocess-r1` (disposable evidence). This confirms the
selected compile branch, not runtime equivalence.

The existing `tests/observation/win16_generic_probe.asm` exercises a real
three-argument `CallProc32W` path, and historical acceptance is in
`docs/etc/evidence/m0-t447-s2-scope-freeze.md`. It does not establish the
wrong-convention callee behavior documented by the assembly helper.

## Remaining design and proof

Trace original cdecl/stdcall bit conversion, argument order, zero through
32 arguments and stack restoration across the selected compiler branch and
real guest transition. Compare a deliberately mismatched callee under both
implementations in an isolated host fixture, then verify the guest return
path. Assess compiling the original host assembly using the pinned original
`callconv.inc` and narrowly selecting its original C callsite, without
globally enabling the kernel-VDM `i386` profile. Selecting `i386` for a
whole translation unit may change unrelated declarations and preprocessor
branches and must be audited; do not simply toggle it or use the historical
prebuilt `obj/i386/callpr32.obj`. No product change or guest rebuild is
authorized by this audit record.

## Generated-code comparison

A second x86 MSVC compile used the cached provider graph flags plus `/O2`
and produced an assembly listing at `build/t473-s9/callpr-assembly-r1`
(SHA-256 `E6195C6A7331745FCEE36237D651B70873AA66C3A77D01B574DF5E41E2B82794`). Its portable cdecl path pushes 32 words and adjusts
ESP by 128 after the call; its stdcall case arms rely on the called function
to remove their argument bytes. Original `callpr32.asm` instead saves and
restores ESP around the call irrespective of the callee cleanup convention.
This confirms a concrete stack-cleanup strategy difference in the selected
generated code, but does not by itself establish a crash or the behavior of
a particular guest program. A controlled mismatched-convention callee and
real guest witness remain needed. `fastwow.h` and `wow32.h` also contain
other i386-dependent branches, which is why globally adding `/Di386` is
not a narrow helper reconnection.

## Isolated host stack mismatch witness

An x86-only noninteractive host experiment assembled the checked-in original
`callpr32.asm` using pinned OpenNT `callconv.inc`, then compared that actual
helper with the selected portable C call pattern against the same three-arg
CDECL function deliberately declared as STDCALL. The C fixture uses `/O2
/Oy-` and explicitly restores its stack after measuring the portable call so
the experiment does not continue on corrupted stack. The disposable run is
`build/t473-s9/callpr-host-compare-r1`; it exited 0 and recorded
`portable_result=14 portable_esp_delta=-12 original_result=14
original_esp_delta=0`. Original source SHA-256
`03A5F10DB0449058F61822ADFA928713DFD8F5CF3F31F6A46656AFF4D4C78BE7`; original
include SHA-256 `044E0712A074C7FD61E1A4CA04FAC1FF860F028D289C92E320B01CC9C7690BB5`;
fixture SHA-256 `63FAA0E29894A7C74FA68E1212F016C716E37623F9863CECE677C54774426DA4`.

The fixture independently reproduces the strategy difference and exercises
the actual assembled original helper. It does not execute the original
`WK32ICallProc32` body or a Win16 task and therefore is not a product-bug
reproduction or end-to-end repair validation. The real selected object was
separately preprocessed and inspected as described above.

Reproduction uses the x86 Visual Studio build environment:

```text
ml /nologo /c /DDBG=0 /I<OpenNT-public-sdk-inc> /Focallpr32.obj <repo>/src/mvdm/wow32/i386/callpr32.asm
cl /nologo /O2 /Oy- /MT /Feprobe.exe callpr-host-stack-probe.c callpr32.obj
probe.exe
```

Use the pinned include identified above; do not link an existing prebuilt
`callpr32.obj` or interpret this host-only fixture as a guest acceptance.
