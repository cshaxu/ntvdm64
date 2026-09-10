# M0 T276 S12: SoftPC package-boundary disposition baseline

## Scope

This audit slice covers all 998 byte-exact selected paths under
`softpc.new`: 339 source units, 585 declaration inputs and 74 resource/build
inputs. It is a provisional original-subpackage boundary result only. No
historical machine body, firmware input, host-control body or adapter ABI is
enabled.

## Reproducible result

`Audit-T276S12SoftpcDisposition.ps1` joins each selected T276/S1 file row to
its T276/S2 literal original build membership and records a source-tree role.
It produces a complete 998-row ledger with no unclassified path. The verifier
rejects duplicate identities, enabled rows, a missing Bochs-replacement class,
or a missing historical-build-tool class.

The initial role counts are:

- 489 historical machine execution/device paths: CCPU, CPU, CVIDC, video,
  disks, keyboard/mouse, comms, system and base DOS units. Their execution
  ownership is Bochs; they are explicit parallel-machine exclusions, not
  candidates for an extra SoftPC core.
- 59 original `host/src` mixed host-boundary paths, plus 189 host
  declarations/link and build inputs. This set contains control, monitor,
  machine, device and UI units and requires per-file SoftPC/CCPU/SAS, Win32,
  monitor and BOP review.
- 156 shared `base/inc` machine declarations and eight `base/support` inputs.
  They remain original source and are reviewed only through
  `adapter-softpc`—not copied into an adapter.
- 58 BIOS/ROM/data and base firmware paths. They are assigned to the separate
  `opennt-mvdm-firmware` mirror and are `adapter-bochs` firmware-input
  questions only, never a second machine executor or host runtime library.
- 19 debugger/disassembler inputs, seven original build-tool inputs, two Unix
  port inputs, nine historical `obj.vdm` composition paths and two build
  control paths. Each is explicitly separated rather than being mistaken for
  a default host runtime library.

## Original source evidence

The classification is anchored in the original component descriptions, not a
compile failure: `base/ccpu386/sources` declares the `ccpu386` library;
`base/cvidc/sources`, `base/video/sources`, `base/disks/sources`,
`base/system/sources` and peers declare the original SoftPC libraries;
`host/src/sources` declares the original `spchost/src` library; and
`obj.vdm/sources` declares the original `ntvdm` program composition. The
original `bios/makefile` builds `NTIO.SYS`; `debugger/sources` declares the
separate `vdmdebug` program; and `dat2obj/sources` declares a separate console
build tool.

Some source files lack a literal `sources` membership because the T276/S2
parser deliberately does not expand historical macros. Those rows remain
provisional and cannot enter a build based on this grouping alone.

## Next audit step

The next S12 slice must inspect the 59 `host/src` paths and their
reached interfaces as complete original owner groups. It must distinguish
adapter-softpc, adapter-win32, adapter-vdm-monitor, adapter-bop and app
composition edges, preserve original function shapes and register any
unavailable historical boundary. It must not infer a provider from a BOP trace
or enable a CPU/device source beside Bochs.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S12SoftpcDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12SoftpcDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 softpc disposition verification passed: 998 paths.`
