# M0 T318 S2 P53 — Original configuration owner cohort

## Scope and ordering

This record closes the original `softpc.new/host/src/config.c::config` cohort
which executes between `host_main` setup and `cpu_init`/`scs_init`:

```
Base VDM GetNextVDMCommand(ASKING_FOR_PIF)
  -> original PIF/default selection
  -> original PIF memory and console policy
  -> original SAS size/init and CPU-state binding
  -> original ROM/BIOS mapping and UMB list
  -> original host runtime setup
```

## PIF and ordinary-child disposition

The selected `--ordinary-child` Base VDM declaration supplies the original
`VDMINFO` request through the existing source-shaped Base VDM client.  Original
`config()` retains its `ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY` request and
the original current-directory/title handling.

For the declared profile no PIF file is required.  Original
`nt_pif.c::GetPIFData` first calls `SetPifDefaults`; if the historical
`%windir%\\_default.pif` is absent or unreadable, it returns `FALSE` with those
defaults retained.  Original `config()`/`process_pif_exe` then continues with
that default `PIF_DATA`; it does not invoke a terminal path.  This preserves
the original no-PIF semantics and does not require a new PIF provider.

The only PIF display-mode terminal calls are under the selected X86GFX
full-screen transition branch.  The fixed windowed, real-console profile does
not select that branch.  Its ordinary title lookup falls back to the original
desktop-handle behavior if no console HWND is found; this is non-terminal.

## Machine configuration disposition

- Original memory calculation assigns XMS/EMS/DPMI values before original
  `sas_init`; P52 separately proves XMS's nonzero prerequisite.
- Original `sas_init` establishes the SAS carrier, after which original
  `InitNtCpuInfo` binds CPU40 state.  No guest pointer is passed through a
  host-native identity conversion here.
- Original `GetROMsMapped` and `locateNativeBIOSfonts` execute before later
  memory users.  The selected V7 VGA firmware/media closure is P7; the fixed
  stage carries that immutable firmware.
- Original `InitUMBList` and `host_runtime_init` remain selected original
  owner calls.  UMB allocation itself is later guest configuration work and
  is covered with its XMS dependency by P49.

## Result

The complete original configuration cohort is source-shaped under the fixed
ordinary-child profile.  Its only private historical command transport is the
already selected Base VDM boundary; no CSR/BaseSrv body is executed directly.
No unclassified `DisplayErrorTerm`, profile, PIF, ROM, SAS, or memory
prerequisite remains before original `InitialiseDosEmulation`.

No product source changes, therefore no frozen-container observation is
admissible.  The next valid runtime action remains contingent on a genuine
source-shaped NTIO/guest-entry repair, not on changing console ownership,
debugger state, media, or arguments.
