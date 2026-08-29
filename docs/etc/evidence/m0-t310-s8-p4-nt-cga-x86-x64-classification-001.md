# M0 T310 S8 P4: nt_cga x86/x64 Classification

## Question

Do the reached CGA host display warnings identify a pointer, callback, or
integer-width transition that requires an x86/x64 source repair?

## Inputs and procedure

- Read all 29 tracked records in the native CGA display provider.
- Read the affected screen rectangle construction and diagnostic macro call.
- Force formal Ninja rebuilds of obj/host/nt_cga.obj on x64 and x86.

## Result

- The 28 C4244 records assign screen coordinates to the historical signed
  16-bit console rectangle fields. They are guest/display value-range choices
  with identical x86 and x64 representation, not native pointer or ABI flow.
- The C4002 trace macro record is a retained source-diagnostic shape issue,
  likewise unrelated to x86/x64 host compatibility.
- Both formal objects compile. No source change, overlay, mapping manager, or
  warning suppression is justified by this cluster.
