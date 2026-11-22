# T484 S2 worker CMake migration

## Scope and implementation

The P6 baseline is 7cddb9861, package972c2293-4961-4c9e-b1eb-bd690b0ad3a4.
The x86 worker graph now uses native CMake targets for NTVDM, VCDEX,
VDMREDIR, Hook32 and the existing fixture composition. The canonical
`tools/build.cmd` builds the AMD64 producers before configuring x86; it no
longer copies historical release images into the build as bootstrap inputs.

Nineteen original SOURCES groups still determine351 C units dynamically.
The remaining admitted sources retain explicit per-unit profiles. Native
MIDL, RC, MASM, original export preprocessing and pinned private ordinals
remain in their original roles. Fixed source projections and NXVM import
validation use native CMake metadata. No frozen Ninja graph is a build input.
No product implementation, mirror or original guest is changed.

The original configuration projection and NXVM manifest validator are shared
with the AMD64 metadata rather than duplicated. The old PowerShell worker
generator remains pending migration of its test-tool consumers in S3; the
product configure/build path no longer invokes it. Publication script
migration remains S4. This is not final interpreter-free clean acceptance.

## Verification

Working receipts are in `build/t484-s2-worker-cmake`. The old Ninja graph is
used only with `ninja -t commands` for comparison, never executed.

- All791 selected C compiler commands,67 archive commands and153 link
  commands match ordered source/flag/link composition after removing only
  CMake output/dependency infrastructure from comparison. The final inventory
  separately permits the single test-link repair described below.
- All274 normalized export definition lines match the prior generated DEF.
- The seven RC/MASM/MIDL commands retain their selected inputs and options;
  MASM adds its equivalent CMake output and compile-only spelling.
- AMD64330 compiler profiles,73 archives and41 links still match P6.
- The first worker comparison caught a CMake list passed as one VDMREDIR
  link argument. It was corrected before any product compilation.

Initial real full build PID14020 passed, followed by the final cached full
build PID26812 after all metadata/test corrections. The latter compiled and
linked no product code; it refreshed the five compatibility image-copy slots.
Both builds used `tools/build.cmd` rather than the historical generator.

Publication PID24164 installed package d8983d2b-3fe1-4d18-8ea9-64613c84346c
to assets/release and O:/winnt/system32; all22 image hashes match. Nine raw
images differ from P6 only in verified PE timestamps. NTVDM has4 timestamp
bytes and80 diagnostic source-path separator bytes changed, with identical
length and all other bytes equal. The prior raw WOW32 input was not retained;
comparison of the two published WOW32 images with their explicit package IDs
made equal finds only4 PE timestamp bytes. This is not a raw-hash equality claim.

Targeted run PID25116 passed guest environment,71 per-request COMMAND launch
assertions,167 capture assertions, the original C-VID vector/memory fixture,
and DPMI/CCPU cases177-184. The real compiler dependency gate checks326 A/B
objects across8 graphs; all544 authored/generated compiler profiles pass.

Two earlier fixture failures are retained, not counted as passes:

- The unchanged old capture link omitted ntdll.lib for its existing
  RtlNtStatusToDosError call. Only that fixture link now adds the import library.
- The launch fixture's deliberate invalid-handle probe raised
  STATUS_INVALID_HANDLE in GetHandleInformation. A temporary phase diagnostic
  proved this exact call; it was removed. The final negative probe treats only
  that exception as absence, preserving valid-handle object comparison and
  propagating every other exception. Product behavior is unchanged.

Compact receipts and the comparison-only baseline graph are under
[worker migration artifacts](../../../artifacts/build-migration/t484/s2-worker-cmake/commands-comparison-final.json).
Stability PID13472 passed two consecutive configurations: ROM/DEF bytes and
timestamps stay unchanged, with no product rebuild. Raw input hashes still
match the published manifest, and all22 destination hashes remain correct.
The containing P7 commit delivers this batch and closes S2. S3 is admitted
sequentially; S4/S5 and the overall T remain unfinished.
