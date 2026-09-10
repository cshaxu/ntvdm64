# M0 T95 S2: Host-Drive Policy Admission Snapshot

## Decision

`M0 T95 S2` consumes the existing CLI child-only variables
`NTDOS64_HOST_INCLUDE_DRIVES` and `NTDOS64_HOST_EXCLUDE_DRIVES` exactly once,
while installing the adapter session. The result is an adapter-local
`bx_ntvdm_host_drive_snapshot_v1`. It is not copied to Bochs, it is not a
guest ABI, and it performs no DOS/DEM/BOP action.

## Policy Contract

- Each variable is a comma-separated ASCII drive-letter set. Empty means no
  restriction; malformed characters, empty elements, and duplicates fail
  closed.
- The observed inventory is `GetLogicalDrives` plus one `GetDriveTypeW` call
  per observed root. Roots with an unknown/no-root type are removed before
  admission.
- With an include set, only its currently present letters begin eligible.
  Without one, every currently present letter begins eligible.
- The exclusion set is then removed, irrespective of the include set.
- The adapter stores only the admitted drive letter/type inventory. A letter
  not admitted has type zero in the copied snapshot.
- The snapshot is fixed for the lifetime of the installed adapter session;
  neither Bochs nor a guest request can invoke another host enumeration.

This satisfies the owner rule that `--exclude-drives` wins, and that no
selection flags permits all host logical drives. It deliberately does not
claim that those drives are yet DOS-searchable.

## Separation From Existing Inputs

The pre-existing `host_drive_inventory` in a BYOB profile remains a
deterministic, static input to the already-admitted `demGetDrives` service.
It is neither replaced by nor merged into S2's live host snapshot. This
prevents host-policy admission from silently changing established guest-service
fixtures, and prevents static profile data from being misrepresented as a live
machine observation.

## Implementation Surface

- `src/bx-ntvdm-adapter/bx_ntvdm_host_drive_policy.{h,c}`: parsing, pure
  policy application, validity checks, and the one Win32 capture operation.
- `src/bx-ntvdm-adapter/bx_ntvdm_adapter_runtime.{h,c}`: owns the snapshot at
  session install and returns a value copy only to same-process adapter code.
- `src/cli/ntdos64_run.c`: unchanged in this S; it already normalizes and
  passes the policy through the child-only engine environment.
- `refs/bochs`: unchanged.

## Reproduction

From the repository root:

```powershell
cmake -S . -B artifacts\build\cli-component-layout-check
cmake --build artifacts\build\cli-component-layout-check --target `
  bx-ntvdm-host-drive-policy-test bx-ntvdm-adapter-runtime-test
.\artifacts\build\cli-component-layout-check\bx-ntvdm-host-drive-policy-test.exe
.\artifacts\build\cli-component-layout-check\bx-ntvdm-adapter-runtime-test.exe
```

Expected markers:

```text
bx-ntvdm-host-drive-policy-test: deterministic include/exclude snapshot policy verified
bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified
```

The first test is deterministic and covers include-only, exclude-only,
include/exclude conflict, default visibility, malformed policy, and a missing
observed drive type. The second proves the runtime installs a valid snapshot
and that corrupting an exported value copy does not mutate session state.

## Explicit Non-Claims

S2 does not implement `FindFirst`, `FindNext`, FCB/DTA state, 8.3 projection,
directory traversal, host-path opening, file handles, mutations, DOS drive
mapping, DPB results, or a Bochs device. Those are separate queue entries
`M0 T95 S3` through `S5` and require their own source/ABI closure.
