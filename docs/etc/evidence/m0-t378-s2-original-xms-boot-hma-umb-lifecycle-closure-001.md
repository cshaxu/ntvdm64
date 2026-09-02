# M0 T378 S2 — Original XMS boot / HMA / UMB lifecycle closure

## Question

Does the current CPU40/SoftPC product retain the complete original
`CONFIG.NT -> HIMEM.SYS -> XMSInit -> A20/UMB/INT 15` boot cohort without a
synthetic device, XMS dispatcher, guest-media rewrite, or raw host-pointer
carrier?

## Inputs

- T378 S1's byte-identical `HIMEM.SYS` and `DOSX.EXE` stage identities.
- Original `mvdm-host/dos/command/cmdconf.c`,
  `softpc.new/host/src/nt_msscs.c`, `xms.486/{xms,xmsa20,xmsblock,xmsdisp,
  xmsmisc,xmsumb}.c`, and the immutable HIMEM source/artifact pair.
- Existing original-source proofs T340 S2 and T371 S2, plus the current
  `M0-T371/S2/formal-x86` product graph.

## Procedure

1. Re-read the complete original configuration expansion loop.  It preserves
   `device=%SystemRoot%\\system32\\himem.sys` and
   `lh %SystemRoot%\\system32\\dosx`; it filters only the original
   `country=` and `shell=` special cases.  Its historical long-path comment
   is not a device/loadhigh removal path.
2. Re-read the bootstrap order: original `scs_init` calls `XMSInit` after
   `DemInit` and before debugger initialization.  Original `config.c` retains
   its one-megabyte XMS minimum specifically for HIMEM and DOSX.
3. Re-run the existing source-shaped original XMS startup fixture in a fresh
   T378 work directory.  It compiled original `xms.c` and `suballoc.c` with
   the established mapping-manager backing boundary and exited zero for both
   the original VDM-allocation and `STATUS_NOT_IMPLEMENTED` fallback branches.
4. Re-run the formal CPU40 product target:

   ```text
   build\\M0-T371\\S2\\formal-x86\\run-ninja-parallel.cmd original-softpc-process.exe
   ```

   Ninja reported `no work to do`.  Its current graph explicitly selects
   `cmdconf.c`, `nt_msscs.c`, all six original XMS bodies, and the required
   mapping-manager binding; the target is already linked.

## Observations

- `cmdconf.c` preserves the selected original device/loadhigh records as
  lines in the temporary original configuration files.  No app-side parser or
  substitute device is involved.
- `HIMEM.SYS` keeps its original `InitInterrupt -> InitDriver` sequence and
  reaches the original XMS service contract for A20, extended memory and UMB
  publication.
- Original `xmsdisp.c` retains the twelve-entry table; `xmsmisc.c` retains
  `UpdateKbdInt15(CS:AX)` and the original `CX=xmsMemorySize` result.
- The only reached modern seam is the established session mapping-manager
  backing at the historical process-address boundary.  Numeric XMS addresses,
  A20 state coordinates and INT 15 results remain original guest values.
- The fresh fixture build displayed pre-existing, unsuppressed warnings
  (`suballoc.c` C4701 and unused XMS startup arguments).  They do not change
  this CPU40/x86 source-binding result.

## Result

S2 is source-, binding- and formal-link closed.  This is a whole original
configuration/device/XMS cohort result, not a claim that the frozen product
observation has independently emitted a HIMEM marker.  The next owner is the
complete immutable DOSX-to-DPMI handoff cohort; no `52:xx` leaf repair is
admitted.

## Follow-up

T378 S3 owns the original `$Exec`/DOSX real-mode handoff, `MS_bop_3`,
`DpmiDispatch`, protected-mode frame/stack and normal or source-shaped
unavailable return.  S5 remains the only fixed-container observation.
