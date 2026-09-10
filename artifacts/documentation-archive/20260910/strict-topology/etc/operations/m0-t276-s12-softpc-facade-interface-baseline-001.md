# M0 T276 S12 P6: SoftPC/SAS facade interface baseline

## Question

How can the ten historical `softpc.new/host/src` machine-facade sources retain
their original interface forms without reintroducing a second SoftPC executor
beside Bochs?

## Inputs and method

The selected files are byte-exact source mirror inputs. Their T276/S12 P3
call/definition coverage and original source forms were reviewed in complete
facade-group order. The companion ledger records twelve logical interface
groups, original source spans, historical dependency, one recovery owner and
the predecessor gate. It deliberately distinguishes an original API shape from
an enablement decision.

## Findings

- `nt_cpu.c` supplies original CPU-run, IRQ and historical CPU-internal forms.
  The run/IRQ surface can only be recovered as a same-shaped
  `adapter-softpc -> adapter-bochs` facade; `cpu_simulate`, GDP register
  pointers and A3/Nano CPU internals are Bochs-replacement exclusions.
- `sim32.c` is the decisive SIM32/SAS boundary. `Sim32pGetVDMPointer` and its
  exports do not license native host pointers in MVDM state. A future facade
  may expose a pointer only for a checked synchronous lease; copied read/write
  paths and MVDM-visible values remain fixed-width.
- `nt_mem.c` and the backing part of `sim32.c` implement historical process
  reservation/commit mechanics. They retain source evidence and NTSTATUS
  contracts, but Bochs owns guest RAM topology. The host virtual address used
  by the original body cannot be repurposed as a guest linear address on either
  x86 or x64.
- `nt_emm.c` and `x86_emm.c` share EMM API shape but contain alternate
  historical backing models. Both require a future guest physical-page mapping
  seam and session-owned opaque backing identity; the x86 MONITOR path also
  reaches private NT section views.
- `nt_umb.c` retains the useful original UMB owner-state algorithm, but its
  direct private section mapping, hardware registry discovery and VDD hooks
  are cross-owner prerequisites. It cannot be assigned to `adapter-bochs`,
  because UMB/XMS/VDD are VDM semantics, not generic machine semantics.
- `copy_fnc.c` is bounded host-buffer support only. `cpucstbs.c` is an A3CPU
  profile exclusion. `nt_sas.c` is a function-table/lease shape. `stubs.c` is
  historical build-profile support and is not a provider implementation.

## Decision

No facade source is enabled. Recovery divides into only three allowed forms:

1. same-shaped `adapter-softpc` facade over typed `adapter-bochs` mechanics;
2. source-derived adapter operation where the original depends on private NT
   memory/section machinery, while retaining its observed results and errors;
3. explicit profile exclusion for a second CPU executor or historical build
   stub.

This is not a request to create a generic memory, EMM or compatibility layer.
The future adapter ABI must be admitted from the recorded SIM32/SAS, guest
physical-page and UMB/VDD interface groups and be reused only by their original
callers.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12SoftpcFacadeInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 SoftPC/SAS facade interface verification passed: 12 rows.`
