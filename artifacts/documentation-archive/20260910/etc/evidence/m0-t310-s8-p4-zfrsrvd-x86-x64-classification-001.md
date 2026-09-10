# M0 T310 S8 P4: zfrsrvd x86/x64 Classification

## Question

Do the selected CCPU floating-point service warnings still indicate host x86/x64 ABI defects?

## Inputs and procedure

- Read the exact zfrsrvd call sites and original providers.
- Forced formal zfrsrvd object builds under x64 and x86.

## Observations

- Current source includes original c_addr.h, c_intr.h, and intx.h, and locally publishes the exact existing FLDENV contract. The old implicit-call diagnostics are absent in both builds.
- Remaining conversions are fixed guest FPU/interrupt widths: IU32 or IS32 to IU16. ZFRSRVD itself accepts an IU32 guest instruction word. None is a native host pointer, handle, callback, or mapping identity.

## Result

Five obsolete ABI records are closed and one K&R record is retained as non-x86/x64 source style. No mirror source is changed and no warning is suppressed.
