# M0 T290 S3 P4 — Redirector host-handle binding

## Question

Can the original Redirector's `HANDLE_FROM_WORDS(BP,BX)` shape use the
existing session host-resource mapping without exposing an x86/x64 native
handle in guest-visible state?

## Inputs and procedure

- Exact original declaration carrier: `mvdm-support/inc/vdmredir.h`.
- The Redirector include binding redefines only `HANDLE_FROM_WORDS` after the
  unchanged carrier has been read.
- It resolves the combined 32-bit value through the existing
  `mvdm_host_identity` facade and the current session's sole `host_resource`
  mapping-manager instance.
- Formal `/MT` Ninja fixtures execute the binding and recompile the complete
  original `vrnmpipe.c` object on x86 and x64.

## Observation

- x64 and x86 `redirector-handle` fixtures both exit `0`: a published opaque
  identity resolves to the original native value, while zero returns
  `INVALID_HANDLE_VALUE` and `ERROR_INVALID_HANDLE`.
- x64 and x86 original `vrnmpipe` helper fixtures both exit `0` after taking
  the same binding.
- The previous x64 `DWORD`-to-`HANDLE` narrowing diagnostics are absent.
  Unresolved remaining warnings are only pointer/address and string branches
  not selected by this part.

## Interpretation and boundary

This binding is a narrow identity conversion, not a named-pipe provider: it
does not create, duplicate, close or publish handles.  A native handle is a
process-local value after resolution only.  `57:xx`, ingress, asynchronous
workers, guest-pointer transfer and actual pipe I/O remain disabled.

## Follow-up

S3 can now compose a selected original synchronous pipe function after its
guest span is isolated under the P1 pointer scope and its Win32 operation is
bound through the declared outgoing facade.
