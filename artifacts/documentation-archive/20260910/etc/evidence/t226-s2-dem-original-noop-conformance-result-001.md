# T226 S2 DEM original-noop conformance result 001

## Question

Do all OpenNT `demNotYetImplemented` entries in the callable DEM table retain
the original `setCF(0); return` contract?

## Source basis

`src/opennt/base/mvdm/dos/dem/demdisp.c` maps `50:1F`, `50:24`, `50:26`,
`50:28`, `50:2B`, `50:40`, and `50:43` to `demNotYetImplemented`. Its body
only clears CF and returns; it does not access a host device, file, or guest
buffer.

The historical CCPU dispatch body itself cannot be linked into x64 CLI code.
`bx_ntvdm_dem_provider_v1` is the existing source-derived owner seam with the
same selector table, resume and flag contract.

## Procedure and result

A fresh formal MSVC x64 `/MT` Ninja graph at
`build/M0-T226-S2/original-noop-20260817a/` built and ran
`t226-s2-dem-original-noop-conformance-fixture.exe`.

The fixture traverses BOP ingress, provider selection, DEM package facade and
the original-noop provider for every one of the seven services. It asserts
`RIP+4`, no GPR delta, CF write-mask only, and CF clear. The fixture printed
its full-family conformance marker and exited zero.

## Boundary

This closes only the explicit original-noop subfamily inside DEM S2. It does
not treat another deferred service as a no-op, enable a device profile, or
close DEM's file/FCB, GSET, lifecycle, raw-media or terminal groups.