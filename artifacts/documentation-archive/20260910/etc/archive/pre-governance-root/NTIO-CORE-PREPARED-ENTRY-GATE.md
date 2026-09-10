# NTIO Core Prepared-Entry Gate

Status: pre-execution contract, 2026-08-07. This document connects the fixed
NT4 EN-US BYOB profile to the current `ntvdm64` core-machine research probe. It
does not authorize a runnable DOS engine, a synthetic BIOS/BOP/DEM service, or
an NTIO execution attempt.

## Established Inputs

The fixed first profile requires `NTIO.SYS`, `NTDOS.SYS`, and `COMMAND.COM`.
`byob_profile_validate_file` now validates their metadata and file identities
before any machine construction. The historical NTIO build evidence establishes
that the selected structural image is headerless, is loaded at `0070:0000`,
and begins with a guest near jump to `0070:0370`. The historical NTIO source
also establishes that the first `SVC_DEMLOADDOS` request is issued as
`C4 C4 50 11` and asks the original DEM chain to place NTDOS at the guest
destination carried in `DI:0000`.

The current core-machine probe independently proves, under one MinGW-w64 GCC
toolchain, that an external consumer can create a real-mode CPU, freeze
configured transitions, reset it, apply an ordinary-RAM entry plan, and execute
owned guest bytes. It is a source-level/static-archive experiment only; it is
not a DLL ABI.

## Required Pre-Entry Transaction

An eventual NTIO prefix experiment must perform this exact ordering:

1. Validate the full selected BYOB manifest and every selected component.
2. Acquire NTIO bytes through a separately admitted, handle-stable loader. It
   must compare the same opened file against the selected size and hash before
   releasing the transient host buffer. It must not reuse a path after a prior
   validation pass without a new identity check.
3. Build a core-machine topology while configuration is open. Query the full
   physical range `[0x700, 0x700 + ntio_bytes)` for ordinary writable RAM and
   reject a ROM/device-provider overlap or address overflow.
4. Freeze topology, reset to the clean machine boundary, then atomically apply
   one real-mode entry plan: `CS:IP = 0070:0000` with an ordinary-RAM preload
   containing the complete NTIO image. The plan must use the image origin, not
   skip the source-defined first jump.
5. Record the redacted image role, byte count, load address, initial documented
   boot-register values, and first stop reason. Do not retain the image bytes
   after the session ends.

Any failed validation or mapping query leaves the core machine uncreated or
unmodified. A failed entry-plan application leaves its post-reset CPU and RAM
baseline unchanged.

## Conditions Not Yet Satisfied

This gate deliberately cannot execute NTIO today. The following historical
owners are still absent from a runnable original chain:

- the original CCPU/SAS/ROM initialization prefix or an evidence-backed owned
  execution adapter;
- original `io_init`, `reset`, and `scs_init` completion;
- original BIOS table reachability from the executing CPU;
- the historical `MS_bop_0 -> DemDispatch -> demLoadDos` path with the admitted
  per-session runtime-root seam; and
- source-derived IVT, BDA, boot-medium, device, interrupt, and timer state.

The existing self-authored CCPU lifecycle adapter, IVT/BDA scaffolds, and
single BOP/DEM handlers are explicitly excluded. They may compare a bounded
trace only; they cannot supply any item in this list or make this gate pass.

## Transition Rule

The core public surface remains neutral. It may register generic invalid
instruction patterns with constrained outcomes, but it must not expose BOP,
SVC, NTIO, NTDOS, or Microsoft-named concepts. A future private historical
adapter may register the observed byte pattern only after the original
historical consumer is linked and its IP/register ownership is recorded.
It must transfer into that original consumer, not decode a selector or provide
a self-authored DOS result.

`CORE-HISTORICAL-TRANSITION-BRIDGE-GAP.md` records why the current callback
cannot call original `MS_bop_0` directly: the historical path owns a complete
mutable CPU/SAS context whereas the current core callback deliberately exposes
only a constrained generic snapshot and patch.

## First Acceptance Evidence

The first executable acceptance target is intentionally small: a selected
BYOB NTIO image reaches the first unavailable *original* historical owner and
stops with a redacted trace. It does not claim that `SVC_DEMLOADDOS` was
handled, that NTDOS loaded, or that a command processor ran.

Only after this trace reaches the original `SVC_DEMLOADDOS` boundary may the
R2.2 per-session runtime-root seam be connected to original `demLoadDos`.
Only after that original NTDOS transition succeeds may `COMMAND.COM /C` become
an execution acceptance target.

## Current Loader Evidence

`byob_image_load_exact` is now the admitted transient component loader. It
does not reuse the validator's closed file handle or trust a path after a prior
hash. Instead it opens the selected rooted regular file anew with write/delete
sharing excluded, reads and hashes all selected bytes through that one handle,
and releases the buffer explicitly after the caller's pre-entry decision. The
owned fixture proves this behavior for the validated NTIO descriptor only; it
does not create a core machine or preload guest RAM.
