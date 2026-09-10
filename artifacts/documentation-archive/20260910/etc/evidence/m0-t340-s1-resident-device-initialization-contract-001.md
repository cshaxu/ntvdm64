# M0 T340 S1 — Resident device-initialization source contract

## Question

What original package executes after `54:05`, what the resident DOS-BIOS
devices actually require for their initialization requests, and which first
non-trivial owner package follows them?

## Inputs

- Selected MVDM guest sources: `doskrnl/dos/msinit.asm`,
  `doskrnl/dos/dev.asm`, `doskrnl/bios/msbio1.asm`, `mschar.asm`,
  `sysinit1.asm`, `sysinit2.asm`, and `sysconf.asm`.
- Selected guest HIMEM package under `mvdm-guest/dos/v86/dev/himem/`, plus the
  staged `build/r338/mvdm/config.nt` and `system32/HIMEM.SYS`.
- Selected host sources `dos/command/cmdconf.c` and `softpc.new/host/src/nt_pif.c`.
- The closed T339 passive observation, which reached `54:05` but did not
  expose a 50:3D callsite discriminator.

## Procedure

1. Follow `DOSINIT::SVC_CMDSETINFO` to the first `charinit` request.
2. Enumerate `res_dev_list`, each strategy/interrupt entry and function-00
   dispatch table result.
3. Follow the returned SysInit configuration pass through `DEVICE=` load and
   driver initialization.
4. Inspect the selected configuration and the first referenced driver package
   without changing its media, code, machine behavior or run container.

## Observations

- `msinit.asm` builds a `DEVINIT` request in DOSDATA and `DEVIOCALL2` calls
  the resident header strategy followed by interrupt.
- All eleven initial headers are internal BIOS character-device entries. Their
  function-00 table entries (`con_table`, `aux_table`, `prn_table`, and
  `tim_table`) are the same original `bc_exvec`, which clears carry and
  returns. Their init path therefore does not invoke a host console, serial,
  parallel, timer or video operation.
- The HMA guard remains source-relevant but does not add a dependency to this
  transaction: the request packet is in DOSDATA, which the original
  `cdev_entry` recognizes as already A20-enabled.
- `perdrv` transfers only after the complete chain to `SVC_DEMGETDPBLIST`.
  This is a later owner boundary, not proof of execution in the fixed run.
- `config.nt` is normal host input, not a fallback. `cmdconf.c` filters its
  supplied `shell=` line and writes its own normalized COMMAND.COM shell line
  while retaining only the allowed third-party-shell or environment parameter.
  A selected config input that cannot be opened follows the existing terminal
  direction; there is no alternate `cmdconf.c` fallback implementation.
- The selected configuration contains `DOS=HIGH, UMB` and
  `DEVICE=%SystemRoot%\system32\HIMEM.SYS`. `multi_pass=1` executes that
  driver only after the resident initialization transaction returns to SysInit.
  The driver body is present in the immutable guest mirror and its source
  requires original XMS, A20, INT 15 and HMA/UMB contracts.

## Interpretation and confidence

High confidence: a post-`54:05` delay cannot be justified by inventing a
host-console implementation for resident `CON`, nor by treating the direct
resident headers as configuration-driver calls. The original resident cohort
is already code-complete guest source and its init operation is a no-op
success path. The first source-defined non-trivial successor is the complete
HIMEM/SoftPC XMS-A20-INT15 cohort introduced by the selected `DEVICE=` line.

This does not prove that the fixed run has reached HIMEM, nor that HIMEM is
the observed delay. It only sets the owner-correct next recovery/admission
boundary and rules out a trace-selected console/port shim.

## Follow-up

T340 S2 may be admitted only for the whole original HIMEM device and its
SoftPC XMS/A20/INT15/HMA contracts. It must retain original driver order,
request packet and failure semantics, use the existing mapping-manager only
at real host/guest pointer seams, and must not add a synthetic device or alter
`config.nt` to evade the path.
