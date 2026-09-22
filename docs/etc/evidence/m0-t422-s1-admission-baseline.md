# T422 S1 WOW32 admission baseline

## Purpose

This is the immutable input baseline for T422 S1.  It distinguishes the
accepted DOS product package from the partial formal WOW32 provider build.
It is provenance and composition evidence only; it is not Win16 runtime
acceptance and does not alter any guest file.

## Admission revision and source selection

- Admission commit: `ddd58d641 Admit M0 T422 WOW32 recovery audit`.
- Selected original WOW32 manifest: `src/mvdm/wow32/sources`, SHA-256
  `D24767FF50C4C7017E6FCF5F550E120DB0F580F7C07C77CBE7E99E4432F52B36`
  (77 active C inputs).
- The pinned immutable USER profile is verified by
  `tests/observation/verify-wow-user-profile.ps1`: PMODE32, 49 binary
  consumer checks, USER build identity `84000000`.

## Immutable Win16 media

| File | Canonical package path | SHA-256 |
| --- | --- | --- |
| WRITE.EXE | `O:/winnt/WRITE.EXE` | `08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B` |
| WINMINE.EXE | `O:/winnt/WINMINE.EXE` | `A9D2AFBD1AA98E38F3679F3A1A5DC0463367DD47C9AB1792E5AB8BBDC3441C0C` |
| SOL.EXE | `O:/winnt/SOL.EXE` | `B42EB8E65446B5D5DFE06E4A7519E5203BC8A4FA5DF82C6CA6FCD3C5C68AA52B` |
| DOSX.EXE | `O:/winnt/system32/DOSX.EXE` | `C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF` |
| KRNL386.EXE | `O:/winnt/system32/KRNL386.EXE` | `D404576899CC11D23F368F2B30554E6108657CD37809837B341FD37E6B1A7AF4` |
| USER.EXE | `O:/winnt/system32/USER.EXE` | `6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C` |

These files are test inputs only.  T422 may inspect and execute them but must
not modify them.

## Current product versus formal provider candidate

The installed product files observed at admission were `run16.exe`
`6309ABB312B9C61E9229894C418310645D5AEF7C1ADB7B364E8FFCB6E6BD6841`,
`basesrv.exe` `F2AC466D7D1954C15534F346F147577BE8F74FFCE854DB1B09ABF12B27BED0AA`,
`ntvdm.exe` `8846B063DBEC26545EBD5EA0563D01182061325E1009C5885AA98317755FA354`,
`dtmgr.exe` `BEF23AC59F48AFDCF079C37FA2CFC31E104CF79BD89B202D693631F11BE69668`,
and `VDMREDIR.DLL` `F699B5735A9D475AC37C527FB06DA4D8606F91D4265149F26ADEB5DE58D91BCE`.

`O:/winnt/WOW32.DLL` is an older 504,320-byte artifact dated 2026-09-14,
SHA-256 `EBA4C084C8E818110A32E6FE3963074F10A3EC9673CC4D29B19B8FE3E637D6C5`.
It is neither the current formal provider nor a T422 acceptance candidate.
The latest preceding formal provider is
`build/M0-T421/S4/wow32-ntvdd-r1/wow32.dll`, 561,664 bytes, SHA-256
`9CF81795CEBD00B0ADC8EAE3044C7C66217981A6B96F8ADC3E66C3B04EBA4261`.
Its map proves local `UserRegisterWowHandlers@8` composition, but no runtime
acceptance is inferred from that fact.

## Reproduction

```powershell
& .\tests\observation\verify-wow-user-profile.ps1 -RepositoryRoot (Get-Location).Path
& .\tests\observation\verify-wow-output-composition.ps1 `
  -BuildRoot 'build/M0-T421/S4/wow32-ntvdd-r1' -RequireLocalRegistration
```

Both commands were run against the stated baseline.  The first produced
`WOW_USER_PROFILE_SOURCE_AND_BINARY_OK`; the second reported a local 20-slot
registration symbol.  Both explicitly disclaim runtime acceptance.
