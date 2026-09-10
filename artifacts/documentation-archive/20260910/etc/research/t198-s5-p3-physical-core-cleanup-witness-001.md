# T198 S5 P3 Physical Core Cleanup Witness 001

## Question

After removing the retired product-composition body from the adopted CPU
exception translation unit, does the finite CPU5 machine still source-build
and traverse its fixed generic `#UD` bridge to controlled stop?

## Inputs

- `src/bx-core/cpu/exception.cc` after physical deletion of the historical
  callback, runtime-dispatch, guest-copy and product-exit implementation.
- `src/bx-mantle/bx_ntvdm_generic_ud_bridge.*` and the finite CPU5 runner.
- `tools/probe/Invoke-T198S3FiniteNativeRunProbe.ps1` with `-UdStopFixture`.

## Procedure

1. Scan the core exception source for the retired interceptor/runtime symbols,
   environment selection, product exit, and guest/service vocabulary.
2. Run the focused mantle boundary and finite-run boundary checks.
3. Run the probe in a fresh `t198-s5-ud-stop-probe-r3` build root.  It projects
   the pinned CPU5/x86 configuration, source-builds the whole adopted CPU
   closure and finite mantle, injects `UD2`, and runs it with the private
   typed-stop fixture enabled.

## Observations

- The core scan returned no legacy composition hit.
- Both focused boundary checks passed.
- The r3 probe compiled the complete CPU5/Pentium-MMX object closure, linked
  the finite machine, then compiled the opt-in exception object and fixed
  mantle bridge.  Its finite native fixture built and ran successfully.

## Interpretation And Confidence

The only retained active core seam is the selector-blind copied generic event
and typed outcome.  The bridge can request the finite machine's controlled
stop; it contains no BOP, OpenNT, DOS, provider, product-main, GUI or adapter
runtime behavior.  Confidence is high for the stated source-build and bounded
fixture claim.  This is not a continuous guest-execution or host-service
claim.

## Follow-up

Admit the next T198 slice only after mapping the fixed mantle record to the
existing `bx-vdm` global ingress and typed outcome contracts.  Do not restore
the retired direct product-composition path.
