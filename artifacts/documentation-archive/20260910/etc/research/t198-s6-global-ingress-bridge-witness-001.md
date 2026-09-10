# T198 S6 Global Ingress Bridge Witness 001

## Question

Can a fixed copied generic `#UD` record enter the existing global `bx-vdm`
ingress and provider registry without attaching an individual provider or
historical runtime composition?

## Inputs And Procedure

- New `bx_ntvdm_vdm_generic_ud_bridge_v1.*` and its focused C test.
- The existing CPU-state, instruction-window, exception, CPU-result, ingress
  and provider-registry source objects.
- `C:\msys64\usr\bin\bash.exe` with UCRT64 GCC 16.1.0, with all temporary
  files confined to `artifacts/build/t198-s6-vdm-bridge/tmp`.

The test was compiled with `-std=c11 -Wall -Wextra -Werror` and run from the
same artifact root.  It supplies `C4 C4 50 11`, a non-BOP byte, an incomplete
BOP prefix and an unknown selector.

## Observations

- Compilation and execution returned zero.
- `50:11` is copied into normal `bx-vdm` records, classified by the common
  ingress as DEM/mapped-deferred, then selected by the registry as deferred
  DEM with original-OpenNT precedence.
- The alternative bridge still returns zero, so no generic resume/stop outcome
  reaches the CPU for that classification.
- Non-BOP, incomplete and unknown-selector inputs remain non-accepted routes.
- Static boundary scan excludes runtime/session/environment/guest-transfer,
  OpenNT, GUI and callback references.

## Interpretation And Follow-up

The first global-plane attachment is source and test closed, but it is only a
classification observer.  It does not make a BOP service executable and is
not yet a cross-island composed link: the default mantle bridge remains the
bare CPU5 fixture input.  The next slice must separately admit the composed
link artifact and then a whole provider/transaction family; it must not call
the historical adapter runtime directly.
