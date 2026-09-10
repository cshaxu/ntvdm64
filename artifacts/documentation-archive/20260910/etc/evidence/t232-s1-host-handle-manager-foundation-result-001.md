# M0 T232 S1 — Host-Handle Manager Foundation Result

## Question

Can one `bx-vdm` session-owned component safely represent external 32/64-bit
Win32 `HANDLE` values as opaque 16-bit guest identifiers, without introducing
BOP or DOS policy into Bochs or the manager?

## Inputs

- Owner request admitting M0 T232 S1.
- Existing DEM table: `src/bx-vdm/bop/shim/dem_direct_host_session.c`.
- Existing COMMAND table: `src/bx-vdm/bop/shim/command_misc_shim.c`.
- Formal manifest: `tools/build/t225-s7-full-module-manifest.json`.

## Procedure

1. Added `bx_ntvdm_host_handle_manager` under the adapter shim directory.
2. Represented one entry in two bucketed indexes: external `HANDLE` to guest
   ID and guest ID to external `HANDLE`.
3. Made guest IDs nonzero, monotonic for the life of a manager session, and
   limited to `uint16_t`; released IDs are deliberately not recycled.
4. Added explicit owned/borrowed lifetime semantics and a focused fixture.
5. Generated `build/M0-T232-S1/formal-r1` with
   `New-T225S7FullNinjaGraph.ps1`, built the manager target, and ran it.

## Observations

- The target source-built the complete `bx-vdm` static-library closure plus
  `t232-s1-host-handle-manager-fixture.exe` under MSVC x64 `/MT`.
- The fixture exited zero. It verified initial ID one, host-to-guest and
  guest-to-host lookup, duplicate publication, invalid zero release,
  borrowed-release non-closure, owned-release closure, and exhaustion after
  ID `0xffff`.
- The manager stores the actual `HANDLE` only in host-private heap entries;
  its exposed ID type is `uint16_t`. It uses `uintptr_t` only to hash the
  host-private value, so neither x86 nor x64 truncation enters the guest ABI.

## Interpretation And Confidence

The S1 foundation is source-built and locally verified. It provides the
owner-requested unified manager contract, but it is not yet the product route:
DEM and COMMAND still retain their independent 64-entry tables. Confidence is
high for the standalone allocation/lifetime ABI and intentionally limited for
cross-family behavior until S2--S4.

## Follow-up

S2 must inject one manager instance into the DEM direct context/session and
delete `dem_direct_host_session.handles[64]`. S3 must inject the same kind of
session-owned manager into COMMAND and delete `handle_tokens[64]`; both shims
must retain the original register-pair layouts while requiring a zero high
word. S4 closes the cross-family migration and formal regression.
