# T198 S23 P1 NTIO Request Builder 001

## Question

Can `bx-vdm` convert an already identity-checked NTIO image to the closed
mantle v2 pre-entry record without pulling source selection, BOP dispatch, or
guest semantics into the machine layers?

## Inputs

- Source-locked NTIO contract: 33,792 bytes (`0x8400`), physical `0x700`,
  preserve `0x714..0x717`, entry `0070:0000`.
- `src/bx-vdm/bx_ntvdm_ntio_preentry_v1.cc` and its C++ fixture.

## Procedure

Compile `bx_ntvdm_ntio_preentry_v1.cc` and
`tests/bx-vdm/t198_s23_ntio_preentry_request_fixture.cc` with MSVC x64 `/MT`,
the pinned CPU5 configuration projection, and no product executable. Run the
fixture; then run `tests/bx-vdm/Test-NtioPreentryRequestBoundary.ps1`.

## Observations

The fixture exited zero.  It asserts the copied v2 image, exact source-locked
addresses and entry fields, and rejects a mismatched identity byte count. The
boundary scan excludes BOP dispatch, DEM, filesystem, OpenNT/DOS, SIM and CPU
dependencies.

## Interpretation And Confidence

P1 establishes the VDM-side request builder only.  Exact SHA-256 validation
remains the preceding BYOB image-loader responsibility; the builder accepts
only its identity-checked image and matching descriptor length. It does not
yet prove execution of the real artifact or `50:11`.

## Follow-Up

Compile this builder with the existing boot-namespace/provider closure and
run the explicit source-built NTIO artifact to the first real BOP boundary.
