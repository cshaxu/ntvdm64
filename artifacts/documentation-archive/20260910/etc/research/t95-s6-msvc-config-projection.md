# T95 S6 auditable MSVC configuration projection

## Question

Can the minimal MSVC/x86 syntax investigation retain the pinned Bochs Win32
platform declarations without continuing a hand-authored reconstruction of
`config.h.in`?

## Inputs and procedure

`tools/build/Project-BochsConfig.ps1` accepts only a fresh analysis root, the pinned
`refs/bochs/config.h`, and
`tools/build/t95-s6-msvc-config-projection-manifest.json`.  The manifest pins the
input SHA-256 and names every permitted source-forced or deny-list replacement,
including its exact old and new values.  The tool rejects a changed input hash,
missing or duplicate input definition, duplicate manifest name, unexpected
manifest field, unsupported addition, missing derived-condition anchor, or an
existing output root.  Its report records the input/output hashes and every
replacement/addition.

The projection preserves the original generated header's C typedefs, MSVC
`osdep.h` format path, extrema, `GET32L`/`GET32H`, platform API declarations,
and conditional validation machinery.  These are platform mechanics from the
pinned Win32 input, not feature inheritance.  The manifest overlays only the
selected CPU3/x86 and deny-by-default profile: no SMP/APIC (APIC remains a
checked derived result), FPU/MSR/VMX/SVM/AVX/WX, GUI/text configuration,
plugin/debug/instrumentation, PCI/USB/video/network/sound/storage product
facilities, or the registered #UD interception.  The latter is added as an
explicit `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0` default-off projection entry.

This package does not invoke a Bochs compiler, linker, executable, configure
script, VS project, `bochs.exe`, or `all`; it does not modify `refs/bochs`,
`config.h.in`, or Bochs code.

## Verification and interpretation

`tests/bx-ntvdm-adapter/Test-BochsConfigProjection.ps1` exercises a standalone
synthetic header and proves deterministic identical outputs, retention of an
untouched typedef, reporting of all named changes, and fail-closed rejection of
missing, duplicate, and unexpected replacements before an output root is
created.  It is a tool-boundary test, not a Bochs configuration or runtime
claim.

The next gate is review of the concrete pinned-manifest projection.  Only after
that review may the coordinator admit one fresh-root `/Zs` syntax observation;
the resulting configuration still cannot be promoted to a product recipe,
link, or runtime profile without separate closure evidence.
