# Original CCPU SHLD body and decoder references

The original SHLD source is byte-identical in both compared editions and the
current source. The selected CCPU40 graph compiles its object into the original
CPU library. The observer binds the SHLD object and decoder to released NTVDM
bytes and resolves two decoder references to the selected SHLD body.

This is a partial source integration, not proof of all SHLD behavior. The
original body masks the shift count and returns without modifying flags for
zero count. Actual operand widths, register/memory forms, count boundaries,
flags, fault ordering and guest execution remain S11 obligations. The paired
BTC/RCL [review](ccpu-bit-rotate-image-review.md) independently retains those
instructions' own limits. No guest or product image was changed or executed.
