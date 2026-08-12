# T130 S1 machine-composition outcome contract 001

## Question

How can the reached machine-composition seam distinguish a valid decline from
an original-handler mechanics failure without putting a selector, service, or
device decision into Bochs?

## Inputs

- `src/bx-ntvdm-machine-composition/bx_ntvdm_machine_bop_v1.h` already defines
  the fixed results `DECLINED=0`, `HANDLED_RESUME=1`, and
  `MECHANICS_FAILURE=2`.
- The component currently converts that result to a boolean in
  `bx_ntvdm_machine_composition_v1_probe`.
- The admitted T121 image enables the default-off generic seam, while T130's
  bounded trace reaches `C4 C4 02` and falls through.

## Contract

T130 may return the existing enum unchanged through the component seam. The
Bochs caller may write one generic outcome number for every non-declined
result, and may resume only when the value is `HANDLED_RESUME`; every other
value retains the original exception path. The component remains the only
place that recognizes its catalogue entry and invokes `unexpected_int`.

## Negative boundary

The change may not add a selector or marker constant, BOP/SoftPC/OpenNT/DOS
term, guest address policy, adapter call, device initialization, port model,
or CPU-state update to Bochs. It may not change the original handler or use a
failed mechanics result as a resume authorization. The focused test must cover
decline, handled, and mechanics failure.
