# T229 S2 — GSET, drive, CWD and volume conformance ledger

The 15 callable `demdisp.c` identities in this owner group share session drive
admission, captured volume data, checked guest buffers, and the shared DEM
mutation profile. A typed route alone is not completion evidence.

| BOP | OpenNT handler | Direct/Readonly conformance evidence | Remaining matrix |
| --- | --- | --- | --- |
| `50:0D` | `demGetBootDrive` | P — admitted boot drive in AL | registry fallback variants |
| `50:0E` | `demGetDriveFreeSpace` | P — captured DOS geometry and invalid-drive failure | large-volume reduction |
| `50:0F` | `demGetDrives` | P — admitted drive count | sparse projection variants |
| `50:10` | `demGSetMediaID` | P — checked VOLINFO get; original CF-only set failure | OEM label conversion |
| `50:13` | `demQueryCurrentDir` | P — CDS/root projection | invalid-media reset |
| `50:14` | `demQueryDate` | P — packed weekday/year/month/day registers | clock boundary |
| `50:15` | `demQueryTime` | P — packed hour/minute/second registers | hundredths boundary |
| `50:18` | `demSetCurrentDir` | P — Direct session CWD; Readonly pre-host refusal | non-root path/error |
| `50:19` | `demSetDate` | P — Direct `SetLocalTime`; Readonly AL=FF | successful time-change authorization |
| `50:1A` | `demSetDefaultDrive` | P — Direct host session CWD; Readonly refusal | mismatched-drive/error |
| `50:1B` | `demSetDTALocation` | P — DTA/PDB/SFT registration | malformed pointer variants |
| `50:1C` | `demSetTime` | P — Direct `SetLocalTime`; Readonly AL=FF | successful time-change authorization |
| `50:25` | `demGetDPB` | P — full 35-byte DPB and invalid drive | physical media layouts |
| `50:41` | `demGetComputerName` | P — checked OEM host-name buffer and CX contract | empty-name fallback |
| `50:46` | `demGetDPBList` | P — linked compact DPB chain and BP result | multi-drive chain |

`P` means a source-shaped Direct/Readonly local session proof exists; it does
not claim every host or error variant is exhausted.