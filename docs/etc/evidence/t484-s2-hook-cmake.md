# T484 S2 P3 native Hook graph

tools/build/hook/CMakeLists.txt replaces New-NativeHook64Ninja.ps1, which is
removed. The root x64 configuration invokes native CMake configuration for
the same stable hook tree. Its25 production source entries and their order
match the prior manifest. MIDL remains the native RPC producer; original
exports, MSVC options and per-unit A/B include boundaries are preserved.
The install fixture obtains run16 from the real native launcher producer.
No published binary or x86 reference graph is used to define this component.

The generated command wrapper is CMD-only. CMake owns the declarative target
graph; no CMake script-mode recipe interpreter or frozen Ninja graph was added.
The source manifest is still generated from the actual declared inputs.
No product source or original guest changed: mirror code delta0points/0lines.

## Evidence

- Source comparison:25 before/25 after, identical paths/order.
- Native DLL and three existing fixture targets built. Actual machine/exports/
  ordinal contract matches the prior DLL.
- Strict boundaries checked9 graphs/434 A/B objects. The parser now recognizes
  both slash and dash MSVC include/define spellings and CMake's separate
  output/source arguments. Only the literal working-directory phony marker
  is ignored; no source/header or layer is exempted.
- Hook-only authored macro-profile check passed. Image-machine tests and118
  installer/lifecycle assertions passed (PID12216).
- Final root cached full build PID26692 passed,2026-11-22T16:45:43Z to16:49:20Z.
- Raw comparison:4 identical images,7 with only PE timestamp changes. Hook64
  differs by6 timestamp bytes; the other changed images by4. Code-byte identity
  is checked separately from the later resource package stamp.
- Publication PID7988 passed to both destinations,11 images/22 hashes,
  package a56f5975-e552-4ccb-96a3-55fd0e3622d0.
- Post-publication PID26208 passed strict boundaries, the Hook profile,
  image-machine tests and118 lifecycle assertions using copies of the actual
  released DLLs, then reverified all22 published hashes.
- Documentation, relative-link and diff checks passed (PID19884). The
  containing S2 P3 commit is this batch's delivery identity.

Compact evidence is retained under
[s2-hook](../../../artifacts/build-migration/t484/s2-hook/post-publication-result.json).
Runtime Full evidence is reused only for unchanged/timestamp-only product
inputs under owner authorization; the new build and Hook-specific tests above
actually ran. This is not a new Full runtime pass or S2 closure.

## Outstanding full-profile check

The full authored macro-profile check failed on five unchanged WOW source
units: wow_control_brush, wow_control_state, wow_control_fnid,
wow_default_window_bits and wow_class_client. The previous161b9dc27 version
of the same test reproduced exactly these five reports (PID15180,exit1).
The test's default remains the full set; a component selector permits the
changed Hook boundary to be tested independently. No WOW exception was added
and the full failure is not relabelled as passing. Resolve the original-body
profile/derived-source classification during the remaining WOW graph migration
before S2 closure. Preserve original ABI rather than blindly deleting macros.

Task-local runtime copies remain under build/t484-s2-hook for this batch;
their compact evidence is retained before disposal. Remaining host graph
owners are the CCPU worker, native worker family and WOW32 generators.
