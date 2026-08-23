# M0 T243 S2 — Headless BOP-5F Source Recovery Result

## Result

The selected first-profile BOP `5F` sub-contract is closed.  The source-built
guest reached real-mode `C4 C4 5F` at `8e08:45a6` with `AX=BEEF`; the v2 route
accepted it, made the one source-shaped `useHostInt10` state publication, set
CF and resumed execution.  The next observed selector was independently
unimplemented `66`, so this is not a claim of an interactive keyboard, full
IVT lifecycle, or guest EXEC continuity.

## Source and ownership

The retained source shape is:

```text
NTIO spckbd.asm: CS:SI kio_table, DS resident segment, AX=BEEF
  -> SoftPC nt_bop.c: MS_bop_F
  -> SoftPC keybd_io.c: kb_setup_vectors
  -> CF -> original JC isk_int9
```

`src/bx-vdm/bop/opennt/softpc/spckbd_handoff.c` retains only the source table
guard (word 32), word 17 (`useHostInt10`) and the historical CF completion.
The named `spckbd_handoff_shim` transports those exact segmented accesses over
the existing selector-blind mantle checked-RAM ABI.  No Bochs source was
changed; neither bx-core nor bx-mantle recognizes a BOP, DOS, OpenNT, or a
keyboard table.

The rest of `keybd_io.c` remains deliberately out of scope: callback table
words, monitor iret hooks, keyboard hardware, IRQ1/PIC/timer, video and
printer behavior need their own PC-device owner package.  This is a source
shape plus minimal ABI seam, not an invented keyboard implementation.

## Contract and local matrix

On the declared `STREAM_IO=2` profile the route:

1. accepts only real-mode `C4 C4 5F` with `AX=BEEF`;
2. checked-reads only `CS:SI` words 32 and 17;
3. checked-writes byte `2` only at `DS:word17`;
4. produces `RIP+3` and a CF flag mask/value, with no GPR/segment/device
   mutation;
5. rejects a bad guard, inaccessible source/destination, wrong execution mode
   or AX without a partial write.

`t243-s2-spckbd-handoff-fixture.exe` passes these positive and negative cases.
The fresh external formal Ninja graph at
`build/M0-T243-S2/formal-r1` compiles and links that fixture and
`bin/ntdos64-native.exe` from the audited module manifest.

## Bounded native observation

The external direct run used the source-built roots `build/output/dos` and
`build/output/wow16`, with the root's `CONFIG.NT`/`AUTOEXEC.NT`, a one-million
instruction budget and BOP/UD observation enabled.  It completed inside the
30-second watchdog with exit `4` (budget terminal).  The relevant ordered
entries in `build/M0-T243-S2/native-observation-r2/stdout.log` are:

```text
bop[17] cs=8e08 eip=000045a6 selector=5f disposition=1 eax=0000beef
bop[18] cs=8e08 eip=000079c5 selector=66 disposition=0
```

The terminal is the pre-existing unimplemented `66` loop at `0216:0740`.
It is a separate owner boundary.  The observation is therefore solely a
verification that the admitted BOP-5F transaction is live on the source-built
NTIO/NTDOS/COMMAND path; it authorizes no trace-led follow-up patch.

## Disposition

`BOP-DEPENDENCY-104` and `BOP-TOP-5F` now record the completed headless
sub-contract.  A future keyboard/device package must begin from the remaining
unread table fields and original `keybd_io.c` lifecycle, not from this narrow
route.
