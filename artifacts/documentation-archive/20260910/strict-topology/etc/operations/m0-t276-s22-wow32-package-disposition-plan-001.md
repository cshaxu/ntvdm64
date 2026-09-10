# M0 T276 S22: W8 `wow32` package disposition plan

## Objective

Audit all 201 selected `wow32` paths as one original WOW32 DLL owner package.
The audit records condition-aware build selection, active provider subdomains
and every reached historical interface family before enabling a source body.

## Inputs

- Exact `src/opennt-mvdm-host/wow32` mirror and `wow32/sources`.
- The condition-aware [build-selection baseline](m0-t276-s22-wow32-build-selection-baseline-001.md)
  and file-disposition ledger.
- T276/S1–S21 source, interface and package records.

## Method

1. Preserve the 78 active build inputs, eight inactive source forms, 105
   declaration/table forms and ten resource/build forms as distinct evidence.
2. Classify each selected active source by original subdomain: thunk/core,
   USER/GDI, shell/COM/media/printing, or auxiliary table/resource support.
3. Inspect only original reached boundaries. Record whether the interface
   remains a public Win32 call, needs `adapter-win32`, needs
   `adapter-softpc`/`adapter-vdm-monitor` mechanical access, or needs the
   session guest-memory/opaque-handle mapping boundary.
4. Preserve original package order and make no source or build enablement
   decision. WOW-specific user/GDI private product APIs are evidence for
   `adapter-wow`, not authorization to invent a replacement layer.

## Required decisions

- `WOW32ResolveMemory`-style guest pointers use only a synchronous lease from
  the session-owned guest-memory mapping-manager instance.
- 16-bit/32-bit handle aliases use the session-owned host-resource mapping
  manager where native identity crosses width; no pointer or HANDLE is cast
  into a guest or historical 32-bit field.
- `gdi32p`, `user32p`, VDD/private server and NT4 product-shell imports remain
  named historical boundaries. The audit must not treat modern public Win32
  symbols as automatically equivalent.

## Non-goals

No source body, adapter ABI, formal build edge, Ninja execution, guest run or
WOW implementation is enabled by this audit.

## Exit evidence

Every selected path has an exact provisional role and disposition; active
provider bodies have an original subdomain and source-proven boundary family;
all guest pointer and handle crossings name their session mapping instance;
and unresolved private historical facilities retain an explicit later owner.
