# T217 S14: Composition Stack-Capacity Result 001

## Question

Why did the current source-built x64 composition fixture terminate with
`0xC00000FD`, and may a build-capacity adjustment be treated as a BOP or
OpenNT semantic repair?

## Inputs

- The active T217 S14 DOS image-loader handle-family brief in
  `docs/STATUS.md`.
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm`: `FastOrSlow`,
  `SVC_DEMFASTREAD`, and carry-driven `SVC_DEMREAD` fallback.
- `src/opennt/base/mvdm/dos/dem/demdisp.c`: original
  `SVC_DEMFASTREAD -> demNotYetImplemented` table entry and its carry-clear
  default.
- Existing adapter transaction bounds:
  `BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES` and
  `BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD`, both 65535 bytes.
- The source-built x64 boot-namespace composition fixture.

## Procedure

1. The original fixture linked with the MSVC default 1 MiB stack and produced
   a runnable image, but its run terminated with `0xC00000FD`.
2. A read-only control link over the identical objects added only
   `/STACK:8388608`; it exited zero.
3. The fixture build contract now supplies that explicit 8 MiB reserve and
   records it in its JSON evidence. No guest source, BOP selector route,
   OpenNT provider, Bochs code, machine capability, or mutation profile was
   changed.
4. Fresh result: x64, `/MT`, `stackReserveBytes: 8388608`, link exit 0,
   run exit 0, passed true.

## Observations

- The composition path contains several independently bounded automatic
  payload arrays of 65535 bytes in package/session/plane routing frames.
  Their nesting exceeds the ordinary 1 MiB Windows executable stack reserve.
- The 8 MiB control changes only host process storage reservation and makes
  the same object closure run to completion. It does not alter guest state,
  register ABI, carry/error behavior, filesystem policy, or BOP dispatch.
- The focused readonly-file fixture independently reports its existing bounded
  open/seek/read/fast-read/close lifecycle as verified.

## Interpretation

This is a build-capacity seam at recovery-ladder rung 2: a minimal host
composition setting required by the adapter's already-declared fixed maximum
transaction buffers. It is not a replacement for any OpenNT algorithm and
not an intrusion into Bochs or the host system.

The result does not close T217 S14. The full fixture's service sweep proves
that the composed provider closure can execute under its declared storage
budget, but it does not by itself prove a declared-image positive `50:42`
route with its opaque token, checked guest write, and subsequent normal
handle lifecycle. That proof needs a dedicated package fixture rather than a
trace-led handler change.

## Follow-up

Retain the explicit stack contract for composition fixtures. Next, map and
construct a package-level declared-image fast-read regression that exercises
OpenNT's `50:42`/CF/`50:16` relationship without using the historical
`demNotYetImplemented` no-op as a false success path.

## Confidence

High that the observed termination is fixed-capacity stack exhaustion; high
that the new linker setting is selector-blind and non-semantic. No claim is
made for complete loader-family closure or native runtime behavior.
