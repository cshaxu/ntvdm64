# T228 S1 P29 — EndInit Reentry Mechanics Correction

## Question

Do the later `4444h` values observed at `0C41:0354` show a failed initial
COMMAND environment setup, or an invalid later return into COMMAND's
one-time initialization tail?

## Inputs

- Hash-locked Direct/Readonly observation:
  `build/M0-T228-S1/p21-software-int-observation-20260818a/readonly.stdout.log`.
- Rebuilt original COMMAND link map:
  `build/M0-T228-S1/command-link-map-rebuild-20260818a/base/mvdm/dos/v86/cmd/command/command.map`.
- Original OpenNT sources: `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm:439..686`,
  `stub.asm:123..190`, `command2.asm:284..418`, and `init.asm:2167..2418`.
- Original NTDOS EXEC handoff:
  `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm:1296..1309`.
- Retained P17, P18, P25, and P28 records.

## Procedure

1. Map observed `0C41:0354` to the rebuilt COMMAND map and original `EndInit`.
2. Compare the first and later visits in the same immutable native observation.
3. Read original `EndInit` lifetime and the `PDB_Exit -> LodCom_Trap -> LodCom_Entry -> LodCom` return path.

## Observations

The map places `EndInit` at `0332h`; its first `INT 21h` `SETBLOCK` call is at
`0354h`. At software-interrupt record 45 (sequence 231820), the first visit
has `CX=001Eh` and `DX=0A14h`: the expected values after the original code
loads `UsedEnv` and `TrnSize`. The following allocation/deallocation calls
continue at `0369h`, `038Ah`, `0395h`, and `03B9h`.

Only later, at record 55 (sequence 310763), does that same `0354h` site have
`CX=4444h` and `DX=4444h`. The map also places `Pipe1` and `EndInit` at the
same `0332h` address. `rdata.asm` explicitly documents this overlap as the
way `EndInit` is discarded after initialization. The routine then jumps to
`LodCom_Trap`, whose stub pushes old/current data segments and transfers
through patched `LodCom_Entry`. Original NTDOS EXEC separately installs the
parent return destination in both `PDB_Exit` and `ADDR_INT_TERMINATE`.

## Interpretation

The first initialization was valid. `4444h` is not evidence of a missing host
environment, failed `54:0F` provider, or initial COMMAND image-load failure.
It is evidence of later execution in a code/data region that original COMMAND
intentionally discards.

The unresolved owner domain is the original COMMAND resident-stub
relocation/termination-reload continuity together with the original NTDOS
parent return handoff. This remains one complete owner-domain question. It
does not authorize a BOP leaf, adapter MCB/PSP synthesis, guest-image edit, or
COMMAND-specific Bochs behavior.

## Follow-up

Compare the complete original `PDB_Exit`, `LodCom_Trap`, patched
`LodCom_Entry`, and `LodCom` transfer chain against current guest control
transfer. If scalar history cannot distinguish it, admit only a default-off,
selector-blind mechanical control-transfer observer through the intrusion
register; it must not recognize BOP, OpenNT, DOS, a guest address, or an
opcode in product behavior.