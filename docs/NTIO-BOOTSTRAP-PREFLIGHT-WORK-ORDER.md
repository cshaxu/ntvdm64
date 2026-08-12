# NTIO Bootstrap Preflight Work Order

Status: M5 implementation work order, 2026-08-08. This is a pure BYOB-image
planning component for the fixed NT4 profile. It does not create a machine,
write guest memory, or execute NTIO.

## Source Facts

`src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c` shows the x86 host
sequence: read the four-byte virtual-interrupt state at physical `0x714`,
load NTIO at `0x700`, restore the preserved value at `0x714`, and set
`CS:IP` to `0070:0000`. `vint.h` fixes the state size as four bytes and the
NTIO load segment as `0070`.

Current core cold reset zeroes ordinary RAM. Therefore an empty owned machine
produces an all-zero x86 virtual-interrupt state, but preflight must accept the
actual reset-state bytes supplied by its future mantle caller rather than
inventing a hidden default.

## Outcome

Add a default-disabled owned fixture that builds one private NTIO preload:

```text
validated transient NTIO bytes + copied reset-state bytes
  -> private complete preload at physical 0x700
  -> preserved bytes restored at preload offset 0x14
  -> declared real-mode CS:IP 0070:0000
```

The source image remains unchanged. There is exactly one preload range, so a
future core entry plan does not receive overlapping copies.

## Module Boundary

| File | Responsibility | Must not do |
| --- | --- | --- |
| `nt4_ntio_bootstrap_plan.[ch]` | size/range preflight and private preload construction | open files, call core, create IVT/BDA, choose boot registers |
| `nt4_ntio_bootstrap_plan_test.c` | owned bytes and atomic negative cases | use actual BYOB content or historical objects |

The plan exposes load physical address, `CS:IP`, byte count, and an owned
private preload buffer. It does not expose a mutable guest-memory pointer or a
CPU register plan. Boot-medium registers, BDA/IVT, devices, and NTDOS loading
remain explicitly unproven.

## Required Tests

1. Valid private construction preserves all source bytes except the exact
   four restored state bytes; it does not change the source buffer.
2. Zero/too-short image and capacity-overflow cases reject without allocating
   or changing an existing output plan.
3. A nonzero supplied virtual-interrupt state is preserved exactly, proving
   the operation is not silently tied to zeroed RAM.
4. The target has no historical-object, core archive, or default CLI link
   dependency.

## Integration Gate

After core C1-C3 from `CORE-MANTLE-NTDOS-DELTA-PACKET.md` are available,
mantle may consume the preload as one ordinary-RAM entry-plan copy. This
work order alone does not satisfy the NTIO bootstrap cut or authorize a run.

## Implementation Evidence

On 2026-08-08, the plan was implemented in
`src/nt4_ntio_bootstrap_plan.[ch]` and tested by the default-disabled
`nt4-ntio-bootstrap-plan-test` target. The test uses only owned 32-byte input
and a nonzero four-byte reset-state fixture. It proves source immutability,
private preload construction, exact byte restoration at offset `0x14`, the
fixed `0070:0000` entry declaration, and rejection of too-short image and
ordinary-RAM capacity failures without output allocation.

The recorded verification was:

```powershell
cmake --build artifacts/build/current/default-mingw-verify --target nt4-ntio-bootstrap-plan-test --parallel 4
ctest --test-dir artifacts/build/current/default-mingw-verify --output-on-failure -R '^(owned-monitor-m[01]|nt4-oracle-date-adapter|nt4-ntio-bootstrap-plan)$'
cmake --build artifacts/build/current/default-mingw-verify --target ntdos64-run --parallel 4
```

All four fixtures passed; the normal CLI response file still contains only
`CMakeFiles/ntdos64-run.dir/src/runner/ntdos64_run.c.obj`.
