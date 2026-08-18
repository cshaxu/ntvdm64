# M0 T225 — Guest image layout and lifecycle correction 001

The source-built NTDOS file is not a permanently fixed real-mode image.
`origin.asm`/`stripz.c` and `NTDOS.MAP` use a `0x2250` strip relationship;
`sysinit1.asm:903-928` relocates the loaded image to a temporary segment `T`,
with DOSCODE at `T-0x225`; `DosInit` then copies/migrates DOSDATA to low
memory. COMMAND, PSP, arena and device-chain state are later guest-owned.

Consequently neither `runtime CS:EIP - 0x8B00` nor a raw final-file offset
proves ownership of a later runtime instruction. The observed 50:11 EDI=08B0
proves only the initial typed NTDOS copy destination. It cannot alone map the
later 0C41:1F97 BOUND fault to an NTDOS file byte.

The correct source-first sequence is `50:11 -> sysinit temporary relocation
-> DosInit data migration -> charinit -> original NTIO/BIOS device chain`.
The previously retained S5 address map and charinit boundary establish that
these are distinct guest-owned lifecycles. No BOP, host capability, Bochs
CPU/device or adapter repair is selected by this correction.
