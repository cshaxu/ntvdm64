# DLL-entry BOOLEAN return contract

**Historical S10 audit:** the candidate described below has since been
integrated into the working-tree generator and is not yet published. Current
implementation and bounded loader evidence are recorded in
[S11 foundation candidates](s11-foundation-candidates.md); the original
fixture findings below do not alone prove real guest DLL lifecycle.

The external loader frontier led to a concrete project ABI mismatch rather
than a need to port NT4 NTDLL. Original `wow32.c::W32DllInitialize` and
`wow32.h` declare BOOLEAN. The selected Windows ABI defines it as one byte;
the fixture checks that size. The original function explicitly does not use
its third Context argument. Original `ldrsnap.c` captures the initializer
status from AL, consistent with that return type.

`tools/build/New-T404S5Wow32ProviderNinja.ps1` instead generates an `extern int`
declaration for this function and forwards its full integer result from
modern CRT DllMain. The actual selected bridge object propagates high EAX
bits which do not belong to a BOOLEAN return value. This violates the
original provider's declared return contract even when the current compiler
happens to clear those bits on particular failure branches.

The independently authored `dll-entry-return-probe.c` links that actual bridge
object to a controlled provider with the correct BOOLEAN ABI. Its AL is zero
or one, while the unspecified high EAX bits are A55A0000. The probe checks
true attach, false attach, false thread notification and false detach; it also
checks that both existing explicit-unload cleanup calls still occur once.

The [controlled probe result](dll-entry-return-result.json) shows the actual
bridge returns A55A0000 for a false attach, while a test-only candidate whose
sole change is `extern unsigned char` returns zero. Both preserve two cleanup
calls. The [image correspondence](dll-entry-image-result.json) matches the
50-byte selected bridge to published WOW32.DLL with three call relocation
fields masked. It is not a proof about every callee or actual loader ordering.

Reproduce with `verify-dll-entry-return.ps1 -BuildRoot <fresh-build-directory>`;
it builds and executes only owned native probe processes. The candidate C
file and executables stay in build evidence, never in the product or release.
`verify-dll-entry-image.py` accepts `--object`, `--image` and `--output` and
only reads the selected binary inputs. Neither operation loads a guest or
modifies original source/installed files.

This establishes a controlled ABI failure, **not** an observed real WOW heap
allocation failure, failed startup or corruption. The actual original attach
failure paths have not been shown to retain nonzero high EAX bits. The finite
S11 repair is to use the real BOOLEAN return declaration at the generated
bridge, preserving CRT and original entry/cleanup ownership. Then verify true
and false returns, normal notifications and unload through the actual DLL,
plus existing WOW lifecycle and coherent package gates. Expected mirror diff
is zero; the proposed change is one project-owned generator declaration, with
no guest change or private NTDLL import. The candidate is not implemented or
published by this S10 audit.

## Real WOW32 loader boundary, source recheck

The boolean ABI repair concerns the host WOW32.DLL process initializer, not
an arbitrary 16-bit guest DLL's entry routine. The selected `nt_bop.c` first
enters the WOW page domain, calls `SafeLoadLibrary("WOW32")`, resolves
`W32Init`, `W32Dispatch` and required auxiliary exports, then calls `W32Init`.
The selected `mvdm_softpc_firmware.c` maps this name to the staged product's
`system32/WOW32.DLL` and invokes the host loader. Thus successful real WOW
startup and the previously observed loaded-module path establish a normal
host-DLL load, while the direct and Windows-loader BOOLEAN fixtures prove the
bridge's false-return treatment only with a controlled provider.

Original `W32DllInitialize` can return FALSE when `CreateSmallHeap` or
`HeapCreate` fails during process attach. A failed native load is handled by
the original `nt_bop.c` load-failure path, which requests VDM termination;
missing required exports separately call `FreeLibrary` before termination.
These are source-traced branches, not injected real-provider failure results.
The call chain and controlled tests do not prove actual heap-failure cleanup,
thread notifications or guest DLL initialization/unload semantics. No source
or guest change was made for this recheck.
