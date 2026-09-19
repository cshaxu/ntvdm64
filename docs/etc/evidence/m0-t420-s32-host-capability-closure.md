# M0 T420 S32 — SoftPC host capability closure

## Question

Can the complete selected original `softpc.new/host/src` package retain the
original worker entry, Console and host-provider lifecycle under the selected
Win32/x86 CCPU40 product without a second worker lifecycle or default-on
observation implementation?

## Source disposition

The original `sources` manifest selects 47 distinct C translation units (the
historical repeated `nt_msscs.c` entry is one physical unit).  The S12 complete
package ledger remains the source/hash authority for that manifest.  This S32
rerun compared every selected unit to the pinned OpenNT path with whitespace
at line ends ignored only for comparison.  Twenty selected units are byte
identical.  The other 27 have only registered source-first carriers:

| Carrier family | Selected units | Disposition |
| --- | ---: | --- |
| Modern x86 declaration/ABI spelling | `config`, `copy_fnc`, `nt_bop`, `nt_com`, `nt_eoi`, `nt_mem`, `nt_mouse`, `nt_sound`, `nt_thred`, `nt_timer`, `nt_unix` | Keep only where current MSVC/x86 rejects the original declaration or callback spelling. |
| CCPU40/SAS and descriptor execution | `nt_aorc`, `nt_cpu`, `nt_emm`, `nt_msscs`, `sim32` | The selected CCPU40 machine is the product CPU; original CPU30/kernel monitor mechanics are not selected. |
| Bounded guest-memory or platform resource binding | `nt_fdisk`, `nt_rflop`, `nt_rez`, `nt_pif`, `stubs` | Preserve original caller and ordering, binding only the unavailable worker-local backing, firmware root or VDM_TIB storage. |
| Console/process lifetime | `nt_error`, `nt_event`, `nt_fulsc`, `nt_hosts`, `nt_reset` | Retain original provider policy with the finite public Console and worker termination boundary. |

`nt_graph.c` is now byte-identical to the pinned original: the prior unused
`WriteConsoleA` result carrier and retired default-off comments were removed.
`nt_bop.c` has no observation hook; its three remaining source deltas are the
explicit modern-x86 function-pointer conversions required to preserve the
original `GetProcAddress` call shapes.  No new mirror source, overlay,
adapter lifecycle, broker policy, or worker entry was introduced.

The production entry remains the original `obj.vdm/ntvdm.c::main`.  Its finite
standalone pre-init/finish binding is process-local only: it connects the
worker's BaseClient transport and local heap/session resources, then enters
the original `host_main`; it does not classify programs, schedule commands or
own BaseSrv records.

## Verification

- The documented serial executor rebuilt the declared formal x86 graph at
  `build/M0-T420/S31/formal-x86-r1`.  It produced fresh `run16.exe`,
  `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`, `VDMREDIR.dll`, and the worker
  lifecycle fixture.  The target's `nt_bop.c` and `nt_graph.c` objects were
  compiled from the current sources.
- `ccpu-thread-lifecycle-test.exe` passed with
  `CCPU thread lifecycle normal-and-abnormal OK`.
- The five matching runtime files were deployed to `O:\winnt`; source and
  deployed SHA-256 values match in
  `O:\winnt\logs\m0-t420-s32-formal-artifacts-r1.json`.
- `Verify-CommandExitStatus.ps1` passed all 17 transcript-gated direct,
  nested and `COMMAND /c` routes, including MEM, EDIT, native stream/EOF,
  guest exit code 7 and repeat-worker paths.  Its complete result is
  `O:\winnt\logs\m0-t420-s32-command-r1-summary.json`.
- The post-run process check found no `run16.exe`, `basesrv.exe` or
  `ntvdm.exe` test residue.

## Result

S32 is closed.  The selected host package has a complete original-owner
disposition, the worker remains original-entry first, and the removed mirror
noise did not regress normal or abnormal worker lifecycle or the established
product routes.
