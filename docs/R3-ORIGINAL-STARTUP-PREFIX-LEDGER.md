# R3 Original Startup Prefix Ledger

Status: source-derived R3.C correction, 2026-08-07. This document replaces
the unsafe shorthand that treated `InitialiseDosEmulation` as the start of a
machine. It does not authorize a runtime target.

## Source Order

The original OpenNT process path in `base/support/main.c` is, in relevant
order:

```text
gfi_init
-> init_virtual_drivers
-> config
   -> process PIF/session policy
   -> sas_term
   -> calculate memory policy
   -> sas_init
      -> c_sas_scratch_address
      -> host_sas_init
      -> c_sas_connect_memory
      -> rom_init
      -> copyROM
   -> InitUMBList
   -> host_runtime_init
-> CMOS pickup/equipment handling
-> cpu_init
-> VGA-global/screen setup
-> InitialiseDosEmulation
   -> io_init
   -> reset
   -> SetupInstallableVDD
   -> scs_init
   -> historical NTIO file load and entry
```

The research profile may not call the final historical file-load code, PIF
shell, VDD setup, ambient-console discovery, registry/profile lookup, or host
current-directory mutation. Those exclusions do not make their preceding
machine state optional.

## Consequences For R3.C

1. `sas_init` is the first required original machine initialization boundary,
   not `reset`. It supplies the `SasVector`, CCPU M-area, writable memory
   classification, and the original ROM initialization call sequence.
2. `rom_init` reads historical ROM/configuration resources. It is an explicit
   contained-resource admission gate, not a place to reinstate IVT/BDA
   scaffolding or synthetic BIOS bytes.
3. `config` mixes required machine sizing with excluded process-shell/PIF and
   console policy. The narrow prefix fixture proves its structural default-PIF
   path can reach the first original `sas_term` lifecycle call when only the
   outer command/session inputs are replaced. It does not admit general PIF
   semantics, CSR/BaseSrv behavior, or later `config` branches as runtime
   policy.
4. `cpu_init` is after SAS setup. A successful standalone `c_cpu_init` fixture
   is therefore insufficient evidence for a bootable configuration.
5. `io_init` merely installs empty I/O routing tables. It occurs before reset
   in the DOS-emulation portion, but neither it nor `reset` can initialize a
   valid CCPU machine without the preceding SAS/ROM stage.

## Next Trace Cut

The first R3.C executable is a default-disabled source-owner trace limited to:

```text
admitted per-session profile inputs
-> original sas_init
-> original rom_init/copyROM
-> original cpu_init
-> stop before or at the first unavailable lower host/provider callback
```

It must report the original caller, symbol, and redacted role/size/address
metadata. It must not load NTIO, write an IVT/BDA scaffold, start guest
execution, bind a BOP/DEM handler, emulate a device response, or convert an
unresolved callback into success.

The first portion of this cut is now evidenced by
`R3-CONFIG-PREFIX-TRACE-LEDGER.md`: original `config` reaches original
`sas_init -> rom_init/copyROM` and stops at original `InitNtCpuInfo` without
guest execution. Only after the remaining original configuration owners are
traced can the next target append original `io_init -> reset`; `scs_init`
follows only after its separate process/session dependencies have an admitted
direct-CLI boundary.

## Current `config`/Resource Closure Measurement

The existing default-disabled
`ntdos64-opennt-config-resource-link-fixture` includes original `config.c`,
`nt_rez.c`, and the original `nt_pif.c` parser directly but calls none of
them. The parser is intentionally retained so `GetPIFData` is not misclassified
as a modern seam. The fixture also links the separately tested direct-CLI
first-PIF transport and title-identity seams, but does not call `config()`.
The current captured diagnostic is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/config-resource-pif-context-r3.log`,
SHA-256 `31AD167AFE915D859BD8F309B20718A16821A5E4198E8A7A18ADE9532B0743CF`.
It has 24 distinct unresolved symbols.

| Group | Symbols | R3 disposition |
| --- | --- | --- |
| Excluded historical product-shell/PIF/console state | `CntrlHandlerState`, `DosSessionId`, `SetConsoleKeyShortcuts`, `VDMForWOW`, `fSeparateWow`, `hWndConsole` | Retain original PIF parsing, but do not imitate CSR/BaseSrv, PIF-console policy, or event UI. The direct CLI runner needs one explicit per-session input record. `GetNextVDMCommand` and the obsolete title identity are independently fixture-proven and link-visible only. |
| Required but not yet admitted machine profile state | `xmsMemorySize`, `init_lim_configuration_data`, `sas_init`, `InitNtCpuInfo`, `InitUMBList`, `gfi_floppy_active` | The future R3.C trace must choose a fixed, recorded memory/EMS/drive profile and pass it through the original machine owners. It cannot inherit this process-global state. |
| Explicitly excluded physical-media policy | `host_using_fdisk`, `host_fdisk_valid`, `host_fdisk_change`, `host_fdisk_active` | Keep physical-drive configuration unavailable. No placeholder drive response is permitted. |
| Original PIF parser's host conversion/error paths | `ExpandEnvironmentStringsOem`, `bPifFastPaste`, `RcMessageBox` | Preserve them as parser-owned dependencies. The first CLI profile cannot ask for PIF fast paste or interactive dialogs; parse failure must retain source defaults and report a bounded error. |
| Resource and error/write side paths | `host_find_file`, `host_block_timer`, `host_error`, `host_release_timer` | A later read-only, manifest-contained resource seam may satisfy an original read caller. Resource writes and timer manipulation remain unavailable. |

This result makes the next implementation boundary precise: a modern outer
profile must not call `config()` as a convenience wrapper. It must instead
provide only the profile data that a separately admitted original machine
caller consumes, then leave SAS/ROM/CPU behavior in original code. The exact
caller and data shape remain an R3.C design/trace gate; this link diagnostic
is not authority to add such a shim yet.

The first narrow transport fixture is now independently admitted in
`R3-DIRECT-CLI-COMMAND-TRANSPORT-CONTRACT.md`. It supplies only the original
first PIF-query ABI and is linked into this diagnostic only; it remains absent
from every executing configuration or runtime target until the `pif-default`
role and the subsequent original caller are selected.

The original PIF default-parser fixture is separately runnable and proves the
minimal structural PIF path does not reach unadmitted PIF UI/environment
callbacks. The later default-disabled `config` prefix fixture now proves the
same path reaches original `sas_term` with title restoration and temporary-PIF
deletion; its exact input and stop rule are recorded in
`R3-CONFIG-PREFIX-TRACE-LEDGER.md`. SAS/ROM and every later closure owner stay
unadmitted.
