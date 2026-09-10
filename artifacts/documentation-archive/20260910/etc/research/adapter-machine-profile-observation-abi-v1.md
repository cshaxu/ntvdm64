# Adapter Machine-Profile Observation ABI v1

## Purpose

The pending-operation lifecycle requires observation ranges to originate from
declared machine-profile data rather than an arbitrary environment variable,
CLI address, or Bochs source constant. This ABI supplies that narrow origin
without assigning guest, firmware, device, DOS or host-service meaning to an
address.

## Record

`bx_ntvdm_machine_profile_v1` is a fixed-width C11 record containing up to
four `id -> physical range` declarations. Each declaration has a non-zero
opaque ID, zero flags and one non-empty `bx_ntvdm_guest_range`. The record has
no Bochs object/pointer, output buffer, firmware value, host path, selector,
service, or guest semantic name.

The profile is materialized only inside the adapter from the selected BYOB
profile's optional `machine_observations` declarations. The runner validates
and passes that owned metadata as part of profile selection; it does not read
guest memory, expose a raw address CLI option, or pass a separate address
environment variable. The declaration remains subject to a selected Bochs
configuration and evidence before any actual read may be admitted.

## Preparation Contract

`bx_ntvdm_machine_profile_v1_prepare_observation` accepts a validated profile,
opaque declaration ID, copied `#UD` boundary and CPU snapshot. It emits the
existing observation transaction only if:

- the profile and all declarations are structurally valid for the supplied
  aperture;
- the requested ID exists; and
- the resulting transaction passes its own #UD/range/output-size preflight.

The adapter runtime may queue that prepared record through the existing single
pending-operation owner, only with an adapter-owned output span. Preparation
and queueing do not read Bochs memory, allocate output, resume the CPU, or
choose a stop point. A separate generic Bochs memory seam must take the queued
record at its same copied boundary before any bytes can exist in that output.

## Verification

`byob-profile-test`, `bx-ntvdm-machine-profile-abi-test`,
`bx-ntvdm-host-session-test`, and `bx-ntvdm-adapter-runtime-test` passed in
`artifacts/build/current/adapter-cli-r1`. Together they prove strict manifest
parsing; successful profile-to-transaction preparation; rejection of duplicate
and unknown IDs, zero-length and out-of-aperture ranges; and one-time
same-boundary adapter queue/take. They do not prove a Bochs observation read.

## Next Gate

The final sentence below predates adapter runtime queueing. Queueing is now
proven at the adapter boundary; the remaining gate is a selected stop/resume
contract plus consumption through the generic Bochs ordinary-RAM reader.

The first concrete profile declarations must be attached to an evidence record
for a selected Bochs native configuration. They may then name the ranges the
first-startup descriptor requires to observe, but the names and numeric values
belong in that profile evidence—not in this ABI or in Bochs source. Only after
that can a passive #UD stop fixture queue one declaration through the common
pending-operation lifecycle.
