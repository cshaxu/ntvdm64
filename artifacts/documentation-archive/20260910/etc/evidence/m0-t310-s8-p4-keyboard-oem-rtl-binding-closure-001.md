# M0 T310 S8 P4 — keyboard OEM RTL binding closure

## Original caller and declaration

Original `mvdm-host/softpc.new/host/src/nt_keycd.c` calls
`RtlOemToUnicodeN` to convert one BIOS OEM byte into the `WCHAR` field of a
`KEY_EVENT_RECORD`.  The original `public/sdk/inc/ntrtl.h` declares that
fixed-buffer `NTSTATUS` API, but the selected `opennt-host` true subset had
omitted it.  The caller body remains byte-identical.

`OPENNT-HOST-011` restores only that original declaration to the selected
same-named declaration subset.  It does not import unrelated NTRTL product
surface.

## Modern binding

`ADAPTER-WIN32-027` implements the same original signature in the existing
adapter RTL source with public `MultiByteToWideChar(CP_OEMCP, ...)`.  It
preserves caller-owned buffers, the byte capacity and byte-result arguments,
and `NTSTATUS` success/invalid-parameter/buffer-overflow direction.  The
input and output remain host-local conversion buffers; no guest address or
host pointer is mapped or exported.

## Verification and disposition

- Forced formal Ninja x86 and x64 compilation of original `nt_keycd.c` and
  `opennt_support_rtl.c` completes with zero errors and zero implicit
  `RtlOemToUnicodeN` declarations.
- The `MapVirtualKey` `UINT` to `WORD` diagnostics at original lines 588 and
  595 occur on both architectures and represent the historical bounded
  keyboard-code field, not a pointer-width conversion.  They remain visible
  and are classified `not-x86-x64-fixed-input-scalar`.
- The P4 worklist records the call as
  `closed-same-shaped-oem-rtl-binding`.
