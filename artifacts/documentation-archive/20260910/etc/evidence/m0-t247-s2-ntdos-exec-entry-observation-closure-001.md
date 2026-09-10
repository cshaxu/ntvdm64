# M0 T247 S2 — NTDOS EXEC entry PDB observation closure

## Question

Can the project inspect the source-built NTDOS child PDB at the exact original
`$Exec` handoff without implementing, allocating or modifying DOS process
state in `bx-vdm`?

## Recovery

The original `msproc.asm:$Exec:Xfer_To_User` calls `SVC_DEMENTRYDOSAPP`
(`50:36`) after `$Dup_PDB`, PDB vector/environment writes and command-tail
copy, and before it loads the child stack and executes `retf`.  The active
imported `demmisc.c:demEntryDosApp` continues to own that BOP body.

`src/bx-vdm/bop/observation/bx_ntvdm_ntdos_exec_entry_observation_v1.*`
attaches only after the existing generic bridge has accepted that original
route with a typed Resume result.  It:

1. rejects every non-real-mode, non-`50:36`, non-Resume or repeated event;
2. treats the low 16 bits of copied `EDX` as the guest PDB segment;
3. reuses `bx_ntvdm_ntdos_exec_observation_v1` to validate and read exactly
   50 bytes through the existing checked-RAM callback; and
4. records fixed `pdb.inc` fields (return/interrupt vectors, parent PDB and
   environment) in adapter-owned diagnostic storage.

It does not retain a guest pointer, write guest RAM, change the original
provider result, add a selector, or model PDB/arena/JFN state.  No Bochs,
mantle, original guest or imported OpenNT provider source was changed.

## Verification

Fresh formal graph: `build/M0-T247-S2/formal-r1` (MSVC x64, `/MT`, CPU5/P-MMX).

```text
ninja -C build/M0-T247-S2/formal-r1 \
  bin/t247-s2-ntdos-exec-entry-observation-fixture.exe
```

The focused fixture passes and proves:

- a real-mode `C4 C4 50 36`, DX=`1234h` event copies the expected PDB vectors,
  parent PDB and environment from physical `12340h`;
- the copied 50-byte guest prefix is byte-for-byte unchanged;
- a typed Stop outcome is ignored; and
- an out-of-range PDB segment records a bounded read failure rather than
  dereferencing or wrapping guest memory.

## Conclusion and transfer

S2 closes the smallest source-defined EXEC setup observation seam.  It proves
the next S may ask whether a source-built local DOS child can execute and
return through the already staged guest `$Exit/$Abort/reset_environment`
sequence.  It does **not** claim that this continuity has occurred.

HMA/A20-off, remote JFNs, WOW/unknown binaries, debugger notifications and
actual machine execution remain their named owners; no new BOP work is
selected from trace output.
