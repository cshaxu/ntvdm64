# Adapter r14 Profile-Observation Object Closure

## Scope

r14 admits the profile-owned observation declaration path into the adapter
runtime only. It does not add a Bochs stop point, BOP decoder, device,
firmware value, guest-memory read, DOS service or guest startup path.

The source path is:

```text
BYOB profile machine_observations
  -> adapter machine-profile record
  -> copied-boundary observation transaction
  -> single pending-operation slot
```

The reverse path is intentionally absent: no Bochs source calls this request
yet, and no output byte is populated.

## Configuration Evidence

`artifacts/build/bochs-2.6-native-nogui-ucrt-r14` was copied from the pinned
`refs/bochs` import and configured with the existing native `nogui` UCRT64
options. `configure` completed and generated `Makefile`. The wrapper timed
out afterwards while writing its metadata file; this is recorded as wrapper
incompletion, not as a configure failure.

No recursive `make`, default target, or full `bochs.exe` build was requested.

## Explicit Object Set

The following one command completed with exit code zero in r14:

```text
make ntdos64_adapter/bx_ntvdm_exception_abi.o
     ntdos64_adapter/bx_ntvdm_cpu_state_abi.o
     ntdos64_adapter/bx_ntvdm_instruction_window_abi.o
     ntdos64_adapter/bx_ntvdm_guest_range.o
     ntdos64_adapter/bx_ntvdm_guest_write_abi.o
     ntdos64_adapter/bx_ntvdm_observation_transaction_abi.o
     ntdos64_adapter/bx_ntvdm_machine_profile_abi.o
     ntdos64_adapter/bx_ntvdm_host_service_catalog.o
     ntdos64_adapter/bx_ntvdm_host_transaction_abi.o
     ntdos64_adapter/bx_ntvdm_session_resource_abi.o
     ntdos64_adapter/bx_ntvdm_host_service_plane.o
     ntdos64_adapter/bx_ntvdm_host_session.o
     ntdos64_adapter/bx_ntvdm_adapter_runtime.o
     ntdos64_adapter/byob_profile.o
     ntdos64_adapter/byob_identity.o
     ntdos64_adapter/byob_image.o
     cpu/exception.o memory/misc_mem.o
```

This is object-closure evidence only. It proves the r14 adapter source and
the already-admitted generic #UD/ordinary-RAM seams agree at compile time.
It does not prove an executable link, reset fixture, memory observation,
NTIO startup, NTDOS load, or COMMAND execution.

## Trigger Addendum

The r14 private source copy was subsequently refreshed with the profile-owned
neutral trigger parser, machine-profile matcher and runtime-owned 4096-byte
output slot. Explicit compilation of `byob_profile.o`,
`bx_ntvdm_machine_profile_abi.o`, and `bx_ntvdm_adapter_runtime.o` completed
with exit code zero. No Bochs CPU or memory source changed for this addendum.

## Pending-Observation Consumption Addendum

`BX-MEM-011` was registered before source modification. r14 then explicitly
compiled the refreshed adapter runtime and `cpu/exception.o` with exit code
zero. The Bochs block can consume only a pending observation at the copied
boundary and invoke existing `copy_from_ordinary_ram`; it does not contain
trigger bytes, a profile identifier, range selection, BOP dispatch, or guest
service semantics. This is still not executable-link or runtime evidence.

## Link Decision

The prior r12 executable link remains evidence for the write transaction
fixture only. Mixing its archived libraries with r14 objects would not prove a
same-version executable closure, so it is explicitly rejected. A new link is
permitted only after the observation consumer has a selected, non-semantic
stop/resume contract and its complete same-version object set is declared.

## Next Gate

Choose one minimal profile-owned passive observation trigger and define:

1. how it reaches the generic #UD seam without BOP/DEM meaning;
1. whether it resumes or makes the bounded run stop;
1. which immutable profile ID it consumes; and
1. how `copy_from_ordinary_ram` transfers bytes only after the adapter has
   returned the same-boundary pending transaction.

Until those four facts exist, no Bochs call site may consume an observation
request and no guest startup result may be claimed.
