# M0 T474 S3 — shared process-image architecture query

The owner approved importing `src/lib-win32/process-image/image.c` and `image.h`
unchanged from Win16-style commit `d92c301551cd316e935f90cfe9afe0d75ce91c8e`.
SHA-256 respectively: `c9bb55054465aaba97259bc778feef444943022796231ca0b46fa10c0011b210`
and `1f267696c2e4cca4177a3592779f74284f0ba2f6ea3b2b325cbab295c271c0e7`.
The source is MIT-licensed; the full identical Win16-style license is retained
at `src/lib-win32/process-hook/LICENSE.win16style` and applies to this import.

The product hook installer and its cross-width helper use the imported
`win32_process_image_machine` to select or confirm a target's running machine.
Generic process-hook remains independent of process-image. The original
SEC_IMAGE classifier and NTSRV target machine/subsystem revalidation remain.
The remote PE-header check still validates actual staging. The imported function
requires `IsWow64Process2` and returns `ERROR_NOT_SUPPORTED` for a native host
other than AMD64. Its failure codes and inability to inspect an inaccessible
process do not promise equivalence with reopening a file; no fallback claims
otherwise.

Cached x86/x64 builds and the source-ownership gate passed. x86 and x64 test
executables each checked current process, invalid handles, x86/x64 suspended
children, query-limited and denied handles. The real product hook fixtures each
passed 168 assertions including same- and opposite-width propagation; the
missing-hook-DLL payload fixture passed all four width pairs. These tests do not
prove behavior on unsupported native architectures or older Windows versions.

The Full selected product suite passed against the staged ten-image package,
including DOS/native nested handoff, version negative controls, GUI launch,
Console return, frontend/worker loss and DOS4GW launch. Winmine, SOL and WRITE
windows were observed; this is not gameplay or editing acceptance. The exact
tested ten images and hook license were installed and SHA-256 verified against
the package manifest in `assets/release` and `O:/winnt/system32`.

After Full, the two persistent architecture-test build targets were added and
cached builds rerun. The rebuilt staged files were identical except that five
EXEs differed only in the PE header and IMAGE_DEBUG_DIRECTORY timestamps;
section-directory parsing verified both sets of offsets. The already-tested
package was published. Both persistent x86 and x64 architecture targets built
and passed their cross-width, limited-access and invalid-handle cases. No guest
was modified or rebuilt.
