# T235 S5 Guest Bundle Result 001

## Question

Can the fixed executable-sibling `dos/` and `wow16/` bundle contain every
currently available OpenNT/OpenNT-4.5 guest binary, while identifying whether
each file is original prebuilt material or an original-toolchain rebuild?

## Inputs

- `refs/opennt` prebuilt MVDM DOS/WOW payloads and system bitmap fonts.
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm` original prebuilt additions,
  including `dpmi/486/dosx.exe`.
- Managed `src/opennt` plus checked-in OpenNT-4.5 MASM/LINK16 and DOS packing
  tools for the only missing files: `HIMEM.SYS` and `REDIR.EXE`.

## Procedure

`Build-T235S5GuestFallbacks.ps1` rebuilt HIMEM and REDIR in fresh build roots.
`Stage-T235S5GuestBundle.ps1` then populated `build/output/dos/` and
`build/output/wow16/`, giving original prebuilt material precedence. Finally,
`Test-T235S5GuestBundle.ps1` checked every manifest destination and recorded
source against its SHA-256.

## Observation

The final manifest contains 115 files: 87 `opennt-prebuilt`, 26
`opennt-4.5-prebuilt`, and two `source-built`. `DOSX.EXE` is the original
OpenNT-4.5 product file (36,616 bytes, SHA-256
`c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`).
The two rebuilds match their retained historical identities:

| File | Origin | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| `dos/HIMEM.SYS` | source-built | 4,768 | `08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08` |
| `dos/REDIR.EXE` | source-built | 3,338 | `b6e9fad30a5423ead9ecb45c8e28197ea62a39187d36241f9018db82facac3a7` |

The runner policy fixture compiled with strict warnings and returned 47 for
both direct `.COM` and `.PIF` inputs using only sibling `dos/` and `wow16/`.
No `ntvdmcfg.yaml` was present.

## Interpretation

The requested package inventory is complete for guest binaries currently
available in the approved local OpenNT and OpenNT-4.5 trees. The manifest is
the per-file provenance authority; it distinguishes original product binaries
from the two reproducible fallbacks without claiming guest execution, WOW16
loading, or redistribution approval.
