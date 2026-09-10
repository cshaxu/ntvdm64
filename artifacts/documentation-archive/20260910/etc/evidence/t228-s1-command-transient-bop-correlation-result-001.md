# T228 S1 P22 — COMMAND transient BOP correlation result

## Question

Does the bounded, hash-locked T228 execution actually route the selected DEM
file lifecycle after COMMAND reaches the map-supported `EndInit` procedure,
such that a `bx-vdm` file-provider repair could account for the observed
zero-filled transient target?

## Inputs and Observation Boundary

This result reuses the P21 diagnostic executable
`build/M0-T228-S1/p21-software-int-cli-20260818a/bin/ntdos64-native.exe`
(SHA-256 `562c2dff333c44f8649f6d6b9f6ca0a657760465a0c38dfecd58736c7460bb76`)
and the hash-locked four-image input manifest
`c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`.

The child process was created with exactly `SystemRoot`, `ComSpec`, the two
retained startup-source variables, and `OPAQUE_A` through `OPAQUE_D` (each
1,000 characters).  It used the existing default-off CLI observers
`--observe-bop-sequence`, `--observe-software-interrupts`, and
`--observe-budget-terminal-position`, a one-million tick budget, and no
source, guest input, provider, CPU, or Bochs change.  The existing BOP
sequence observer is adapter-local and copies only generic pre-dispatch BOP
facts plus the existing outcome disposition.

Two independently created cleared-environment roots retain the repeated
Direct/Readonly result.  The accepted evidence root is
`build/M0-T228-S1/p22-bop-and-software-int-observation-20260818b`; the repeat
is `...20260818c`.  In each root Direct and Readonly returned budget exit `4`
with byte-identical stdout SHA-256
`37ad2e751bcaedb326ddf6a637ac6ce733f1404b0d2e5a39d15f3182e433158d` and
stderr SHA-256
`52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860`.
The repeated Direct output is also byte-identical to the accepted root.

## Correlation

The BOP ring contains 40 records and reports no overflow.

1. Before `50:36`, it records the accepted DEM namespace lifecycle
   `50:12` (open), `50:00` (seek), `50:16` (read, twice), and `50:02`
   (close).  This is direct runtime confirmation of the already code-complete
   `bx_ntvdm_boot_namespace_plane_v1` / readonly file-family route.
2. Record 32 is accepted `50:36`, the original NTDOS entry-to-DOS-app
   lifecycle boundary.  It precedes the two exact linked COMMAND `EndInit`
   calls: `0C41:03A6` and `0C41:03CB`, both `54:0F`.
3. No `50:12`, `50:00`, `50:16`, `50:02`, or `50:42` record appears after
   those two `EndInit` calls before the bounded terminal.  Because the ring
   did not overflow, this is a complete statement about BOP records in this
   observation, not a tail-only inference.
4. The accompanying P20-style software-interrupt record continues to show
   the original `EndInit` `INT 21h` allocation/deallocation form.  P15's exact
   map places `03A6h` and `03CBh` inside `EndInit`; the `command.map` range
   starts it at `0332h` and ends initialization code at `0437h`.

The record must not label unrelated resident addresses as `LoadCom` or
`ReadCom`: the exact map publishes `LoadCom` at `0000:11D9` and `ChkSum` at
`0000:1228`, whereas the raw software-interrupt entries at `0C41:1C07` and
`0C41:1C11` lie outside that range.  Register resemblance is not a valid
source attribution.

## Result and Repair Disposition

The complete selected DEM file family is reachable and accepted before
COMMAND startup, but this observation does not show it executing after the
mapped `EndInit` calls.  Therefore the known zero-filled dynamic transient
cannot be repaired by adding, widening, or special-casing `50:12/00/16/02` or
`50:42` in `bx-vdm`.  That family is already present and its current observed
instance is not a post-`EndInit` reload witness.

P22 narrows the remaining defect to original guest COMMAND/NTDOS control and
interrupt-return state within the already selected transient/MCB owner domain.
It does not prove a specific allocation result, MCB header mutation, checksum
branch, or IRET result; software-interrupt records are pre-delivery only.  It
selects no code repair, no BOP leaf task, no adapter allocator, and no Bochs
semantic change.  A future diagnostic, if separately admitted, must observe a
whole mechanical interrupt-return/control boundary without interpreting a DOS
function or guest address.
