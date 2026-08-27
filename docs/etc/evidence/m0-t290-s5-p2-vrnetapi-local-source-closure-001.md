# M0 T290 S5 P2 — original NetAPI local-result source closure

## Question

Can the small local-result branches in the untouched original
`vdmredir/vrnetapi.c` compile on both supported host widths and retain their
source results without reconstructing the private Lanman/SMB product shell?

## Inputs

- Exact original `src/mvdm-host/vdmredir/vrnetapi.c`.
- Exact selected DS, public-internal base and public-internal network
  declaration packages in `mvdm-platform-abi`.
- The existing same-shaped `adapter-mvdm-host-out/win32/include/nt.h` carrier,
  extended only with the historical three-value `NT_PRODUCT_TYPE` declaration
  required by the selected `netlibnt.h`.
- `tests/mvdm-host/vdmredir/t290_s5_vrnetapi_local_fixture.c` and
  `tools/build/New-T290S5VrNetApiLocalNinja.ps1`.

## Procedure

1. Verify every imported declaration against its recorded OpenNT source hash.
2. Generate and execute the Ninja fixture on MSVC x86 `/MT` and x64 `/MT`.
3. Call only original `VrGetDomainName`, `VrGetLogonServer`,
   `VrNetGetDCName`, `VrNetWkstaSetInfo`, `VrReturnAssignMode`, and
   `VrSetAssignMode` bodies.
4. Keep the original source translation unit intact. The fixture link uses
   `/FORCE:UNRESOLVED` solely because the same object also contains unselected
   private Lanman/SMB/provider bodies. The fixture invokes none of those
   unresolved call sites.

## Observations

- Both architectures compile the complete original `vrnetapi.c` body after
  selecting the three declaration cohorts.
- On both architectures the fixture observes the four source
  `ERROR_NOT_SUPPORTED` + carry-set results and the two source no-op results
  without register mutation.
- The linker reports 50 unresolved imports belonging to uncalled sibling
  bodies: historical Lanman transaction, Xs/Rx, VDM-pointer and encryption
  paths. They are not hidden or stubbed, and no produced fixture is a product
  input.

## Interpretation

Confidence is high for these six exact original local results. This closes
their local compilation and behavior evidence only. `VrGetComputerName` and
the bounded `BX=1` `VrGetUserName` form remain source-composition candidates;
their guest output spans must be explicitly declared to the existing
Redirector pointer scope. The `BX=0` user-name form and all transaction,
NetBIOS and DLC paths retain their source-proven unavailable/other-owner
dispositions.
