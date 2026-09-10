# M0 T310 S8 P4 host stub/source-form disposition

## Scope

This review covers the selected host-platform records that are visibly present
on both x86 and x64 but do not carry native-width identities:

- all C4431 records in `softpc.new/host/src/stubs.c`;
- the C4431 record for `StartupCharHeight` in `nt_fulsc.c`;
- both C4057 records in `nt_unix.c` for its static `BYTE[256]` error-text
  buffer returned through the original `LPSTR` API.

## Result

The `stubs.c` bodies are historical unselected/default SoftPC stubs using
K&R implicit `int`; their integer `0/1` results retain the same width on x86
and x64. `StartupCharHeight` is likewise an original default-`int` private
console scalar. The `nt_unix.c` conversions are `BYTE *`/`char *` views of
the same private one-byte storage; neither is a host pointer-to-integer,
guest address, handle, ABI carrier, or calling-convention conversion.

All records remain compiler-visible and source-unchanged. They are classified
as non-x86/x64 source forms. A later source-style cleanup, if desired, must
not be represented as an x64 compatibility repair.
