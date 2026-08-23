# M0 T259 S4 P3 — OpenNT Host Dialog and OEM Facade Reconciliation

Status: completed S4 part

## Question

Can the reached OpenNT `RcErrorDialogBox`, `RcMessageBox`, and `CreateFileOem`
host calls be recovered without changing their imported owner bodies or
introducing a Bochs/CCPU product shell?

## Inputs

- OpenNT `base/mvdm/softpc.new/host/src/nt_error.c`:
  `RcErrorDialogBox`, `RcMessageBox`, and `OemMessageToAnsiMessage`.
- OpenNT `base/mvdm/dos/dem/demmisc.c` and COMMAND
  `cmdmisc.c`, `cmdenv.c`, `cmdpif.c`.
- Existing OpenNT-shaped `demfile_shim.c:CreateFileOem` facade.
- Fresh external formal graph `build/M0-T259-S4/formal-r15/`.

## Recovered shape

`opennt_error_dialog_facade.{h,c}` is a shared `bx-vdm` host facade. It
preserves the original call parameters and sequence:

1. bounded OEM input conversion for first and second text;
2. period-separated prefix construction;
3. `LoadStringA(GetModuleHandleA(NULL), error, ...)` lookup; and
4. modal host notification with an original-style result category.

The historical `ntvdm` resource table and custom editable dialog template are
not composable inputs. If the public `LoadStringA` lookup has no resource, the
facade displays a public-API `MessageBoxA` fallback containing the original
resource id. `RcMessageBox` maps Abort/Retry/Ignore result categories to the
historical constants. The `RMB_EDIT` branch deliberately returns Abort: a
plain `MessageBoxA` cannot edit and safely return the OEM `msg2` buffer, so it
must not impersonate that PIF UI capability.

`demmisc` now calls the existing global `CreateFileOem` facade instead of
calling `CreateFileA` directly. This restores the same OEM-to-Unicode public
Win32 path already used by the direct OpenNT DEM file owner.

## Deliberately non-recovered adjacent calls

| Historical spelling | Disposition | Owner boundary |
| --- | --- | --- |
| `host_lpt_flush_initialize` | explicit no-effect while no LPT flush state exists | printer/device package |
| `HostFloppyReset` | explicit unavailable machine operation | FDC/DMA/CMOS machine package |
| `VDDCreateUserHook`, `ModuleLoad/Free/SegmentMove` | deferred; no VDD broker is recreated | VDD/debugger package |
| `DbgPrompt` and `RcMessageBox(..., RMB_EDIT)` | source-shaped terminal/failure behavior only; no input is fabricated | debugger/PIF-console package |
| `Sim32GetVDMPointer` in long-lived/debug paths | not widened; the bounded session mapping manager remains the only legal pointer bridge | mapping migration / debugger package |

These are not success substitutes. They remain represented by their original
call spelling at the mirror boundary and are queued by the tracker owner rows.

## Verification

The fresh x64 `/MT`, CPU5/Pentium-MMX formal Ninja graph built **454 nodes**
and produced **79 executables** (78 fixtures plus `ntdos64-native`). A second
dry run reported `ninja: no work to do.`

Focused r15 results:

- `t230-s9-demmisc-direct-import-fixture`: passes the original `demExitVDM`
  report-and-terminate order while a fixture-only dialog suppressor proves
  resource id `ED_BADSYSFILE` was observed.
- `t231-s2-command-misc-direct-import-fixture`: passes existing COMMAND ABI
  checks and verifies the recovered `RcMessageBox` returns historical Abort
  under the fixture suppressor.
- `t230-s8-demerror-lock-direct-import-fixture`: still passes its NTDLL
  Unicode and DEM error/lock contract.

No imported OpenNT mirror, `bx-core`, or `bx-mantle` source file changed.

## Interpretation

This satisfies both gates at once: original OpenNT interface form is reused
where public Win32 can satisfy it, and the replacement stays outside both the
OpenNT mirrors and Bochs. The custom NT4 dialog/resource/VDD/device shells
are neither silently ignored nor rebuilt under new names.
