# M0 T245 S2 — NTDOS EXEC observation predecessor classification

## Question

Can the active source-built startup path reach the first declared local
`INT 21h/AH=4Bh, AL=0` profile without inventing a PSP, PDB, JFN, arena or
parent-return transition; and, if not, is the preceding owner unambiguous?

## Inputs

- OpenNT `base/mvdm/dos/v86/inc/pdb.inc`, `dos/misc.asm` and
  `dos/msproc.asm`.
- Source-built `NTIO.SYS` (`33792` bytes,
  `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937`) and
  `NTDOS.SYS` (`27858` bytes,
  `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84`)
  identities established by the T245 S1 source map.
- T243's bounded source-built observation, which reaches `BOP 5F`, resumes,
  and next reaches the unimplemented `BOP 66`.
- New fixture-only record
  `src/bx-vdm/bx_ntvdm_ntdos_exec_observation_v1.{h,c}` and
  `tests/bx-vdm/t245_s2_ntdos_exec_observation_fixture.c`.

## Procedure

1. Derived the copied prefix directly from `pdb.inc`: exit, CTRL-C and fatal
   vectors at offsets `10`, `14`, `18`; parent PDB at `22`; environment at
   `44`; fixed prefix size `50` bytes.
2. Kept the decoder in `bx-vdm`: it accepts a caller-owned checked copy only,
   has no guest pointer, and cannot allocate or modify a DOS process block.
3. Added the record and fixture to the formal x64 `/MT`, CPU5/P-MMX Ninja
   module graph.
4. Outside the sandbox generated
   `build/M0-T245-S2/formal-r1`, then built and ran
   `bin/t245-s2-ntdos-exec-observation-fixture.exe`.

## Observations

- The fixture accepts `PDB=1234h` only as bounded physical range
  `12340h..12371h`, rejects `PDB=ffffh` because its 50-byte prefix would leave
  the one-MiB fixture machine, and rejects every non-exact prefix length.
- It copies the documented vectors and the parent/environment segments
  verbatim. It does not interpret a vector, duplicate a JFN, allocate an
  arena, or write guest memory.
- Formal Ninja rebuilt `105` steps, including the new `bx-vdm` object and
  fixture, linked successfully, and the fixture returned zero.
- No source-built run can yet present a PDB to this record: the verified boot
  sequence reaches `spcemm.asm:InitSpcEmm` and its `BOP 66` first. That
  original caller treats `BX=0` as the no-EMS outcome and skips installation
  of `INT 67h`.

## Interpretation

S2 closes the observation contract and classifies the pre-EXEC predecessor.
It does **not** claim guest `EXEC` reachability or a parent return. The next
work is a separately admitted source-first recovery of the narrow
`InitSpcEmm` unavailable result; it must retain `BX=0`/resume semantics while
not enabling an EMS page frame, `INT 67h`, `BOP 68`, host PIF, or any Bochs
device. That recovery remains adapter-routed OpenNT/SoftPC source ownership;
Bochs remains selector-blind.

## Confidence And Follow-up

High confidence for the PDB prefix layout and the immediate predecessor,
because both are explicit source contracts and the fixture checks its range
and decode boundary. Runtime confidence is deliberately limited to the
already observed `5F -> 66` sequence. A later T245 subtask must rerun the
same source-built bundle after the narrowly admitted predecessor is available
and then either emit a copied PDB fact or classify the next predecessor.
