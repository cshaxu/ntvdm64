# M0 T396 S23 — WOW32 system-import loader recovery 001

## Question

Can the complete original WOW32 provider retain its original x86 stdcall
source ABI while naming the actual undecorated exports of current SysWOW64
system DLLs, and can the ordinary product load that provider?

## Inputs and procedure

S22 established that the prior provider asked USER32, KERNEL32 and NTDLL for
decorated `@N` names which those live DLL export tables do not publish.  The
S23 generator now writes the raw loader-visible name to each build-local
import-library definition and emits one build-only C object containing only
COFF `/alternatename` directives.  Each directive maps the unchanged original
source's decorated x86 reference (and its IAT reference) to that raw import
library symbol.  It implements no system API and changes no WOW32 source body.

The fresh x86 build retained all 77 bodies selected by the original `sources`
manifest, the four already-admitted provider support objects, the original
resource, the S20 parent import contract, and all four generated system
import libraries.  Its final `link` invocation used the generated graph's
complete input order and no `/FORCE` option.  It produced:

```text
build/M0-T396/S23/wow32-x86-final/wow32.dll
bytes: 493056
SHA-256: 3A5DA0515EE1DA1B0A1A2015BF5C57E8D169D88CED6AAE760A9C0DEE3CC5E96E
```

`llvm-readobj --coff-imports` confirms every previously reached corrected
system import is now loader-visible without `@N`: `SetCursorContents`, both
`RegisterWow*` entries, and the reached `Csr*`, `Nt*`, and `Rtl*` imports.
There is no decorated spelling among that correction set.  Other decorated
imports in the PE image belong to the explicitly supplied parent
`ntvdm32.exe` contract and are not system-DLL imports being repaired here.

The fixed `O:\\t396` stage was then updated with that exact DLL hash.  The
disposable x86 loader surrogate first maps the selected S20 parent image with
`DONT_RESOLVE_DLL_REFERENCES`, then calls ordinary `LoadLibraryA` for the
staged provider.  It recorded:

```text
parent=0x01150000 parent-error=0x000000b7 wow=0x6a300000 wow-error=0x00000000
```

Finally the normal Console-owning non-debug observer ran the staged S20
product with `system32\\WOWEXEC.EXE`, no scripted Console input and no modal
continuation.  Its 30-second bounded result was:

```text
result=exited
exit=0xc0000005
elapsed-ms=7422
loaded-module-count=23
loaded-wow32=yes
```

The product's existing default-off top-level exception record gives the new
post-load boundary without changing it:

```text
code=0xc0000005 address=0x7789f8ca access=0x00000000 target=0x00000008
return=0x3724ac76 base=0x00100000
```

## Interpretation and confidence

This is high-confidence loader recovery.  S21's `SafeLoadLibrary("WOW32")`
failure is gone in both the focused loader check and the ordinary product;
the module count rises from 15 to 23 and the product reports
`loaded-wow32=yes`.  The preserved original source ABI is bridged only inside
the build/link carrier, while the PE loader sees the names exported by current
Windows.

The new access violation is not a claim that `W32Init`, dispatch, or a Win16
client lifecycle succeeds.  Its source/module owner has not yet been mapped,
so S24 owns that provenance before any semantic change is considered.

## Follow-up

S24 must map the ordinary post-load exception's module and source return path,
distinguish an original WOW32 call boundary from a host callback/ABI failure,
and record the smallest truthful next owner.  It may not alter the original
provider bodies, fabricate a dispatch result, or suppress the exception.
