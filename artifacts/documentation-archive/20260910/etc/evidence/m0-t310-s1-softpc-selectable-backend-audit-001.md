# M0 T310 S1 — original SoftPC selectable-backend source/contract audit

## Question

Can the product restore original SoftPC/CCPU as a real CLI-selectable backend
while retaining the currently working Bochs mechanical backend and one imported
MVDM host composition per session?

## Inputs and method

The audit read the exact byte-identical MVDM mirror inputs and original build
manifests:

- `softpc.new/base/ccpu386/sources` (`TARGETNAME=ccpu386`, `TARGETTYPE=LIBRARY`);
- `softpc.new/host/src/sources` (`TARGETNAME=src`, `TARGETTYPE=LIBRARY`);
- `softpc.new/obj.vdm/sources` (`TARGETNAME=ntvdm`, original library order and
  `CCPU` conditional selection);
- host roots `nt_cprgs.c`, `nt_cpu.c`, `nt_aorc.c`, `nt_reset.c`, `nt_error.c`,
  `nt_msscs.c`, `sim32.c`, `nt_sas.c`, `nt_mem.c`, and `nt_umb.c`;
- executor roots `c_main.c`, `c_getset.c`, `ccpusas4.c` and `ntthread.c`.

The commands used were:

```powershell
Get-Content src/mvdm-host/softpc.new/base/ccpu386/sources
Get-Content src/mvdm-host/softpc.new/host/src/sources
Get-Content src/mvdm-host/softpc.new/obj.vdm/sources
rg -n "c_cpu_simulate|cpu_simulate|host_simulate|host_simulate_func|CpuH|CpuInt_c|Sim32|sas_|host_cpu_|host_reset|InitialiseDosEmulation|scs_init" src/mvdm-host/softpc.new
Get-FileHash <each selected root> -Algorithm SHA256
```

The resulting physical root and interface records are in the companion
`m0-t310-s1-softpc-source-root-ledger.tsv` and
`m0-t310-s1-softpc-backend-interface-ledger.tsv`.

## Findings

1. The original CCPU is a full executor, not a small CPU helper. Its original
   `sources` manifest lists 132 translation units covering decode, integer,
   protected mode, exception, FPU, string, paging, SAS and thread support.
   `nt_cprgs.c` binds the historical MVDM register and `host_simulate` function
   tables directly to those CCPU bodies. It must therefore be imported and
   tested as a separate selected backend, never embedded in `adapter-bochs`.

2. Bochs and CCPU must be alternatives, not cooperative CPU layers. The common
   contract is deliberately limited to session-owned create, reset, bounded
   run/resume, copied stop result and teardown. Historical MVDM call spellings
   remain bound by `adapter-mvdm-host-out/softpc`; `adapter-bochs` remains
   purely Bochs mechanical assembly and has no SoftPC/CCPU vocabulary.

3. `nt_cprgs.c`, `nt_cpu.c` and `nt_aorc.c` are the canonical original
   function-table/register contract. Existing Bochs-backed forms may support
   their source spelling, but they are not a reason to retain a replacement
   body if direct original source becomes composable in S2.

4. `sim32.c`, `nt_sas.c` and the CCPU-side `ccpusas4.c` establish the decisive
   mapping rule: original 32-bit pointer aliases cannot cross a modern x86/x64
   component boundary. Both selected backends require the existing
   session-owned guest-memory mapping-manager and bounded lease rules. This is
   a same-shaped binding requirement, not a new mapping manager. This is the
   owner-approved source-modification exception: every such original 32-bit
   pointer/handle/VDM alias is replaced by the existing mapping manager and
   bounded lease. Each change must retain the original function shape, call
   ordering and observable failure rule, carry `DIVERGENCE:` at the exact
   source location and be indexed by the mirror README.

5. `nt_reset.c`, `nt_error.c` and `nt_msscs.c` are inseparable original host
   composition roots. Their historic Console/User/VDD/private-process and
   filesystem-install branches prevent a direct modern product link today,
   but their source bodies remain selected candidates. S2 must retain source
   layout and bind only the reached modern facade, with every divergence
   registered. In particular, `AddSystemFiles` cannot mutate a host drive;
   it remains unavailable until an explicitly approved staged-guest-input
   binding is designed.

6. The historical `nt_mem.c` virtual-address backing and `nt_umb.c` section/
   VDD UMB model cannot be treated as current CPU backend plumbing. They remain
   named later XMS/VDD owners. Bochs remains current guest-RAM owner; no
   original host pointer may be recast as guest RAM.

7. The original target metadata proves that CCPU is a historical x86 target.
   This audit has no evidence that the intact CCPU body is x64 composable. S2
   must build a separate x86 original graph first and run an explicit x64
   source-compatibility probe. Failure is a source fact, not permission to
   rewrite the executor. A later owner decision may choose a brokered x86
   SoftPC backend for x64 CLI, but this S does not admit that process boundary.

## S1 contract decision

The next implementation may introduce one fixed-width project-owned backend
selection record outside mirrors, with enum values `softpc` and `bochs`, and
only inactive/create/reset/run/stop/teardown state. It is owned by session and
chosen by app before backend creation. The record contains no CPU state,
guest pointer, host pointer, handle, BOP selector or service result.

`adapter-mvdm-host-out/softpc` is the sole historical-shape binding consumer.
It dispatches only to the already bound backend. It cannot choose policy,
accept a backend switch while active, or join CCPU and Bochs object graphs.

## Source-recovery ladder

| Need | First usable recovery rung | Rejected lower-rung shortcut |
| --- | --- | --- |
| CCPU executor | Whole original `ccpu386` source target | Copy selected instruction helpers into Bochs or an adapter. |
| CCPU register/run shape | Original `nt_cprgs.c`/`nt_cpu.c`/`nt_aorc.c` through selected backend binding | New generic CPU frame API exposed to MVDM callers. |
| SIM32/SAS | Original call shape through session lease/mapping seam | Raw native guest pointer or family-private mapper. |
| Reset/error/MSSCS | Whole original roots with named source-shaped external bindings | Cropped Bochs-only reset/error/startup helpers. |
| Memory/UMB | Retain original evidence until XMS/VDD packages own it | Put UMB/DOS semantics in adapter-bochs. |

## S2 entry criteria

S2 may start with the exact original `ccpu386` source manifest and the selected
host-control roots. It must create separate MSVC `/MT` x86 and x64 Ninja
candidate graphs under `build/M0-T310/`, demonstrate original source identity,
and classify every compile/link residual by the two ledgers. It must not claim
a runnable SoftPC backend, modify a mirror, activate a CLI option, or call a
Bochs API from CCPU source.
