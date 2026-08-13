# T198 S17 MSVC x64 Native BOP Composition Witness 001

## Question

Can the S16 source-built adapter session run through the established
selector-blind Bochs `#UD` mechanics without adding BOP semantics to core or
mantle?

## Procedure

The S17 r4 closure uses the complete CPU5/Pentium-MMX x64 native-core seed,
the S16 r12 MSVC x64 adapter object closure, `bcrypt.lib` for the existing
BYOB startup-contract model, the existing finite mantle runner, its fixed
ordinary-RAM action object, and the replacement generic-`#UD` exception
object. The external fixture is
`tests/bx-vdm/t198_s12_native_composed_ud_fixture.cc`.

Its guest bytes are exactly `C4 C4 54 0C F4`: the historical four-byte BOP
form for COMMAND `54:0C`, then a neutral HLT after typed resume. The fixture
binds the existing adapter composition before native entry and unbinds after
the finite run. The BOP operation uses the pre-existing source-derived
boot-file plane and its standard fixed action: a successful adapter result
requires the mantle ordinary-RAM action to have accepted its checked write.

The initial all-in-one script exceeded the outer 120-second process limit
after producing r4's executable. The already generated executable was then
run directly, with its existing link response and a fresh `dumpbin /headers`
inspection retained in the same r4 build directory. This is a reporting-limit
workaround, not a substituted compiler or artifact.

## Result

`artifacts/build/t198-s17-native-bop-r4/t198-s3-finite-native-run-probe.exe`
exited zero. Its headers identify machine `8664` and PE32+. The run log shows
CPU reset, CPU5 CPUID, memory allocation, and the post-resume HLT. The HLT
warning is the known finite bare-machine terminal condition; it is not an
interrupt, BIOS, DOS, or continuous-execution claim.

The link response contains the exact S16 adapter objects, the mantle's
`mechanical_action.obj`, the finite runner and the replacement exception. It
contains no MinGW object, historical OpenNT runtime, Bochs product executable,
GUI/device archive or CLI engine. `bcrypt.lib` is the only platform library.

The proof of the ordinary-RAM write is a necessary synchronous consequence of
the existing composition contract: COMMAND `54:0C` returns typed `RESUME`
only after its non-empty write action succeeds; a failed action makes the
adapter decline and cannot reach the observed HLT. This is evidence of the
existing action transaction, not a new RAM observer or a new BOP behavior.

## Boundary

Core retains only copied event capture and typed outcome application. Mantle
retains the finite-run fixture helpers and the selector-blind RAM action.
`bx-vdm` alone recognizes `C4 C4 54 0C`, selects the provider and owns session
binding. The resulting limited proof is one bounded native transaction; it
does not establish continuous DOS execution, a host filesystem, or completion
of the BOP table.
