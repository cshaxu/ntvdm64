# M0 T378 S4 — Formal DOS-extender workload matrix closure

## Fixed product and stage

The current formal CPU40/x86 target was re-invoked:

```text
build\M0-T371\S2\formal-x86\run-ninja-parallel.cmd original-softpc-process.exe
```

Ninja reported `no work to do`.  The selected final product and its staged
copy are both SHA-256:

```text
09d666c501b5ef859dd36dcac1c537db6c9f2d695ac2ca1c444d1762977085a8
```

The immutable selected extender media remain byte-identical:

- original HIMEM mirror and `O:\ntvdm64\system32\HIMEM.SYS`:
  `08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08`;
- original DOSX artifact and `O:\ntvdm64\system32\DOSX.EXE`:
  `c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`.

The authoritative stage tool was re-run with `--update`, but without the
sole opt-in `--replace-product` switch.  Thus it would have refused any
changed staged guest or firmware byte (and any changed product EXE).  The
refreshed `O:\ntvdm64\runtime-manifest.json` SHA-256 is:

```text
499dd7d55de944173359973e1eb01ed6468a036e8c49a130b5c0b5cb873420cc
```

## Complete selected owner matrix

```text
CONFIG.NT device=HIMEM.SYS
  -> original cmdconf / guest configuration-device route
  -> immutable original HIMEM.SYS
  -> original XMSInit, A20, UMB, INT 15 package

AUTOEXEC.NT lh DOSX / declared DOSX.EXE workload
  -> original COMMAND/DEM guest $Exec route
  -> immutable original DOSX.EXE
  -> original MS_bop_3
  -> original 25-entry DpmiDispatchTable and DPMI32 bodies
```

Every selected host cohort is directly present in the formal final link:
`original-mvdm-command.lib`, `original-mvdm-dem.lib`,
`original-mvdm-xms.lib`, `original-mvdm-dpmi32.lib`, the CPU40/SoftPC archives
and their existing same-shaped bindings.  Guest/firmware artifacts remain
staged media, never static-library inputs.

## Result and S5 contract

S4 is closed.  No source, BOP behavior, media byte or machine behavior
changed.  S5 may execute exactly one non-debug, console-owning observation
against this EXE and the fixed `O:\ntvdm64` stage, with declared command
`DOSX.EXE` and the standard bounded timeout.  It must report only the first
actual source-owned result; it may not create an extender service repair.
