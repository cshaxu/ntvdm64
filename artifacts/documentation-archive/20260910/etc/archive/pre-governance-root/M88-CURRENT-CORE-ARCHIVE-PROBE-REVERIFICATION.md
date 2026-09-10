# M88 Current Core Archive Probe Reverification

Status: completed read-only integration probe, 2026-08-08.

## Purpose

This reruns the pre-existing default-disabled `ntdos64-core-machine-probe`
against the current independently built `ntvdm64` archives. It verifies the
toolchain/source integration seam without changing the external repository,
adding a local executor, or claiming a stable DLL ABI.

## Inputs

The external source revision was `79f8a8105f8df7a3ff3d2e2b0b667fa18c645260`.
The independently built MinGW archives were:

| Archive | SHA-256 |
| --- | --- |
| `libcore-machine.a` | `6EE7426FD8F9DBACECA76692B2A49C0EC676F3F85FABA7D9D27CDB46B057007C` |
| `libcore-machine-executor.a` | `86A21A37560C64E906EC85AEBD8C559BF0C4CF0E1667D1DC00C08A6C3004D889` |
| `libtype-facade.a` | `EA2CD2E8DB901F96C35D4E6C060D6258F9A14CA0D468951FB039E66D052BFCF9` |

`ntdos64` configured only its local
`artifacts/build/current/m3-core-probe` directory with
`NTDOS64_ENABLE_NTVDM64_CORE_EXPERIMENT=ON`, the external source path, and
those archive paths. No external source, build setting, or archive was
written by this task.

## Result

The probe rebuilt and printed:

```text
CPU has encountered an illegal instruction at L00000000.
#UD(0) - undefined
ntdos64 core-machine probe: OK
```

The diagnostic is the expected precondition for the probe's registered owned
`D6` undefined-instruction transition. It then reaches the finite core-owned
`STOP` disposition. The resulting probe executable is local build evidence
only.

A separate normal build rebuilt `ntdos64-run`; invoking it with no arguments
returned exit status `2` and its usage text. Therefore the archive experiment
does not enter the normal runner link or CLI path.

## Proven Scope

This verifies cross-tree, same-toolchain composition of core creation, frozen
configuration, reset, prepared ordinary-RAM entry, the existing constrained
real-mode `#UD` transition, and software execution to a stop.

It does not verify configured observation of a valid instruction, a paused
multi-span transaction, a copied-value external ABI, historical BOP/DEM
dispatch, NTIO/NTDOS loading, or any guest file. M79 and M86 remain controlling:
the M70 executable-wrapper gate is still closed.

`NTVDM64-CORE-MACHINE-PROBE.md` remains the generic probe contract; this
record supplies its current archive identity and narrower M79-compatible
interpretation.
