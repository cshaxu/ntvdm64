# T198 S7 Cross-Island Link Witness 001

## Question

Can the classification-only `bx-vdm` bridge, built by i686 MinGW-w64, replace
the default mantle bridge in an MSVC/x86 finite CPU5 link without bringing in
a provider, session, CLI or product shell?

## Inputs And Procedure

MSYS2 i686 GCC 16.1.0-6 built exactly eight C objects: state, window,
exception ABI, delta, result, ingress, registry and bridge.  The bridge object
is PE/i386.  `Invoke-T198S3FiniteNativeRunProbe.ps1` rebuilt the CPU5 seed,
compiled `exception.cc` with the fixed bridge macro, excluded the default
mantle bridge object, linked the eight i386 objects using MSVC/x86, then ran
only HLT bytes.

## Observations

- i686 bridge SHA-256:
  `62A7086D187DAE8B76F7236BA1F14ADEDB5442ED1F32CA0D6A05B06F8A6A4E9D`.
- MSVC link map resolves `_bx_ntvdm_mantle_generic_ud_bridge_v1` and
  `_bx_ntvdm_mantle_generic_ud_fixture_stop` from `bridge.obj`.
- Link and HLT run both returned zero.  The finite executable SHA-256 is
  `D87FE8F86C77A85B3E8F3C5F2759028868A1B4BFF6E216D46DE1EEC6ACD6B212`.
- The fixture contains HLT, not UD2; no BOP, ingress, registry or provider
  runtime was executed.

## Interpretation

The i686 fixed-C bridge is now link-compatible with the MSVC/x86 minimal
machine when treated as an alternative bridge object.  This proves only
architecture, C linkage and finite lifecycle compatibility.  It does not
prove a BOP dispatch, a provider result, guest-memory transfer, continuous
execution, CLI transport or any historical OpenNT runtime composition.
