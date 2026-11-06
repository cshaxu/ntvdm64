# T478 S20 — DIB.DRV failure destructor and retained rollback boundary

Baseline: 111f4b9b8. Finding 49 contains both an existing cleanup API defect
and broader original post-selector failure rollback limitations. This stage
corrects the destructor; it does not add a transactional rollback contract.

## Minimal production correction

W32HandleDibDrv creates a memory DC, temporary file/section and HBITMAP, then
maps the section, attaches VDM/descriptor mappings, changes the guest selector
and allocates the DIBINFO registration node. Failures after bitmap creation
already reach its finally cleanup, including MapViewOfFile failure before any
guest selector change. Both original families call CloseHandle(hbm) there.
A bitmap is a GDI object; that call does not release it. After the existing
DeleteDC, DIV-876 replaces only that call with DeleteObject(hbm). Success,
registration order, return value, callback/selector handling and file ownership
are unchanged. No new DIB feature, retry, replacement or guest change.

## Scope of retained original limitations

The selected non-i386 path calls VdmAddVirtualMemory and
VdmAddDescriptorMapping before RET_SETDIBSEL, followed by W32AddDibInfo.
The latter allocates with malloc_w and can fail. Original error cleanup does
not unwind the already mapped view/VDM/descriptor/selector state. Its normal
W32RestoreOldDib path calls RET_FREEDIBSEL to allocate/copy back original
storage and itself can fail; simply unmapping on registration failure would
leave guest/host mappings referring to released storage. Blindly adding such
cleanup would not be a safe destructor correction.

Under the owner no-new-recovery ceiling, retain this original post-mutation
failure/rollback limitation explicitly. Do not claim registration failure is
fully transactional, impossible, or safe under all memory pressure. No forced
late registration failure, guest selector rollback or new retry was tested.
S21 separately owns normal retirement/lockcount behavior; it is not silently
included here. Reopen stronger failure rollback only under a separately
admitted valid-contract repair or explicit owner scope.

## Bounded native verification

[Fixture](../../../test/integration/wow_dib_failure_cleanup_test.c) and
[runner](../../../test/integration/verify-wow-dib-failure-cleanup.ps1) extract
only the actual W32HandleDibDrv finally block and compile it with MSVC. This is
not whole-owner/guest execution. Real native DCs, temporary delete-on-close
files, sections and selected DIB bitmaps model each acquisition prefix. Both
failure cleanup and successful ownership transfer are checked. Native GDI
resource counts establish retirement; successful resources are retired by the
test. Original leaked bitmap is also explicitly reclaimed by the fixture.

Baseline: 83 checks, one failure (one GDI object remains). Corrected: 82 checks,
zero failures. The extra baseline check is its explicit leaked-bitmap disposal.
Initial fixture diagnostics exposed that querying an already closed kernel
handle raises STATUS_INVALID_HANDLE under this host, and GetObjectType may
still report a stale DC type; neither was treated as a product failure. The
fixture alone catches the invalid-handle query and uses GetGuiResources for
GDI lifetime. It does not add product exception handling or assume naturally
occurring allocation failure. The real baseline leak survives these corrected
controls and disappears with the one API substitution.

## Delivery

Coherent x86/x64 builds and all 16 Full gates passed.
[Receipt](../../../artifacts/mvdm-audit/shard-3/dib-registration-s20-results.json)
records original identities, focused baseline/current results, Full timings and
all ten release hashes. Package e6000fef-d461-4e0f-b17c-decb791bd449
is published only to this worktree assets/release. DOS4GW/NESticle remain
owner-excluded, not counted as passing. No O:/winnt write or push. S21 follows.
