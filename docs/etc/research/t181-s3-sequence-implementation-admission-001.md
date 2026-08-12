# T181 S3 sequence implementation admission 001

Date: 2026-08-12  
Packet: M0 T181 S3  
Disposition: admits one unit-test-first source implementation slice (S4), followed by focused local verification (S5).

## Admission decision

S1/S2 establish a closed replacement for the unavailable historical COMMAND
broker: a v5 profile and fixed launch-plan declare exactly two guest slots,
the namespace materializes them, and the existing `54:01` copied transaction
delivers one slot per successful commit.  This is a single provider closure,
not an incremental service workaround.  S4 is admitted to implement the
complete closure in source and unit tests.

## Admitted source surface

- `src/cli/byob_profile.[ch]`: v5 exact validation and bounded copied slot
  descriptors.
- a versioned CLI launch-plan module plus `src/cli/ntdos64_run.c`: derive the
  two slots once from the selected target and fixed terminal declaration;
  transmit one bounded, profile-cross-checked handoff.
- `src/bx-ntvdm-adapter/bx_ntvdm_readonly_namespace.[ch]` and
  `bx_ntvdm_boot_namespace_provider_v1.[ch]`: materialize both named files
  and their immutable metadata.
- `src/bx-ntvdm-adapter/bx_ntvdm_cmd_get_next_service.[ch]`: replace the
  one-target boolean with the S1 pending/commit sequence state while retaining
  the exact copied `54:01` gather/write ABI.
- `src/bx-ntvdm-adapter/bx_ntvdm_adapter_runtime.c`: identity-load and bind
  the complete plan only during local adapter installation.
- corresponding focused tests and CMake declarations.

The test closure is local native source only: profile/plan validation,
readonly namespace, boot namespace provider, COMMAND `54:01` service, and
adapter installation tests.  The existing legacy-endpoint register scan must
also remain clean.

## Explicit exclusions

S4/S5 may not relink the engine, rebuild/modify Bochs, start a guest runtime
trace, enable `54:11` result transport, change controlled-stop dispatch,
introduce host files/directories/queues/processes, or import/rehost
`cmdGetNextCmd`/`GetNextVDMCommand`.  An engine composition or runtime
observation requires a later packet after S5 proves this source closure.
