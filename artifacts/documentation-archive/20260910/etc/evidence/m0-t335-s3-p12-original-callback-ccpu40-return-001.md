# M0 T335 S3 P12 — Original CallBack16 / CCPU40 bounded-return proof

## Claim

The selected original `CallBack16` body can complete its narrow non-GUI host
transaction with the original CCPU40 recursive return ordering.  This is a
source-contract proof only; it is not a claim that the Win16
`WOW16_From_CallBack16` guest trampoline or generic WOW runtime has run.

## Selected source and boundary

- `src/mvdm-host/wow32/wcall16.c` supplies the original `CallBack16` body.
- `src/mvdm-host/softpc.new/host/src/nt_cpu.c` supplies original
  `host_simulate()` and the selected CCPU40 path.
- `src/mvdm-host/softpc.new/host/src/nt_aorc.c` supplies the original
  `get/set{IP,SP,SS}` interface body.
- `src/adapter-mvdm-host-out/wow/wow_callback_frame_lease.c` is the admitted
  bounded replacement for the former raw `VDMFRAME`/`CBVDMFRAME` aliases.
- `src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c` supplies the
  same-shaped current-thread storage used by original `CURRENTPTD()`.

The fixture creates one original `TD` in `NtCurrentTeb()->WOW32Reserved`,
places original-format caller and callback frames in session guest memory,
then calls the original `CallBack16`.  The CCPU input contains the original
CCPU40 `D6 FE` unsimulate return.  That instruction proves recursive return
ordering only; it deliberately does not impersonate the actual Win16 guest
trampoline identified by P9.

## Test-only seams

`wcall16.c` also contains unrelated `LocalLock16` and `GetExePtr16` exports.
The linker therefore requires their private support names even though this
fixture never calls them.  The test-only
`tests/mvdm-host/t335_s3_wcall16_unreachable_seams.c` supplies only
`fWowMode`, `ghModCache`, and an immediate-`DebugBreak` checked-pointer
fallback.  These are not production providers and make an accidental path
expansion fail visibly.

## Reproduction

First generate/build the selected original CCPU40 lifecycle base graph:

```text
powershell -ExecutionPolicy Bypass -File tools/build/New-T313CcpuLifecycleNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64 -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T335\S3\P6\ccpu-lifecycle-x86
ninja -C build/M0-T335/S3/P6/ccpu-lifecycle-x86 -j 6
```

Then generate and execute the add-on graph with Node 22 and Ninja:

```text
node tools/build/Generate-T335S3WowCallbackCcpuNinja.mjs O:\repos.hobby\ntvdm64 O:\repos.hobby\ntvdm64\build\M0-T335\S3\P6\ccpu-lifecycle-x86 O:\repos.hobby\ntvdm64\build\M0-T335\S3\P12\formal-x86
ninja -C build/M0-T335/S3/P12/formal-x86 -j 6
build\M0-T335\S3\P12\formal-x86\original-callback-ccpu-x86.exe
```

The generated x86 executable exits `0`.  It verifies the original callback
frame fields, returned `AX:DX`, restored task stacks, and post-return IP.
P8 and P11 retain the paired x86/x64 selected-source and product-link
evidence; this recursive execution proof is intentionally x86-only under the
current CCPU40 execution policy.

## S3 conclusion

S3 is closed.  Its declared non-GUI profile has source initialization,
BaseVDM command-route binding, bounded guest-frame ownership, and one original
source-shaped CCPU40 recursive-return transaction.  Remaining GUI/USER/GDI,
WOWEXEC, CSRSS/BaseSrv transport, actual Win16 trampoline execution, VDD,
FAX and cross-process behavior remain explicitly outside this profile and
must not be inferred from this proof.
