# T225 S8 Primary SHARE Source-Build Result 001

## Question

Can the primary SHARE target be rebuilt from managed OpenNT source with its
original tools16 MASM/LINK16 contract?

## Procedure and observation

`tools/build/Build-T225S8PrimaryShare.ps1` created a fresh stage, copied only
`share.asm`, `share.lnk`, and the makefile from `src/opennt`, assembled the
original one-source target with OpenNT MASM, and linked the unchanged response
file with OpenNT LINK16 `/E`.

The resulting manifest-bearing stage reports `SHARE.EXE` at 882 bytes and
SHA-256 `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc`,
which is the locked primary identity.  No prebuilt executable was copied into
the stage.

## Disposition

SHARE is restored as a fresh primary-original-toolchain source-built input.
This is build provenance only; it does not execute guest code or close the
four-image S8 fixture until COMMAND and NTDOS have their own fresh manifests.
