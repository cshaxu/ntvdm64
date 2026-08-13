# T194 S11 one-slot normal-return asset admission 001

Date: 2026-08-12  
Packet: M0 T194 S11  
Disposition: no current v6 runtime asset set; SHARE binary recovery required.

## Provenance and intended mapping

OpenNT `share.asm` is the selected normal-return candidate: it executes
`MOV AH,4Ch; XOR AL,AL; INT 21h; RET`. T177 reproduced the unchanged source
with historical MASM/LINK16 as an 882-byte image whose SHA-256 is
`69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.
Its v4 observation mapped that image to the canonical profile filename
`TARGET.EXE`, which is the correct existing profile contract; it must not be
admitted as a raw `SHARE.EXE` filename.

## Current workspace check

`artifacts/toolchain-runs/share-tools16-opennt-v1` contains the source and
`share-build-manifest.json`, but no output executable. The retained manifests
refer to former workspace output locations. Repository-wide artifact searches
find no current file matching the recorded SHA-256 or an eligible
`TARGET.EXE` copy. Consequently there is no complete v6 BYOB root containing
the verified `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, and source-built SHARE
mapped as `TARGET.EXE`.

## Result

This is an asset/provenance blocker before execution, not a Bochs, adapter,
BOP, or COMMAND result. S11 did not copy, rename, build, or execute an asset.
S12 must separately recover the exact artifact under the source policy and
record the profile-root materialization before any observation can be admitted.
