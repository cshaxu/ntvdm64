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

## Selected production thunk controlled witness

`build/t473-s9/callpr-product-probe-r1` links the actual cached production
`wow32/obj/wkgthunk.obj` (SHA-256 `50E3000F28A6A6C943B7B521BDCD39E6DF1B61D2EC8C07A42C1D8599811C83B5`) and
an independently compiled copy of the original assembly helper (SHA-256
`D981FAA8BDFC3B5A7482E2761107EB1CFEAACB73A57E97D555E76FA803A05967`). It supplies
a source-shaped VDMFRAME, a valid three-word ICALLPROC32 and isolated host
callbacks. The fixture has local host-boundary stubs; the pointer-conversion
mask is zero and unexpected unrelated stub calls terminate with distinct
errors. It is not a guest or scheduler fixture.

Using the same argument payload and expected value 10, a normal `__stdcall`
target returned through the selected original thunk and the subprocess
exited 0. With a `__cdecl` target while the thunk request advertises stdcall,
the subprocess printed the pre-call witness but did not return from the thunk
and exited `0xC0000409`. A separate control calling the original x86 helper
with the same target and source-Pascal-reordered arguments returned 10 and
exited 0. This is a controlled host reproduction through the actual selected
original thunk object, not an end-to-end Win16 application or proof that
every calling-convention mismatch fails. The first run used an incorrect
expected result of 14; its normal case returned 10 and failed the assertion.
The rerun corrected the assertion to the actual source-Pascal argument order.

Fixture artifact SHA-256
`16F4606C4D285391CEA3D2102F230D81D83D84D5AE4E2B3D23946C5C78E2F3C9`. The compiled
fixture SHA-256 was `23963EE529A0438003A51CB41CD97CC5B292C9B277C01C1B345F0177C9AAE4BA`;
the retained artifact differs only by a final blank line. Build
with the WOW32 graph cflags, x86 MSVC and `/O2 /Gy`; link its object with the
selected `wkgthunk.obj`, freshly assembled original helper, system imports
and `legacy_stdio_definitions.lib`. Run each of `normal`, `mismatch`, and
`original` in an independent owned process; normal is invoked without an
argument. The mismatch run is deliberately expected to terminate abnormally.

## Candidate narrow integration, not implemented

The source has three `#ifdef i386` regions: the first two surround x87
save/restore in `WK32LoadLibraryEx32`, while the third selects the assembly
helper for `WK32ICallProc32`. A possible minimal recovery is a dedicated
build-time selection at only the third conditional plus compilation/linking
of the original x86 helper, retaining the original argument conversion and
source-Pascal reversal. This is a proposed mirror divergence, not a global
architecture switch. Before applying it, review the helper's saved-register
ABI, original macro include provenance, debugger/exception unwinding, all
source/destination cdecl combinations, 0/1/32 argument bounds and pointer
conversion/guest-memory lifetime. Verify both a real thunk-body host fixture
and unchanged guest integration, then perform the coherent package gate.
No compatibility change or runtime implementation has been made here.

## Narrow host candidate and ABI discovery

A disposable candidate changed only the `WK32ICallProc32` conditional in a
copy of the original C source to allow a dedicated x86 helper selection;
it retained the provider graph flags and linked the freshly assembled
original host helper. The same selected-thunk fixture returned 10 and exited
0 in both the normal stdcall and the previously failing mismatched cdecl
cases. This is preliminary host-only evidence; no product file was changed.

A separate register probe then found the unmodified helper clobbers EBX:
the sentinel `12345678` became `0000000c` after three arguments. The
original helper uses EBX as the byte count without saving it; `callconv.inc`
`cPublicProc` does not add a saving prologue. Current x86 Windows treats EBX
as nonvolatile. The disposable candidate's current compiled caller pops its
saved EBX immediately after the helper, but this does not make the helper a
generally ABI-conforming public callable. Therefore the initial passing
candidate is not sufficient to justify direct production linking.

An independent isolated naked x86 adapter that saves EBX, forwards all three
helper parameters unchanged and restores EBX returned 14 and preserved the
sentinel in `abi-adapter-probe.c`. This demonstrates feasibility only: its
stack/unwind, nested callbacks, exceptions, all calling conventions, zero and
32 arguments, and actual Win16 behavior remain unverified. The experiment
uses the original helper body rather than modifying it, and does not establish
that this exact adapter should be the final product integration.

## Narrow candidate thunk and helper composition

`build/t473-s9/callpr-chain-matrix-r1` compiled a disposable copy of the
original thunk with only its third `i386` conditional extended for a dedicated
x86 generic-call selection. A compile-time symbol alias directs its existing
helper call through the tested EBX-preserving host adapter, which forwards
to the freshly assembled original `callpr32.asm`. The normal project provider
flags and `/O2` were retained. No product source or guest was modified.

With source-shaped VDMFRAMEs and controlled host boundary stubs, this
composition passed zero- and one-argument cases, all four source/destination
convention flag combinations against both stdcall and cdecl three-argument
targets (including intentionally mismatched declarations), a 32-argument
cdecl target and propagation of one structured exception. The standalone
helper/adapter matrix also passed a nested native call and checked host stack
pointer preservation. These are focused host fixtures, not real guest,
task scheduler, resource cleanup or full exception-unwind acceptance.

The compiled candidate C source SHA-256 is
`D9C2FCFD9C785085285BB39D165E5EEFB1B32EEBC76DC7945EA1D60EABC924A4`; the compiled
fixture source SHA-256 is `6FAEA539EA700D49838BA7C6D10C115B469AA9906516EE79717F59B198BBA908`;
the retained fixture artifact SHA-256 is `ED030CBCEC4039ABFE135EC74E0B31E06D9CE5347507CCE6DF26CC13E0B80D7B`. The fixture
artifact differs only in final newline representation. The original helper
source/include identities and the isolated EBX sentinel witness are recorded
above. Reproduction requires the x86 MSVC environment, the provider graph
flags, `/DMVDM_WOW_X86_CALLPROC32`, a local helper symbol alias to
`wow_callproc32_guarded`, and the original MASM helper assembled from source.
Keep the candidate entirely separate from any published product until native
callback reentry, genuine Win16 integration, ownership cleanup and the full
coherent package gates are complete.
